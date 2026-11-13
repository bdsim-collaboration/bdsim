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
#ifndef BDSINTEGRATORMAG_H
#define BDSINTEGRATORMAG_H

#include "BDSAuxiliaryNavigator.hh"
#include "BDSIntegratorDrift.hh"
#include "BDSMagnetStrength.hh"

#include "globals.hh" // geant4 types / globals
#include "G4MagIntegratorStepper.hh"

class G4Mag_EqRhs;

/**
 * @brief Common functionality to BDSIM integrators.
 *
 * This provides a general 4th order Runge Kutta integrator that can be used
 * by the derived class if the coordinates are non-paraxial for example.
 *
 * The derived class must also satisfy G4MagIntegratorStepper.hh's virtual method
 * Stepper.
 * 
 * @author Laurie Nevay
 */

class BDSIntegratorMag: public G4MagIntegratorStepper, public BDSIntegratorDrift, public BDSAuxiliaryNavigator
{
public:
  BDSIntegratorMag(G4Mag_EqRhs* eqOfMIn,
		   G4int        nVariablesIn);
  
  virtual ~BDSIntegratorMag();
  /// @{ Assignment and copy constructor not implemented nor used
  BDSIntegratorMag& operator=(const BDSIntegratorMag&) = delete;
  BDSIntegratorMag(BDSIntegratorMag&) = delete;
  /// @}

  /// Estimate maximum distance of curved solution and chord.
  inline virtual G4double DistChord() const {return distChordPrivate;}

  /// Geant4 requires that the integrator order must be supplied by the derived class.
  inline virtual G4int IntegratorOrder() const {return 2;}

  /// Cache of thin element length to know maximum possible length scale step
  /// for coordinate lookup.
  static G4double thinElementLength;

  /// Cache of the fraction of the momentum outside which don't use a matrix
  /// as it's just not feasible.
  static G4double nominalMatrixRelativeMomCut;

  /// This static variable is updated by BDSFieldManager that marks each
  /// track as primary or not here. This variable is used throughout our
  /// integrators for magnetic fields which inherit this class.
  static G4bool currentTrackIsPrimary;

protected:
  /// Convert final local position and direction to global frame. Allow
    /// scaling of momentum in case localMom is a unit vector
  void ConvertToGlobal(const G4ThreeVector &localPos,
                       const G4ThreeVector &localMom,
                       G4double             yOut[],
		       G4double             yErr[],
                       const G4double       momScaling = 1.0);

  /// Setter for distChord to private member.
  inline void SetDistChord(G4double distChordIn) {distChordPrivate = distChordIn;}
  
  /// Keep a reference to the underlying equation of motion, but through a higher
  /// level pointer than G4EquationOfMotion* so we can use the correct methods. This
  /// class doesn't own this.
  G4Mag_EqRhs* eqOfM;
  
  /// Cache of the number of variables.
  const G4int nVariables;

  /// General integrator that can be used as a backup if the particle momentum is
  /// outside the (transverse) momentum range applicable for the integration scheme
  /// used by the derived integrator.
  G4MagIntegratorStepper* backupStepper;

  /// Whether a magnet has a finite strength or not. Can be set in the constructor for
  /// zero strength elements and then a drift routine is used before anything else.
  G4bool zeroStrength;

  /// Cache of fractional unit momentum limit above which the matrix routines in the derived integrators
  /// revert to a backup integrator to ensure tracking validity.
  G4double backupStepperMomLimit;

private:
  /// Private default constructor to force use of specific constructor
  BDSIntegratorMag() = delete;

  /// Variable used to record the distance from the chord calculated during the step.
  G4double distChordPrivate;
};

#endif
