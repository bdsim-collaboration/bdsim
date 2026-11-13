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
#include "BDSAppropriateTubs.hh"

#include "G4CutTubs.hh"
#include "G4String.hh"
#include "G4ThreeVector.hh"
#include "G4Tubs.hh"
#include "G4Types.hh"

G4VSolid* BDS::AppropriateTubs(const G4String& name,
			       G4double innerRadius,
			       G4double outerRadius,
			       G4double halfLength,
			       G4double startAngle,
			       G4double sweepAngle,
			       const G4ThreeVector& inputFaceNormal,
			       const G4ThreeVector& outputFaceNormal,
			       G4bool   flatFaces)
{
  G4VSolid* result;
  if (flatFaces)
    {result = new G4Tubs(name, innerRadius, outerRadius, halfLength, startAngle, sweepAngle);}
  else
    {
      result = new G4CutTubs(name, innerRadius, outerRadius, halfLength, startAngle, sweepAngle,
			     inputFaceNormal, outputFaceNormal);
    }
  return result;
}
