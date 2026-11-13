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
#ifndef BDSGAP_H
#define BDSGAP_H

#include "globals.hh"
#include "BDSAcceleratorComponent.hh"

/**
 * @brief Gap in accelerator beamline.
 * 
 * @author Will Shields
 */

class BDSGap: public BDSAcceleratorComponent
{
public:
  BDSGap(const G4String& nameIn,
         G4double lengthIn,
         G4double angleIn);
  virtual ~BDSGap();
  
protected:
  virtual void Build();
  
  virtual void BuildContainerLogicalVolume();

private:
  /// Private default constructor to force the use of the supplied one.
  BDSGap() = delete;

  /// @{ Assignment and copy constructor not implemented nor used
  BDSGap& operator=(const BDSGap&) = delete;
  BDSGap(BDSGap&) = delete;
  ///@}
};

#endif
