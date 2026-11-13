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
#include "UDipole.hh"
#include "UDipoleConstructor.hh"

#include "BDSAcceleratorComponent.hh"
#include "BDSBeamlineIntegral.hh"
#include "parser/element.h" // for GMAD::Element

//#include "CLHEP/Units/PhysicalConstants.h"  // for c_light etc
#include "CLHEP/Units/SystemOfUnits.h"

#include "G4String.hh"
#include "G4Types.hh"  // for G4double, G4bool etc

BDSAcceleratorComponent* UDipoleConstructor::Construct(GMAD::Element const* element,
						       GMAD::Element const* /*prevElement*/,
						       GMAD::Element const* /*nextElement*/,
						       const BDSBeamlineIntegral& /*integral*/)
{
  G4String params = G4String(element->userParameters);

  //G4double currentIntegratedArcLength = integral.arcLength;
  //G4double synchronousGlobalTAtEndOfPreviousElement = integral.synchronousTAtEnd;
  
  // Here we pull out the information we want from the parser element and construct
  // one of our components and return it. BDSIM will delete this at the end.
  // If you don't need to use all the parameters, use an /*inline*/ comment to
  // avoid a compiler warning about an unused variable.
  BDSAcceleratorComponent* dipole = new UDipole(element->name,
						element->B*CLHEP::tesla,
						params);
  return dipole;
}
