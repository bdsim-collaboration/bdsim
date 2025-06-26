/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2024.

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
#include "BDSBunchUserFile.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSGlobalConstants.hh"
#include "BDSIonDefinition.hh"
#include "BDSParticleDefinition.hh"
#include "BDSUtilities.hh"
#include "BDSWarning.hh"

#include "parser/beam.h"

#include "G4IonTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4String.hh"

#ifdef USE_GZSTREAM
#include "src-external/gzstream/gzstream.h"
#endif

#include <algorithm>
#include <fstream>
#include <regex>
#include <set>
#include <string>
#include <sstream>
#include <vector>

template <class T>
BDSBunchUserFile<T>::BDSBunchUserFile():
  BDSBunchFileBased("userfile"),
  nlinesIgnore(0),
  nlinesSkip(0),
  nLinesValidData(0),
  particleMass(0),
  lineCounter(0),
  printedOutFirstTime(false),
  anEnergyCoordinateInUse(false),
  changingParticleType(false),
  endOfFileReached(false),
  matchDistrFileLength(false)
{
  ffact = BDSGlobalConstants::Instance()->FFact();
  comment = std::regex("^\\s*\\#|\\!.*");
}

template<class T>
void BDSBunchUserFile<T>::CheckParameters()
{
  BDSBunch::CheckParameters();
  if (distrFile.empty())
    {throw BDSException("BDSBunchUserFile::CheckParameters", "No input file specified for userfile distribution");}
}

template<class T>
BDSBunchUserFile<T>::~BDSBunchUserFile()
{
  CloseBunchFile();
}

template<class T>
void BDSBunchUserFile<T>::OpenBunchFile()
{
  endOfFileReached = false;
  if (!printedOutFirstTime)
    {
      G4cout << "BDSBunchUserFile::OpenBunchFile> opening " << distrFilePath << G4endl;
      printedOutFirstTime = true;
    }
  lineCounter = 0;
  InputBunchFile.open(distrFilePath);
  if (!InputBunchFile.good())
    {throw BDSException("BDSBunchUserFile::OpenBunchFile>", "Cannot open bunch file " + distrFilePath);}
}

template<class T>
void BDSBunchUserFile<T>::CloseBunchFile()
{
  InputBunchFile.clear(); // igzstream doesn't reset eof flags when closing - do manually
  InputBunchFile.close();
  endOfFileReached = true;
}

template<class T>
void BDSBunchUserFile<T>::ParseFileFormat()
{
  struct BDSBunchUserFile::Doublet sd;

  std::regex colons(":");
  std::vector<std::string> results;
  std::sregex_token_iterator iter(bunchFormat.begin(), bunchFormat.end(), colons, -1);
  std::sregex_token_iterator end;
  for (; iter != end; ++iter)
    {
      std::string res = (*iter).str();
      results.push_back(res);
    }
  for (auto const& token : results)
    {
      G4String vari = "";
      G4String rest = "";
      if(token.substr(0,1)=="E" || token.substr(0,1)=="P")
        {
          anEnergyCoordinateInUse = true;
          if (token.substr(0,2)=="Ek")
            {//Kinetic energy (longer name).
              vari = token.substr(0,2);
              rest = token.substr(2);
            }
          else
            {
              vari = token.substr(0,1);
              rest = token.substr(1);
            }
          CheckAndParseUnits(vari, rest, BDS::ParseEnergyUnit);
        }
      else if (token.substr(0,1)=="t")
        {
          vari = token.substr(0, 1);
          rest = token.substr(1);
          CheckAndParseUnits(vari, rest, BDS::ParseTimeUnit);
        }
      else if ( ( token.substr(0,1)=="x" && token.substr(1,1)!="p" ) ||
                ( token.substr(0,1)=="y" && token.substr(1,1)!="p" ) ||
                ( token.substr(0,1)=="z" && token.substr(1,1)!="p" ) )
        {
          vari = token.substr(0,1);
          rest = token.substr(1);
          CheckAndParseUnits(vari, rest, BDS::ParseLengthUnit);
        }
      else if ( (token.substr(0,2)=="xp") ||
                (token.substr(0,2)=="yp") ||
                (token.substr(0,2)=="zp") )
        {
          vari = token.substr(0,2);
          rest = token.substr(2);
          CheckAndParseUnits(vari, rest, BDS::ParseAngleUnit);
        }
      else if (token.substr(0, 1) == "S")
        {
          vari = token.substr(0, 1);
          rest = token.substr(1);
          CheckAndParseUnits(vari, rest, BDS::ParseLengthUnit);
          useCurvilinear = true;
        }
      else if(token.substr(0,5)=="pdgid")
        {
          changingParticleType = true;
          sd.name="pdgid";
          sd.unit=1;
          fields.push_back(sd);
        }
      else if(token.substr(0,1)=="w")
        {
          sd.name="weight";
          sd.unit=1;
          fields.push_back(sd);
        }
      else if (token.substr(0,1)=="-")
        {
          // skip
          sd.name="skip";
          sd.unit=1;
          fields.push_back(sd);
        }
      else
        {
          G4String message = "Cannot determine bunch data format. Failed at token: " + token;
          throw BDSException(__METHOD_NAME__, message);
        }
    }
  
  std::set<G4String> energyKeys = {"E", "Ek", "P"};
  CheckConflictingParameters(energyKeys);
  
  std::set<G4String> zKeys = {"z", "S"};
  CheckConflictingParameters(zKeys);
}

template <typename T>
void BDSBunchUserFile<T>::CheckConflictingParameters(const std::set<G4String>& s) const
{
  auto inSet  = [=](const G4String& v){return s.find(v) != s.end();};
  auto inSetD = [=](const BDSBunchUserFile::Doublet& v){return inSet(v.name);};
  int  count  = std::count_if(fields.begin(), fields.end(), inSetD);
  if (count > 1)
    {
      G4String message = "More than one of the following set in user file columns (\"distrFileFormat\") ";
      for (const auto& st : s)
        {
          message += st;
          message += ", ";
        }
      message += "\nPossibly conflicting information. Ensure only one by skipping others with \"-\" symbol";
      throw BDSException("BDSBunchUserFile::CheckConflictingParameters>", message);
    }
}

template <typename T>
template <typename U>
void BDSBunchUserFile<T>::CheckAndParseUnits(const G4String& uName,
                                             const G4String& rest,
                                             U unitParser)
{
  struct BDSBunchUserFile::Doublet sd;
  if (rest.empty())
    {
      sd.name = uName;
      sd.unit = 1.0;
      fields.push_back(sd);
      return;
    }
  
  G4int pos1 = (G4int)rest.find("[");
  G4int pos2 = (G4int)rest.find("]");
  if (pos1 < 0 || pos2 < 0)
    {
      G4String message = "Missing bracket [] in units of \"distrFileFormat\"\n";
      message += "variable : \"" + uName + "\" and unit \"" + rest + "\"";
      throw BDSException("BDSBunchUserFile::CheckAndParseUnits>", message);
    }
  else
    {
      G4String fmt = rest.substr(pos1 + 1, pos2 - 1);
      sd.name = uName;
      sd.unit = unitParser(fmt);
      fields.push_back(sd);
    }
}

template<class T>
void BDSBunchUserFile<T>::skip(std::stringstream& ss, G4int nValues)
{
  G4double dummyValue;
  for (G4int i = 0; i < nValues; i++)
    {ReadValue(ss, dummyValue);}
}

template<class T>
void BDSBunchUserFile<T>::SkipNLinesIgnoreIntoFile(G4bool usualPrintOut)
{
  if (BDS::IsFinite(nlinesIgnore))
    {
      if (usualPrintOut)
        {G4cout << "BDSBunchUserFile> ignoring " << nlinesIgnore << " lines" << G4endl;}
      std::string line;
      for (G4int i = 0; i < (G4int)nlinesIgnore; i++)
        {
          std::getline(InputBunchFile, line);
          lineCounter++;
          // we must check explicitly if we've gone past the end of the file
          if (InputBunchFile.eof())
            {
              G4String msg = "end of file reached after line " + std::to_string(lineCounter);
              msg += " before nlinesIgnore ("+std::to_string(nlinesIgnore)+") was reached.";
              throw BDSException("BDSBunchUserFile::SkipNLinesIgnoreIntoFile>", msg);
            }
        }
    }
}

template<class T>
void BDSBunchUserFile<T>::SkipNLinesSkip(G4bool usualPrintOut)
{
  if (BDS::IsFinite(nlinesSkip))
    {
      if (usualPrintOut)
        {G4cout << "BDSBunchUserFile> skipping " << nlinesSkip << " valid lines" << G4endl;}
      
      // We can read into the file safely without checking eof() because we know from earlier
      // counting of the number of valid lines in the file that nlinesSkip is not beyond the
      // end of the file (including nlinesIgnore).
      std::string line;
      G4int nLinesValidRead = 0;
      while (nLinesValidRead < nlinesSkip)
        {
          std::getline(InputBunchFile, line);
          if (SkippableLine(line))
            {continue;}
          nLinesValidRead++;
        }
      IncrementNEventsInFileSkipped((unsigned long long int)nlinesSkip);
    }
}

template<class T>
void BDSBunchUserFile<T>::SetOptions(const BDSParticleDefinition* beamParticle,
                                     const GMAD::Beam& beam,
                                     const BDSBunchType& distrType,
                                     G4Transform3D beamlineTransformIn,
                                     const G4double beamlineSIn)
{
  BDSBunchFileBased::SetOptions(beamParticle, beam, distrType, beamlineTransformIn, beamlineSIn);
  particleMass = beamParticle->Mass();
  distrFile     = beam.distrFile;
  distrFilePath = BDS::GetFullPath(beam.distrFile);
  bunchFormat   = beam.distrFileFormat;
  nlinesIgnore  = (G4long)beam.nlinesIgnore;
  nlinesSkip    = (G4long)beam.nlinesSkip;
  matchDistrFileLength = beam.distrFileMatchLength;
  ParseFileFormat();
}

template<class T>
G4bool BDSBunchUserFile<T>::SkippableLine(const std::string& line) const
{
  return std::all_of(line.begin(), line.end(), isspace) || std::regex_search(line, comment);
}

template<class T>
G4long BDSBunchUserFile<T>::CountNLinesValidDataInFile()
{
  OpenBunchFile();
  SkipNLinesIgnoreIntoFile(false);

  std::string line;
  G4long nLinesValid = 0;
  while ( std::getline(InputBunchFile, line) )
    {
      if (SkippableLine(line))
        {continue;} // don't count it
      nLinesValid++;
    }
  CloseBunchFile();
  return nLinesValid;
}

template<class T>
void BDSBunchUserFile<T>::Initialise()
{
  nLinesValidData = CountNLinesValidDataInFile();

  if (nLinesValidData < nlinesSkip)
    {
      G4String msg = "nlinesSkip is greater than the number of valid lines (" + std::to_string(nLinesValidData);
      msg += ") in the user file \"" + distrFilePath + "\"";
      throw BDSException("BDSBunchUserFile::Initialise>", msg);
    }

  auto g = BDSGlobalConstants::Instance();
  G4bool nGenerateHasBeenSet = g->NGenerateSet();
  G4int nEventsPerLoop = (G4int)(nLinesValidData - nlinesSkip);
  nEventsInFile = nEventsPerLoop;
  G4int nAvailable     = nEventsPerLoop * distrFileLoopNTimes;
  G4int nGenerate = g->NGenerate();
  if (matchDistrFileLength)
    {
      if (!nGenerateHasBeenSet)
        {
          g->SetNumberToGenerate(nAvailable);
          G4cout << "BDSBunchUserFile::Initialise> distrFileMatchLength is true -> simulating " << nEventsPerLoop << " events";
          if (distrFileLoopNTimes > 1)
            {G4cout << " " << distrFileLoopNTimes << " times";}
          G4cout << G4endl;
          if (g->Recreate())
            {// have to do this now before the primary generator action is called already in the run
              // we need to start the run with a number of events so we need to calculate here
              G4int nEventsRemaining = nAvailable - g->StartFromEvent();
              g->SetNumberToGenerate(nEventsRemaining);
              G4cout << "BDSBunchUserFile::Initialise> distrFileMatchLength + recreation -> simulate the "
                     << nEventsRemaining << " lines left given startFromEvent including possible looping" << G4endl;
            }
        }
      else
        {// e.g. if recreating a lower number of events - match is on; but ngenerate is lower - must obey
          G4cout << "BDSBunchUserFile::Initialise> matchDistrFileLength has been requested "
                 << "but ngenerate has been specified -> use ngenerate" << G4endl;
          // note we don't need to take care of a recreation offset - this is done later in primary generator action
          if (nGenerate > nAvailable)
            {
              G4String msg = "ngenerate (" + std::to_string(nGenerate) + ") is greater than the number of valid lines (";
              msg += std::to_string(nLinesValidData) + ") and distrFileMatchLength is on.\nChange ngenerate to <= # lines";
              msg += ", or don't specifcy ngenerate.\n";
              msg += "This includes nlinesSkip.";
              throw BDSException("BDSBunchUserFile::Initialise>", msg);
            }
        }
    }
  else
    {
      if ((nGenerate > nEventsPerLoop) && !distrFileLoop)
        {
          G4String msg = "ngenerate (" + std::to_string(nGenerate) + ") is greater than the number of valid lines (";
          msg += std::to_string(nLinesValidData) + ") but distrFileLoop is false in the beam command";
          throw BDSException("BDSBunchUserFile::Initialise>", msg);
        }
    }
  OpenBunchFile();
  SkipNLinesIgnoreIntoFile();
  SkipNLinesSkip();
}
  
template<class T>
void BDSBunchUserFile<T>::EndOfFileAction()
{
  // If the end of the file is reached go back to the beginning of the file.
  // this re-reads the same file again - must always print warning
  G4cout << "BDSBunchUserFile> End of file reached." << G4endl;
  CloseBunchFile();
  if (distrFileLoop)
    {
      G4cout << "BDSBunchUserFile> Returning to beginning of file (including nlinesIgnore & nlinesSkip) for next event." << G4endl;
      OpenBunchFile();
      SkipNLinesIgnoreIntoFile(false);
      SkipNLinesSkip(false);
    }
  else
    {throw BDSException(__METHOD_NAME__, "distrFileLoop off but requesting another set of coordinates.");}
}

template<class T>
void BDSBunchUserFile<T>::RecreateAdvanceToEvent(G4int eventOffset)
{
  BDSBunch::RecreateAdvanceToEvent(eventOffset);
  G4cout << "BDSBunchUserFile::RecreateAdvanceToEvent> Advancing file to event: " << eventOffset << G4endl;
  G4int nEventsPerLoop   = nLinesValidData - nlinesSkip;
  G4int nAvailable       = nEventsPerLoop * distrFileLoopNTimes;
  G4int nEventsRemaining = nAvailable - eventOffset;
  if (eventOffset > nEventsPerLoop)
    {
      if (distrFileLoop)
        {
          // we're recreating a file where we looped on the data - don't repeatedly read - just read up to
          // the right point as if we'd looped the file
          G4int nToRoll = eventOffset % nEventsPerLoop;
          eventOffset = nToRoll;
        }
      else
        {
          G4String msg = "eventOffset (" + std::to_string(eventOffset) + ") is greater than the number of valid data lines in this file.\n";
          msg += "This includes nlinesSkip.";
          throw BDSException("BDSBunchUserFile::RecreateAdvanceToEvent>", msg);
        }
    }
  
  G4int nGenerate = BDSGlobalConstants::Instance()->NGenerate();
  if (nGenerate > nEventsRemaining && !distrFileLoop)
    {
      G4String msg = "ngenerate (" + std::to_string(nGenerate) + ") requested in recreate mode is greater than number\n";
      msg += "of remaining valid lines in file (" + std::to_string(nEventsRemaining) + ") and distrFileLoop is turned off.";
      throw BDSException("BDSBunchUserFile>", msg);
    }
  // note we cannot update ngenerate here as we're already being called from the primary
  // generator action in the start of the event after BeamOn(nEvents) has been called
  // therefore this adjustment for recreation + match is done earlier in this class

  // we should now be completely safe to read into the file ignoring comment lines and
  // without checking eof()
  std::string line;
  for (G4int i = 0; i < eventOffset; i++)
    {
      std::getline(InputBunchFile, line);
      if (SkippableLine(line))
        {continue;}
      lineCounter++;
    }
}

template<class T>
BDSParticleCoordsFullGlobal BDSBunchUserFile<T>::GetNextParticleValid(G4int /*maxTries*/)
{
  // no looping - just read one particle from file
  return GetNextParticle();
}

template<class T>
BDSParticleCoordsFull BDSBunchUserFile<T>::GetNextParticleLocal()
{
  if (InputBunchFile.eof() || InputBunchFile.fail())
    {EndOfFileAction();}

  G4double E = 0, Ek = 0, P = 0, x = 0, y = 0, z = 0, xp = 0, yp = 0, zp = 0, t = 0;
  G4double weight = 1;
  G4int type = 0;
  
  G4bool zpdef = false; //keeps record whether zp has been read from file
  G4bool tdef  = false; //keeps record whether t has been read from file

  // flag whether we're going to update the particle definition
  G4bool updateParticleDefinition = false;

  // read a whole line at a time for safety - no partially read lines
  std::string line;
  std::getline(InputBunchFile, line);
  lineCounter++;
  
  // skip empty lines and comment lines (starting with # or !)
  G4bool lineIsBad = true;
  while (lineIsBad)
    {
      if (SkippableLine(line))
        {
          if (InputBunchFile.eof() || InputBunchFile.fail())
            {EndOfFileAction();}
          else
            {
              std::getline(InputBunchFile, line);
              lineCounter++;
              continue;
            }
        }
      else
        {lineIsBad = false;}
    }
  
  // no check the line has the right number of 'words' in it ->
  // split line on white space - doesn't inspect words themselves
  // checks number of words, ie number of columns is correct
  std::vector<std::string> results;
  std::regex wspace("\\s+"); // any whitepsace
  // -1 here makes it point to the suffix, ie the word rather than the wspace
  std::sregex_token_iterator iter(line.begin(), line.end(), wspace, -1);
  std::sregex_token_iterator end;
  for (; iter != end; ++iter)
    {
      std::string res = (*iter).str();
      results.push_back(res);
    }
  
  if (results.size() < fields.size())
    {// ensure enough columns
      std::string message = "Invalid line at line " + std::to_string(lineCounter) +
        ".  Expected " + std::to_string(fields.size()) +
        " columns , but got " + std::to_string(results.size()) +
        ".";
      throw BDSException(__METHOD_NAME__, message);
    }
  
  std::stringstream ss(line);
  for (auto it=fields.begin();it!=fields.end();it++)
    {
      if(it->name=="skip")
        {double dummy; ReadValue(ss, dummy);}
      else if(it->name=="Ek")
        {ReadValue(ss, Ek); Ek *= (CLHEP::GeV * it->unit);}
      else if(it->name=="E")
        {ReadValue(ss, E); E *= (CLHEP::GeV * it->unit);}
      else if(it->name=="P")
        {ReadValue(ss, P); P *= (CLHEP::GeV * it->unit);}
      else if(it->name=="t")
        {ReadValue(ss, t); t *= (CLHEP::s * it->unit); tdef = true;}
      else if(it->name=="x")
        {ReadValue(ss, x); x *= (CLHEP::m * it->unit);}
      else if(it->name=="y")
        {ReadValue(ss, y); y *= (CLHEP::m * it->unit);}
      else if(it->name=="z")
        {ReadValue(ss, z); z *= (CLHEP::m * it->unit);}
      else if(it->name=="xp") {ReadValue(ss, xp); xp *= ( CLHEP::radian * it->unit );}
      else if(it->name=="yp") {ReadValue(ss, yp); yp *= ( CLHEP::radian * it->unit );}
      else if(it->name=="zp") {ReadValue(ss, zp); zp *= ( CLHEP::radian * it->unit ); zpdef = true;}
      else if(it->name=="pdgid")
        {// particle type
          ReadValue(ss, type);
          updateParticleDefinition = true; // update particle definition after finished reading line
          delete particleDefinition;
          particleDefinition = nullptr;
        }
      else if (it->name == "S")
        {
          ReadValue(ss, z);
          z *= CLHEP::m * it->unit;
        }
      else if(it->name=="weight")
        {ReadValue(ss, weight);}
    }

  // coordinate checks
  if (anEnergyCoordinateInUse)
    {
      if (particleDefinition)
        {// no checking here that only one variable is set - must ensure in this class
          particleDefinition->SetEnergies(E, Ek, P);
          E = particleDefinition->TotalEnergy();
        }
      else // we must update this call of this function to ensure valid particle definition
        {updateParticleDefinition = true;}
    }
  else // If energy isn't specified, use the central beam energy
    {E = E0;}
  
  // compute zp from xp and yp if it hasn't been read from file
  xp += Xp0;
  yp += Yp0;
  if (!zpdef)
    {zp = CalculateZp(xp,yp,1);}
  // compute t from z if it hasn't been read from file
  if (!tdef)
    {t=0;} //TBC
  
  if (updateParticleDefinition || changingParticleType)
    {
      // type is an int so FindParticle(int) is used here
      G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
      G4ParticleDefinition* g4ParticleDef = nullptr;
      BDSIonDefinition* ionDef = nullptr;
      if (type < 1e9) // not a pdg ion
        {g4ParticleDef = particleTable->FindParticle(type);}
      else
        {
          G4IonTable* ionTable = particleTable->GetIonTable();
          G4int ionA, ionZ, ionLevel;
          G4double ionE;
          G4IonTable::GetNucleusByEncoding(type, ionZ, ionA, ionE, ionLevel);
          ionDef = new BDSIonDefinition(ionA, ionZ, ionZ);
          g4ParticleDef = ionTable->GetIon(ionDef->Z(), ionDef->A(), ionDef->ExcitationEnergy());
        }
      
      if (!g4ParticleDef)
        {throw BDSException("BDSBunchUserFile> Particle \"" + std::to_string(type) + "\" not found");}
      // Wrap in our class that calculates momentum and kinetic energy.
      // Requires that one of E, Ek, P be non-zero (only one).
      delete particleDefinition;
      try
        {
          particleDefinition = new BDSParticleDefinition(g4ParticleDef, E, Ek, P, ffact, ionDef);
          E = particleDefinition->TotalEnergy();
          particleDefinitionHasBeenUpdated = true;
        }
      catch (const BDSException& e)
        {// if we throw an exception the object is invalid for the delete on the next loop
          particleDefinition = nullptr; // reset back to nullptr for safe delete
          throw e;                      // rethrow
        }
    }

  return BDSParticleCoordsFull(X0+x,Y0+y,Z0+z,xp,yp,zp,t,z,E,weight);
}

template <class T>
template <typename Type>
void BDSBunchUserFile<T>::ReadValue(std::stringstream& stream, Type& value)
{
  stream >> value;
}

template class BDSBunchUserFile<std::ifstream>;

#ifdef USE_GZSTREAM
template class BDSBunchUserFile<igzstream>;
#endif

namespace BDS
{
G4double ParseEnergyUnit(const G4String& fmt)
{
  G4double unit= 1.0;
  if (fmt == "TeV")
    {unit = 1e3;}
  else if (fmt == "GeV")
    {unit = 1.0 ;}
  else if (fmt == "MeV")
    {unit = 1e-3;}
  else if (fmt == "KeV")
    {unit = 1e-6;}
  else if (fmt == "eV")
    {unit = 1e-9;}
  else
    {throw BDSException(__METHOD_NAME__, "Unrecognised energy unit! " +fmt);}
  return unit;
}

G4double ParseLengthUnit(const G4String& fmt)
{
  G4double unit = 1.0;
  if (fmt == "m")
    {unit = 1;}
  else if (fmt == "cm")
    {unit = 1e-2;}
  else if (fmt == "mm")
    {unit = 1e-3;}
  else if (fmt == "mum" || fmt == "um")
    {unit = 1e-6;}
  else if (fmt == "nm")
    {unit = 1e-9;}
  else
    {throw BDSException(__METHOD_NAME__, "Unrecognised length unit! " + fmt);}
  return unit;
}

G4double ParseAngleUnit(const G4String& fmt)
{
  G4double unit = 1.0;
  if (fmt == "rad")
    {unit = 1;}
  else if (fmt == "mrad")
    {unit = 1e-3;}
  else if (fmt == "murad" || fmt == "urad")
    {unit = 1e-6;}
  else if (fmt == "nrad")
    {unit=1e-9;}
  else
    {throw BDSException(__METHOD_NAME__, "Unrecognised angle unit! " + fmt);}
  return unit;
}

G4double ParseTimeUnit(const G4String& fmt)
{
  G4double unit = 1.0;
  if (fmt == "s")
    {unit = 1;}
  else if (fmt == "ms")
    {unit = 1.e-3;}
  else if (fmt == "mus" || fmt == "us")
    {unit = 1e-6;}
  else if (fmt == "ns")
    {unit = 1e-9;}
  else if (fmt == "mm/c")
    {unit = (CLHEP::mm/CLHEP::c_light)/CLHEP::s;}
  else if (fmt == "nm/c")
    {unit = (CLHEP::nm/CLHEP::c_light)/CLHEP::s;}
  else
    {throw BDSException(__METHOD_NAME__, "Unrecognised time unit! " + fmt);}
  return unit;
}

}
