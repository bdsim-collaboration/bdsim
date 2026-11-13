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
#ifndef HISTOGRAMDEFSET_H
#define HISTOGRAMDEFSET_H
#include "SpectraParticles.hh"

#include <map>
#include <set>
#include <string>

class HistogramDef;

/**
 * @brief Specification for a set of histograms.
 *
 * @author L. Nevay
 */

class HistogramDefSet
{
public:
  HistogramDefSet(const std::string&  branchNameIn,
                  const HistogramDef* baseDefinitionIn,
                  const std::set<ParticleSpec>& particlesSpecs = {},
                  const std::string&  particleSpecificationIn = "",
                  const std::string&  definitionLineIn = "");
  ~HistogramDefSet();

  static std::string AddPDGFilterToSelection(const ParticleSpec& particleSpec,
                                             const std::string& selection,
                                             const std::string& branchName);

  /// Remove a substring from a string.
  std::string RemoveSubString(const std::string& stringIn,
                              const std::string& wordToRemove) const;

  void ReplaceStringInVariable(const std::string& match,
                               const std::string& replacement);

  enum class samplertype {plane, cylindrical, spherical};
  void SetSamplerType(samplertype stIn) {samplerType = stIn;}

  friend std::ostream& operator<< (std::ostream &out, const HistogramDefSet& s);

  std::string   branchName;
  HistogramDef* baseDefinition;
  std::map<ParticleSpec, HistogramDef*> definitions;
  std::vector<HistogramDef*>            definitionsV; ///< Vector version for easy iteration.
  bool          dynamicallyStoreIons;
  bool          dynamicallyStoreParticles; ///< Dynamically store all non-ions.

  enum class writewhat {all, particles, ions, topN, topNParticles, topNIons};
  writewhat     what;
  int           topN;

  std::string   definitionLine; ///< Original definition line purely for print out.
  samplertype samplerType;
};

#endif
