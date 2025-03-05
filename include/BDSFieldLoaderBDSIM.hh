/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2024.

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
#ifndef BDSFIELDLOADERBDSIM_H
#define BDSFIELDLOADERBDSIM_H

#include "BDSDimensionType.hh"
#include "BDSFieldValue.hh"

#include "G4String.hh"

#include <map>
#include <string>
#include <vector>

#ifdef USE_GZSTREAM
#include "src-external/gzstream/gzstream.h"
#endif

class BDSArray4DCoords;
class BDSArray3DCoords;
class BDSArray2DCoords;
class BDSArray1DCoords;

/**
 * @brief Loader for BDSIM format fields.
 *
 * Loaders are given through specific public methods so the exact
 * type can be returned.
 *
 * The input stream type is templated.
 *
 * This can exit the program if an invalid specification is loaded.
 * 
 * @author Laurie Nevay
 */

template <class T>
class BDSFieldLoaderBDSIM
{
public:
  BDSFieldLoaderBDSIM();
  ~BDSFieldLoaderBDSIM();
  
  BDSArray4DCoords* Load4D(const G4String& fileName); ///< Load a 4D array.
  BDSArray3DCoords* Load3D(const G4String& fileName); ///< Load a 3D array.
  BDSArray2DCoords* Load2D(const G4String& fileName); ///< Load a 2D array.
  BDSArray1DCoords* Load1D(const G4String& fileName); ///< Load a 1D array.

private:
  /// Ensure any member variables are reset between usages.
  void CleanUp();

  /// Close file and exit program in case of an error.
  void Terminate(const G4String& message = "");

  /// General loader for any number of dimensions.
  void Load(const G4String& fileName,
	    const unsigned int nDim);

  /// Process on line of data. Index of 0 corresponds to a default value of field of 0.
  /// This allows various dimensional loading to use the same function.
  void ProcessData(const std::string& line,
                   const unsigned long xIndex,
                   const unsigned long yIndex,
                   const unsigned long zIndex,
                   unsigned long long currentLineNumber);

  /// Templated iostream for std::ifstream and gzstream as well
  T file;
  
  /// Number of columns to read.
  unsigned long                nColumns;  ///< Number of columns to read.
  std::vector<G4float>         lineData;  ///< Vector to store line data during loading.
  BDSFieldValue                fv;        ///< Temporary field value.
  std::map<G4String, double>   header;    ///< Header keys and values.
  BDSArray4DCoords*            result;    ///< Resultant array from loading.
  G4String                     loopOrder; ///< String for looping order when loading.
  const std::vector<G4String>  headerMustBePositiveKeys;
  unsigned long indexOfFirstFieldValue;

  struct DimensionKeys
  {
    G4String number;
    G4String min;
    G4String max;
  };
  std::map<BDSDimensionType, DimensionKeys> dimKeyMap;
  std::map<BDSDimensionType, G4double> dimUnitsMap;
};

#endif
