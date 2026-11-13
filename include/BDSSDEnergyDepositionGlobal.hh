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
#ifndef BDSSDENERGYDEPOSITIONGLOBAL_H
#define BDSSDENERGYDEPOSITIONGLOBAL_H

#include "BDSHitEnergyDepositionGlobal.hh"
#include "BDSSensitiveDetector.hh"

class G4HCofThisEvent;
class G4Step;
class G4TouchableHistory;
class G4Track;

/**
 * @brief Generates BDSHitsEnergyDepositionGlobal from step information.
 *
 * This class interrogates a G4Step and generates an energy deposition hit if there was
 * a change in energy. This assigns the energy deposition to a point randomly (uniformly)
 * along the step.
 * 
 * It does not use curvilinear coordinates and only uses global coordinates.
 * 
 * @author Laurie Nevay
 */

class BDSSDEnergyDepositionGlobal: public BDSSensitiveDetector
{
public:
  explicit BDSSDEnergyDepositionGlobal(const G4String& name,
				       G4bool killedParticleMassAddedToElossIn = false);
  virtual ~BDSSDEnergyDepositionGlobal();

  virtual void Initialize(G4HCofThisEvent* HCE);

  /// The standard interface here to process a step from Geant4. Record
  /// all the relevant coordinates here. Records the energy deposited along
  /// the step.
  virtual G4bool ProcessHits(G4Step* aStep,
			     G4TouchableHistory* th);

  /// An extra interface that can be used when artificially killing a track
  /// to add that kill action as energy deposition of the particle there.
  /// Note, this method always uses the total energy of the particle. There
  /// is no step here, so it's just the total energy of the particle.
  virtual G4bool ProcessHitsTrack(const G4Track* track,
				  G4TouchableHistory* th);

  /// Provide access to last hit.
  virtual G4VHit* last() const;
  
private:
  /// assignment and copy constructor not implemented nor used
  BDSSDEnergyDepositionGlobal& operator=(const BDSSDEnergyDepositionGlobal&);
  BDSSDEnergyDepositionGlobal(BDSSDEnergyDepositionGlobal&);
  BDSSDEnergyDepositionGlobal() = delete;

  G4bool   killedParticleMassAddedToEloss;
  G4String colName;         ///< Collection name.
  BDSHitsCollectionEnergyDepositionGlobal* hits;
  G4int    HCIDe;

  ///@{ Per hit variable.
  G4double energy;
  G4double preStepKineticEnergy;
  G4double postStepKineticEnergy;
  G4double stepLength;
  G4double weight;
  G4double X,Y,Z;      // Global coordinates.
  G4double globalTime; // Time since start of event.
  G4int    pdgID;
  G4int    trackID;
  G4int    parentID;
  G4int    turnsTaken;
  ///@}
};

#endif

