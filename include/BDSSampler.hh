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
#ifndef BDSSAMPLER_H
#define BDSSAMPLER_H

#include "BDSGeometryComponent.hh"

#include "globals.hh" // geant4 types / globals

class BDSApertureInfo;
class BDSBeamPipe;

/**
 * @brief Base class and registry of sampler instances.
 * 
 * @author Laurie Nevay
 */

class BDSSampler: public BDSGeometryComponent
{
public:
  explicit BDSSampler(const G4String& nameIn,
                      G4int filterSetIDIn = -1);
  BDSSampler() = delete; ///< No default constructor.

  /// @{ Assignment and copy constructor not implemented nor used
  BDSSampler& operator=(const BDSSampler&) = delete;
  BDSSampler(BDSSampler&) = delete;
  /// @}
  virtual ~BDSSampler(){;}

  /// @{ Accessor.
  virtual inline G4String GetName() const {return name;}
  inline G4int GetFilterSetID() const {return filterSetID;}
  /// @}
  
  /// Normally the sampler has nullptr material. But, in case we use it in the mass
  /// world, we should set a valid material. This function sets it to G4_GALACTIC.
  void MakeMaterialValidForUseInMassWorld();
  
protected:
  /// Common construction tasks such as creating a logical volume from the solid
  /// and visualisation options.
  virtual void CommonConstruction();
  
  /// Attach sensitive detector to containerLogicalVolume.
  virtual void SetSensitivity();
  
  const G4String name;
  const G4int    filterSetID;
};

#endif
