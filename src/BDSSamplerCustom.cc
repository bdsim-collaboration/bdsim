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
#include "BDSApertureInfo.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSGlobalConstants.hh"
#include "BDSSamplerCustom.hh"

#include "globals.hh" // geant4 types / globals
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4Tubs.hh"

G4double BDSSamplerCustom::chordLength = -1;

BDSSamplerCustom::BDSSamplerCustom(const G4String&        nameIn,
				   const BDSApertureInfo& shape,
                                   G4int                  filterSetIDIn):
  BDSSampler(nameIn, filterSetIDIn)
{
  /*
  BDSApertureFactory fac;
  containerSolid = fac.CreateAperture(name + "_aperture",
				      BDSSamplerPlane::chordLength,
				      shape);
  */
  // We make the sampler 10x bigger than normal as it's still really small
  // but less likely to cause overlap problems. The original sampler width
  // is designed to be functional but as small as possible to avoid introducing
  // extra length for optical tracking.
  switch (shape.apertureType.underlying())
    {
    case BDSApertureType::circular:
    case BDSApertureType::circularvacuum:
      {
	containerSolid = new G4Tubs(name + "_solid",
				    0,
				    shape.aper1,
				    0.5*chordLength,
				    0,
				    CLHEP::twopi);
	break;
      }
    case BDSApertureType::rectangular:
      {
	containerSolid = new G4Box(name + "_solid",
				   shape.aper1,
				   shape.aper2,
				   0.5*chordLength);
	break;
      }
    default:
      {
	std::string msg = "Shape \"" + shape.apertureType.ToString() + "\" is not currently supported.\n";
	msg += "Please use circular or rectangular.";
	throw BDSException(__METHOD_NAME__, msg);
	break;
      }
    }

  BDSExtent ae = shape.Extent();
  G4double  dz = 0.5*chordLength;
  SetExtent(BDSExtent(ae.XNeg(), ae.XPos(), ae.YNeg(), ae.YPos(), -dz, dz));

  CommonConstruction();
}
