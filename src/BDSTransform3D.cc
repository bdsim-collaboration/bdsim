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
#include "BDSTransform3D.hh"

#include "G4RotationMatrix.hh"
#include "G4String.hh"
#include "G4ThreeVector.hh"
#include "G4Types.hh"

BDSTransform3D::BDSTransform3D(const G4String& nameIn,
			       G4double dxIn,
			       G4double dyIn,
			       G4double dzIn,
			       G4double dPhiIn,
			       G4double dThetaIn,
			       G4double dPsiIn):
  BDSAcceleratorComponent(nameIn,0,0,"transform3d"),
  rotationMatrix(G4RotationMatrix()),
  dx(dxIn), dy(dyIn), dz(dzIn),
  dPhi(dPhiIn), dTheta(dThetaIn), dPsi(dPsiIn),
  angle(0)
{
  rotationMatrix.set(dPhiIn, dThetaIn, dPsiIn);
  G4ThreeVector axis;
  rotationMatrix.getAngleAxis(angle,axis); // sets the variables by reference
  axisX = axis.x();
  axisY = axis.y();
  axisZ = axis.z();
}

BDSTransform3D::BDSTransform3D(const G4String& nameIn,
                               G4double dxIn,
                               G4double dyIn,
                               G4double dzIn,
                               G4double axisXIn,
                               G4double axisYIn,
                               G4double axisZIn,
                               G4double angleIn):
  BDSAcceleratorComponent(nameIn,0,0,"transform3d"),
  rotationMatrix(G4RotationMatrix()),
  dx(dxIn), dy(dyIn), dz(dzIn),
  axisX(axisXIn),
  axisY(axisYIn),
  axisZ(axisZIn),
  angle(angleIn)
{
  G4ThreeVector axis(axisXIn, axisYIn, axisZIn);
  rotationMatrix.set(axis,angle);
  dPhi   = rotationMatrix.getPhi();
  dTheta = rotationMatrix.getTheta();
  dPsi   = rotationMatrix.getPsi();
}

BDSTransform3D::~BDSTransform3D()
{;}
