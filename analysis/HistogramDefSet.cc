/* 
Beam Delivery Simulation (BDSIM) Copyright (C) BDSIM Collaboration, 2001 - 2026.

This file is part of BDSIM.

BDSIM is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published 
by the Free Software Foundation version 3 of the License.

BDSIM is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BDSIM.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "HistogramDef.hh"
#include "HistogramDefSet.hh"
#include "RBDSException.hh"
#include "SpectraParticles.hh"

#include <map>
#include <regex>
#include <set>
#include <stdexcept>
#include <string>

HistogramDefSet::HistogramDefSet(const std::string&  branchNameIn,
                                 const HistogramDef* baseDefinitionIn,
                                 const std::set<ParticleSpec>& particlesSpecs,
                                 const std::string&  particleSpecificationIn,
                                 const std::string&  definitionLineIn):
  branchName(branchNameIn),
  dynamicallyStoreIons(false),
  dynamicallyStoreParticles(particlesSpecs.empty()),
  what(writewhat::all),
  topN(1),
  definitionLine(definitionLineIn),
  samplerType(samplertype::plane)
{
  if (!baseDefinitionIn)
    {throw std::invalid_argument("invalid histogram definition");}

  baseDefinition = baseDefinitionIn->Clone();

  if (!particlesSpecs.empty())
    {
      for (const auto& particleSpec : particlesSpecs)
        {
          HistogramDef* h = baseDefinitionIn->Clone();
          std::string thisHistName = "Spectra_" + h->histName + "_";
          if (particleSpec.first > 0) // prepend + character for positive values so we always have + or -
            {thisHistName += "+";}
          thisHistName += std::to_string(particleSpec.first);
          h->histName = thisHistName;
          std::string suffix;
          switch (particleSpec.second)
            {
              case RBDS::SpectraParticles::primary:
                {suffix = "_Primary"; break;}
              case RBDS::SpectraParticles::secondary:
                {suffix = "_Secondary"; break;}
              default:
                {break;}
            }
          h->histName += suffix;
          h->selection = AddPDGFilterToSelection(particleSpec, h->selection, branchName);
          definitions[particleSpec] = h;
          definitionsV.push_back(h);
        }
    }
  else
    {
      std::string spec = particleSpecificationIn;
      std::transform(spec.begin(), spec.end(), spec.begin(), ::tolower);
      spec = RemoveSubString(spec, "{");
      spec = RemoveSubString(spec, "}");

      if (spec.find("all") != std::string::npos || spec.find("ions") != std::string::npos)
        {dynamicallyStoreIons = true;}
      if (spec.find("all") != std::string::npos || spec.find("particles") != std::string::npos)
        {dynamicallyStoreParticles = true;}

      std::map<std::string, writewhat> keys = {{"all",           writewhat::all},
                                               {"particles",     writewhat::particles},
                                               {"ions",          writewhat::ions}};

      auto search = keys.find(spec);
      if (search != keys.end())
        {what = search->second;}
      else
        {
          std::regex topNR("top(\\d+)(?:particles|ions)*");
          std::smatch match;
          if (std::regex_search(spec, match, topNR))
            {
              try
                {topN = std::stoi(match[1]);}
              catch (...)
                {topN = 1;}
              if (spec.find("particles") != std::string::npos)
                {what = writewhat::topNParticles;}
              else if (spec.find("ions") != std::string::npos)
                {what = writewhat::topNIons;}
              else
                {what = writewhat::topN;}
            }
          else // this will happen if 'top' isn't in the specification or it generally doesn't match
            {throw RBDSException("Invalid particle specifier \"" + particleSpecificationIn + "\"");}
        }
    }
}

HistogramDefSet::~HistogramDefSet()
{
  delete baseDefinition;
  for (auto kv : definitions)
    {delete kv.second;}
}

std::string HistogramDefSet::RemoveSubString(const std::string& stringIn,
                                             const std::string& wordToRemove) const
{
  std::string result = stringIn;
  while (result.find(wordToRemove) != std::string::npos)
    {
      size_t pos = result.find(wordToRemove);
      result.erase(pos, wordToRemove.size());
    }
  return result;
}

void HistogramDefSet::ReplaceStringInVariable(const std::string& match,
                                              const std::string& replacement)
{
  baseDefinition->ReplaceStringInVariable(match, replacement);
  for (auto* def : definitionsV)
    {def->ReplaceStringInVariable(match, replacement);}
}

std::ostream& operator<< (std::ostream &out, const HistogramDefSet& s)
{
  out << "Spectra: " << s.baseDefinition->histName;
  if (!s.definitionLine.empty())
    {out << ": \"" << s.definitionLine << "\"";}
  out << "\n";
  for (const auto* d : s.definitionsV)
    {out << *d;}
  return out;
}

std::string HistogramDefSet::AddPDGFilterToSelection(const ParticleSpec& particleSpec,
                                                     const std::string& selection,
                                                     const std::string& branchName)
{
  long long int pdgID = particleSpec.first;

  if (pdgID == 0) // '0' is our code for total or really all particles in 1x histogram
    {return selection;} // just return whatever selection is

  RBDS::SpectraParticles flag = particleSpec.second;
  std::string flagFilter;
  switch (flag)
    {
      case RBDS::SpectraParticles::primary:
        {flagFilter = "&&" + branchName + ".parentID==0"; break;}
      case RBDS::SpectraParticles::secondary:
        {flagFilter = "&&" + branchName + ".parentID>0"; break;}
      default:
        {break;}
    }
  std::string filter = branchName+".partID=="+std::to_string(pdgID) + flagFilter;

  // input selection could be:
  // 1
  // samplerName.weight
  // samplerName.variable*number/number
  // samplerName.variable<value // Boolean expression
  // variable*(Boolean)
  // we need to put in the bonus Boolean if needed
  // check if it has a Boolean expression already in it
  std::string result;
  std::regex boolOperator("&&|[<>!=]=|[<>]|\\|\\|");
  std::smatch match;
  if (std::regex_search(selection, match, boolOperator))
    {// has boolean operator somewhere in it
      std::string afterBool = match.suffix();
      std::size_t bracketPos = afterBool.find(')');
      result = selection; // copy it
      if (bracketPos == std::string::npos) // no bracket found - so just Boolean condition on its own
        {result += "&&" + filter;}
      else
        {result.insert(match.position()+ match.length() + bracketPos, "&&"+filter);}
    }
  else if (selection.empty() || selection == "1") // technically the selection shouldn't be empty, but just in case...
    {result = filter;}
  else
    {result = selection + "*("+filter+")";}
  return result;
}

