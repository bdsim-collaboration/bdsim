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
#ifndef BDSFIELDLOADERQUERYPOINTS_H
#define BDSFIELDLOADERQUERYPOINTS_H
#include "BDSFourVector.hh"

#include "G4Types.hh"

#include <vector>

/**
 * @brief A loader for up to 4D points for querying fields.
 * 
 * @author Laurie Nevay
 */

template <class T>
class BDSFieldLoaderQueryPoints
{
public:
  BDSFieldLoaderQueryPoints();
  ~BDSFieldLoaderQueryPoints();

  std::vector<BDSFourVector<G4double>> Load(const G4String& fileName,
					    std::vector<G4String>* columnNamesIn) const;
};

namespace BDS
{
  /// Detect whether gzipped file or not, instantiate right template and load the data.
  /// This will expand the path to the full one and check it exists and throw an exception
  /// if it doesn't.
  std::vector<BDSFourVector<G4double>> LoadFieldQueryPoints(const G4String& fileName,
							    std::vector<G4String>* columnNamesIn);
}

#endif
