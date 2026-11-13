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
#include "BDSGeometryFormat.hh"
#include "BDSDebug.hh"

BDSGeometryFormat::BDSGeometryFormat()
{ 
  allowedSpecs();
}

BDSGeometryFormat::~BDSGeometryFormat()
{;}

BDSGeometryFormat::BDSGeometryFormat(G4String format)
{
  allowedSpecs();
  spec(format);
}

void BDSGeometryFormat::allowedSpecs(){
  addAllowedSpec((G4String)"none");
  addAllowedSpec((G4String)"gmad");
  addAllowedSpec((G4String)"mokka");
#ifdef USE_LCDD
  addAllowedSpec((G4String)"lcdd");
#endif
#ifdef USE_GDML
  addAllowedSpec((G4String)"gdml");
#endif
}

/*
G4bool BDSGeometryFormat::compare(G4String val) const{
  BDSGeometryFormat* testf = new BDSGeometryFormat(val);
  G4bool result = compare(testf);
  delete testf;
  return result;
}
*/
