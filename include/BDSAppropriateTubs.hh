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
#ifndef BDSAPPROPRIATETUBS_H
#define BDSAPPROPRIATETUBS_H

#include "G4ThreeVector.hh"
#include "G4Types.hh"

class G4String;
class G4VSolid;

/**
 * @brief Return either G4Tubs or G4CutTubs depending on flat face.
 *
 * Utility function to always instantiate the right solid to avoid
 * getting an exception from Geant4.
 * 
 * @author Laurie Nevay
 */

namespace BDS
{
  G4VSolid* AppropriateTubs(const G4String&      name,
			    G4double             innerRadius,
			    G4double             outerRadius,
			    G4double             halfLength,
			    G4double             startAngle,
			    G4double             sweepAngle,
			    const G4ThreeVector& inputFaceNormal,
			    const G4ThreeVector& outputFaceNormal,
			    G4bool               flatFaces);
}

#endif
