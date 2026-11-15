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
#include "BDSAperture.hh"
#include "BDSApertureFactory.hh"
#include "BDSDebug.hh"
#include "BDSExtent.hh"
#include "BDSSamplerCustom.hh"

#include "G4String.hh"

G4double BDSSamplerCustom::chordLength = -1;


BDSSamplerCustom::BDSSamplerCustom(const G4String& nameIn,
                                   BDSAperture*    shape,
                                   G4int           filterSetIDIn):
  BDSSampler(nameIn, filterSetIDIn)
{
  // Make general samplers thicker to be more tolerant on possible
  // problems with overlapping faces - unlike normal samplers were
  // BDSIM strictly controls the layout.
  BDSApertureFactory fac;
  auto contProduct = fac.CreateSolid(name + "_aperture", chordLength, shape);
  containerSolid = contProduct.product;
  allSolids.insert(contProduct.otherSolids.begin(), contProduct.otherSolids.end());
  
  // We make the sampler 10x bigger than normal as it's still really small
  // but less likely to cause overlap problems. The original sampler width
  // is designed to be functional but as small as possible to avoid introducing
  // extra length for optical tracking.

  BDSExtent ae = shape->Extent();
  G4double  dz = chordLength * 0.5;
  SetExtent(BDSExtent(ae.XNeg(), ae.XPos(), ae.YNeg(), ae.YPos(), -dz, dz));

  BDSSampler::CommonConstruction();
}
