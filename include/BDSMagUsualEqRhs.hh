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
#ifndef BDSMAGUSUALEQRHS_H
#define BDSMAGUSUALEQRHS_H

#include "BDSAuxiliaryNavigator.hh"
#include "BDSIntegratorDrift.hh"

#include "globals.hh" // geant4 types / globals
#include "G4MagIntegratorStepper.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "G4ChargeState.hh"

/**
 * @brief Override G4Mag_UsualEqRhs, provides BDSIM integrators access to particle attributes.
 *
 * @author William Shields
 */

class BDSMagUsualEqRhs: public G4Mag_UsualEqRhs
{
public:
  BDSMagUsualEqRhs(G4MagneticField* MagField);

  virtual ~BDSMagUsualEqRhs(){;}

  /// Accessor to variable for normalising to charge
  inline G4double FCof() const {return fCof_val;}

  /// Accessor to particle mass
  inline G4double Mass() const {return fMassCof;}

  /// Calculate particle velocity W.R.T speed of light
  G4double Beta(const G4double y[6]);

  /// Calculate particle velocity W.R.T speed of light
  G4double Beta(const G4ThreeVector& mom);

  /// Calculate total particle energy
  G4double TotalEnergy(const G4double y[]);

  /// Calculate total particle energy
  G4double TotalEnergy(const G4ThreeVector& mom);

  /// Copy of class method from G4Mag_UsualEqRhs
  virtual void SetChargeMomentumMass(G4ChargeState particleCharge,
                                     G4double MomentumXc,
                                     G4double particleMass);

protected:
  G4double fMassCof;  ///< Particle mass squared.
  G4double fCof_val;

};

#endif
