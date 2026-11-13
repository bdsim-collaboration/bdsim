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
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSHistBinMapper.hh"
#include "BDSScorerConversionLoader.hh"
#include "BDSPSPopulationScaled.hh"
#include "BDSUtilities.hh"

#include "G4IonTable.hh"
#include "G4PhysicsVector.hh"
#include "G4String.hh"
#include "G4Types.hh"
#include "G4VSolid.hh"
#include "G4VPVParameterisation.hh"

#include <cmath>
#include <fstream>
#include <iterator>
#include <string>
#include <dirent.h>

#ifdef USE_GZSTREAM
#include "src-external/gzstream/gzstream.h"
#endif

BDSPSPopulationScaled::BDSPSPopulationScaled(const G4String& scorerName,
					     G4int           depth):
  G4VPrimitiveScorer(scorerName, depth),
  HCID(-1),
  EvtMap(nullptr)
{;}

BDSPSPopulationScaled::BDSPSPopulationScaled(const G4String& scorerName,
                                             const G4String& pathname,
                                             G4int           depth):
  BDSPSPopulationScaled(scorerName, depth)
{
  G4String directory = BDS::GetFullPath(pathname);
  if (directory.back() != '/')
    {directory += '/';}

  std::vector<G4String> dirsAngle;
  std::vector<G4String> filesParticle;
  
  dirsAngle = LoadDirectoryContents(directory);
  
  G4cout << "Scorer \"" << GetName() << "\" - adding conversionFiles from: " + directory << G4endl;
  
  for (const auto& dirnameAng : dirsAngle)
    {
      G4String dirAng = directory + dirnameAng;      
      filesParticle = LoadDirectoryContents(dirAng);
      
      BDSScorerConversionLoader<std::ifstream> loader;
      
      G4double ang = (G4double)std::stod(dirnameAng);
      angles.push_back(ang); // Store the angle for interpolation
      G4int angleIndex = (G4int)angles.size() - 1;
      
      std::vector<G4int> ionPIDs; // Store the ion particle ids vs. angle for interpolation
      
      for (const auto& filePDG : filesParticle)
        {
	  G4String filepathPDG = dirAng + '/' + filePDG;
	  G4int pid = (G4int)std::stoi(filePDG);
	  
	  if (filePDG.substr((filePDG.find_last_of(".") + 1)) == "gz" && BDS::FileExists(filepathPDG))
            {
#ifdef USE_GZSTREAM
	      BDSScorerConversionLoader<igzstream> loaderC;   
	      conversionFactors[angleIndex][pid] = loaderC.Load(filepathPDG, true);
#else
	      throw BDSException(__METHOD_NAME__, "Compressed file loading - but BDSIM not compiled with ZLIB.");
#endif
            }
	  else if (BDS::FileExists(filepathPDG))
            {conversionFactors[angleIndex][pid] = loader.Load(filepathPDG, true);}
	  
	  if (pid > 1e7)
            {ionPIDs.push_back(pid);}
        }
      ionParticleIDs[angleIndex] = ionPIDs;
    }
  
  G4cout << "Scorer \"" << GetName()  << " Loaded data for " << angles.size() << " angles: [";
  for (auto a : angles)
    {
      G4cout << a;
      if (a == angles.back())
	{break;}
      G4cout << ", ";
    }
  G4cout << "]*rad" << G4endl;
}

BDSPSPopulationScaled::~BDSPSPopulationScaled()
{
  for (const auto& af : conversionFactors)
    {
      for (auto cf : af.second)
        {delete cf.second;}
    }
}

void BDSPSPopulationScaled::Initialize(G4HCofThisEvent* HCE)
{
  EvtMap = new G4THitsMap<G4double>(detector->GetName(), GetName());
  if (HCID < 0)
    {HCID = GetCollectionID(0);}

  HCE->AddHitsCollection(HCID, EvtMap);
}

void BDSPSPopulationScaled::EndOfEvent(G4HCofThisEvent* /*HEC*/)
{
  fCellTrackLogger.clear();
}

void BDSPSPopulationScaled::clear()
{
  EvtMap->clear();
  fCellTrackLogger.clear();
}

G4bool BDSPSPopulationScaled::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{
  G4double stepLength = aStep->GetStepLength();
  
  if (!BDS::IsFinite(stepLength))
    {return false;}

  G4double radiationQuantity = 0;
  G4int index = GetIndex(aStep);
  G4TrackLogger& tlog = fCellTrackLogger[index];

  if (tlog.FirstEnterance(aStep->GetTrack()->GetTrackID()))
    {
      const G4VTouchable* touchable = aStep->GetPreStepPoint()->GetTouchable();
      auto rot = touchable->GetRotation();
      G4ThreeVector unitZ = G4ThreeVector(0, 0, 1);
      G4ThreeVector blmUnitZ = unitZ.transform(*rot);
      
      auto momDirection = aStep->GetPreStepPoint()->GetMomentumDirection();
      
      // The angle used for the lookup is sign-independent and spans the range 0 to pi/2
      G4double angle = std::fmod(std::abs(momDirection.angle(blmUnitZ)), CLHEP::pi);
      if (angle > CLHEP::pi/2.)
        {angle = CLHEP::pi - angle;}
      
      G4double kineticEnergy = aStep->GetPreStepPoint()->GetKineticEnergy();
      G4double weight = aStep->GetPreStepPoint()->GetWeight();
      G4double factor = GetConversionFactor(aStep->GetTrack()->GetDefinition()->GetPDGEncoding(),
                                            kineticEnergy,
                                            angle);
      radiationQuantity = weight * factor;
      
      EvtMap->add(index, radiationQuantity);
    }
  
  return true;
}

G4double BDSPSPopulationScaled::GetConversionFactor(G4int    particleID,
						    G4double kineticEnergy,
						    G4double angle) const
{
  G4double angleNearestIndex = NearestNeighbourAngleIndex(angles, angle);

  if (angleNearestIndex < 0)
    {return 0;}

  std::map<G4int, G4PhysicsVector*> conversionFactorsPart;
  auto searchAngle = conversionFactors.find(angleNearestIndex);
  if (searchAngle != conversionFactors.end())
    {conversionFactorsPart = searchAngle->second;}
  else
    {return 0;}
  
  if (particleID < 1e7)
    {
      auto search = conversionFactorsPart.find(particleID);
      if (search != conversionFactorsPart.end())
        {return search->second->Value(kineticEnergy);}
      else
        {return 0;}
    }
  else
    {
      // Get the nearest neighbour ion particle ID based on the ion Z
      G4int particleIDNearest = NearestNeighbourIonPID(ionParticleIDs.find(angleNearestIndex)->second, particleID);
      if (particleIDNearest < 0)
        {return 0;}

      // Get the ion Z in order to normalise the kinetic energy for table look-up
      G4double nearestIonZ = (G4double)GetZFromParticleID(particleID);
      
      auto searchIon = conversionFactorsPart.find(particleIDNearest);
      if (searchIon != conversionFactorsPart.end())
        {return searchIon->second->Value(kineticEnergy / nearestIonZ);}
      else
        {return 0;}
    }
}

std::vector<G4String> BDSPSPopulationScaled::LoadDirectoryContents(const G4String& dirname)
{
  std::vector<G4String> contents;
  
  struct dirent* entry = nullptr;
  DIR* dp = nullptr;

  dp = opendir(dirname.c_str());
  if (dp == nullptr)
    {throw BDSException(__METHOD_NAME__, "Cannot open directory " + dirname);}
  else
    {
      while ((entry = readdir(dp)) != nullptr)
        {
	  std::string name_string(entry->d_name);
	  if (name_string.front() != '.')
            {contents.emplace_back(name_string);}
        }
    }
    closedir(dp);
    return contents;
}

G4int BDSPSPopulationScaled::NearestNeighbourAngleIndex(const std::vector<G4double>& vec, G4double value) const
{
  if (vec.empty())
    {return -1;}

  G4double nearestNeighbourAngle;
  
  auto const it = std::upper_bound(vec.begin(), vec.end(), value) - 1;
  if (it == vec.end())
    {nearestNeighbourAngle = vec.back();}
  else
    {nearestNeighbourAngle = *it;}
  
  auto const itr = std::find(vec.begin(), vec.end(), nearestNeighbourAngle);
  auto index = (G4int) std::distance(vec.begin(), itr);

  return index;
}

G4int BDSPSPopulationScaled::NearestNeighbourIonPID(const std::vector<G4int>& vec, G4int value) const
{
  // Make a vector of the ion Z for all ion particles
  std::vector<G4int> vecZ;
  for (auto pid: vec)
    {vecZ.push_back(GetZFromParticleID(pid));}
  
  if (vecZ.empty())
    {return -1;}

  G4int nearestNeighbourZ;
  // Perform nearest neighbour interpolation on the ion Z
  auto const it = std::upper_bound(vecZ.begin(), vecZ.end(), GetZFromParticleID(value)) - 1;
  if (it == vecZ.end())
    {nearestNeighbourZ = vecZ.back();}
  else
    {nearestNeighbourZ = *it;}

  // Find the index of the nearest neighbour Z - used to look up the full particle ID
  auto const itr = std::find(vecZ.begin(), vecZ.end(), nearestNeighbourZ);
  int index = (int)std::distance(vecZ.begin(), itr);
  
  return vec.at(index);
}

G4int BDSPSPopulationScaled::GetZFromParticleID(G4int particleID) const
{
  G4int Z = 0;
  G4int A = 0;
  G4double E = 0.0;
  G4int lvl = 0;
  G4IonTable::GetNucleusByEncoding(particleID, Z, A, E, lvl);
  return Z;
}

void BDSPSPopulationScaled::PrintAll()
{
  G4cout << " MultiFunctionalDet  " << detector->GetName() << G4endl;
  G4cout << " PrimitiveScorer " << GetName() << G4endl;
  G4cout << " Number of entries " << EvtMap->entries() << G4endl;
  auto itr = EvtMap->GetMap()->begin();
  for (; itr != EvtMap->GetMap()->end(); itr++)
    {
      G4cout << "  copy no.: " << itr->first
	     << "  population: " << *(itr->second) / GetUnitValue()
	     << " [quantity]"
	     << G4endl;
    }
}
