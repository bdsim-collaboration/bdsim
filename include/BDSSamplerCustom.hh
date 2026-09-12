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
#ifndef BDSSAMPLERCUSTOM_H
#define BDSSAMPLERCUSTOM_H 

#include "BDSSampler.hh"

#include "globals.hh" // geant4 types / globals

class BDSApertureInfo;

/** 
 * @brief Custom shaped sampler with fixed thickness.
 * 
 * Does not retain ownership of BDSApertureInfo instance.
 * 
 * @author Laurie Nevay
 */

class BDSSamplerCustom: public BDSSampler
{
public:
  BDSSamplerCustom() = delete; ///< No default constructor.
  BDSSamplerCustom(const G4String&        name,
		   const BDSApertureInfo& shape,
                   G4int filterSetIDIn = -1);
  
  /// @{ Assignment and copy constructor not implemented nor used.
  BDSSamplerCustom& operator=(const BDSSamplerCustom&) = delete;
  BDSSamplerCustom(BDSSamplerCustom&) = delete;
  /// @}

  virtual ~BDSSamplerCustom(){;}
  
  /// Access the sampler plane length in other classes.
  inline static G4double ChordLength() {return chordLength;}
  
  static G4double chordLength;
};

#endif
