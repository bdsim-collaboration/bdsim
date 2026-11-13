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
#include "BDSCrystalInfo.hh"
#include "BDSCrystalType.hh"
#include "BDSUtilities.hh"

#include "globals.hh"

#include <limits>

BDSCrystalInfo::BDSCrystalInfo(G4Material*     materialIn,
			       const G4String& dataIn,
			       BDSCrystalType  crystalTypeIn,
			       G4double        lengthXIn,
			       G4double        lengthYIn,
			       G4double        lengthZIn,
			       G4double        sizeAIn,
			       G4double        sizeBIn,
			       G4double        sizeCIn,
			       G4double        alphaIn,
			       G4double        betaIn,
			       G4double        gammaIn,
			       G4int           spaceGroupIn,
			       G4double        bendingAngleYAxisIn,
			       G4double        bendingAngleZAxisIn,
                   G4double        miscutAngleYIn):
  material(materialIn),
  data(dataIn),
  shape(crystalTypeIn),
  lengthX(lengthXIn),
  lengthY(lengthYIn),
  lengthZ(lengthZIn),
  sizeA(sizeAIn),
  sizeB(sizeBIn),
  sizeC(sizeCIn),
  alpha(alphaIn),
  beta(betaIn),
  gamma(gammaIn),
  spaceGroup(spaceGroupIn),
  bendingAngleYAxis(bendingAngleYAxisIn),
  bendingAngleZAxis(bendingAngleZAxisIn),
  miscutAngleY(miscutAngleYIn)
{;}

G4double BDSCrystalInfo::BendingRadius(G4double length,
				       G4double angle) const
{
  if (BDS::IsFinite(angle))
    {return length / angle;}
  else
    {return std::numeric_limits<double>::max();}
}
