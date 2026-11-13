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
#ifndef BDSTRANSFORM3D_H
#define BDSTRANSFORM3D_H
#include "BDSAcceleratorComponent.hh"

#include "G4RotationMatrix.hh"
#include "G4String.hh"
#include "G4Types.hh"


/**
 * @brief Transform in beam line that takes up no space.
 *
 * This applies a transformation to cumulative curvilinear
 * coordinates as the beam line is built allowing an arbitrary
 * rotation or displacement of the reference coordinates from
 * that point on. A tilt or offset should be used instead of 
 * two of these in conjunction (sandwiching) to offset a particular
 * BDSAcceleratorComponent.
 *
 * @author Laurie Nevay
 */

class BDSTransform3D: public BDSAcceleratorComponent
{
public:
  BDSTransform3D() = delete;
  BDSTransform3D& operator=(const BDSTransform3D&) = delete;
  BDSTransform3D(BDSTransform3D&) = delete;
  
  BDSTransform3D(const G4String& aName,
                 G4double x,
		 G4double y,
		 G4double z,
		 G4double phi,
		 G4double theta,
		 G4double psi);
  
  BDSTransform3D(const G4String& nameIn,
                 G4double x,
                 G4double y,
                 G4double z,
                 G4double axisXIn,
                 G4double axisYIn,
                 G4double axisZIn,
                 G4double angleIn);

  virtual ~BDSTransform3D();
  
  G4RotationMatrix rotationMatrix;
  G4double dx;
  G4double dy;
  G4double dz;
  G4double dPhi;
  G4double dTheta;
  G4double dPsi;
  G4double axisX;
  G4double axisY;
  G4double axisZ;
  G4double angle;

private:
  /// Contractual function overload from virtual base class BDSAcceleratorComponent.
  virtual void BuildContainerLogicalVolume(){;};
};

#endif
