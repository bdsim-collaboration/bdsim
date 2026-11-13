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
#ifndef BDSSTEP_H
#define BDSSTEP_H

#include "G4ThreeVector.hh"

class G4VPhysicalVolume;

/**
 * @brief A simple class to represent the positions of a step.
 *
 * @author Laurie Nevay
 */

class BDSStep
{
public:
  BDSStep();
  BDSStep(G4ThreeVector preStepPointIn,
	  G4ThreeVector postStepPointIn,
	  G4VPhysicalVolume* volumeForTransformIn = nullptr);
  ~BDSStep();

  /// @{ Accessor.
  inline G4ThreeVector PreStepPoint()  const {return preStepPoint;}
  inline G4ThreeVector PostStepPoint() const {return postStepPoint;}
  inline G4VPhysicalVolume* VolumeForTransform() const {return volumeForTransform;}
  /// @}

  /// @{ Setter.
  inline void SetPreStepPoint(G4ThreeVector preIn)   {preStepPoint = preIn;}
  inline void SetPostStepPoint(G4ThreeVector postIn) {postStepPoint = postIn;}
  inline void SetVolumeForTransform(G4VPhysicalVolume* volIn) {volumeForTransform = volIn;}
  /// @}

  /// Mirror function to G4ThreeVector::rotateZ(). Returns copy that's rotated.
  BDSStep rotateZ(const G4double& angle);
  
private:
  G4ThreeVector preStepPoint;
  G4ThreeVector postStepPoint;
  G4VPhysicalVolume* volumeForTransform; ///< The volume that was used for the transform.
};

#endif
