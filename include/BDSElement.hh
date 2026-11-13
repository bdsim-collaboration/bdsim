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
#ifndef BDSELEMENT_H
#define BDSELEMENT_H

#include "BDSAcceleratorComponent.hh"
#include "BDSGeometryExternal.hh"

#include "globals.hh"

#include <set>
#include <vector>

/**
 * @brief A class for a generic piece of external geometry.
 * 
 * Allows any arbitrary geometry and magnetic field map to be used
 * as an accelerator component in the beamline. Geometry and magnetic fields are imported
 * from an external file (each) and can be specified in various formats.
 */

class BDSElement: public BDSAcceleratorComponent
{
public:
  BDSElement() = delete;
  BDSElement(const G4String& name,
	     G4double        arcLength,
	     G4double        horizontalWidthIn,
	     const G4String& geometry,
	     G4double        angle                       = 0,
	     std::vector<G4String>* namedVacuumVolumesIn = nullptr,
	     G4bool          autoColourGeometryIn        = true,
	     G4bool          markAsCollimatorIn          = false,
         G4bool          stripOuterVolume            = false);
  virtual ~BDSElement(){;}

  /// @{ Assignment and copy constructor not implemented nor used
  BDSElement& operator=(const BDSElement&) = delete;
  BDSElement(BDSElement&) = delete;
  /// @}

  // This is a little convoluted because ultimately we can't change the
  // behaviour of the base class destructor, which will always be called
  // even if overridden. So if we want a BDSGeometryComponent that's uniquely
  // managed by the geometry factory (ie we don't own here) we have to forward
  // things on for the same access but no deletion.
  /// @{ Overloads of functions in BDSGeometryComponent.
  virtual std::set<G4VPhysicalVolume*> GetAllPhysicalVolumes()  const;
  virtual std::set<G4RotationMatrix*>  GetAllRotationMatrices() const;
  virtual std::set<G4VisAttributes*>   GetAllVisAttributes()    const;
  virtual std::set<G4UserLimits*>      GetAllUserLimits()       const;
  virtual std::set<BDSGeometryComponent*> GetAllDaughters()     const;
  virtual std::set<G4VSolid*>          GetAllSolids()           const;
  virtual std::set<G4LogicalVolume*>   GetAllLogicalVolumes()   const;
  virtual std::map<G4LogicalVolume*, BDSSDType> GetAllSensitiveVolumes() const;
  virtual std::set<G4LogicalVolume*>   GetAllBiasingVolumes()   const;
  virtual void ExcludeLogicalVolumeFromBiasing(G4LogicalVolume* lv);
  virtual void AttachSensitiveDetectors();
  /// @}

protected:
  /// Specialise the method so as not to not overwrite the vis attributes
  /// of the container volume that for external geometry are always made visible.
  virtual void Build();

  /// This does the full construction.  Loads the external geometry and field if there is one.
  virtual void BuildContainerLogicalVolume();
   
private:
  G4double horizontalWidth;
  G4String geometryFileName;
  std::vector<G4String> namedVacuumVolumes;
  G4bool   autoColourGeometry;
  G4bool   markAsCollimator;
  G4bool   stripOuterVolume;

  /// Cache of the constructed geometry.  Used to forward onto various BDSGeometryComponent functions.
  BDSGeometryExternal* geometry;
};

#endif
