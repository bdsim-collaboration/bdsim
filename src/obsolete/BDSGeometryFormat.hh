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
#ifndef __BDSGEOMETRYFORMAT_HH
#define __BDSGEOMETRYFORMAT_HH

#include "BDSSpec.hh"
#include <list>

class BDSGeometryFormat: public BDSSpec{
public:
  BDSGeometryFormat();
  ~BDSGeometryFormat();
  BDSGeometryFormat(G4String format);
  //  virtual G4bool compare(G4String val) const; //Compare to string
  
private:
  virtual void allowedSpecs();
  
};
#endif


