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
#include "BDSBin.hh"

#include "globals.hh"

#include <ostream>
#include <utility>

BDSBin::BDSBin(G4double xMinIn, G4double xMaxIn):
  xmin(xMinIn), xmax(xMaxIn), total(0.0), sumWeightsSquared(0.0)
{;}

BDSBin BDSBin::operator+= (const G4double& weight)
{
  total += weight;
  sumWeightsSquared += weight*weight;
  return *this;
}

bool BDSBin::InRange(G4double x) const
{
  if ( x >= xmin && x < xmax)
    {return true;}
  else
    {return false;}
}

std::pair<G4double, G4double> BDSBin::GetXMeanAndTotal() const
{
  G4double xmean = (xmax + xmin)/2.0;
  return std::make_pair(xmean,total);
}

std::ostream& operator<< (std::ostream &out, BDSBin const &bin)
{
  return out << "(" << bin.xmin << " , " << bin.xmax << ") : " << bin.total;
}
