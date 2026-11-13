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
#ifndef BDSCAVITYELEMENT_H
#define BDSCAVITYELEMENT_H

#include "BDSAcceleratorComponent.hh"

#include "globals.hh" // geant4 globals / types

class BDSCavityInfo;
class BDSFieldInfo;
class G4Material;

/**
 * @brief RF Cavity.  Uses factories to construct appropriate geometry.
 * 
 * @author Stuart Walker
 */

class BDSCavityElement: public BDSAcceleratorComponent
{
public:
  BDSCavityElement(const G4String&       name,
		   G4double             chordLength,
		   G4Material*          vacuumMaterialIn,
		   const BDSFieldInfo*  vacuumField,
		   const BDSCavityInfo* cavityInfoIn);

  virtual ~BDSCavityElement();

  inline const BDSFieldInfo* GetFieldInfo() const {return vacuumFieldInfo;}
  inline const BDSCavityInfo* GetCavityInfo() const {return cavityInfo;}

protected:
  /// Provided to fulfill BDSAcceleratorComponent requirement. Doesn't do anything.
  /// All construction is in Build.
  virtual void BuildContainerLogicalVolume() override;

  /// The full construction sequence that calls all methods in order
  virtual void Build() override;
  
  /// Creates field objects - doesn't nothing by default and derived classes can override.
  virtual void BuildField();

  /// Recipe for field.
  const BDSFieldInfo* vacuumFieldInfo;

  /// Convenience shortcut to cavity information inside field information object.
  const BDSCavityInfo* cavityInfo;
  
private:
  /// Private constructor to force the use of the provided one.
  BDSCavityElement() = delete;

  /// @{ Assignment and copy constructor not implemented nor used
  BDSCavityElement& operator=(const BDSCavityElement&) = delete;
  BDSCavityElement(BDSCavityElement&) = delete;
  /// @}

  /// Cache of material to be used for vacuum - does not own it.
  G4Material* vacuumMaterial;
};

#endif
