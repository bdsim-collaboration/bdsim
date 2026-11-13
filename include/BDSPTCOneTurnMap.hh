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
#ifndef BDSPTCONETURNMAP_H
#define BDSPTCONETURNMAP_H

#include "BDSParticleCoordsFullGlobal.hh"

#include "globals.hh" // Geant4 typedefs
#include "G4Track.hh"

#include <vector>
#include <set>

class BDSParticleDefinition;

/**
 * @brief Class to load and use PTC 1 turn map.
 *
 * This class uses PTC units internally for calculating the result of the map.
 *
 * @author Stuart Walker.
 */

class BDSPTCOneTurnMap
{
public:
  struct PTCMapTerm
  {
    G4double coefficient;
    G4int nx;
    G4int npx;
    G4int ny;
    G4int npy;
    G4int ndeltaP;
  };
    
  BDSPTCOneTurnMap() = delete;                                   ///< Default constructor.
  BDSPTCOneTurnMap(const BDSPTCOneTurnMap &other) = default;     ///< Copy constructor.
  BDSPTCOneTurnMap(BDSPTCOneTurnMap &&other) noexcept = default; ///< Move constructor. 
  virtual ~BDSPTCOneTurnMap() {;}                                ///< Destructor.

  /// Main constructor with path to maptable file.
  BDSPTCOneTurnMap(const G4String& maptableFile,
		   const BDSParticleDefinition* designParticle);

  /// Decides whether or not this should be applied. Can add more
  G4bool ShouldApplyToPrimary(G4double momentum, G4int turnstaken);

  /// Load initial coordinates where the beam started and convert to PTC coordinates.
  void SetInitialPrimaryCoordinates(const BDSParticleCoordsFullGlobal& coords,
				    G4bool offsetS0);

  /// Update coordinates if the last turn is greater than the number of turns taken.
  void UpdateCoordinates(G4ThreeVector localPosition,
			 G4ThreeVector localMomentum,
			 G4int         turnstaken);

  /// Return the coordinates for this turn.
  void GetThisTurn(G4double& x,
		   G4double& px,
		   G4double& y,
		   G4double& py,
		   G4double& pz,
		   G4int turnstaken);

private:
  G4double Evaluate(std::vector<PTCMapTerm>& terms,
		    G4double x,
		    G4double px,
            G4double y,
		    G4double py,
		    G4double deltaP) const;

  G4double initialPrimaryMomentum;
  G4bool   beamOffsetS0;
  G4double referenceMomentum;
  G4double mass;

  std::set<G4int> turnsScattered;

  G4int    lastTurnNumber;
  G4double xLastTurn;
  G4double pxLastTurn;
  G4double yLastTurn;
  G4double pyLastTurn;
  G4double deltaPLastTurn;

  std::vector<PTCMapTerm> xTerms;
  std::vector<PTCMapTerm> yTerms;
  std::vector<PTCMapTerm> pxTerms;
  std::vector<PTCMapTerm> pyTerms;
  std::vector<PTCMapTerm> deltaPTerms;
};

#endif
