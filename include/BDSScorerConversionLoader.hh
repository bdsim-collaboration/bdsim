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
#ifndef BDSSCORERCONVERSIONLOADER_H
#define BDSSCORERCONVERSIONLOADER_H

#include "G4String.hh"
#include "G4DataVector.hh"
#include "G4PhysicsVector.hh"

#ifdef USE_GZSTREAM
#include "src-external/gzstream/gzstream.h"
#endif

/**
 * @brief Loader for scoring conversion tables as function of energy.
 *
 * This loads a file with expected two columns of energy and a numerical
 * value.
 *
 * @author Laurie Nevay
 */

template <class T>
class BDSScorerConversionLoader
{
public:
  BDSScorerConversionLoader();
  ~BDSScorerConversionLoader();

  /// Load the file.
  G4PhysicsVector* Load(const G4String& fileName,
			G4bool          silent=false);
private:

  /// Templated iostream for std::ifstream and gzstream as well
  T file;
};

#endif
