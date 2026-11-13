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
#ifndef BDSGEOMETRY_H
#define BDSGEOMETRY_H

#include "BDSGeometryType.hh"
#include "BDSMagFieldMesh.hh"

#include "globals.hh" // geant4 types / globals
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4MagneticField.hh"

#include <map>
#include <vector>

class BDSGeometry
{
public:
  BDSGeometry();
  virtual ~BDSGeometry();
  BDSGeometry(BDSGeometryType format, G4String file);

  /// Delegated initialisation of variables for multiple constructors (saves duplication).
  void Init();



  virtual void Construct(G4LogicalVolume* containerLogicalVolume) = 0;

  void SetMultiplePhysicalVolumes(G4VPhysicalVolume* aPhysVol);
  std::vector<G4LogicalVolume*>   GetSensitiveComponents();
  std::vector<G4VPhysicalVolume*> GetMultiplePhysicalVolumes();
  std::vector<G4LogicalVolume*>   GetGFlashComponents();

  G4VPhysicalVolume* GetAlignOutVolume() const;
  G4VPhysicalVolume* GetAlignInVolume()  const;
  BDSGeometryType    Format()            const;
  G4String           File()              const;
  G4String           ContainingDir()     const;
  G4bool             HasFields()         const;
  G4int              NPoleField()        const;
  G4bool             HasUniformField()   const;
  G4double           Length()            const;

  std::map<G4String, G4ThreeVector> UniformFieldVolField();
  std::map<G4String, G4double> QuadVolBgrad();
  std::map<G4String, G4double> SextVolBgrad();
  std::map<G4String, G4double> OctVolBgrad();

  G4MagneticField* Field();

protected:  
  std::vector<G4LogicalVolume*>   sensitiveComponents;
  std::vector<G4LogicalVolume*>   gFlashComponents;
  std::vector<G4VPhysicalVolume*> multiplePhysicalVolumes;

  G4VPhysicalVolume* alignInVolume;
  G4VPhysicalVolume* alignOutVolume;

  G4bool   hasFields;
  G4int    nPoleField;
  G4bool   hasUniformField;
  G4double length; //overall length in z.

  std::map<G4String, G4ThreeVector> uniformFieldVolField;
  std::map<G4String, G4double> quadVolBgrad;
  std::map<G4String, G4double> sextVolBgrad;
  std::map<G4String, G4double> octVolBgrad;

  BDSMagFieldMesh* field;

private:
  BDSGeometryType format;
  G4String        file;
  G4String        containingDir;
};

inline void BDSGeometry::SetMultiplePhysicalVolumes(G4VPhysicalVolume* aPhysVol)
{multiplePhysicalVolumes.push_back(aPhysVol);}

inline  std::vector<G4LogicalVolume*> BDSGeometry::GetSensitiveComponents()
{return sensitiveComponents;}

inline  std::vector<G4VPhysicalVolume*> BDSGeometry::GetMultiplePhysicalVolumes()
{return multiplePhysicalVolumes;}

inline  std::vector<G4LogicalVolume*> BDSGeometry::GetGFlashComponents()
{return gFlashComponents;}

inline  G4VPhysicalVolume* BDSGeometry::GetAlignInVolume() const
{return alignInVolume;}

inline  G4VPhysicalVolume* BDSGeometry::GetAlignOutVolume() const
{return alignOutVolume;}

inline  G4bool BDSGeometry::HasFields() const
{return hasFields;}

inline  G4int BDSGeometry::NPoleField() const
{return nPoleField;}

inline  G4bool BDSGeometry::HasUniformField() const
{return hasUniformField;}

inline G4double BDSGeometry::Length() const
{return length;}

inline std::map<G4String, G4ThreeVector> BDSGeometry::UniformFieldVolField()
{return uniformFieldVolField;}

inline std::map<G4String, G4double> BDSGeometry::QuadVolBgrad()
{return quadVolBgrad;}

inline std::map<G4String, G4double> BDSGeometry::SextVolBgrad()
{return sextVolBgrad;}

inline std::map<G4String, G4double> BDSGeometry::OctVolBgrad()
{return octVolBgrad;}

inline  G4MagneticField* BDSGeometry::Field()
{return field;}

inline BDSGeometryType BDSGeometry::Format() const
{return format;}

inline G4String BDSGeometry::File() const
{return file;}

inline G4String BDSGeometry::ContainingDir() const
{return containingDir;}

#endif
