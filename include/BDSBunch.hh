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
#ifndef BDSBUNCH_H
#define BDSBUNCH_H

#include "BDSBunchType.hh"
#include "BDSParticleCoords.hh"
#include "BDSParticleCoordsFull.hh"
#include "BDSParticleCoordsFullGlobal.hh"
#include "BDSParticleDefinition.hh"

#include "globals.hh"
#include "G4Transform3D.hh"

class BDSBeamline;

namespace GMAD
{
  class Beam;
}

/**
 * @brief The base class for bunch distribution generators.
 *
 * After instantiation of a class, use SetOptions(), then CheckParmeters(), then Initialise().
 *
 * @author Stewart Boogert
 */

class BDSBunch
{
public:
  BDSBunch();
  explicit BDSBunch(const G4String& nameIn);
  virtual ~BDSBunch();

  /// Make BDSPrimaryGeneratorFileHEPMC a friend so it can use the protected ApplyTransform function.
  friend class BDSPrimaryGeneratorFileHEPMC;
  friend class BDSPrimaryGeneratorFileSampler;

  /// Extract and set the relevant options from the beam definition. The distribution
  /// type is explicitly required as this function may be used inside a nested bunch distribution.
  /// This argument is for the most part ignored, but there's no way to have a default for it.
  /// Also, some classes can cover multiple input distributions so need to know which one they're
  /// meant to be.
  virtual void SetOptions(const BDSParticleDefinition* beamParticle,
                          const GMAD::Beam& beam,
                          const BDSBunchType& distrType,
                          G4Transform3D beamlineTransformIn = G4Transform3D::Identity,
                          const G4double beamlineS = 0);
  
  /// Check the parameters for the given bunch distribution and exit if they're
  /// problematic or unphysical.
  virtual void CheckParameters();

  /// Any initialisation - to be used after SetOptions, then CheckParameters.
  virtual void Initialise();

  /// Main interface. Calls GetNextParticleLocal() and then applies the curvilinear
  /// transform if required.
  BDSParticleCoordsFullGlobal GetNextParticle();
  
  /// A hint of whether we expect to require and extended particle set (ie pions, kaons, muons).
  virtual G4bool ExpectChangingParticleType() const {return false;}

  /// Interface to allow multiple calls until a safe particle is generated. This will
  /// repeatedly call GetNextParticle() until a particle is generated where the total
  /// energy is greater than the rest mass. This can be overridden by derived classes
  /// if they do not wish this behaviour. This may throw an exception if the maximum
  /// number of attempts (maxTries) is exceeded.
  virtual BDSParticleCoordsFullGlobal GetNextParticleValid(G4int maxTries = 100);

  /// An action that is called at the beginning of a run when we know the number of
  /// events that'll be generated. By default this is nothing, but can be used to
  /// calculate sample mean offsets in some derived classes.
  virtual void BeginOfRunAction(G4int numberOfEvents,
                                G4bool batchMode);

  /// Access the beam particle definition.
  inline virtual const BDSParticleDefinition* ParticleDefinition() const {return particleDefinition;}

  /// Set the flag to whether we're only generating primaries only. This sets the member
  /// variable generatePrimariesOnly which skips trying to perform a curvilinear transform
  /// if true. i.e. if we're generating primaries only, there's no beam line.
  virtual void SetGeneratePrimariesOnly(G4bool generatePrimariesOnlyIn);

  /// Each derived class can override this default method of reference
  /// position. If S0 > 0 or derived class changes member bool 'curvilinear'
  /// z0 will be treated as S and the global z0 be calculated.
  virtual BDSParticleCoordsFull GetNextParticleLocal();

  /// Access whether there's a finite S offset and therefore we're using a CL transform.
  G4bool UseCurvilinearTransform() const {return useCurvilinear;}

  /// When recreating events, it's possible that setting the seed state may not
  /// be sufficient for the bunch to get the right distribution. This is true when
  /// the bunch coordinates are based on an external source of data i.e. user bunch
  /// file. This default method allows such a distribution to advance to the correct
  /// event number. Updates the bunch index by default.
  virtual void RecreateAdvanceToEvent(G4int eventOffset);

  /// Access whether the beam particle is an ion or not.
  inline G4bool BeamParticleIsAnIon() const {return particleDefinition->IsAnIon();}

  /// Update the Geant4 Particle Definition inside particleDefinition member in the
  /// case it's an ion. This can only be done later one once the run has started. Since
  /// the particle definition is kept here in the bunch this interface allows control
  /// over it being updated.
  virtual void UpdateIonDefinition();

  /// Whether the particle definition has been updated since the last call to
  /// GetNextParticle() or GetNextParticleValid().
  inline G4bool ParticleDefinitionHasBeenUpdated() const {return particleDefinitionHasBeenUpdated;}

  /// Calculate zp safely based on other components.
  static G4double CalculateZp(G4double xp, G4double yp, G4double Zp0);

  /// Work out whether either the geometric or normalised emittances are set and update
  /// the variables by reference with the values. Can throw exception if more than
  /// one set in either x and y.
  static void SetEmittances(const BDSParticleDefinition* beamParticle,
			    const GMAD::Beam& beam,
			    G4double&         emittGeometricX,
			    G4double&         emittGeometricY,
			    G4double&         emittNormalisedX,
			    G4double&         emittNormalisedY);

  /// Distribution name.
  inline G4String Name() const {return name;}

  /// Get the current bunch index for writing to output.
  inline G4int CurrentBunchIndex() const {return currentBunchIndex;}

  /// Calculate which bunch index we should be at given an event index.
  void CalculateBunchIndex(G4int eventIndex);

protected:
  /// Apply either the curvilinear transform if we're using curvilinear coordinates or
  /// just apply the general beam line offset in global coordinates to the 'local'
  /// curvilinear coordinates.
  BDSParticleCoordsFullGlobal ApplyTransform(const BDSParticleCoordsFull& localIn) const;

  /// Apply a rotation about unitZ for the local coordinates according to member variable tilt.
  void ApplyTilt(BDSParticleCoordsFull& localIn) const;
  
  /// Add on the offset in T for the current bunch number (i*bunchPeriod).
  void ApplyBunchTiming(BDSParticleCoordsFullGlobal& localIn) const;

  /// Calculate the global coordinates from curvilinear coordinates of a beam line.
  BDSParticleCoordsFullGlobal ApplyCurvilinearTransform(const BDSParticleCoordsFull& localIn) const;

  G4String name; ///< Name of distribution
  
  ///@{ Centre of distributions
  G4double X0;
  G4double Y0;
  G4double Z0;
  G4double S0;
  G4double T0; 
  G4double Xp0; 
  G4double Yp0;
  G4double Zp0;
  G4double E0;
  G4double P0;     ///< central momentum
  G4double tilt;
  G4double sigmaT;
  G4double sigmaP;
  G4double sigmaE;
  G4double sigmaEk;
  ///@}
  
  /// @{ Bunch offset in time parameters.
  bool useBunchTiming;
  G4int currentBunchIndex;
  G4int eventsPerBunch;
  G4double bunchPeriod;
  /// @}
  
  /// Whether to ignore z and use s and transform for curvilinear coordinates
  G4bool useCurvilinear;

  /// Particle definition for bunch - this class owns it.
  BDSParticleDefinition* particleDefinition;

  /// Whether the particle definition has been updated since last call to
  /// GetNextParticle() or GetNextParticleValid().
  G4bool particleDefinitionHasBeenUpdated;

  G4bool finiteTilt; ///< Flag of whether to apply beam rotation.
  /// @{ Flags to ignore random number generator in case of no finite E or T.
  G4bool finiteSigmaE;
  G4bool finiteSigmaT;
  /// @}

  /// Whether we're only generating the primaries only and therefore there's no beamline
  /// to get a transform from and we shouldn't try.
  G4bool generatePrimariesOnly;
  
private:
  /// Transform that beam line starts with that will also be applied to coordinates.
  G4Transform3D beamlineTransform;

  G4double beamlineS; ///< Beamline initial S position
  G4double mass2;     ///< Cache of mass squared as required to convert from p to E.
  
  /// A reference to the fully constructed beamline that's lazily instantiated.
  mutable const BDSBeamline* beamline;
};

#endif
