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
#include "BDSBLM.hh"
#include "BDSBLMFactory.hh"
#include "BDSBLMType.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSExtent.hh"
#include "BDSGeometryExternal.hh"
#include "BDSGeometryFactory.hh"
#include "BDSGlobalConstants.hh"
#include "BDSMaterials.hh"
#include "BDSUtilities.hh"

#include "parser/blmplacement.h"

#include "globals.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4Orb.hh"
#include "G4Tubs.hh"

#include <map>

BDSBLMFactory::BDSBLMFactory()
{;}

BDSBLMFactory::~BDSBLMFactory()
{;}

void BDSBLMFactory::PositiveFinite(G4double value,
    const G4String& parameterName,
    const G4String& blmName) const
{
  if (!BDS::IsFinite(value) || value < 0)
    {throw BDSException(__METHOD_NAME__, "\"" + parameterName + "\" must be +ve finite for blm named \"" + blmName + "\"");}
}

BDSBLM* BDSBLMFactory::CreateBLM(const GMAD::BLMPlacement& bp,
    G4VSensitiveDetector* sd)
{
  return CreateBLM(G4String(bp.name),
                   G4String(bp.geometryFile),
                   G4String(bp.geometryType),
                   G4String(bp.blmMaterial),
                   bp.blm1 * CLHEP::m,
                   bp.blm2 * CLHEP::m,
                   bp.blm3 * CLHEP::m,
                   bp.blm4 * CLHEP::m,
                   sd,
                   bp.bias);
}

BDSBLM* BDSBLMFactory::CreateBLM(const G4String& name,
                                const G4String& geometryFile,
                                const G4String& geometryType,
                                const G4String& material,
                                G4double blm1,
                                G4double blm2,
                                G4double blm3,
                                G4double /*blm4*/,
                                G4VSensitiveDetector* sd,
                                const G4String& bias)
{
  // if geometry file is specified then we load the external file.
  BDSBLM* result = nullptr;
  if (!geometryFile.empty())
    {
      BDSGeometryExternal* blmGeom = BDSGeometryFactory::Instance()->BuildGeometry(name, geometryFile);
      result = new BDSBLM(blmGeom);
    }
  else
    {
      BDSBLMType shape = BDS::DetermineBLMType(geometryType);
      switch (shape.underlying())
        {
        case BDSBLMType::cylinder:
          {result = BuildBLMCylinder(name, material, blm1, blm2); break;}
        case BDSBLMType::cube:
          {result = BuildBLMCube(name, material, blm1, blm2, blm3); break;}
        case BDSBLMType::sphere:
          {result = BuildBLMSphere(name, material, blm1); break;}
        default:
          {break;}
        }
    }
  if (!result)
    {return result;}

  // attach sensitivity
  for (auto lv : result->GetAllLogicalVolumes())
    {lv->SetSensitiveDetector(sd);}
  result->GetContainerLogicalVolume()->SetSensitiveDetector(sd); // attach separately to the container

  // biasing - name of objects attached here and built later
  result->SetBias(bias);
  return result;
}

BDSBLM* BDSBLMFactory::BuildBLMCylinder(const G4String& name,
                                        const G4String& material,
                                        G4double        halfLength,
                                        G4double        radius)
{
  PositiveFinite(halfLength, "blm1 (half length)", name);
  PositiveFinite(radius,     "blm2 (radius)",      name);
  G4Tubs* shape = new G4Tubs(name + "_solid",
                             0,
                             radius,
                             halfLength,
                             0,
                             CLHEP::twopi);
  BDSExtent ext = BDSExtent(radius, radius, halfLength);
  return CommonConstruction(name, material, shape, ext);
}

BDSBLM* BDSBLMFactory::BuildBLMCube(const G4String& name,
                                    const G4String& material,
                                    G4double        halfLengthX,
                                    G4double        halfLengthY,
                                    G4double        halfLengthZ)
{
  PositiveFinite(halfLengthX, "blm1 (half length in x)", name);
  PositiveFinite(halfLengthY, "blm2 (half length in y)", name);
  PositiveFinite(halfLengthZ, "blm3 (half length in z)", name);
  G4Box* shape = new G4Box(name + "_solid", halfLengthX, halfLengthY, halfLengthZ);
  BDSExtent ext = BDSExtent(halfLengthX, halfLengthY, halfLengthZ);
  return CommonConstruction(name, material, shape, ext);
}

BDSBLM* BDSBLMFactory::BuildBLMSphere(const G4String& name,
                                      const G4String& material,
                                      G4double        radius)
{
  PositiveFinite(radius, "blm1 (radius)", name);
  G4Orb* shape = new G4Orb(name + "_solid", radius);
  BDSExtent ext = BDSExtent(radius, radius, radius);
  return CommonConstruction(name, material, shape, ext);
}

BDSBLM* BDSBLMFactory::CommonConstruction(const G4String&  name,
                                          const G4String&  material,
                                          G4VSolid*        shape,
                                          const BDSExtent& extent)
{
  G4Material* mat = BDSMaterials::Instance()->GetMaterial(material);
  G4LogicalVolume* lv = new G4LogicalVolume(shape, mat, name + "_lv");
  lv->SetUserLimits(BDSGlobalConstants::Instance()->DefaultUserLimits());
  BDSBLM* result = new BDSBLM(shape, lv, extent);
  return result;
}
