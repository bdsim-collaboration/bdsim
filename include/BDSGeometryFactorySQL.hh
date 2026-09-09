/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2024.

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
#ifndef BDSGEOMETRYFACTORYSQL_H
#define BDSGEOMETRYFACTORYSQL_H

#include "BDSGeometryFactoryBase.hh"

#include "globals.hh"
#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"

#include <list>
#include <map>
#include <unordered_map>
#include <vector>

class BDSMySQLTable;

class G4Colour;
class G4LogicalVolume;
class G4Region;
class G4UserLimits;
class G4VisAttributes;

/**
 * @brief Geometry factory for SQL geometry.
 *
 * The extents are accumulated during the construction of the solids in a vector
 * then these are used to expand the extent of the full element using the placement
 * offset that's done at a later stage.
 *
 * @author Lawrence Deacon.
 */

class BDSGeometryFactorySQL: public BDSGeometryFactoryBase
{
public:
  BDSGeometryFactorySQL();
  virtual ~BDSGeometryFactorySQL(){;}
  
  static void SetDefaultRigidity(G4double rigidityIn) {defaultRigidity = rigidityIn;}

  /// Main interface overrides base class method to construct a file with
  /// optional colour mapping.
  virtual BDSGeometryExternal* Build(G4String componentName,
                                     G4String fileName,
                                     std::map<G4String, G4Colour*>* colourMapping    = nullptr,
                                     G4bool                 autoColour               = true,
                                     G4double               suggestedLength          = 0,
                                     G4double               suggestedHorizontalWidth = 0,
                                     std::vector<G4String>* vacuumBiasVolumeNames    = nullptr,
                                     G4bool                 makeSensitive            = true,
                                     BDSSDType              sensitivityType          = BDSSDType::energydep,
                                     BDSSDType              vacuumSensitivityType    = BDSSDType::energydepvacuum,
                                     G4UserLimits*          userLimitsToAttachToAllLVs = nullptr,
                                     G4bool                 detectSchema              = false);

protected:
  /// Clean up members.
  virtual void CleanUp();

  /// Non-virtual clean up as used in constructor.
  void CleanUpSQL();

private:
  std::unordered_map<G4VSolid*, BDSExtent> unShiftedExtents;

  // For List of uniform fields for volumes
  std::list<G4ThreeVector> UniformField;
  std::list<G4String> Fieldvol; 

  // For List of Quad/Sext/Oct Fields
  std::list<G4double> QuadBgrad;
  std::list<G4String> Quadvol; 
  std::list<G4double> SextBgrad;
  std::list<G4String> Sextvol;
  std::list<G4double> OctBgrad;
  std::list<G4String> Octvol;
  
  std::vector<G4LogicalVolume*> VOL_LIST;

  G4int NVariables;
  G4double VisRed; 
  G4double VisGreen;
  G4double VisBlue;
  G4double VisAlpha;
  G4String VisType;
  G4String Material;
  G4String TableName;
  G4String Name;

  G4double PosX;
  G4double PosY;
  G4double PosZ;
  G4double RotPsi;
  G4double RotTheta;
  G4double RotPhi;
  G4double K1,K2,K3,K4;
  G4String PARENTNAME;
  G4String InheritStyle;
  G4String Parameterisation;
  G4String MagType;
  G4int align_in;
  G4int align_out;
  G4int SetSensitive;
  G4int PrecisionRegion;
  G4int ApproximationRegion;
  G4double FieldX, FieldY, FieldZ;

  G4double lengthUserLimit;

  G4Region* precisionRegionSQL;
  G4Region* approximationRegionSQL;

  void BuildSQLObjects(G4String file);
  void SetCommonParams(BDSMySQLTable*,G4int);
  void SetPlacementParams(BDSMySQLTable*,G4int);
  G4VisAttributes* VisAtt();
  G4UserLimits* UserLimits(G4double);
  void SetLogVolAtt(G4LogicalVolume*, G4double);
  void SetLogVolRegion(G4LogicalVolume*);
  G4LogicalVolume* BuildCone(BDSMySQLTable* aSQLTable, G4int k);
  G4LogicalVolume* BuildEllipticalCone(BDSMySQLTable* aSQLTable, G4int k);
  G4LogicalVolume* BuildPolyCone(BDSMySQLTable* aSQLTable, G4int k);
  G4LogicalVolume* BuildBox(BDSMySQLTable* aSQLTable, G4int k);
  G4LogicalVolume* BuildTrap(BDSMySQLTable* aSQLTable, G4int k);
  G4LogicalVolume* BuildTorus(BDSMySQLTable* aSQLTable, G4int k);
  G4LogicalVolume* BuildSampler(BDSMySQLTable* aSQLTable, G4int k);
  G4LogicalVolume* BuildTube(BDSMySQLTable* aSQLTable, G4int k);
  G4LogicalVolume* BuildEllipticalTube(BDSMySQLTable* aSQLTable, G4int k);
  //G4LogicalVolume* BuildPCLTube(BDSMySQLTable* aSQLTable, G4int k);
  G4RotationMatrix* RotateComponent(G4double psi,
				    G4double phi,
				    G4double theta);
  G4RotationMatrix* rotateComponent;
  void PlaceComponents(BDSMySQLTable* aSQLTable, std::vector<G4LogicalVolume*> VOL_LIST);
  
  G4LogicalVolume* itsMarkerVol;
  std::vector<BDSMySQLTable*> itsSQLTable;
  //  BDSFieldMagSQL* itsMagField;
  //  BDSSDSampler* SensDet;

  void  SetMultiplePhysicalVolumes(G4VPhysicalVolume* aPhysVol);

  /// Search SQL table for variableName and if exists assign to variable
  template <typename T>
  void AssignVariable(BDSMySQLTable* aSQLTable, G4int k, G4String variableName, T& variable);

  /// As the samplers are registered and placed in a separate loop, we need to
  /// store a cache of the copy numbers (for lookup in the output) w.r.t. the
  /// particular logical volume (referenced by pointer);
  std::map<G4LogicalVolume*, G4int> samplerIDs;

  std::set<G4LogicalVolume*> sensitiveComponents;

  G4VPhysicalVolume* alignInVolume;
  G4VPhysicalVolume* alignOutVolume;

  std::map<G4String, G4ThreeVector> uniformFieldVolField;
  std::map<G4String, G4double> quadVolBgrad;
  std::map<G4String, G4double> sextVolBgrad;
  std::map<G4String, G4double> octVolBgrad;

  /// Cache of default rigidity for possibly constructing fields (only SQL)
  static G4double defaultRigidity;
};

#endif
