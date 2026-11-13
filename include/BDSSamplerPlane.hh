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
#ifndef BDSSAMPLERPLANE_H
#define BDSSAMPLERPLANE_H 

#include "BDSSampler.hh"

#include "globals.hh" // geant4 types / globals

/** 
 * @brief Rectangular sampler with fixed thickness but variable x,y.
 * 
 * This produces a square box that's 4e-8m thick but has square half
 * width of boxHalfWidth. This thickness is chosen to be just above
 * the geometry tolerance set in bdsim.cc.  If any dimension is smaller 
 * than the geometry tolerance, Geant4 will exit.
 */

class BDSSamplerPlane: public BDSSampler
{
public:
  BDSSamplerPlane() = delete; ///< No default constructor.
  BDSSamplerPlane(const G4String& name,
		  G4double boxHalfWidth,
                  G4int filterSetIDIn = -1);
  /// @{ Assignment and copy constructor not implemented nor used.
  BDSSamplerPlane& operator=(const BDSSamplerPlane&) = delete;
  BDSSamplerPlane(BDSSamplerPlane&) = delete;
  /// @}

  virtual ~BDSSamplerPlane(){;}

  /// Access the sampler plane length in other classes.
  inline static G4double ChordLength() {return chordLength;}

  /// The chord length for all is fixed and can be static.
  static G4double chordLength;
};

#endif
