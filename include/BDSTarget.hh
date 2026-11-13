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
#ifndef BDSTARGET_H
#define BDSTARGET_H

#include "BDSCollimator.hh"

class G4Colour;
class G4Material;

/**
 * @brief A class for a box or cylinder piece of 1 material.
 * 
 * Uses a rectangular collimator with no aperture.
 * 
 * @author Laurie Nevay
 */

class BDSTarget: public BDSCollimator
{
public:
  BDSTarget(const G4String& name,
	    G4double    length,
	    G4double    horizontalWidth,
	    G4Material* collimatorMaterial,
	    G4Colour*   colour        = nullptr,
	    G4bool      circularOuter = false);
  virtual ~BDSTarget(){;};

  virtual void BuildInnerCollimator() {;}

private:
  /// Private default constructor to force the use of the supplied one.
  BDSTarget();

  /// @{ Assignment and copy constructor not implemented nor used
  BDSTarget& operator=(const BDSTarget&) = delete;
  BDSTarget(BDSTarget&) = delete;
  ///@}
};

#endif
