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
#ifndef BDSBLM_H
#define BDSBLM_H

#include "BDSExtent.hh"
#include "BDSGeometryComponent.hh"

#include "G4String.hh"

class G4LogicalVolume;
class G4VSolid;

/**
 * @brief A beam loss monitor.
 *
 * This is a wrapper class for BDSGeometryComponent. Currently,
 * this provides no additional members, but experience with beam pipes
 * and magnet outer geometry says that this class will undoubtedly be
 * required. Easier to implement now than rewrite all factories later.
 * 
 * @author Laurie Nevay
 */

class BDSBLM: public BDSGeometryComponent
{
public:
  /// No default constructor
  BDSBLM() = delete;

  /// Accept a component and register it as a daughter.
  BDSBLM(BDSGeometryComponent* geometry);

  /// This constructor is used when the BLM is a 1 volume BLM.
  BDSBLM(G4VSolid*        containerSolidIn,
         G4LogicalVolume* containerLVIn,
         BDSExtent        extentIn = BDSExtent());

  virtual ~BDSBLM(){;}

  /// Accessor.
  G4String Bias() const {return bias;}

  /// Use a setter function rather than add to the constructor as for now we
  /// can get away with using the base class constructor.
  void SetBias(const G4String& biasIn) {bias = biasIn;}

  /// We override this and do nothing so that the default energy deposition
  /// sensitivity isn't attached. We handle this explicitly in the BLM creation.
  virtual void AttachSensitiveDetectors(){;}

private:
  G4String bias = "";
};

#endif
