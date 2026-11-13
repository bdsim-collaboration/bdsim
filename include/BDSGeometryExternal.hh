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
#ifndef BDSGEOMETRYEXTERNAL_H
#define BDSGEOMETRYEXTERNAL_H

#include "BDSGeometryComponent.hh"

#include <set>

class G4LogicalVolume;

/**
 * @brief A loaded piece of externally provided geometry.
 *
 * This is a wrapper class for BDSGeometryComponent. Currently,
 * this provides no additional members, but experience with beam pipes
 * and magnet outer geometry says that this class will undoubtedly be
 * required. Easier to implement now than rewrite all factories later.
 * 
 * @author Laurie Nevay
 */

class BDSGeometryExternal: public BDSGeometryComponent
{
public:
  /// No default constructor
  BDSGeometryExternal() = delete;
  /// Use constructors of BDSGeometryComponent
  using BDSGeometryComponent::BDSGeometryComponent;

  virtual ~BDSGeometryExternal(){;}

  /// Register a set of volumes to be identified as vacuum volumes for the BDSAcceleratorComponent.
  void RegisterVacuumVolumes(const std::set<G4LogicalVolume*>& vacuumVolumesIn){vacuumVolumes = vacuumVolumesIn;}
  /// Access the vacuum volumes.
  const std::set<G4LogicalVolume*>& VacuumVolumes() const {return vacuumVolumes;}

private:
  std::set<G4LogicalVolume*> vacuumVolumes;
};

#endif
