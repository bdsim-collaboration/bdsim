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
#ifndef BDSFIELDLOADERPOISSON_H
#define BDSFIELDLOADERPOISSON_H

#include "G4String.hh"

#ifdef USE_GZSTREAM
#include "src-external/gzstream/gzstream.h"
#endif

class BDSArray2DCoords;

/**
 * @brief Loader for 2D Poisson SuperFish SF7 files.
 * 
 * @author Laurie Nevay
 */

template <class T>
class BDSFieldLoaderPoisson
{
public:
  BDSFieldLoaderPoisson();
  ~BDSFieldLoaderPoisson();

  /// Load the 2D array of 3-Vector field values.
  BDSArray2DCoords* LoadMag2D(G4String fileName);

private:
  /// Templated iostream for std::ifstream and gzstream as well.
  T file;
};

#endif
