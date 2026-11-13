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
#ifndef BDSBUNCHEVENTGENERATOR_H
#define BDSBUNCHEVENTGENERATOR_H 

#include "BDSBunchFileBased.hh"

#include "G4RotationMatrix.hh"
#include "G4Types.hh"

#include <set>

/**
 * @brief A wrapper of BDSBunch to include a filter for the events
 * loaded by an event generator.
 * 
 * @author Laurie Nevay
 */

class BDSBunchEventGenerator: public BDSBunchFileBased
{
public: 
  BDSBunchEventGenerator();
  virtual ~BDSBunchEventGenerator(); 
  /// @{ Assignment and copy constructor not implemented nor used
  BDSBunchEventGenerator& operator=(const BDSBunchEventGenerator&) = delete;
  BDSBunchEventGenerator(BDSBunchEventGenerator&) = delete;
  /// @}
  
  // Make friends so they can access protected members
  friend class BDSPrimaryGeneratorFileHEPMC;
  friend class BDSPrimaryGeneratorFileSampler;
  
  virtual void SetOptions(const BDSParticleDefinition* beamParticle,
			  const GMAD::Beam&            beam,
			  const BDSBunchType&          distrType,
			  G4Transform3D                beamlineTransformIn = G4Transform3D::Identity,
			  G4double                     beamlineS = 0);
  virtual void CheckParameters();

  /// Return whether a particle is within the phase space cuts for an event
  /// generator file particle.
  G4bool AcceptParticle(const BDSParticleCoordsFull& coords,
			G4double rpOriginal,
			G4double kineticEnergy,
			G4int    pdgID);

  /// Get a rotation matrix according to Xp0 and Yp0.
  G4RotationMatrix ReferenceBeamMomentumOffset() const;
  
protected:
  /// Split on white space and try and find names and IDs in the G4ParticleTable.
  /// Because it uses G4ParticleTable this should only be called once the particle table
  /// is initialised.
  void ParseAcceptedParticleIDs();

  /// @{ Cache of limit.
  G4int    eventGeneratorNEventsSkip;
  G4double eventGeneratorMinX;
  G4double eventGeneratorMaxX;
  G4double eventGeneratorMinY;
  G4double eventGeneratorMaxY;
  G4double eventGeneratorMinZ;
  G4double eventGeneratorMaxZ;
  G4double eventGeneratorMinXp;
  G4double eventGeneratorMaxXp;
  G4double eventGeneratorMinYp;
  G4double eventGeneratorMaxYp;
  G4double eventGeneratorMinZp;
  G4double eventGeneratorMaxZp;
  G4double eventGeneratorMinRp;
  G4double eventGeneratorMaxRp;
  G4double eventGeneratorMinT;
  G4double eventGeneratorMaxT;
  G4double eventGeneratorMinEK;
  G4double eventGeneratorMaxEK;
  G4double Rp0;
  /// @}

  /// Vector (sorted) of permitted particles.
  std::set<G4int> acceptedParticles;

private:
  G4bool firstTime;                ///< Flag to prepare acceptedParticles on first call.
  G4bool testOnParticleType;       ///< Flag whether to bother applying search.
  G4String acceptedParticlesString;///< Cache of string for parsing on first query.
};

#endif
