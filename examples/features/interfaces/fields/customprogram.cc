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
#include "BDSException.hh"
#include "BDSFieldInfo.hh"
#include "BDSFieldFactory.hh"
#include "BDSFieldFormat.hh"
#include "BDSFieldObjects.hh"
#include "BDSFieldType.hh"
#include "BDSIntegratorType.hh"
#include "BDSInterpolatorType.hh"

#include "G4Transform3D.hh"
#include "G4Field.hh"

#include <iostream>

int main(int argc, char** argv)
{
  BDSFieldInfo recipe = BDSFieldInfo(BDSFieldType::bmap3d,
                                     1, // beam brho - doesn't matter for field map
                                     BDSIntegratorType::g4classicalrk4,
                                     nullptr, // set of normalised magnet strengths - ignore
                                     false, // turn off global transform - won't work outside bdsim
                                     G4Transform3D(), // overall transform for field
                                     "3dexample.dat.gz",  // path to file
                                     BDSFieldFormat::bdsim3d, // file format
                                     BDSInterpolatorType::cubic3d);

  BDSFieldObjects* field;
  try
    {field = BDSFieldFactory::Instance()->CreateField(recipe);}
  catch (BDSException& e)
    {std::cout << e.what() << std::endl; return 1;}

  G4double point[4] = {0,10,20,0}; // x,y,z,t
  G4double fieldValue[6];
  field->GetField()->GetFieldValue(point, fieldValue);

  for (int i = 0; i < 6; i++)
    {std::cout << fieldValue[i] << " ";}
  std::cout << std::endl;
  // should update first 3 numbers of array
  
  //G4LogicalVolume* lv;
  //field->AttachToVolume(lv); // by default attaches to all daughters recursively
  
  return 0;
}
