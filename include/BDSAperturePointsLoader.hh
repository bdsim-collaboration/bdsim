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
#ifndef BDSAPERTUREPOINTSLOADER_H
#define BDSAPERTUREPOINTSLOADER_H
#include "BDSFourVector.hh"

#include "G4String.hh"
#include "G4TwoVector.hh"
#include "G4Types.hh"

#include <map>
#include <vector>

/**
 * @brief A loader for up to set of XY points for an aperture.
 *
 * Templated for both regular and gzip reading. This will use
 * a cache of files and reuse the same points if asked again
 * for the same file.
 *
 * The cache (see next class in this header) owns the points vectors.
 * 
 * @author Laurie Nevay
 */

template <class T>
class BDSAperturePointsLoader
{
public:
  BDSAperturePointsLoader();
  ~BDSAperturePointsLoader();

  /// Main interface. Load a file name and multiply the X,Y coordinates
  /// by the value of unit. If this is 1.0 by default then it means mm.
  std::vector<G4TwoVector>* Load(const G4String& fileName,
                                 G4double unit = 1.0) const;
};

/**
 * @brief A holder for loaded aperture points files.
 * 
 * This is a separate class from the loader as the loader
 * is templated and this would result in a different cache
 * for each format loaded.
 * 
 * Singleton.
 *
 * @author Laurie Nevay
 */

class BDSAperturePointsCache
{
public:
  /// Access the singleton instance.
  static BDSAperturePointsCache* Instance();
  
  ~BDSAperturePointsCache();

  /// Delete all cached points from memory and clear the map of files loaded.
  void ClearCachedFiles();

  /// Retrieve a cached files. Will return nullptr if not found.
  std::vector<G4TwoVector>* FindCachedFile(const G4String& fileNameAndUnits) const;

  /// Add an entry to the cache.
  void CacheFile(const G4String& fileNameAndUnits,
                 std::vector<G4TwoVector>* contents);
  
private:
  BDSAperturePointsCache() = default;
  
  static BDSAperturePointsCache* instance;
  
  std::map<G4String, std::vector<G4TwoVector>*> cachedFiles;
};

namespace BDS
{
  /// Detect whether gzipped file or not, instantiate right template and load the data.
  /// This will expand the path to the full one and check it exists and throw an exception
  /// if it doesn't.
  std::vector<G4TwoVector>* LoadAperturePoints(const G4String& fileName,
                                               const G4String& unit = "");
}

#endif
