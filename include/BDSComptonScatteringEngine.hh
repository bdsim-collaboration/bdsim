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
#ifndef BDSCOMPTONSCATTERINGENGINE_H
#define BDSCOMPTONSCATTERINGENGINE_H

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"
#include "G4DynamicParticle.hh"

#include "CLHEP/Units/PhysicalConstants.h"
#include "G4INCLRandom.hh"
#include "G4RotationMatrix.hh"
#include "G4StokesVector.hh"
#include "CLHEP/Units/PhysicalConstants.h"
#include "G4INCLRandom.hh"

class BDSComptonScatteringEngine
{
public:
  BDSComptonScatteringEngine();
  ~BDSComptonScatteringEngine();

  G4double CrossSection(G4double photonEnergy, G4int partIn);
  void PerformCompton(const G4ThreeVector& boost, G4int partIn);
  void SetParticle(G4int partIDIn);
  
  inline void setIncomingGamma(G4LorentzVector incomingGammaIn) {incomingGamma=incomingGammaIn;}
  inline void setIncomingElectron(G4LorentzVector incomingElectronIn) {incomingElectron=incomingElectronIn;}
  inline G4LorentzVector GetScatteredGamma()     {return scatteredGamma;};
  inline G4LorentzVector GetScatteredElectron()  {return scatteredElectron;};
  G4double KleinNishinaDifferential(G4double theta);
  G4double MCMCTheta();
  G4RotationMatrix CalculateRotation() const;

  /// Set the Stokes parameters of the incoming photon. These are not a spatial vector: xi1 and xi2
  /// are linear polarisation relative to referenceAxisIn (any vector not parallel to the photon
  /// direction, in the same frame as the incoming photon momentum) and xi3 is circular polarisation.
  inline void SetIncomingGammaPolarization(const G4StokesVector& incomingGammaPolarizationIn,
                                           const G4ThreeVector& referenceAxisIn)
  {
    incomingGammaPolarization = incomingGammaPolarizationIn;
    incomingGammaPolarizationAxis = referenceAxisIn;
  }
  inline void SetIncomingElectronPolarization(G4StokesVector incomingElectronPolarizationIn) {incomingElectronPolarization=incomingElectronPolarizationIn;}
  G4double MCPhi(G4double theta, G4double scatteredEnergy);
  G4double PolarizationCrossSectionPhi(G4double theta, G4double phi, G4double scatteredEnergy);
  G4double ScatteredEnergy(G4double theta);
  G4double PolarizationCrossSectionMaxPhi(G4double theta,G4double EP);
private:
  /// Rotation from the frame the cross-section is evaluated in (incoming photon along +z,
  /// phi measured from +x) to the frame of the incoming photon momentum.
  G4RotationMatrix TheoryToRestFrameRotation() const;

  /// Re-express the incoming photon Stokes parameters with respect to the x axis of the theory frame.
  G4StokesVector GammaPolarizationInTheoryFrame(const G4RotationMatrix& theoryToRest) const;

  G4LorentzVector incomingGamma;
  G4LorentzVector scatteredGamma;
  G4LorentzVector incomingElectron;
  G4LorentzVector scatteredElectron;
  G4StokesVector incomingGammaPolarization;
  G4ThreeVector  incomingGammaPolarizationAxis;
  G4StokesVector incomingElectronPolarization;
  G4StokesVector gammaPolarizationTheory;    ///< incomingGammaPolarization in the theory frame.
  G4StokesVector electronPolarizationTheory; ///< incomingElectronPolarization in the theory frame.
  G4StokesVector outgoingGammaPolarization;
  G4StokesVector outgoingElectronPolarization;
  G4double particleMass;
  G4double particleRadius;

  G4int partID;
};

#endif
