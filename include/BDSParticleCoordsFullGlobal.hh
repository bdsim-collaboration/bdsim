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
#ifndef BDSPARTICLECOORDSFULLGLOBAL_H
#define BDSPARTICLECOORDSFULLGLOBAL_H 

#include "BDSParticleCoords.hh"
#include "BDSParticleCoordsFull.hh"

#include "G4Types.hh"

#include <ostream>

/**
 * @brief A set of particle coordinates in both local and global.
 *
 * The beam line index is an optional index for convenience representing
 * the index in the beam line
 * 
 * @author Laurie Nevay
 */

class BDSParticleCoordsFullGlobal
{
public:
  BDSParticleCoordsFullGlobal();
  BDSParticleCoordsFullGlobal(const BDSParticleCoordsFull& localIn,
			      const BDSParticleCoords&     globalIn);
  ~BDSParticleCoordsFullGlobal(){;}
  
  /// Output stream
  friend std::ostream& operator<< (std::ostream& out, BDSParticleCoordsFullGlobal const& p);
  
  BDSParticleCoordsFull local;
  BDSParticleCoords     global;
  G4int                 beamlineIndex; ///< Optional index for where transform was found.
};

#endif
