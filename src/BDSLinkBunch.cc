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
#include "BDSLinkBunch.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSIonDefinition.hh"
#include "BDSParticleDefinition.hh"

#include "globals.hh"
#include "G4IonTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4String.hh"

#include <vector>

BDSLinkBunch::BDSLinkBunch():
  BDSBunch("sixtracklink"),
  currentIndex(0),
  currentExternalParticleID(0),
  currentExternalParentID(0),
  currentParticleDefinition(nullptr),
  size(0)
{;}

BDSLinkBunch::~BDSLinkBunch()
{;}

BDSParticleCoordsFull BDSLinkBunch::GetNextParticleLocal()
{
  if (currentIndex >= size)
    {
      G4cout << __METHOD_NAME__ << "looping to start of bunch" << G4endl;
      currentIndex = 0;
    }

  G4int ci = currentIndex;
  currentIndex++;
  
  auto particle = particles[ci];
  currentParticleDefinition = particle->particleDefinition;
  particleDefinitionHasBeenUpdated = true;
  //UpdateGeant4ParticleDefinition(particleDefinition->PDGID()); // TBC
  UpdateIonDefinition();
  
  currentExternalParticleID = particle->externalParticleID;
  currentExternalParentID   = particle->externalParentID;
  
  return particle->coords;
}

void BDSLinkBunch::AddParticle(BDSParticleDefinition* particleDefinitionIn,
                               const BDSParticleCoordsFull& coordsIn,
                               int   externalParticleID,
                               int   externalParentID)
{
  particles.emplace_back(new BDSParticleExternal(particleDefinitionIn, coordsIn, externalParticleID, externalParentID));
  size = (G4int)particles.size();
}

void BDSLinkBunch::ClearParticles()
{
  currentIndex = 0;
  size = 0;
  for (auto p : particles)
    {delete p;}
  particles.clear();
}

void BDSLinkBunch::UpdateGeant4ParticleDefinition(G4int pdgID)
{
  G4ParticleDefinition* newParticleDefinition = nullptr;
  if (!particleDefinition->IsAnIon())
    {
      G4ParticleTable::G4PTblEncodingDictionary* encoding = G4ParticleTable::fEncodingDictionary;
      auto search = encoding->find(pdgID);
      if (search != encoding->end())
        {newParticleDefinition = search->second;}
      else
        {throw BDSException(__METHOD_NAME__,"PDG ID \"" + std::to_string(pdgID) + "not found in particle table");}
    }
  else
    {
      G4IonTable* ionTable = G4ParticleTable::GetParticleTable()->GetIonTable();
      BDSIonDefinition* ionDefinition = particleDefinition->IonDefinition();
      newParticleDefinition = ionTable->GetIon(ionDefinition->Z(),
                                               ionDefinition->A(),
                                               ionDefinition->ExcitationEnergy());
    }
  particleDefinition->UpdateG4ParticleDefinition(newParticleDefinition);
  // Note we don't need to take care of electrons here. These are automatically
  // allocated by Geant4 when it converts the primary vertex to a dynamic particle
  // (in the process of constructing a track from it) (done in G4PrimaryTransformer)
  // this relies on the charge being set correctly - Geant4 detects this isn't the same
  // as Z and adds electrons accordingly.
#if G4VERSION_NUMBER > 1049
  // in the case of ions the particle definition is only available now
  // fix the looping thresholds now it's available
  BDS::FixGeant105ThreshholdsForParticle(newParticleDefinition);
#endif
}

void BDSLinkBunch::UpdateIonDefinition()
{
  if (!currentParticleDefinition->IsAnIon())
    {return;}
  
  G4IonTable* ionTable = G4ParticleTable::GetParticleTable()->GetIonTable();
  BDSIonDefinition* ionDefinition = currentParticleDefinition->IonDefinition();
  G4ParticleDefinition* ionParticleDef = ionTable->GetIon(ionDefinition->Z(),
                                                          ionDefinition->A(),
                                                          ionDefinition->ExcitationEnergy());
  currentParticleDefinition->UpdateG4ParticleDefinition(ionParticleDef);
  // Note we don't need to take care of electrons here. These are automatically
  // allocated by Geant4 when it converts the primary vertex to a dynamic particle
  // (in the process of constructing a track from it) (done in G4PrimaryTransformer)
  // this relies on the charge being set correctly - Geant4 detects this isn't the same
  // as Z and adds electrons accordingly.
#if G4VERSION_NUMBER > 1049
  // in the case of ions the particle definition is only available now
  // fix the looping thresholds now it's available
  BDS::FixGeant105ThreshholdsForParticle(ionParticleDef);
#endif
}