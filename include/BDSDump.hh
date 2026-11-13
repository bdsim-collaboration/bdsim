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
#ifndef BDSDUMP_H
#define BDSDUMP_H

#include "globals.hh"

#include "BDSAcceleratorComponent.hh"

/**
 * @brief An infinite absorber.
 *
 * @author Laurie Nevay
 */

class BDSDump: public BDSAcceleratorComponent
{
public:
  BDSDump(G4String nameIn, 
	  G4double lengthIn,
	  G4double horizontalWidthIn,
	  G4bool   circular = false);
  
  virtual ~BDSDump();

  /// Override base class version and return beam pipe material if it exists.
  virtual G4String Material() const override {return "infiniteabsorber";}

protected:
  /// Call default build then override visualisation attributes.
  virtual void Build() override;

  /// Build a simple box or cylinder.
  virtual void BuildContainerLogicalVolume() override;

  /// Customised user limits.
  virtual void BuildUserLimits() override;

private:
  /// No default constructor.
  BDSDump() = delete;

  G4double horizontalWidth;
  G4bool   circular;
  
  /// @{ Assignment and copy constructor not implemented nor used
  BDSDump& operator=(const BDSDump&) = delete;
  BDSDump(BDSDump&) = delete;
  /// @}
};

#endif
