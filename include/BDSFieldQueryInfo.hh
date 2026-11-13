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
#ifndef BDSFIELDQUERYINFO_H
#define BDSFIELDQUERYINFO_H

#include "BDSFourVector.hh"

#include "G4AffineTransform.hh"
#include "G4String.hh"
#include "G4Types.hh"

#include <vector>

/**
 * @brief Holder class for all information required for a field query.
 * 
 * @author Laurie Nevay
 */

class BDSFieldQueryInfo
{
public:
  struct QueryDimensionInfo
  {
    /// Because we initialise the values in the declaration here, we must provide
    /// a constructor for C++ 11 only (as the initialisation means it's non-aggregate).
    QueryDimensionInfo(G4int nIn, G4double minIn, G4double maxIn):
      n(nIn), min(minIn), max(maxIn)
    {;}
    QueryDimensionInfo(): n(1), min(0), max(0) {;}
    G4int n = 1;
    G4double min = 0;
    G4double max = 0;
    G4double StepSize() const {return (max - min)/n;}
  };

  /// Usual constructor with number of points to query in each dimension.
  BDSFieldQueryInfo(const G4String& nameIn,
                    const G4String& outfileMagneticIn,
                    const G4String& outfileElectricIn,
                    G4bool queryMagneticIn,
                    G4bool queryElectricIn,
                    QueryDimensionInfo xInfoIn,
                    QueryDimensionInfo yInfoIn,
                    QueryDimensionInfo zInfoIn,
                    QueryDimensionInfo tInfoIn,
                    const G4AffineTransform& globalTransformIn = G4AffineTransform(),
                    G4bool overwriteExistingFilesIn = false,
                    const G4String& fieldObjectIn = "",
                    G4bool printTransformIn = false,
                    G4bool checkParametersIn = true,
                    G4bool drawArrowsIn = true,
                    G4bool drawZeroValuePointsIn = true,
                    G4bool drawBoxesIn = true,
                    G4double boxAlphaIn = 0.2);

  /// Alternative constructor with list of exact points to query.
  BDSFieldQueryInfo(const G4String& nameIn,
                    const G4String& outfileMagneticIn,
                    const G4String& outfileElectricIn,
                    G4bool queryMagneticIn,
                    G4bool queryElectricIn,
		    const std::vector<BDSFourVector<G4double>>& pointsToQueryIn,
		    const std::vector<G4String>& pointsColumnNamesIn,
		    G4bool overwriteExistingFilesIn = false,
		    const G4String& fieldObjectIn = "",
		    G4bool checkParametersIn = true,
                    G4bool drawArrowsIn = true,
		    G4bool drawZeroValuePointsIn = true,
		    G4bool drawBoxesIn = true,
		    G4double boxAlphaIn = 0.2);
  ~BDSFieldQueryInfo();
  
  G4String name;
  G4String outfileMagnetic;
  G4String outfileElectric;
  G4bool   queryMagnetic;
  G4bool   queryElectric;
  QueryDimensionInfo xInfo;
  QueryDimensionInfo yInfo;
  QueryDimensionInfo zInfo;
  QueryDimensionInfo tInfo;

  std::vector<BDSFourVector<G4double>> pointsToQuery;
  std::vector<G4String> pointsColumnNames;
  
  G4AffineTransform globalTransform;
  
  G4bool overwriteExistingFiles;
  G4bool printTransform;

  G4String fieldObject; ///< Optional for use in interpolator.
  
  G4bool checkParameters; ///< For internal testing use only.
  
  G4bool drawArrows;
  G4bool drawZeroValuePoints;
  G4bool drawBoxes;
  G4double boxAlpha;
  
  /// Whether to query a specific set of points.
  G4bool SpecificPoints() const {return !pointsToQuery.empty();}
};

#endif
