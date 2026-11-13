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
#include "BDSArray4D.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSFieldValue.hh"

#include "globals.hh" // geant4 types / globals

#include <ostream>
#include <string>
#include <vector>


BDSArray4D::BDSArray4D(G4int nXIn, G4int nYIn, G4int nZIn, G4int nTIn):
  nX(nXIn), nY(nYIn), nZ(nZIn), nT(nTIn),
  defaultValue(BDSFieldValue()),
  data(std::vector<BDSFieldValue>(nTIn*nZIn*nYIn*nXIn))
{;}

BDSFieldValue& BDSArray4D::operator()(G4int x,
				      G4int y,
				      G4int z,
				      G4int t)
{
  OutsideWarn(x,y,z,t); // keep as a warning as can't assign to invalid index
  return data[t*nZ*nY*nX + z*nY*nX + y*nX + x];
}

const BDSFieldValue& BDSArray4D::GetConst(G4int x,
					  G4int y,
					  G4int z,
					  G4int t) const
{
  if (Outside(x,y,z,t))
    {return defaultValue;}
  return data[t*nZ*nY*nX + z*nY*nX + y*nX + x];
}
  
const BDSFieldValue& BDSArray4D::operator()(G4int x,
					    G4int y,
					    G4int z,
					    G4int t) const
{
  return GetConst(x,y,z,t);
}

G4bool BDSArray4D::Outside(G4int x,
			   G4int y,
			   G4int z,
			   G4int t) const
{
  G4bool rx = x < 0 || x > nX-1;
  G4bool ry = y < 0 || y > nY-1;
  G4bool rz = z < 0 || z > nZ-1;
  G4bool rt = t < 0 || t > nT-1;
  return rx || ry || rz || rt;
}

void BDSArray4D::OutsideWarn(G4int x,
			     G4int y,
			     G4int z,
			     G4int t) const
{
  if (Outside(x,y,z,t))
    {
      throw BDSException(__METHOD_NAME__, "(" +
			 std::to_string(x) + ", " +
			 std::to_string(y) + ", " +
			 std::to_string(z) + ", " +
			 std::to_string(t) + ") is outside array");
    }
}

std::ostream& BDSArray4D::Print(std::ostream& out) const
{
  out << "#x = " << nX << ", ";
  out << "#y = " << nY << ", ";
  out << "#z = " << nZ << ", ";
  out << "#t = " << nT << G4endl;
  
  for (G4int t = 0; t < nT; t++)
    {
      for (G4int z = 0; z < nZ; z++)
	{
	  for (G4int y = 0; y < nY; y++)
	    {
	      for (G4int x = 0; x < nX; x++)
		{out << BDSArray4D::GetConst(x,y,z,t) << G4endl;}
	    }
	}
    }
  return out;
}

std::ostream& operator<< (std::ostream& out, BDSArray4D const &a)
{
  return a.Print(out);
}
