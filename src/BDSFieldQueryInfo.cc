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
#include "BDSFieldQueryInfo.hh"

BDSFieldQueryInfo::BDSFieldQueryInfo(const G4String& nameIn,
                                     const G4String& outfileMagneticIn,
                                     const G4String& outfileElectricIn,
                                     G4bool queryMagneticIn,
                                     G4bool queryElectricIn,
                                     QueryDimensionInfo xInfoIn,
                                     QueryDimensionInfo yInfoIn,
                                     QueryDimensionInfo zInfoIn,
                                     QueryDimensionInfo tInfoIn,
                                     const G4AffineTransform& globalTransformIn,
                                     G4bool overwriteExistingFilesIn,
                                     const G4String& fieldObjectIn,
                                     G4bool printTransformIn,
                                     G4bool checkParametersIn,
                                     G4bool drawArrowsIn,
                                     G4bool drawZeroValuePointsIn,
                                     G4bool drawBoxesIn,
                                     G4double boxAlphaIn):
  name(nameIn),
  outfileMagnetic(outfileMagneticIn),
  outfileElectric(outfileElectricIn),
  queryMagnetic(queryMagneticIn),
  queryElectric(queryElectricIn),
  xInfo(xInfoIn),
  yInfo(yInfoIn),
  zInfo(zInfoIn),
  tInfo(tInfoIn),
  globalTransform(globalTransformIn),
  overwriteExistingFiles(overwriteExistingFilesIn),
  printTransform(printTransformIn),
  fieldObject(fieldObjectIn),
  checkParameters(checkParametersIn),
  drawArrows(drawArrowsIn),
  drawZeroValuePoints(drawZeroValuePointsIn),
  drawBoxes(drawBoxesIn),
  boxAlpha(boxAlphaIn)
{;}

BDSFieldQueryInfo::BDSFieldQueryInfo(const G4String& nameIn,
                                     const G4String& outfileMagneticIn,
                                     const G4String& outfileElectricIn,
                                     G4bool queryMagneticIn,
                                     G4bool queryElectricIn,
                                     const std::vector<BDSFourVector<G4double>>& pointsToQueryIn,
                                     const std::vector<G4String>& pointsColumnNamesIn,
                                     G4bool overwriteExistingFilesIn,
                                     const G4String& fieldObjectIn,
                                     G4bool checkParametersIn,
                                     G4bool drawArrowsIn,
                                     G4bool drawZeroValuePointsIn,
                                     G4bool drawBoxesIn,
                                     G4double boxAlphaIn):
  name(nameIn),
  outfileMagnetic(outfileMagneticIn),
  outfileElectric(outfileElectricIn),
  queryMagnetic(queryMagneticIn),
  queryElectric(queryElectricIn),
  pointsToQuery(pointsToQueryIn),
  pointsColumnNames(pointsColumnNamesIn),
  overwriteExistingFiles(overwriteExistingFilesIn),
  printTransform(false),
  fieldObject(fieldObjectIn),
  checkParameters(checkParametersIn),
  drawArrows(drawArrowsIn),
  drawZeroValuePoints(drawZeroValuePointsIn),
  drawBoxes(drawBoxesIn),
  boxAlpha(boxAlphaIn)
{;}

BDSFieldQueryInfo::~BDSFieldQueryInfo()
{;}