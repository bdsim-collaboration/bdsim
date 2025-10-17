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
#ifndef BDSLINKRUNACTION_H
#define BDSLINKRUNACTION_H
#include "BDSHitSamplerLink.hh"

#include "G4Types.hh"
#include "G4UserRunAction.hh"

class G4Run;

/**
 * @brief Simplified run action to hold link hits.
 * 
 * @author Laurie Nevay
 */

class BDSLinkRunAction: public G4UserRunAction
{
public:
  BDSLinkRunAction();
  virtual ~BDSLinkRunAction();
  
  virtual void BeginOfRunAction(const G4Run* aRun);
  virtual void EndOfRunAction(const G4Run* aRun);

  void AppendHits(G4int currentEventIndex,
                  G4int externalParticleID,
                  G4int externalParentID,
                  const BDSHitsCollectionSamplerLink* hits);

  BDSHitsCollectionSamplerLink* SamplerHits() const {return allHits;}
  void ClearSamplerHits() {delete allHits; allHits = nullptr;}

  inline G4int NSecondariesToReturn() const {return nSecondariesToReturn;}
  inline G4int NPrimariesToReturn()   const {return nPrimariesToReturn;}
  inline G4int MaximumExternalParticleID() const {return maximumExternalParticleID;}
  inline void SetMaximumExternalParticleID(G4int maxExtPartID) {maximumExternalParticleID = maxExtPartID;}
  
private:
  BDSHitsCollectionSamplerLink* allHits;
  G4int nSecondariesToReturn;
  G4int nPrimariesToReturn;
  G4int maximumExternalParticleID;
};

#endif

