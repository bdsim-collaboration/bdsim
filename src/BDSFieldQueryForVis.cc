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
#include "BDSFieldQueryForVis.hh"
#include "BDSFieldQueryInfo.hh"

#include "G4ThreeVector.hh"
#include "G4Types.hh"

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <vector>


BDSFieldQueryForVis::BDSFieldQueryForVis():
  maxFieldB(0),
  maxFieldE(0),
  drawZeroValuePoints(false)
{;}

BDSFieldQueryForVis::~BDSFieldQueryForVis()
{;}

void BDSFieldQueryForVis::CleanUp()
{
  BDSFieldQuery::CleanUp();
  values.clear();
  maxFieldB = -std::numeric_limits<G4double>::max();
  maxFieldE = -std::numeric_limits<G4double>::max();
}

void BDSFieldQueryForVis::QueryField(const BDSFieldQueryInfo* query)
{
  G4int totalNPoints = query->xInfo.n * query->yInfo.n * query->zInfo.n * query->tInfo.n;
  values.reserve(totalNPoints);
  drawZeroValuePoints = query->drawZeroValuePoints;
  BDSFieldQuery::QueryField(query);
}

void BDSFieldQueryForVis::GetFieldValue(const G4ThreeVector& globalXYZ,
                                        const G4ThreeVector& globalDirection,
                                        G4double tGlobal,
                                        G4double fieldValue[6])
{
  BDSFieldQuery::GetFieldValue(globalXYZ, globalDirection, tGlobal, fieldValue);
  G4double bFieldMag = SimpleMag(fieldValue[0], fieldValue[1], fieldValue[2]);
  G4double eFieldMag = SimpleMag(fieldValue[3], fieldValue[4], fieldValue[5]);
  maxFieldB = std::max(maxFieldB, bFieldMag);
  maxFieldE = std::max(maxFieldE, eFieldMag);
  if ( (bFieldMag == 0) && (eFieldMag == 0) && !drawZeroValuePoints)
    {return;} // don't store 0 field points - we don't need to retain the structure or complete array shape
  values.emplace_back( std::array<G4double, 9>({globalXYZ.x(), globalXYZ.y(), globalXYZ.z(),
                                                fieldValue[0], fieldValue[1], fieldValue[2],
                                                fieldValue[3], fieldValue[4], fieldValue[5]}) );
}

G4double BDSFieldQueryForVis::SimpleMag(G4double x, G4double y, G4double z)
{
  return std::sqrt(x*x + y*y + z*z);
}