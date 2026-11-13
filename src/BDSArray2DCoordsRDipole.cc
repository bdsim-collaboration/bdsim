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
#include "BDSArray2DCoordsRDipole.hh"
#include "BDSFieldValue.hh"

#include "globals.hh"

#include <algorithm>
#include <cmath>
#include <ostream>


BDSArray2DCoordsRDipole::BDSArray2DCoordsRDipole(BDSArray2DCoords* arrayIn):
  BDSArray2DCoords(*arrayIn),
  returnValue(BDSFieldValue())
{;}

G4bool BDSArray2DCoordsRDipole::OutsideCoords(G4double x,
					      G4double y,
					      G4double z,
					      G4double t) const
{
  G4bool rx = x < -xMax || x > xMax;
  G4bool ry = y < -yMax || y > yMax;
  G4bool rz = z <  zMin || z > zMax;
  G4bool rt = t <  tMin || t > tMax;
  return rx || ry || rz || rt;
}

G4double BDSArray2DCoordsRDipole::ArrayCoordsFromX(G4double x) const
{
  // xmin becomes -xmax
  return (x + xMax) / xStep;
}

G4double BDSArray2DCoordsRDipole::ArrayCoordsFromY(G4double y) const
{
  return (y + yMax) / yStep;
}

G4int BDSArray2DCoordsRDipole::NearestX(G4double x) const
{
  return (G4int)round((x+xMax)/xStep);
}

G4int BDSArray2DCoordsRDipole::NearestY(G4double y) const
{
  return (G4int)round((y+yMax)/yStep);
}

const BDSFieldValue& BDSArray2DCoordsRDipole::GetConst(G4int x,
						       G4int y,
						       G4int z,
						       G4int t) const
{
  if (Outside(x,y,z,t))
    {return defaultValue;}

  // Data is really
  // A

  // This interface makes it look like (0,0) bottom left, top right (2*xsize, 2*ysize)
  // in array coords - ie int number of elements.
  // B | A
  // -----
  // C | D

  // factors to multiply result by - saves duplicate testing of which quadrant etc
  G4double xr = 1;
  G4double yr = 1;
  G4int    xi = x; // copy of x index that can be modified.
  G4int    yi = y; 
  
  if (x < nX)
    {
      if (y < nY)
	{// C quadrant - coordinate flip is a subtraction in array coords
	  xi = nX - 1 - x;
	  yi = nY - 1 - y;
	}
      else
	{// B quadrant
	  xi = nX - 1 - x; xr = -1; // flip
	  yi = y - nY;
	}
    }
  else
    {
      if (y < nY)
	{// D quadrant
	  xi = x - nX; xr = -1;// just offset index, flip x component
	  yi = nY - 1 - y;
	}
      else
	{// A quadrant
	  xi = x - nX;
	  yi = y - nY;
	}
    }

  returnValue = BDSArray2DCoords::GetConst(xi,yi,z,t);
  
  returnValue[0] = returnValue.x() * xr;
  returnValue[1] = returnValue.y() * yr;

  return returnValue;
}

G4bool BDSArray2DCoordsRDipole::Outside(G4int x,
					G4int y,
					G4int z,
					G4int t) const
{
  G4bool rx = x < 0 || x > 2*(nX-1);
  G4bool ry = y < 0 || y > 2*(nY-1);
  G4bool rz = z < 0 || z > nZ-1;
  G4bool rt = t < 0 || t > nT-1;
  return rx || ry || rz || rt;
}

std::ostream& BDSArray2DCoordsRDipole::Print(std::ostream& out) const
{
  std::ostream& out2 = BDSArray2DCoords::Print(out);

  out2 << G4endl << "REFLECTED VERSION" << G4endl;
  
  for (G4int t = 0; t < nT; t++)
    {
      for (G4int z = 0; z < nZ; z++)
	{
	  for (G4int y = 0; y < 2*nY; y++)
	    {
	      for (G4int x = 0; x < 2*nX; x++)
		{out2 << GetConst(x,y,z,t) << G4endl;}
	    }
	}
    }
  return out2;
}

std::ostream& operator<< (std::ostream& out, BDSArray2DCoordsRDipole const &a)
{
  return a.Print(out);
}
