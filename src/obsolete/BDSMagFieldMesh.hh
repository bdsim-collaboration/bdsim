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
#ifndef BDSMAGFIELDMESH_H
#define BDSMAGFIELDMESH_H

#include "G4MagneticField.hh"
#include "G4CachedMagneticField.hh"
#include "globals.hh"               // geant4 globals / types
#include "G4RotationMatrix.hh"

class G4VPhysicalVolume;

/**
 * @brief An abstract wrapper for G4MagneticField that provides the ability for a 
 * field map supplied in local cartesian coordinates to global cartesian
 * coordinates as required by Geant4 integrators. 
 * 
 * @author Lawrence Deacon <lawrence.deacon@ucl.ac.uk>
 */

class BDSMagFieldMesh: public G4MagneticField
{
public:
  BDSMagFieldMesh();  
  ~BDSMagFieldMesh();

  /// Convert the local coordinates of the provided field mesh into global coordinates.
  virtual void Prepare(G4VPhysicalVolume* referenceVolume);

  void SetOriginRotation(G4RotationMatrix* rot);
  void SetOriginRotation(G4RotationMatrix rot);
  void SetOriginTranslation(G4ThreeVector trans);

  virtual void   GetFieldValue( const G4double point[4],
				G4double* bField) const;
  virtual G4bool HasNPoleFields();
  virtual G4bool HasUniformField();
  virtual G4bool HasFieldMap();

  G4RotationMatrix Rotation() const;

  const G4String GetBFile();
  const G4String GetBFormat();
  
protected:
  void CheckPrepared() const;

  G4ThreeVector     translation;
  G4bool            isPrepared;
  
private:

  G4RotationMatrix* rotation;  
};


#endif
