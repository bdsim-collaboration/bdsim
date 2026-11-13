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
#include "BDSAcceleratorModel.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSGeometryExternal.hh"
#include "BDSGeometryFactorySQL.hh"
#include "BDSGlobalConstants.hh"
#include "BDSMaterials.hh"
#include "BDSMySQLTable.hh"
#include "BDSMySQLVariable.hh"
#include "BDSMySQLWrapper.hh"
#include "BDSSDSampler.hh"
#include "BDSSampler.hh"
#include "BDSSamplerRegistry.hh"
#include "BDSSDManager.hh"
#include "BDSUtilities.hh"

#include "globals.hh"
#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4EllipticalCone.hh"
#include "G4EllipticalTube.hh"
#include "G4IntersectionSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4Polycone.hh"
#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"
#include "G4String.hh"
#include "G4SubtractionSolid.hh"
#include "G4Torus.hh"
#include "G4Trap.hh"
#include "G4Tubs.hh"
#include "G4UnionSolid.hh"
#include "G4UserLimits.hh"
#include "G4Version.hh"
#include "G4VisAttributes.hh"
#include "G4VPhysicalVolume.hh"

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <list>
#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

G4double BDSGeometryFactorySQL::defaultRigidity = std::numeric_limits<double>::max();

BDSGeometryFactorySQL::BDSGeometryFactorySQL()
{
  CleanUpSQL();
}

void BDSGeometryFactorySQL::CleanUp()
{
  CleanUpSQL();
  BDSGeometryFactoryBase::CleanUp();
}

void BDSGeometryFactorySQL::CleanUpSQL()
{
  NVariables = 0;
  VisRed    = 0;
  VisGreen  = 0;
  VisBlue   = 0;
  VisAlpha  = 0;
  VisType   = "";
  Material  = "";
  TableName = "";
  Name      = "";
  PosX      = 0;
  PosY      = 0;
  PosZ      = 0;
  RotPsi    = 0;
  RotTheta  = 0;
  RotPhi    = 0;
  K1        = 0;
  K2        = 0;
  K3        = 0;
  K4        = 0;
  PARENTNAME       = "";
  InheritStyle     = "";
  Parameterisation = "";
  MagType          = "";
  align_in         = 0;
  align_out        = 0;
  SetSensitive     = 0;
  PrecisionRegion  = 0;
  ApproximationRegion = 0;
  FieldX           = 0;
  FieldY           = 0;
  FieldZ           = 0;
  lengthUserLimit  = 0;
  
  unShiftedExtents.clear();
  
  precisionRegionSQL     = nullptr;
  approximationRegionSQL = nullptr;
  rotateComponent        = nullptr;
  itsMarkerVol           = nullptr;

  UniformField.clear();
  Fieldvol.clear();
  QuadBgrad.clear();
  SextBgrad.clear();
  Sextvol.clear();
  OctBgrad.clear();
  Octvol.clear();

  VOL_LIST.clear();

  itsSQLTable.clear();
  samplerIDs.clear();

  sensitiveComponents.clear();

  alignInVolume  = nullptr;
  alignOutVolume = nullptr;

  uniformFieldVolField.clear();
  quadVolBgrad.clear();
  sextVolBgrad.clear();
  octVolBgrad.clear();
}

BDSGeometryExternal* BDSGeometryFactorySQL::Build(G4String /*componentName*/,
                                                  G4String fileName,
                                                  std::map<G4String, G4Colour*>* colourMapping,
                                                  G4bool                   autoColour,
                                                  G4double                 suggestedLength,
                                                  G4double                 suggestedHorizontalWidth,
                                                  std::vector<G4String>* /*namedVolumeNames*/,
                                                  G4bool                 /*makeSensitive*/,
                                                  BDSSDType              /*sensitivityType*/,
                                                  BDSSDType              /*vacuumSensitivityType*/,
                                                  G4UserLimits*          /*userLimitsToAttachToAllLVs*/)
{
  CleanUp();
  
  // strip of the file name effectively
  G4String containingDir = BDS::GetFullPath(fileName, true);
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "loading SQL file:     " << fileName      << G4endl;
  G4cout << __METHOD_NAME__ << "containing directory: " << containingDir << G4endl;
#endif
  std::ifstream ifs;
  ifs.open(fileName);
  if (!ifs.good())
    {throw BDSException(__METHOD_NAME__, "Cannot open file \"" + fileName + "\"");}

  //hasFields = false;
  //nPoleField = 0;
  //hasUniformField = false;

  G4double outerR = suggestedHorizontalWidth*0.5;
  G4VSolid* containerSolid = new G4Box("container_solid",
                                       outerR,
                                       outerR,
                                       suggestedLength*0.5);
  
  const G4String emptyMaterialName = BDSGlobalConstants::Instance()->EmptyMaterial();
  G4Material* emptyMaterial  = BDSMaterials::Instance()->GetMaterial(emptyMaterialName);
  
  itsMarkerVol = new G4LogicalVolume(containerSolid,
                                     emptyMaterial,
                                     "container_lv");
  
  //length=(dynamic_cast<G4Box*>(itsMarkerVol->GetSolid())->GetZHalfLength()*2.0);
  VOL_LIST.push_back(itsMarkerVol);
  G4String fileListLine;
  char buffer[1000];
  while (ifs >> fileListLine)
    {
      if (BDS::StrContains(fileListLine, "#"))
        {ifs.getline(buffer,1000);} // a comment or info line - store it
      else
        {BuildSQLObjects(containingDir + fileListLine);} // a line with file to be parsed
    }
  
  // Close Geomlist file
  ifs.close();

  // update solid
  /*delete containerSolid; // delete existing solid
  containerSolid = new G4Box("container_solid",
                             (xmax - xmin)*0.5,
                             (xmax - xmin)*0.5,
                             (xmax - xmin)*0.5);
  itsMarkerVol->SetSolid(containerSolid); // update container solid
  */
  std::set<G4LogicalVolume*> tempVols;
  for (auto lv : VOL_LIST)
    {tempVols.insert(lv);}

  std::map<G4String, G4Colour*> mapping;
  if (colourMapping)
    {mapping = *colourMapping;}
  auto visesSQL = ApplyColourMapping(tempVols, mapping, autoColour);

  BDSGeometryExternal* result = new BDSGeometryExternal(containerSolid, itsMarkerVol, Extent());
  result->RegisterRotationMatrix(allRotationMatrices);
  result->RegisterSolid(allSolids);
  result->RegisterLogicalVolume(allLogicalVolumes);
  result->RegisterPhysicalVolume(allPhysicalVolumes);
  result->RegisterVisAttributes(allVisAttributes);
  result->RegisterVisAttributes(visesSQL);

  return result;
}

void BDSGeometryFactorySQL::BuildSQLObjects(G4String file)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "loading single file " << file << G4endl;
#endif

  BDSMySQLWrapper sql(file);
  itsSQLTable=sql.ConstructTable();

  for (G4int i=0; i<(G4int)itsSQLTable.size(); i++)
    {
#ifdef BDSDEBUG
      itsSQLTable[i]->Print();
#endif
      TableName = itsSQLTable[i]->GetName();
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << " i = " << i << ", TableName = " << TableName << G4endl;
#endif
      G4int pos = TableName.find("_");
      std::string ObjectType = TableName.substr(pos+1,TableName.length() - pos);
      NVariables = itsSQLTable[i]->GetVariable(0)->GetNVariables();
      for (G4int k=0; k<NVariables; k++)
        {
          SetCommonParams(itsSQLTable[i], k);
          G4LogicalVolume* logVol;
          if(BDS::StrCompare(ObjectType, "CONE") == 0)
            {logVol = BuildCone(itsSQLTable[i],k);}
          else if(BDS::StrCompare(ObjectType, "ELLIPTICALCONE") == 0)
            {logVol = BuildEllipticalCone(itsSQLTable[i],k);}
          else if(BDS::StrCompare(ObjectType, "POLYCONE") == 0)
            {logVol = BuildPolyCone(itsSQLTable[i],k);}
          else if(BDS::StrCompare(ObjectType, "BOX") == 0)
            {logVol = BuildBox(itsSQLTable[i],k);}
          else if(BDS::StrCompare(ObjectType, "TRAP") == 0)
            {logVol =  BuildTrap(itsSQLTable[i],k);}
          else if(BDS::StrCompare(ObjectType, "TORUS") == 0)
            {logVol = BuildTorus(itsSQLTable[i],k);}
          else if(BDS::StrCompare(ObjectType, "SAMPLER") == 0)
            {logVol = BuildSampler(itsSQLTable[i],k);}
          else if(BDS::StrCompare(ObjectType, "TUBE") == 0)
            {logVol =  BuildTube(itsSQLTable[i],k);}
          else if(BDS::StrCompare(ObjectType, "ELLIPTICALTUBE") == 0)
            {logVol =  BuildEllipticalTube(itsSQLTable[i],k);}
          //else if(BDS::StrCompare(ObjectType, "PCLTUBE", G4String::ignoreCase)==0)
          //  {logVol =  BuildPCLTube(itsSQLTable[i],k);}
          else
            {throw BDSException(__METHOD_NAME__ + ObjectType + " not known.");}
          //Set the user limits and visual attributes
          SetLogVolAtt(logVol, lengthUserLimit);
          VOL_LIST.push_back(logVol);
          allLogicalVolumes.insert(logVol);
        }
      PlaceComponents(itsSQLTable[i], VOL_LIST);
    }
}

template<>
void BDSGeometryFactorySQL::AssignVariable<G4double>(BDSMySQLTable* aSQLTable, G4int k, G4String variableName, G4double& variable)
{
  BDSMySQLVariable* sqlVariable = aSQLTable->GetVariable(variableName);
  if(sqlVariable)
    {variable = sqlVariable->GetDblValue(k);}
}

template<>
void BDSGeometryFactorySQL::AssignVariable<G4int>(BDSMySQLTable* aSQLTable, G4int k, G4String variableName, G4int& variable)
{
  BDSMySQLVariable* sqlVariable = aSQLTable->GetVariable(variableName);
  if(sqlVariable)
    {variable = sqlVariable->GetIntValue(k);}
}

template<>
void BDSGeometryFactorySQL::AssignVariable<G4String>(BDSMySQLTable* aSQLTable, G4int k, G4String variableName, G4String& variable)
{
  BDSMySQLVariable* sqlVariable = aSQLTable->GetVariable(variableName);
  if(sqlVariable)
    {variable = sqlVariable->GetStrValue(k);}
}

void BDSGeometryFactorySQL::SetCommonParams(BDSMySQLTable* aSQLTable, G4int k)
{
  //Defaults
  VisRed = VisGreen = VisBlue = 0.5;
  VisAlpha = 0.5;
  VisType = "S";
  Material = BDSGlobalConstants::Instance()->VacuumMaterial();
  Name="";
  PrecisionRegion=0;
  ApproximationRegion=0;

  AssignVariable(aSQLTable,k,"RED"   ,VisRed);
  AssignVariable(aSQLTable,k,"BLUE"  ,VisBlue);
  AssignVariable(aSQLTable,k,"GREEN" ,VisGreen);
  AssignVariable(aSQLTable,k,"ALPHA" ,VisAlpha);
  AssignVariable(aSQLTable,k,"VISATT",VisType);
  AssignVariable(aSQLTable,k,"MATERIAL",Material);
  AssignVariable(aSQLTable,k,"PRECISIONREGION",PrecisionRegion);
  AssignVariable(aSQLTable,k,"APPROXIMATIONREGION",ApproximationRegion);

  AssignVariable(aSQLTable,k,"NAME",Name);
  if(Name=="_SQL")
    {Name = TableName+std::to_string(k) + "_SQL";}
  if(Name.empty())
    {Name = TableName+std::to_string(k);}
  Name = itsMarkerVol->GetName()+"_"+Name;
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " k = " << k << ", Name = " << Name << G4endl;
#endif
}

void BDSGeometryFactorySQL::SetPlacementParams(BDSMySQLTable* aSQLTable, G4int k)
{
  //Defaults
  PosX = PosY = PosZ = 0.;
  RotPsi = RotTheta = RotPhi = 0.;
  K1 = K2 = K3 = K4 = 0.;
  PARENTNAME = "";
  InheritStyle = "";
  Parameterisation = "";
  align_in=0;
  align_out=0;
  SetSensitive=0;
  MagType = "";
  FieldX = FieldY = FieldZ = 0.0;
  Name="";

  AssignVariable(aSQLTable,k,"PARENTNAME",PARENTNAME);
  AssignVariable(aSQLTable,k,"POSX",PosX);
  AssignVariable(aSQLTable,k,"POSY",PosY);
  AssignVariable(aSQLTable,k,"POSZ",PosZ);
  AssignVariable(aSQLTable,k,"ROTPSI",RotPsi);
  AssignVariable(aSQLTable,k,"ROTTHETA",RotTheta);
  AssignVariable(aSQLTable,k,"ROTPHI",RotPhi);
  AssignVariable(aSQLTable,k,"K1",K1);
  AssignVariable(aSQLTable,k,"K2",K2);
  AssignVariable(aSQLTable,k,"K3",K3);
  AssignVariable(aSQLTable,k,"K4",K4);
  AssignVariable(aSQLTable,k,"MAGTYPE",MagType);
  AssignVariable(aSQLTable,k,"FIELDX",FieldX);
  AssignVariable(aSQLTable,k,"FIELDY",FieldY);
  AssignVariable(aSQLTable,k,"FIELDZ",FieldZ);
  AssignVariable(aSQLTable,k,"ALIGNIN",align_in);
  AssignVariable(aSQLTable,k,"ALIGNOUT",align_out);
  AssignVariable(aSQLTable,k,"SETSENSITIVE",SetSensitive);
  AssignVariable(aSQLTable,k,"INHERITSTYLE",InheritStyle);
  AssignVariable(aSQLTable,k,"PARAMETERISATION",Parameterisation);

  // TODO
  //if(PARENTNAME=="")
  //  {PosZ-=Length()/2.0;} //Move position to beginning of element

  PARENTNAME= itsMarkerVol->GetName() + "_" + PARENTNAME;

  AssignVariable(aSQLTable,k,"NAME",Name);
  if(Name=="_SQL")
    {Name = TableName+std::to_string(k) + "_SQL";}
  if(Name.empty())
    {Name = TableName+std::to_string(k);}

  Name = itsMarkerVol->GetName()+"_"+Name;
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " k = " << k << ", Name = " << Name << G4endl;
#endif
}

G4VisAttributes* BDSGeometryFactorySQL::VisAtt()
{
  G4VisAttributes* va = new G4VisAttributes(G4Colour(VisRed, VisGreen, VisBlue, VisAlpha));
  char testChar = VisType[0];
  switch (testChar)
    {
    case 'W': va->SetForceWireframe(true); break;
    case 'I': va->SetVisibility(false); break;
    case 'S': va->SetForceSolid(true); break;
    case 'w': va->SetForceWireframe(true); break;
    case 'i': va->SetVisibility(false); break;
    case 's': va->SetForceSolid(true); break;
    }
  allVisAttributes.insert(va);
  return va;
}

G4UserLimits* BDSGeometryFactorySQL::UserLimits(G4double maxStepLength)
{
  return BDS::CreateUserLimits(BDSGlobalConstants::Instance()->DefaultUserLimits(), maxStepLength);
}

//Set logical volume attributes
void BDSGeometryFactorySQL::SetLogVolAtt(G4LogicalVolume* logVol, G4double k)
{
  logVol->SetVisAttributes(VisAtt());
  logVol->SetUserLimits(UserLimits(k));
  SetLogVolRegion(logVol);
}

void BDSGeometryFactorySQL::SetLogVolRegion(G4LogicalVolume* logVol)
{
  G4Region* region = nullptr;
  if(PrecisionRegion)
    {
      G4cout << "Approximation region should be define in gmad syntax now!" << G4endl;
      region = BDSAcceleratorModel::Instance()->Region("precision");
    }
  if(ApproximationRegion)
    {
      G4cout << "Approximation region should be define in gmad syntax now!" << G4endl;
      region = BDSAcceleratorModel::Instance()->Region("approximation");
    }
  if (region)
    {
      logVol->SetRegion(region);
      region->AddRootLogicalVolume(logVol);
    }
}

G4LogicalVolume* BDSGeometryFactorySQL::BuildCone(BDSMySQLTable* aSQLTable, G4int k)
{
  G4double length;
  G4double rInnerStart;
  G4double rInnerEnd;
  G4double rOuterStart;
  G4double rOuterEnd;
  G4double sphi;
  G4double dphi;
  
  //Defaults
  sphi =0.0;
  dphi = CLHEP::twopi*CLHEP::radian;
  length = rOuterStart = rOuterEnd = 10.*CLHEP::mm;
  rInnerStart = rInnerEnd = 0.0;
  
  AssignVariable(aSQLTable,k,"LENGTH",length);
  AssignVariable(aSQLTable,k,"RINNERSTART",rInnerStart);
  AssignVariable(aSQLTable,k,"RINNEREND"  ,rInnerEnd);
  AssignVariable(aSQLTable,k,"ROUTERSTART",rOuterStart);
  AssignVariable(aSQLTable,k,"ROUTEREND"  ,rOuterEnd);
  AssignVariable(aSQLTable,k,"STARTPHI",sphi);
  AssignVariable(aSQLTable,k,"DELTAPHI",dphi);

  G4Cons* aCone = new G4Cons(Name+"_Cone",
                             rInnerStart,
                             rOuterStart,
                             rInnerEnd,
                             rOuterEnd,
                             length/2,
                             sphi,
                             dphi);
  allSolids.insert(aCone);

  G4double maxR = std::max(rOuterStart, rOuterEnd);
  unShiftedExtents[aCone] = BDSExtent(maxR, maxR, length*0.5);
  
  G4LogicalVolume* aConeVol = new G4LogicalVolume(aCone,
                                                  BDSMaterials::Instance()->GetMaterial(Material),
                                                  Name+"_LogVol");

  lengthUserLimit=length;
  return aConeVol;
}

G4LogicalVolume* BDSGeometryFactorySQL::BuildEllipticalCone(BDSMySQLTable* aSQLTable, G4int k)
{
  G4double lengthZ = 0;
  G4double pxSemiAxis = 0;
  G4double pySemiAxis = 0;
  G4double pzTopCut = 0;
  
  //Defaults
  lengthZ = 10.*CLHEP::mm;
  
  AssignVariable(aSQLTable,k,"LENGTHZ",lengthZ);
  AssignVariable(aSQLTable,k,"XSEMIAXIS",pxSemiAxis);
  AssignVariable(aSQLTable,k,"YSEMIAXIS",pySemiAxis);
  AssignVariable(aSQLTable,k,"ZCUT",pzTopCut);
  
  G4EllipticalCone* aEllipticalCone = new G4EllipticalCone(Name+"_EllipticalCone",
                                                           pxSemiAxis,
                                                           pySemiAxis,
                                                           lengthZ/2,
                                                           pzTopCut);
  allSolids.insert(aEllipticalCone);

  G4double maxX = pxSemiAxis*lengthZ*0.5;
  G4double maxY = pySemiAxis*lengthZ*0.5;
  unShiftedExtents[aEllipticalCone] = BDSExtent(maxX, maxY, lengthZ*0.5);
  
  G4LogicalVolume* aEllipticalConeVol = 
    new G4LogicalVolume(aEllipticalCone,
                        BDSMaterials::Instance()->GetMaterial(Material),
                        Name+"_LogVol");

  lengthUserLimit=lengthZ*0.5;
  return aEllipticalConeVol;
}

G4LogicalVolume* BDSGeometryFactorySQL::BuildPolyCone(BDSMySQLTable* aSQLTable, G4int k)
{
  G4int    numZplanes;
  G4double sphi;
  G4double dphi;
  
  //Defaults
  sphi = 0.0;
  dphi = CLHEP::twopi*CLHEP::radian;
  numZplanes = 0;

  AssignVariable(aSQLTable,k,"NZPLANES",numZplanes);

  std::vector<G4double> rInner = std::vector<G4double>(numZplanes+1);
  std::vector<G4double> rOuter = std::vector<G4double>(numZplanes+1);
  std::vector<G4double> zPos   = std::vector<G4double>(numZplanes+1);
      
  for (G4int planenum=0; planenum<numZplanes; planenum++)
    {
      G4String rInner_ID = "RINNER" + std::to_string(planenum+1);
      G4String rOuter_ID = "ROUTER" + std::to_string(planenum+1);
      G4String zPos_ID = "PLANEPOS" + std::to_string(planenum+1);
      
      AssignVariable(aSQLTable,k,rInner_ID,rInner[planenum]);
      AssignVariable(aSQLTable,k,rOuter_ID,rOuter[planenum]);
      AssignVariable(aSQLTable,k,zPos_ID  ,zPos[planenum]);
    }
  
  AssignVariable(aSQLTable,k,"STARTPHI",sphi);
  AssignVariable(aSQLTable,k,"DELTAPHI",dphi);
  
  G4Polycone* aPolyCone = new G4Polycone(Name+"_PolyCone",
                                         sphi,
                                         dphi,
                                         numZplanes,
                                         zPos.data(),
                                         rInner.data(),
                                         rOuter.data());
  allSolids.insert(aPolyCone);

  G4double maxR = *std::max_element(rOuter.begin(), rOuter.end());
  G4double maxZ = *std::max_element(zPos.begin(), zPos.end());
  unShiftedExtents[aPolyCone] = BDSExtent(maxR, maxR, maxZ);
  
  G4LogicalVolume* aPolyConeVol = 
    new G4LogicalVolume(aPolyCone,
                        BDSMaterials::Instance()->GetMaterial(Material),
                        Name+"_LogVol");

  lengthUserLimit=fabs(zPos[0]-zPos[numZplanes-1])/2;
  
  return aPolyConeVol;
}

G4LogicalVolume* BDSGeometryFactorySQL::BuildBox(BDSMySQLTable* aSQLTable, G4int k)
{
  G4double lengthX;
  G4double lengthY;
  G4double lengthZ;

  lengthX = lengthY = lengthZ = 1.*CLHEP::mm;
  
  AssignVariable(aSQLTable,k,"LENGTHX",lengthX);
  AssignVariable(aSQLTable,k,"LENGTHY",lengthY);
  AssignVariable(aSQLTable,k,"LENGTHZ",lengthZ);

  G4Box* aBox = new G4Box(Name+"_Box",
                          lengthX/2,
                          lengthY/2,
                          lengthZ/2);
  allSolids.insert(aBox);

  unShiftedExtents[aBox] = BDSExtent(lengthX*0.5, lengthY*0.5, lengthZ*0.5);
  
  G4LogicalVolume* aBoxVol = 
    new G4LogicalVolume(aBox,
                        BDSMaterials::Instance()->GetMaterial(Material),
                        Name+"_LogVol");
  
  lengthUserLimit = lengthZ;
  return aBoxVol;
}

G4LogicalVolume* BDSGeometryFactorySQL::BuildTrap(BDSMySQLTable* aSQLTable, G4int k)
{
  
  G4double trapTheta = 0; //Angle between faces of trapezoid
  G4double lengthXPlus = 0;
  G4double lengthXMinus = 0;
  G4double lengthYPlus = 0;
  G4double lengthYMinus = 0;
  G4double lengthZ = 0;
  
  AssignVariable(aSQLTable,k,"TRAPTHETA",trapTheta);
  AssignVariable(aSQLTable,k,"LENGTHXPLUS" ,lengthXPlus);
  AssignVariable(aSQLTable,k,"LENGTHXMINUS",lengthXMinus);
  AssignVariable(aSQLTable,k,"LENGTHYPLUS" ,lengthYPlus);
  AssignVariable(aSQLTable,k,"LENGTHYMINUS",lengthYMinus);
  AssignVariable(aSQLTable,k,"LENGTHZ",lengthZ);
  
  G4Trap* aTrap = new G4Trap(Name+"_Trd",
                             lengthZ/2,
                             trapTheta, 0,
                             lengthYPlus/2,
                             lengthXPlus/2,
                             lengthXPlus/2,
                             0,
                             lengthYMinus/2,
                             lengthXMinus/2,
                             lengthXMinus/2,
                             0);
  allSolids.insert(aTrap);
  unShiftedExtents[aTrap] = BDSExtent();
  
  G4LogicalVolume* aTrapVol = 
    new G4LogicalVolume(aTrap,
                        BDSMaterials::Instance()->GetMaterial(Material),
                        Name+"_LogVol");

  lengthUserLimit = lengthZ*0.5;
  return aTrapVol;
}

G4LogicalVolume* BDSGeometryFactorySQL::BuildTorus(BDSMySQLTable* aSQLTable, G4int k)
{
  G4double rInner;
  G4double rOuter;
  G4double rSwept;
  G4double sphi;
  G4double dphi;
  
  //Defaults
  rSwept = 20.*CLHEP::mm;
  rOuter = 10.*CLHEP::mm;
  rInner = 0.0;
  sphi = 0.0;
  dphi=2*CLHEP::pi*CLHEP::radian;
  
  AssignVariable(aSQLTable,k,"RINNER",rInner);
  AssignVariable(aSQLTable,k,"ROUTER",rOuter);
  AssignVariable(aSQLTable,k,"RSWEPT",rSwept);
  AssignVariable(aSQLTable,k,"ROUTER",rOuter);
  AssignVariable(aSQLTable,k,"STARTPHI",sphi);
  AssignVariable(aSQLTable,k,"DELTAPHI",dphi);
  
  G4Torus* aTorus = new G4Torus(Name+"_Torus",
                                rInner,
                                rOuter,
                                rSwept,
                                sphi,
                                dphi);
  allSolids.insert(aTorus);

  G4LogicalVolume* aTorusVol = 
    new G4LogicalVolume(aTorus,
                        BDSMaterials::Instance()->GetMaterial(Material),
                        Name+"_LogVol");

  lengthUserLimit = rOuter*0.5;
  return aTorusVol;
}

G4LogicalVolume* BDSGeometryFactorySQL::BuildSampler(BDSMySQLTable* aSQLTable, G4int k)
{
  G4double length;
  G4double rInnerStart;
  G4double rInnerEnd;
  G4double rOuterStart;
  G4double rOuterEnd;
  
  //Defaults
  length = rOuterStart = rOuterEnd = 10.*CLHEP::mm;
  rInnerStart = rInnerEnd = 0.0;
  
  AssignVariable(aSQLTable,k,"LENGTH",length);
  AssignVariable(aSQLTable,k,"RINNERSTART",rInnerStart);
  AssignVariable(aSQLTable,k,"RINNEREND"  ,rInnerEnd);
  AssignVariable(aSQLTable,k,"ROUTERSTART",rOuterStart);
  AssignVariable(aSQLTable,k,"ROUTEREND"  ,rOuterEnd);
  
  if (BDSMySQLVariable* sqlName = aSQLTable->GetVariable("NAME"))
    {
      Name = sqlName->GetStrValue(k);
      sqlName->SetStrValue(k,Name+"_SQL");
      Name = sqlName->GetStrValue(k);
    }
  if(Name=="_SQL")
    {Name = TableName+std::to_string(k)+"_SQL";}
  // make sure that each name is unique!
  Name = itsMarkerVol->GetName()+"_"+Name;
     
  G4Cons* aSampler = new G4Cons(Name+"_samp",
                                rInnerStart,
                                rOuterStart,
                                rInnerEnd,
                                rOuterEnd,
                                length/2,
                                0,
                                CLHEP::twopi*CLHEP::radian);
  allSolids.insert(aSampler);
  G4LogicalVolume* aSamplerVol = 
    new G4LogicalVolume(aSampler,
                        BDSMaterials::Instance()->GetMaterial(Material),
                        Name+"_LogVol");

  lengthUserLimit = length*0.5;
  
  aSamplerVol->SetSensitiveDetector(BDSSDManager::Instance()->SamplerPlane());

  G4int samplerID = BDSSamplerRegistry::Instance()->RegisterSampler(Name,nullptr);
  
  samplerIDs[aSamplerVol] = samplerID;
  
  return aSamplerVol;
}

G4LogicalVolume* BDSGeometryFactorySQL::BuildTube(BDSMySQLTable* aSQLTable, G4int k)
{
  G4double rInner;
  G4double rOuter;
  G4double length;
  G4double sphi;
  G4double dphi;
  
  //Defaults 
  length = 100.*CLHEP::mm;
  rOuter = 10.*CLHEP::mm;
  rInner = 0.0;
  sphi = 0.0;
  dphi=2*CLHEP::pi*CLHEP::radian;

  AssignVariable(aSQLTable,k,"RINNER",rInner);
  AssignVariable(aSQLTable,k,"ROUTER",rOuter);
  AssignVariable(aSQLTable,k,"LENGTH",length);
  AssignVariable(aSQLTable,k,"STARTPHI",sphi);
  AssignVariable(aSQLTable,k,"DELTAPHI",dphi);

  G4Tubs* aTubs = new G4Tubs(Name+"_Tubs",
                             rInner,
                             rOuter,
                             length/2,
                             sphi,
                             dphi);
  allSolids.insert(aTubs);
  G4LogicalVolume* aTubsVol = 
    new G4LogicalVolume(aTubs,
                        BDSMaterials::Instance()->GetMaterial(Material),
                        Name+"_LogVol");
  
  lengthUserLimit = length*0.5;
  return aTubsVol;
}

G4LogicalVolume* BDSGeometryFactorySQL::BuildEllipticalTube(BDSMySQLTable* aSQLTable, G4int k)
{
  G4double lengthX;
  G4double lengthY;
  G4double lengthZ;
  
  //Defaults 
  lengthX = 100.*CLHEP::mm;
  lengthY = 50.*CLHEP::mm;
  lengthZ = 200.*CLHEP::mm;

  AssignVariable(aSQLTable,k,"LENGTHX",lengthX);
  AssignVariable(aSQLTable,k,"LENGTHY",lengthY);
  AssignVariable(aSQLTable,k,"LENGTHZ",lengthZ);
  
  G4EllipticalTube* aEllipticalTube = new G4EllipticalTube(Name+"_EllipticalTube",
                                                           lengthX/2,
                                                           lengthY/2,
                                                           lengthZ/2
                                                           );
  
  allSolids.insert(aEllipticalTube);
  G4LogicalVolume* aEllipticalTubeVol = 
    new G4LogicalVolume(aEllipticalTube,
                        BDSMaterials::Instance()->GetMaterial(Material),
                        Name+"_LogVol");
  G4double maxLength = lengthX;
  if (lengthY>lengthX&&lengthY>lengthZ)
    {maxLength = lengthY;}
  else if(lengthZ>lengthY&&lengthZ>lengthX)
    {maxLength = lengthZ;}
  lengthUserLimit = maxLength*0.5;
  return aEllipticalTubeVol;
}

/*
G4LogicalVolume* BDSGeometryFactorySQL::BuildPCLTube(BDSMySQLTable* aSQLTable, G4int k)
{
  G4double aperX;
  G4double aperYUp;
  G4double aperYDown;
  G4double aperDy; 
  G4double thickness;
  G4double length;
  
  //Defaults 
  aperX = 100.*CLHEP::mm;
  aperYUp = 50.*CLHEP::mm;
  aperYDown = 200.*CLHEP::mm;
  aperDy = 0.*CLHEP::mm;
  thickness = BDSGlobalConstants::Instance()->DefaultBeamPipeModel()->beamPipeThickness;
  length = 200.0*CLHEP::mm;
  
  if(aSQLTable->GetVariable("APERX")!=nullptr)
    aperX = aSQLTable->GetVariable("APERX")->GetDblValue(k);
  if(aSQLTable->GetVariable("APERYUP")!=nullptr)
    aperYUp = aSQLTable->GetVariable("APERYUP")->GetDblValue(k);
  if(aSQLTable->GetVariable("APERYDOWN")!=nullptr)
    aperYDown = aSQLTable->GetVariable("APERYDOWN")->GetDblValue(k);
  if(aSQLTable->GetVariable("APERDY")!=nullptr)
    aperDy = aSQLTable->GetVariable("APERDY")->GetDblValue(k);
  if(aSQLTable->GetVariable("THICKNESS")!=nullptr)
    thickness = aSQLTable->GetVariable("THICKNESS")->GetDblValue(k);
  if(aSQLTable->GetVariable("LENGTH")!=nullptr)
    length = aSQLTable->GetVariable("LENGTH")->GetDblValue(k);
  
  BDSPCLTube* aPCLTubeBuilder = new BDSPCLTube(aperX, aperYUp, aperYDown, aperDy, thickness, length, Name+"_PCLTube");
  G4VSolid* aPCLTube = aPCLTubeBuilder->GetSolid();
  delete aPCLTubeBuilder;

  G4LogicalVolume* aPCLTubeVol = 
    new G4LogicalVolume(aPCLTube,
                        BDSMaterials::Instance()->GetMaterial(Material),
                        Name+"_LogVol");
  G4double totalYLength = aperDy+aperYUp+aperYDown+thickness;
  G4double totalXLength = aperX+thickness;
  G4double maxLength = length;
  if (totalYLength>length&&totalYLength>totalXLength){
    maxLength = totalYLength;
  }
  else if(totalXLength>totalYLength&&totalXLength>length){
    maxLength = totalXLength;
  }
  lengthUserLimit = maxLength*0.5;
  return aPCLTubeVol;
}
*/

G4RotationMatrix* BDSGeometryFactorySQL::RotateComponent(G4double psi,G4double phi,G4double theta)
{
  rotateComponent = new G4RotationMatrix;
  if(psi==0 && phi==0 && theta==0)
    {return rotateComponent;}

  G4RotationMatrix* LocalRotation = new G4RotationMatrix;
  G4ThreeVector* localX = new G4ThreeVector(1.,0.,0.);
  G4ThreeVector* localY = new G4ThreeVector(0.,1.,0.);
  G4ThreeVector* localZ = new G4ThreeVector(0.,0.,1.);
  
  LocalRotation->rotate(psi,*localZ);
  localX->rotate(psi,*localZ);
  localY->rotate(psi,*localZ);
  
  LocalRotation->rotate(phi,*localY);
  localX->rotate(phi,*localY);
  localZ->rotate(phi,*localY);
  
  LocalRotation->rotate(theta,*localX);
  localY->rotate(theta,*localX);
  localZ->rotate(theta,*localX);
  
  rotateComponent->transform(*LocalRotation);
  rotateComponent->invert();
  
  return rotateComponent;
}

void BDSGeometryFactorySQL::PlaceComponents(BDSMySQLTable* aSQLTable,
                                            std::vector<G4LogicalVolume*> VOL_LISTIn)
{
  for (G4int k=0; k<NVariables; k++) // Now run through and place according to
    { 
      SetPlacementParams(aSQLTable, k);
      G4int PARENTID=0;
      if(!PARENTNAME.empty())
        {
          PARENTNAME+="_LogVol";
          for (G4int i=0; i<(G4int)VOL_LISTIn.size(); i++)
            {
#if G4VERSION_NUMBER > 1099
              if (G4StrUtil::icompare(PARENTNAME, VOL_LISTIn[i]->GetName()) == 0)
#else
              if(PARENTNAME.compareTo(VOL_LISTIn[i]->GetName(), G4String::ignoreCase)==0)
#endif
                {
                  PARENTID = i;
                  continue;
                }
            }
        }
      
      // to being in line with logvol names (needed for name checking loop
      G4String tmpname = Name+"_LogVol";
      G4int ID=0;
      for (G4int i=0; i<(G4int)VOL_LISTIn.size(); i++)
        {
#if G4VERSION_NUMBER > 1099
    if (G4StrUtil::icompare(tmpname, VOL_LISTIn[i]->GetName()) == 0)
#else
          if(tmpname.compareTo(VOL_LISTIn[i]->GetName(), G4String::ignoreCase)==0)
#endif
            {
              ID = i;
              continue;
            }
        }
      
      if(SetSensitive)
        {sensitiveComponents.insert(VOL_LISTIn[ID]);}
      G4ThreeVector PlacementPoint(PosX,PosY,PosZ);

    if(!InheritStyle.empty())
        { //True if InheritStyle is set
#if G4VERSION_NUMBER > 1099
      if (G4StrUtil::icompare(InheritStyle, "SUBTRACT") == 0)
#else
          if(InheritStyle.compareTo("SUBTRACT", G4String::ignoreCase)==0)
#endif
            {
              G4VSolid* original = VOL_LISTIn[PARENTID]->GetSolid();
              G4VSolid* sub = VOL_LISTIn[ID]->GetSolid();
              VOL_LISTIn[PARENTID]->SetSolid(new G4SubtractionSolid(VOL_LISTIn[PARENTID]->GetName(),
                                                                  original,
                                                                  sub,
                                                                  RotateComponent(RotPsi,RotPhi,RotTheta),
                                                                  PlacementPoint));
              
            }
#if G4VERSION_NUMBER > 1099
      if (G4StrUtil::icompare(InheritStyle, "INTERSECT") == 0)
#else
          else if(InheritStyle.compareTo("INTERSECT", G4String::ignoreCase)==0)
#endif
            {
              G4VSolid* original = VOL_LISTIn[PARENTID]->GetSolid();
              G4VSolid* sub = VOL_LISTIn[ID]->GetSolid();
              VOL_LISTIn[PARENTID]->SetSolid(new G4IntersectionSolid(VOL_LISTIn[PARENTID]->GetName(),
                                                                   original,
                                                                   sub,
                                                                   RotateComponent(RotPsi,RotPhi,RotTheta),
                                                                   PlacementPoint));
              
            }
#if G4VERSION_NUMBER > 1099
      if (G4StrUtil::icompare(InheritStyle, "UNION") == 0)
#else
          else if(InheritStyle.compareTo("UNION", G4String::ignoreCase)==0)
#endif
            {
              G4VSolid* original = VOL_LISTIn[PARENTID]->GetSolid();
              G4VSolid* sub = VOL_LISTIn[ID]->GetSolid();
              VOL_LISTIn[PARENTID]->SetSolid(new G4UnionSolid(VOL_LISTIn[PARENTID]->GetName(),
                                                            original,
                                                            sub,
                                                            RotateComponent(RotPsi,RotPhi,RotTheta),
                                                            PlacementPoint));
            }
        }
      
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << " k = " << k << ", volume = " << VOL_LISTIn[ID]->GetName() << G4endl;
#endif

      G4LogicalVolume* volume = VOL_LISTIn[ID];
      G4int copyNumber = 0;
      auto result = samplerIDs.find(volume);
      if (result != samplerIDs.end())
        {copyNumber = result->second;}

      /// Offset extent of individual solid and expand it.
      G4VSolid* solid = volume->GetSolid();
      BDSExtent ext = unShiftedExtents[solid];
      BDSExtent extShifted = ext.Translate(PlacementPoint);
      ExpandExtent(extShifted);
      
      G4VPhysicalVolume* PhysiComp = 
        new G4PVPlacement(RotateComponent(RotPsi,RotPhi,RotTheta),
                          PlacementPoint,
                          VOL_LISTIn[ID],
                          Name,
                          VOL_LISTIn[PARENTID],
                          false,
                          copyNumber,
                          BDSGlobalConstants::Instance()->CheckOverlaps());
      if(align_in)
        {
          // Make sure program stops and informs user if more than one alignment vol.
          if(alignInVolume)
            {
              G4cerr<<"\nBDSGeometryFactorySQL.cc:486: Trying to align in-beam to SQL volume to " << PhysiComp->GetName() << " but alignment already set to " << alignInVolume->GetName() << G4endl;
              G4Exception("Aborting Program", "-1", FatalException, "");

            }
          else
            {alignInVolume = PhysiComp;}
        }

      if(align_out)
        {
          if(alignOutVolume)
            {
              G4cerr<<"\nBDSGeometryFactorySQL.cc:486: Trying to align out-beam to SQL volume to " << PhysiComp->GetName() << " but alignment already set to " << alignOutVolume->GetName() << G4endl;
              G4Exception("Aborting Program", "-1", FatalException, "");
            }
          else
            {alignOutVolume = PhysiComp;}
        }

      // magnetic rigidity brho
      G4double brho = defaultRigidity;
#if G4VERSION_NUMBER > 1099
    if (G4StrUtil::icompare(MagType, "QUAD") == 0)
#else
      if(MagType.compareTo("QUAD", G4String::ignoreCase)==0)
#endif
        {
          //hasFields = true;
          //nPoleField = 1;
          QuadBgrad.push_back(- brho * K1 / CLHEP::m2);
          Quadvol.push_back(PhysiComp->GetName());
          quadVolBgrad[PhysiComp->GetName()]=(- brho * K1 / CLHEP::m2);
        }
#if G4VERSION_NUMBER > 1099
    if (G4StrUtil::icompare(MagType, "SEXT") == 0)
#else
      if(MagType.compareTo("SEXT", G4String::ignoreCase)==0)
#endif
        {
          //hasFields = true;
          //nPoleField = 2;
          SextBgrad.push_back(- brho * K2 / CLHEP::m3);
          Sextvol.push_back(PhysiComp->GetName());
          sextVolBgrad[PhysiComp->GetName()]=(- brho * K2 / CLHEP::m3);
        }
#if G4VERSION_NUMBER > 1099
    if (G4StrUtil::icompare(MagType, "OCT") == 0)
#else
      if(MagType.compareTo("OCT", G4String::ignoreCase)==0)
#endif
        {
          //hasFields = true;
          //nPoleField = 3;
          OctBgrad.push_back(- brho * K3 / (CLHEP::m2*CLHEP::m2));
          Octvol.push_back(PhysiComp->GetName());
          octVolBgrad[PhysiComp->GetName()]=(- brho * K3 / (CLHEP::m2*CLHEP::m2));
        }

      if(FieldX || FieldY || FieldZ) //if any vols have non-zero field components
        {
          //hasFields = true;
          //hasUniformField=true;
#ifdef BDSDEBUG
          G4cout << "BDSGeometryFactorySQL> volume " << PhysiComp->GetName() << " has the following uniform field: " << FieldX << " " << FieldY << " " << FieldZ << " " << G4endl;
#endif
          UniformField.emplace_back(G4ThreeVector(FieldX*CLHEP::tesla,
                                               FieldY*CLHEP::tesla,
                                               FieldZ*CLHEP::tesla));
          Fieldvol.push_back(PhysiComp->GetName());
          uniformFieldVolField[PhysiComp->GetName()]=G4ThreeVector(FieldX*CLHEP::tesla,
                                                                   FieldY*CLHEP::tesla,
                                                                   FieldZ*CLHEP::tesla);
        }
  }
}
