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
#ifndef BDSTERMINATOR_H
#define BDSTERMINATOR_H

#include "BDSAcceleratorComponent.hh"

/** 
 * @brief Class for small control volume for circular macines.
 * 
 * Controls particles with dynamic user limits
 * that change based on the global constants parameter nturns. Used
 * to control the nunmber of tunrs a primary particle completes in 
 * the machine.
 *
 * It has the same dimensions as a sampler - taken from BDSSamplerPlane.hh.
 * 
 * @author Laurie Nevay
 */

class BDSTerminator: public BDSAcceleratorComponent
{
public:
  BDSTerminator(G4double widthIn);
  virtual ~BDSTerminator();

private:
  /// @{ Assignment and copy constructor not implemented nor used
  BDSTerminator& operator=(const BDSTerminator&) = delete;
  BDSTerminator(BDSTerminator&) = delete;
  /// @}

  /// Override this function in BDSAcceleratorComponent purely to avoid
  /// BDSAcceleratorComponent replacing the user limits on the terminator.
  virtual void Build();

  /// The terminator's full horizontal width
  G4double horizontalWidth;

  /// Construct a simple box and attach an instance of the customised
  /// BDSTerminatorUserLimits() to it.
  virtual void BuildContainerLogicalVolume();
};

#endif
