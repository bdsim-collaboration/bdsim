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
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSExtent.hh"
#include "BDSFieldEInterpolated.hh"
#include "BDSFieldEInterpolated2Layer.hh"

#include "G4ThreeVector.hh"

BDSFieldEInterpolated2Layer::BDSFieldEInterpolated2Layer(BDSFieldEInterpolated* mainFieldIn,
							 BDSFieldEInterpolated* subFieldIn):
  mainField(mainFieldIn),
  subField(subFieldIn),
  subFieldExtent(subFieldIn->Extent())
{
  if (!mainField->ExtentNoOffset().Encompasses(subFieldIn->ExtentNoOffset()))
    {throw BDSException(__METHOD_NAME__, "sub field in field map is bigger than main field.");}
}

BDSFieldEInterpolated2Layer::~BDSFieldEInterpolated2Layer()
{
  delete mainField;
  delete subField;
}

G4ThreeVector BDSFieldEInterpolated2Layer::GetField(const G4ThreeVector& position,
						    const G4double       t) const
{
  // transformations in these objects are only the transform of the field definition,
  // not w.r.t. the beam line which is done already on the incoming position for this instance
  if (subFieldExtent.Encompasses(position))
    {return subField->GetFieldTransformed(position, t);}
  else
    {return mainField->GetFieldTransformed(position, t);}
}

G4bool BDSFieldEInterpolated2Layer::TimeVarying() const
{
  return mainField->TimeVarying() || subField->TimeVarying();
}