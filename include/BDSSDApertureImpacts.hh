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
#ifndef BDSSDAPERTUREIMPACTS_H
#define BDSSDAPERTUREIMPACTS_H

#include "BDSHitApertureImpact.hh"

#include "globals.hh"
#include "G4VSensitiveDetector.hh"

class BDSAuxiliaryNavigator;

class G4HCofThisEvent;
class G4Step;
class G4TouchableHistory;
class G4Track;

/**
 * @brief Generates BDSHitsEnergyDepositions from step information - uses curvilinear coords.
 *
 * This class interrogates a G4Step and generates an energy deposition hit if there was
 * a change in energy. This assigns the energy deposition to a point randomly (uniformly)
 * along the step.  It also uses a BDSAuxiliaryNavigator instance to use transforms from
 * the curvilinear parallel world for curvilinear coordinates.
 */

class BDSSDApertureImpacts: public G4VSensitiveDetector
{
public:
  BDSSDApertureImpacts(const G4String& name);
  virtual ~BDSSDApertureImpacts();

  virtual void Initialize(G4HCofThisEvent* HCE);

  /// The standard interface here to process a step from Geant4. Record
  /// all the relevant coordinates here. Records the energy deposited along
  /// the step.
  virtual G4bool ProcessHits(G4Step* aStep,
			     G4TouchableHistory* th);
  
private:
  /// Assignment and copy constructor not implemented nor used.
  BDSSDApertureImpacts& operator=(const BDSSDApertureImpacts&);
  BDSSDApertureImpacts(BDSSDApertureImpacts&);
  BDSSDApertureImpacts() = delete;
  
  G4String colName;                       ///< Collection name.
  BDSHitsCollectionApertureImpacts* hits;  ///< Hits.
  G4int    HCIDe;                         ///< Hits collection ID of the event.

  /// Navigator for checking points in read out geometry
  BDSAuxiliaryNavigator* auxNavigator;
};

#endif

