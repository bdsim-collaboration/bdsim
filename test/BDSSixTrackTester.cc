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
#include "BDSBunchSixTrackLink.hh"
#include "BDSException.hh"
#include "BDSGlobalConstants.hh"
#include "BDSIMLink.hh"
#include "BDSIonDefinition.hh"
#include "BDSParticleCoordsFull.hh"
#include "BDSParticleDefinition.hh"
#include "BDSParticleExternal.hh"
#include "BDSPhysicsUtilities.hh"
#include "BDSVisManager.hh"

#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4Types.hh"

#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>
#include <utility>

struct Collimator
{
  std::string name;
  std::string materialName;
  double length;
  double aperture;
  double rotation;
  double xOffset;
  double yOffset;
};

std::vector<Collimator> ReadFile(const std::string& filename);
void ParticleClassTests();
void BunchTests();
void AddParticle(BDSBunchSixTrackLink* stp);
void Summarise(BDSIMLink* bds);

int main(int /*argc2*/, char** /*argv2*/)
{
  std::vector<std::string> arguments = {"theprogramnamenormally",
                                        "--file=lhccrystals.gmad",
                                        "--output=none",
                                        "--batch",
                                        "--vis_debug"};
  std::vector<char*> argv;
  argv.reserve(arguments.size());
  for (const auto& arg : arguments)
    {argv.push_back((char*) arg.data());}
  argv.push_back(nullptr);
  
  BDSBunchSixTrackLink* stp = new BDSBunchSixTrackLink();
  BDSIMLink* bds = new BDSIMLink(stp);
  try
    {    
      bds->Initialise((int) argv.size() - 1, argv.data(), true, 100);
      
      std::vector<Collimator> collimators;
      collimators = ReadFile("somecollimators.dat");
      
      for (const auto& c : collimators)
	{
	  bds->AddLinkCollimatorJaw(c.name,
				    c.materialName,
				    c.length,
				    c.aperture,
				    c.aperture,
				    c.rotation,
				    c.xOffset,
				    c.yOffset);
	}
      
      for (const auto& collimator : collimators)
	{
	  bds->ClearSamplerHits();
	  stp->ClearParticles();
	  
	  AddParticle(stp);
	  
	  bds->SelectLinkElement(collimator.name, true);
	  bds->BeamOn((G4int) stp->Size());
	  Summarise(bds);
	}
      
      // test accessing information after construction
      std::cout << "Length of element #6 " << bds->GetChordLengthOfLinkElement(5) << " mm " << std::endl;
      std::cout << "Length of element #6 " << bds->GetChordLengthOfLinkElement("TDI.4R8.B2") << " mm " << std::endl;

      // purely for code coverage
      ParticleClassTests();
      
      BunchTests();
      
#ifdef VISLINK
      BDSVisManager visManager = BDSVisManager(BDSGlobalConstants::Instance()->VisMacroFileName(),
					       BDSGlobalConstants::Instance()->Geant4MacroFileName());
      visManager.StartSession((int)argv.size() - 1, argv.data());
#endif
    }
  catch (const BDSException& exception)
    {
      std::cerr << std::endl << exception.what() << std::endl;
      delete bds;
      exit(1);
    }
  catch (const std::exception& exception)
    {
      std::cerr << std::endl << exception.what() << std::endl;
      delete bds;
      exit(1);
    }
  delete bds;
  return 0;
}

std::vector<Collimator> ReadFile(const std::string& filename)
{
  std::ifstream file;
  file.open(filename);
  std::vector<Collimator> result;
  if (file.fail())
  {throw std::invalid_argument("No such file " + filename);}
  std::string line;
  std::string dummy, cn, mn;
  double le, ap, to, xo, yo;
  while (std::getline(file, line))
    {
      std::stringstream liness(line);
      liness >> dummy >> cn >> mn >> le >> ap >> to >> xo >> yo;
      Collimator co = {cn,mn,le,ap,to,xo,yo};
      result.push_back(co);
    }

  return result;
}

void ParticleClassTests()
{
  G4double totalEnergy = 123*CLHEP::GeV;
  BDSParticleCoordsFull coords = BDSParticleCoordsFull(1e-4, -1e-3, 0,
                                                       0, 0, 1,
                                                       0, 0,
                                                       totalEnergy,
                                                       1);
  
  long long int pdgID = 2212;
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition* particleDef = particleTable->FindParticle((int)pdgID);
  BDSParticleDefinition* particleDefinition = new BDSParticleDefinition(particleDef, totalEnergy, 0, 0, 1, nullptr);
  auto aParticle1 = new BDSParticleExternal(particleDefinition, coords, 2, 1);
  
  // test copy constructor
  auto aParticle2 = new BDSParticleExternal(*aParticle1);
  
  // test move-assigment constructor
  *aParticle2 = std::move(*aParticle1);
  
  // test move constructor
  BDSParticleExternal aParticle3 = std::move(*aParticle2);
  
  delete aParticle2;
  delete aParticle1;
}

void BunchTests()
{
  BDSBunchSixTrackLink* stp = new BDSBunchSixTrackLink();
  stp->ClearParticles();
  delete stp;
}

void AddParticle(BDSBunchSixTrackLink* stp)
{
  G4double totalEnergy = 123*CLHEP::GeV;
  BDSParticleCoordsFull coords = BDSParticleCoordsFull(1e-3, 1e-3, 0,
                                                       0, 0, 1,
                                                       0, 0,
                                                       totalEnergy,
                                                       1);

  long long int pdgID = 2212;
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition* particleDef = particleTable->FindParticle((int)pdgID);
  if (!particleDef)
    {throw BDSException("BDSBunchUserFile> Particle \"" + std::to_string(pdgID) + "\" not found");}
  
  try
    {
      BDSIonDefinition* ionDef = nullptr;
      if (BDS::IsIon(particleDef))
	{
	  G4int a = 208;
	  G4int z = 82;
	  G4double q = 82 * CLHEP::eplus;
	  ionDef = new BDSIonDefinition(a,z,q);
	}
      
      // Wrap in our class that calculates momentum and kinetic energy.
      // Requires that one of E, Ek, P be non-zero (only one).
      BDSParticleDefinition* particleDefinition = nullptr;
      particleDefinition = new BDSParticleDefinition(particleDef, totalEnergy, 0, 0, 1, ionDef);
      stp->AddParticle(particleDefinition, coords, 0, 0);
      delete ionDef; // no longer required
    }
  catch (const BDSException& e)
    {// if we throw an exception the object is invalid for the delete on the next loop
      return;
    }
}

void Summarise(BDSIMLink* bds)
{
  const BDSHitsCollectionSamplerLink* hits = bds->SamplerHits();
  for (G4int i = 0; i < (G4int)hits->entries(); i++)
    {
      BDSHitSamplerLink* hit = (*hits)[i];
      G4cout << hit->coords << G4endl;
    }
}
