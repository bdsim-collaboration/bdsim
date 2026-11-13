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
#include "BDSExecOptions.hh"
#include "BDSGeometrySQL.hh"
#include "BDSGeometryType.hh"
#include "BDSGlobalConstants.hh"
#include "BDSMaterials.hh"
#include "BDSMySQLWrapper.hh"
#include "BDSSamplerSD.hh"
#include "BDSSampler.hh"
#include "BDSSamplerRegistry.hh"
#include "BDSSDManager.hh"
//#include "BDSPCLTube.hh"
#include "BDSUtilities.hh"

#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4EllipticalCone.hh"
#include "G4EllipticalTube.hh"
#include "G4IntersectionSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4Polycone.hh"
#include "G4ProductionCuts.hh"
#include "G4PVPlacement.hh"
#include "G4RegionStore.hh"
#include "G4SubtractionSolid.hh"
#include "G4Torus.hh"
#include "G4Trap.hh"
#include "G4Tubs.hh"
#include "G4UnionSolid.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"
#include "G4VPhysicalVolume.hh"

#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

BDSGeometrySQL::BDSGeometrySQL(G4String filePath):
  BDSGeometry(BDSGeometryType::mokka, filePath),
  rotateComponent(nullptr),
  itsMarkerVol(nullptr)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "loading SQL file: " << filePath << G4endl;
  G4cout << __METHOD_NAME__ << "containing directory: " << ContainingDir() << G4endl;
#endif
  // open the file
  ifs.open(filePath.c_str());
  
  if(!ifs)
    {G4cout << "Unable to load SQL database file: " << filePath << G4endl; exit(1);}
  
  hasFields = false;
  nPoleField = 0;
  hasUniformField = false;
}

BDSGeometrySQL::~BDSGeometrySQL()
{;}

void BDSGeometrySQL::Construct(G4LogicalVolume* containerLogicalVolume)
{
  itsMarkerVol = containerLogicalVolume;
  length=((G4Box*)itsMarkerVol->GetSolid())->GetZHalfLength()*2.0;
  VOL_LIST.push_back(itsMarkerVol);
  G4String fileListLine;
  char buffer[1000];
  while (ifs >> fileListLine)
    {
      if(fileListLine.contains("#"))
	{ifs.getline(buffer,1000);} // a comment or info line - store it
      else
	{BuildSQLObjects((ContainingDir()).c_str() + fileListLine);} // a line with file to be parsed
    }
  
  // Close Geomlist file
  ifs.close();
}

void BDSGeometrySQL::BuildSQLObjects(G4String file)
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
      G4String ObjectType = TableName.substr(pos+1,TableName.length() - pos);
      G4String::caseCompare cmpmode = G4String::ignoreCase;
      NVariables = itsSQLTable[i]->GetVariable(0)->GetNVariables();
      for(G4int k=0; k<NVariables; k++){
	SetCommonParams(itsSQLTable[i], k);
	G4LogicalVolume* logVol;
	if(ObjectType.compareTo("CONE",cmpmode)==0) logVol = BuildCone(itsSQLTable[i],k);
	else if(ObjectType.compareTo("ELLIPTICALCONE",cmpmode)==0) logVol = BuildEllipticalCone(itsSQLTable[i],k);
	else if(ObjectType.compareTo("POLYCONE",cmpmode)==0) logVol = BuildPolyCone(itsSQLTable[i],k);
	else if(ObjectType.compareTo("BOX",cmpmode)==0) logVol = BuildBox(itsSQLTable[i],k);
	else if(ObjectType.compareTo("TRAP",cmpmode)==0) logVol =  BuildTrap(itsSQLTable[i],k);
	else if(ObjectType.compareTo("TORUS",cmpmode)==0) logVol = BuildTorus(itsSQLTable[i],k);
	else if(ObjectType.compareTo("SAMPLER",cmpmode)==0) logVol = BuildSampler(itsSQLTable[i],k);
	else if(ObjectType.compareTo("TUBE",cmpmode)==0) logVol =  BuildTube(itsSQLTable[i],k);
	else if(ObjectType.compareTo("ELLIPTICALTUBE",cmpmode)==0) logVol =  BuildEllipticalTube(itsSQLTable[i],k);
	//else if(ObjectType.compareTo("PCLTUBE",cmpmode)==0) logVol =  BuildPCLTube(itsSQLTable[i],k);
	else {
	  G4cerr << __METHOD_NAME__ << ObjectType << " not known" << G4endl;
	  exit(1);
	}
	//Set the user limits and visual attributes
	SetLogVolAtt(logVol, lengthUserLimit);
	VOL_LIST.push_back(logVol);
      }
      PlaceComponents(itsSQLTable[i], VOL_LIST);
    }
}

void BDSGeometrySQL::SetCommonParams(BDSMySQLTable* aSQLTable, G4int k){
  //Defaults
  VisRed = VisGreen = VisBlue = 0.5;
  VisAlpha = 0.5;
  VisType = "S";
  Material = BDSGlobalConstants::Instance()->VacuumMaterial();
  Name="";
  PrecisionRegion=0;
  ApproximationRegion=0;

  if(aSQLTable->GetVariable("RED")!=nullptr)
    VisRed = aSQLTable->GetVariable("RED")->GetDblValue(k);
  if(aSQLTable->GetVariable("BLUE")!=nullptr)
    VisBlue = aSQLTable->GetVariable("BLUE")->GetDblValue(k);
  if(aSQLTable->GetVariable("GREEN")!=nullptr)
    VisGreen = aSQLTable->GetVariable("GREEN")->GetDblValue(k);
  if(aSQLTable->GetVariable("ALPHA")!=nullptr)
    VisAlpha = aSQLTable->GetVariable("ALPHA")->GetDblValue(k);
  if(aSQLTable->GetVariable("VISATT")!=nullptr)
    VisType = aSQLTable->GetVariable("VISATT")->GetStrValue(k);
  if(aSQLTable->GetVariable("MATERIAL")!=nullptr)
    Material = aSQLTable->GetVariable("MATERIAL")->GetStrValue(k);
  if(aSQLTable->GetVariable("PRECISIONREGION")!=nullptr)
    PrecisionRegion = aSQLTable->GetVariable("PRECISIONREGION")->GetIntValue(k);
  if(aSQLTable->GetVariable("APPROXIMATIONREGION")!=nullptr)
    ApproximationRegion = aSQLTable->GetVariable("APPROXIMATIONREGION")->GetIntValue(k);
  if(aSQLTable->GetVariable("NAME")!=nullptr)
    Name = aSQLTable->GetVariable("NAME")->GetStrValue(k);
  if(Name=="_SQL") Name = TableName+std::to_string(k) + "_SQL";
  if(Name=="") Name = TableName+std::to_string(k);
  Name = itsMarkerVol->GetName()+"_"+Name;
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " k = " << k << ", Name = " << Name << G4endl;
#endif
}

void BDSGeometrySQL::SetPlacementParams(BDSMySQLTable* aSQLTable, G4int k){
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
      if(aSQLTable->GetVariable("PARENTNAME")!=nullptr)
	PARENTNAME = aSQLTable->GetVariable("PARENTNAME")->GetStrValue(k);
      if(aSQLTable->GetVariable("POSX")!=nullptr)
	PosX = aSQLTable->GetVariable("POSX")->GetDblValue(k);
      if(aSQLTable->GetVariable("POSY")!=nullptr)
	PosY = aSQLTable->GetVariable("POSY")->GetDblValue(k);
      if(aSQLTable->GetVariable("POSZ")!=nullptr)
	PosZ = aSQLTable->GetVariable("POSZ")->GetDblValue(k);
      if(aSQLTable->GetVariable("ROTPSI")!=nullptr)
	RotPsi = aSQLTable->GetVariable("ROTPSI")->GetDblValue(k);
      if(aSQLTable->GetVariable("ROTTHETA")!=nullptr)
	RotTheta = aSQLTable->GetVariable("ROTTHETA")->GetDblValue(k);
      if(aSQLTable->GetVariable("ROTPHI")!=nullptr)
	RotPhi = aSQLTable->GetVariable("ROTPHI")->GetDblValue(k);
      if(aSQLTable->GetVariable("K1")!=nullptr)
	K1 = aSQLTable->GetVariable("K1")->GetDblValue(k);
      if(aSQLTable->GetVariable("K2")!=nullptr)
	K2 = aSQLTable->GetVariable("K2")->GetDblValue(k);
      if(aSQLTable->GetVariable("K3")!=nullptr)
	K3 = aSQLTable->GetVariable("K3")->GetDblValue(k);
      if(aSQLTable->GetVariable("K4")!=nullptr)
	K4 = aSQLTable->GetVariable("K4")->GetDblValue(k);
      if(aSQLTable->GetVariable("MAGTYPE")!=nullptr)
	MagType = aSQLTable->GetVariable("MAGTYPE")->GetStrValue(k);
      if(aSQLTable->GetVariable("FIELDX")!=nullptr)
	FieldX = aSQLTable->GetVariable("FIELDX")->GetDblValue(k);
      if(aSQLTable->GetVariable("FIELDY")!=nullptr)
	FieldY = aSQLTable->GetVariable("FIELDY")->GetDblValue(k);
      if(aSQLTable->GetVariable("FIELDZ")!=nullptr)
	FieldZ = aSQLTable->GetVariable("FIELDZ")->GetDblValue(k);
      if(aSQLTable->GetVariable("ALIGNIN")!=nullptr)
	align_in = aSQLTable->GetVariable("ALIGNIN")->GetIntValue(k);
      if(aSQLTable->GetVariable("ALIGNOUT")!=nullptr)
	align_out = aSQLTable->GetVariable("ALIGNOUT")->GetIntValue(k);
      if(aSQLTable->GetVariable("SETSENSITIVE")!=nullptr)
	SetSensitive = aSQLTable->GetVariable("SETSENSITIVE")->GetIntValue(k);
      if(aSQLTable->GetVariable("INHERITSTYLE")!=nullptr)
	InheritStyle = aSQLTable->GetVariable("INHERITSTYLE")->GetStrValue(k);
      if(aSQLTable->GetVariable("PARAMETERISATION")!=nullptr)
	Parameterisation = aSQLTable->GetVariable("PARAMETERISATION")->GetStrValue(k);
      if(PARENTNAME==""){
	PosZ-=Length()/2.0; //Move position to beginning of element
      }
      PARENTNAME= itsMarkerVol->GetName() + "_" + PARENTNAME;
      if(aSQLTable->GetVariable("NAME")!=nullptr){
	Name = aSQLTable->GetVariable("NAME")->GetStrValue(k);
      }
      if(Name=="_SQL") Name = TableName+std::to_string(k) + "_SQL";
      if(Name=="") Name = TableName+std::to_string(k);
      Name = itsMarkerVol->GetName()+"_"+Name;
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << " k = " << k << ", Name = " << Name << G4endl;
#endif
}

G4VisAttributes* BDSGeometrySQL::VisAtt(){
  G4VisAttributes* VisAtt = 
    new G4VisAttributes(G4Colour(VisRed, VisGreen, VisBlue, VisAlpha));
  switch (VisType(0))
    {
    case 'W': VisAtt->SetForceWireframe(true); break;
    case 'I': VisAtt->SetVisibility(false); break;
    case 'S': VisAtt->SetForceSolid(true); break;
    case 'w': VisAtt->SetForceWireframe(true); break;
    case 'i': VisAtt->SetVisibility(false); break;
    case 's': VisAtt->SetForceSolid(true); break;
    }
  return VisAtt;
}

G4UserLimits* BDSGeometrySQL::UserLimits(G4double var){
  G4UserLimits* UserLimits = new G4UserLimits();
  UserLimits->SetMaxAllowedStep(var*0.5);
  UserLimits->SetUserMaxTime(BDSGlobalConstants::Instance()->MaxTime());
  if(BDSGlobalConstants::Instance()->ThresholdCutCharged()>0){
    UserLimits->SetUserMinEkine(BDSGlobalConstants::Instance()->ThresholdCutCharged());
  }
  return UserLimits;
}

//Set logical volume attributes
void BDSGeometrySQL::SetLogVolAtt(G4LogicalVolume* logVol, G4double k){
  logVol->SetVisAttributes(VisAtt());
#ifndef NOUSERLIMITS
  logVol->SetUserLimits(UserLimits(k));
#endif
  SetLogVolRegion(logVol);
}

void BDSGeometrySQL::SetLogVolRegion(G4LogicalVolume* logVol)
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

G4LogicalVolume* BDSGeometrySQL::BuildCone(BDSMySQLTable* aSQLTable, G4int k)
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
  
  if(aSQLTable->GetVariable("LENGTH")!=nullptr)
    length = aSQLTable->GetVariable("LENGTH")->GetDblValue(k);
  if(aSQLTable->GetVariable("RINNERSTART")!=nullptr)
    rInnerStart = aSQLTable->GetVariable("RINNERSTART")->GetDblValue(k);
  if(aSQLTable->GetVariable("RINNEREND")!=nullptr)
    rInnerEnd = aSQLTable->GetVariable("RINNEREND")->GetDblValue(k);
  if(aSQLTable->GetVariable("ROUTERSTART")!=nullptr)
    rOuterStart = aSQLTable->GetVariable("ROUTERSTART")->GetDblValue(k);
  if(aSQLTable->GetVariable("ROUTEREND")!=nullptr)
    rOuterEnd = aSQLTable->GetVariable("ROUTEREND")->GetDblValue(k);
  if(aSQLTable->GetVariable("STARTPHI")!=nullptr)
    sphi = aSQLTable->GetVariable("STARTPHI")->GetDblValue(k);
  if(aSQLTable->GetVariable("DELTAPHI")!=nullptr)
    dphi = aSQLTable->GetVariable("DELTAPHI")->GetDblValue(k);
  
  G4Cons* aCone = new G4Cons(Name+"_Cone",
			     rInnerStart,
			     rOuterStart,
			     rInnerEnd,
			     rOuterEnd,
			     length/2,
			     sphi,
			     dphi);
  
  G4LogicalVolume* aConeVol = 
    new G4LogicalVolume(aCone,
			BDSMaterials::Instance()->GetMaterial(Material),
			Name+"_LogVol");
  lengthUserLimit=length;
  return aConeVol;
}

G4LogicalVolume* BDSGeometrySQL::BuildEllipticalCone(BDSMySQLTable* aSQLTable, G4int k){

  G4double lengthZ = 0;
  G4double pxSemiAxis = 0;
  G4double pySemiAxis = 0;
  G4double pzTopCut = 0;
  
  //Defaults
  lengthZ = 10.*CLHEP::mm;
  
  if(aSQLTable->GetVariable("LENGTHZ")!=nullptr)
    lengthZ = aSQLTable->GetVariable("LENGTHZ")->GetDblValue(k);
  if(aSQLTable->GetVariable("XSEMIAXIS")!=nullptr)
    pxSemiAxis = aSQLTable->GetVariable("XSEMIAXIS")->GetDblValue(k);
  if(aSQLTable->GetVariable("YSEMIAXIS")!=nullptr)
    pySemiAxis = aSQLTable->GetVariable("YSEMIAXIS")->GetDblValue(k);
  if(aSQLTable->GetVariable("ZCUT")!=nullptr)
    pzTopCut = aSQLTable->GetVariable("ZCUT")->GetDblValue(k);
  
  G4EllipticalCone* aEllipticalCone = new G4EllipticalCone(Name+"_EllipticalCone",
							   pxSemiAxis,
							   pySemiAxis,
							       lengthZ/2,
							   pzTopCut);
  
  G4LogicalVolume* aEllipticalConeVol = 
    new G4LogicalVolume(aEllipticalCone,
			BDSMaterials::Instance()->GetMaterial(Material),
			Name+"_LogVol");

  lengthUserLimit=lengthZ*0.5;
  return aEllipticalConeVol;
}

G4LogicalVolume* BDSGeometrySQL::BuildPolyCone(BDSMySQLTable* aSQLTable, G4int k)
{
  G4int numZplanes;
  G4double* rInner = nullptr;
  G4double* rOuter = nullptr;
  G4double* zPos = nullptr;
  G4double sphi;
  G4double dphi;
  
  //Defaults
  sphi = 0.0;
  dphi = CLHEP::twopi*CLHEP::radian;
  numZplanes = 0;
  
  if(aSQLTable->GetVariable("NZPLANES")!=nullptr)
    numZplanes = aSQLTable->GetVariable("NZPLANES")->GetIntValue(k);
  rInner = new G4double[numZplanes+1];
  rOuter = new G4double[numZplanes+1];
  zPos = new G4double[numZplanes+1];
      
  for(G4int planenum=0; planenum<numZplanes; planenum++)
    {
      G4String rInner_ID = "RINNER" + std::to_string(planenum+1);
      G4String rOuter_ID = "ROUTER" + std::to_string(planenum+1);
      G4String zPos_ID = "PLANEPOS" + std::to_string(planenum+1);
      
      if(aSQLTable->GetVariable(rInner_ID)!=nullptr)
	rInner[planenum] = aSQLTable->GetVariable(rInner_ID)->GetDblValue(k);
      if(aSQLTable->GetVariable(rOuter_ID)!=nullptr)
	rOuter[planenum] = aSQLTable->GetVariable(rOuter_ID)->GetDblValue(k);
      
      if(aSQLTable->GetVariable(zPos_ID)!=nullptr)
	zPos[planenum] = aSQLTable->GetVariable(zPos_ID)->GetDblValue(k);
    }
  
  if(aSQLTable->GetVariable("STARTPHI")!=nullptr)
    sphi = aSQLTable->GetVariable("STARTPHI")->GetDblValue(k);
  if(aSQLTable->GetVariable("DELTAPHI")!=nullptr)
    dphi = aSQLTable->GetVariable("DELTAPHI")->GetDblValue(k);
  
  G4Polycone* aPolyCone = new G4Polycone(Name+"_PolyCone",
					 sphi,
					 dphi,
					 numZplanes,
					 zPos,
					 rInner,
					 rOuter);

  G4LogicalVolume* aPolyConeVol = 
    new G4LogicalVolume(aPolyCone,
			BDSMaterials::Instance()->GetMaterial(Material),
			Name+"_LogVol");

  lengthUserLimit=fabs(zPos[0]-zPos[numZplanes-1])/2;
  
  delete [] rInner;
  rInner = nullptr;
  delete [] rOuter;
  rOuter = nullptr;
  delete [] zPos;
  zPos = nullptr;
  
  return aPolyConeVol;
}

G4LogicalVolume* BDSGeometrySQL::BuildBox(BDSMySQLTable* aSQLTable, G4int k)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif

  G4double lengthX;
  G4double lengthY;
  G4double lengthZ;

  lengthX = lengthY = lengthZ = 10.*CLHEP::mm;
  
  if(aSQLTable->GetVariable("LENGTHX")!=nullptr)
    lengthX = aSQLTable->GetVariable("LENGTHX")->GetDblValue(k);
  if(aSQLTable->GetVariable("LENGTHY")!=nullptr)
    lengthY = aSQLTable->GetVariable("LENGTHY")->GetDblValue(k);
  if(aSQLTable->GetVariable("LENGTHZ")!=nullptr)
    lengthZ = aSQLTable->GetVariable("LENGTHZ")->GetDblValue(k);
  
  G4Box* aBox = new G4Box(Name+"_Box",
			  lengthX/2,
			  lengthY/2,
			  lengthZ/2);
  
  G4LogicalVolume* aBoxVol = 
    new G4LogicalVolume(aBox,
			BDSMaterials::Instance()->GetMaterial(Material),
			Name+"_LogVol");
  
  lengthUserLimit = lengthZ;
  return aBoxVol;
}

G4LogicalVolume* BDSGeometrySQL::BuildTrap(BDSMySQLTable* aSQLTable, G4int k)
{
  
  G4double trapTheta = 0; //Angle between faces of trapezoid
  G4double lengthXPlus = 0;
  G4double lengthXMinus = 0;
  G4double lengthYPlus = 0;
  G4double lengthYMinus = 0;
  G4double lengthZ = 0;
  
  if(aSQLTable->GetVariable("TRAPTHETA")!=nullptr)
    trapTheta = aSQLTable->GetVariable("TRAPTHETA")->GetDblValue(k);
  if(aSQLTable->GetVariable("LENGTHXPLUS")!=nullptr)
    lengthXPlus = aSQLTable->GetVariable("LENGTHXPLUS")->GetDblValue(k);
  if(aSQLTable->GetVariable("LENGTHXMINUS")!=nullptr)
    lengthXMinus = aSQLTable->GetVariable("LENGTHXMINUS")->GetDblValue(k);
  if(aSQLTable->GetVariable("LENGTHYPLUS")!=nullptr)
    lengthYPlus = aSQLTable->GetVariable("LENGTHYPLUS")->GetDblValue(k);
  if(aSQLTable->GetVariable("LENGTHYMINUS")!=nullptr)
    lengthYMinus = aSQLTable->GetVariable("LENGTHYMINUS")->GetDblValue(k);
  if(aSQLTable->GetVariable("LENGTHZ")!=nullptr)
    lengthZ = aSQLTable->GetVariable("LENGTHZ")->GetDblValue(k);
  
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
  
  
  G4LogicalVolume* aTrapVol = 
    new G4LogicalVolume(aTrap,
			BDSMaterials::Instance()->GetMaterial(Material),
			Name+"_LogVol");

  lengthUserLimit = lengthZ*0.5;
  return aTrapVol;
}

G4LogicalVolume* BDSGeometrySQL::BuildTorus(BDSMySQLTable* aSQLTable, G4int k)
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
  
  if(aSQLTable->GetVariable("RINNER")!=nullptr)
    rInner = aSQLTable->GetVariable("RINNER")->GetDblValue(k);
  if(aSQLTable->GetVariable("ROUTER")!=nullptr)
    rOuter = aSQLTable->GetVariable("ROUTER")->GetDblValue(k);
  if(aSQLTable->GetVariable("RSWEPT")!=nullptr)
    rSwept = aSQLTable->GetVariable("RSWEPT")->GetDblValue(k);
  if(aSQLTable->GetVariable("STARTPHI")!=nullptr)
    sphi = aSQLTable->GetVariable("STARTPHI")->GetDblValue(k);
  if(aSQLTable->GetVariable("DELTAPHI")!=nullptr)
    dphi = aSQLTable->GetVariable("DELTAPHI")->GetDblValue(k);
  
  G4Torus* aTorus = new G4Torus(Name+"_Torus",
				rInner,
				rOuter,
				rSwept,
				sphi,
				dphi);
  

  G4LogicalVolume* aTorusVol = 
    new G4LogicalVolume(aTorus,
			BDSMaterials::Instance()->GetMaterial(Material),
			Name+"_LogVol");

  lengthUserLimit = rOuter*0.5;
  return aTorusVol;
}

G4LogicalVolume* BDSGeometrySQL::BuildSampler(BDSMySQLTable* aSQLTable, G4int k)
{
  
  G4double length;
  G4double rInnerStart;
  G4double rInnerEnd;
  G4double rOuterStart;
  G4double rOuterEnd;
  
  //Defaults
  length = rOuterStart = rOuterEnd = 10.*CLHEP::mm;
  rInnerStart = rInnerEnd = 0.0;
  
  if(aSQLTable->GetVariable("LENGTH")!=nullptr)
    length = aSQLTable->GetVariable("LENGTH")->GetDblValue(k);
  if(aSQLTable->GetVariable("RINNERSTART")!=nullptr)
    rInnerStart = aSQLTable->GetVariable("RINNERSTART")->GetDblValue(k);
  if(aSQLTable->GetVariable("RINNEREND")!=nullptr)
	rInnerEnd = aSQLTable->GetVariable("RINNEREND")->GetDblValue(k);
  if(aSQLTable->GetVariable("ROUTERSTART")!=nullptr)
    rOuterStart = aSQLTable->GetVariable("ROUTERSTART")->GetDblValue(k);
  if(aSQLTable->GetVariable("ROUTEREND")!=nullptr)
    rOuterEnd = aSQLTable->GetVariable("ROUTEREND")->GetDblValue(k);
  if(aSQLTable->GetVariable("NAME")!=nullptr)
    {
      Name = aSQLTable->GetVariable("NAME")->GetStrValue(k);
      aSQLTable->GetVariable("NAME")->SetStrValue(k,Name+"_SQL");
      Name = aSQLTable->GetVariable("NAME")->GetStrValue(k);
    }
  if(Name=="_SQL") Name = TableName+std::to_string(k)+"_SQL";
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

  G4LogicalVolume* aSamplerVol = 
    new G4LogicalVolume(aSampler,
			BDSMaterials::Instance()->GetMaterial(Material),
			Name+"_LogVol");

  lengthUserLimit = length*0.5;
  
  aSamplerVol->SetSensitiveDetector(BDSSDManager::Instance()->GetSamplerPlaneSD());

  G4int samplerID = BDSSamplerRegistry::Instance()->RegisterSampler(Name,nullptr);
  
  samplerIDs[aSamplerVol] = samplerID;
  
  return aSamplerVol;
}

G4LogicalVolume* BDSGeometrySQL::BuildTube(BDSMySQLTable* aSQLTable, G4int k)
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
  
  if(aSQLTable->GetVariable("RINNER")!=nullptr)
    rInner = aSQLTable->GetVariable("RINNER")->GetDblValue(k);
  if(aSQLTable->GetVariable("ROUTER")!=nullptr)
    rOuter = aSQLTable->GetVariable("ROUTER")->GetDblValue(k);
  if(aSQLTable->GetVariable("LENGTH")!=nullptr)
    length = aSQLTable->GetVariable("LENGTH")->GetDblValue(k);
  if(aSQLTable->GetVariable("STARTPHI")!=nullptr)
    sphi = aSQLTable->GetVariable("STARTPHI")->GetDblValue(k);
  if(aSQLTable->GetVariable("DELTAPHI")!=nullptr)
    dphi = aSQLTable->GetVariable("DELTAPHI")->GetDblValue(k);
  
  G4Tubs* aTubs = new G4Tubs(Name+"_Tubs",
			     rInner,
			     rOuter,
			     length/2,
			     sphi,
			     dphi);
  
  G4LogicalVolume* aTubsVol = 
    new G4LogicalVolume(aTubs,
			BDSMaterials::Instance()->GetMaterial(Material),
			Name+"_LogVol");
  
  lengthUserLimit = length*0.5;
  return aTubsVol;
}

G4LogicalVolume* BDSGeometrySQL::BuildEllipticalTube(BDSMySQLTable* aSQLTable, G4int k)
{
  G4double lengthX;
  G4double lengthY;
  G4double lengthZ;
  
  //Defaults 
  lengthX = 100.*CLHEP::mm;
  lengthY = 50.*CLHEP::mm;
  lengthZ = 200.*CLHEP::mm;

  if(aSQLTable->GetVariable("LENGTHX")!=nullptr)
    lengthX = aSQLTable->GetVariable("LENGTHX")->GetDblValue(k);
  if(aSQLTable->GetVariable("LENGTHY")!=nullptr)
    lengthY = aSQLTable->GetVariable("LENGTHY")->GetDblValue(k);
  if(aSQLTable->GetVariable("LENGTHZ")!=nullptr)
    lengthZ = aSQLTable->GetVariable("LENGTHZ")->GetDblValue(k);
  
  G4EllipticalTube* aEllipticalTube = new G4EllipticalTube(Name+"_EllipticalTube",
							   lengthX/2,
							   lengthY/2,
							   lengthZ/2
							   );
  
  
  G4LogicalVolume* aEllipticalTubeVol = 
    new G4LogicalVolume(aEllipticalTube,
			BDSMaterials::Instance()->GetMaterial(Material),
			Name+"_LogVol");
  G4double maxLength = lengthX;
  if (lengthY>lengthX&&lengthY>lengthZ){
    maxLength = lengthY;
  }
  else if(lengthZ>lengthY&&lengthZ>lengthX){
    maxLength = lengthZ;
  }
  lengthUserLimit = maxLength*0.5;
  return aEllipticalTubeVol;
}

/*
G4LogicalVolume* BDSGeometrySQL::BuildPCLTube(BDSMySQLTable* aSQLTable, G4int k)
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
  thickness = BDSGlobalConstants::Instance()->GetDefaultBeamPipeModel()->beamPipeThickness;
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

G4RotationMatrix* BDSGeometrySQL::RotateComponent(G4double psi,G4double phi,G4double theta)
{
  rotateComponent = new G4RotationMatrix;
  if(psi==0 && phi==0 && theta==0) return rotateComponent;

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


void BDSGeometrySQL::PlaceComponents(BDSMySQLTable* aSQLTable, std::vector<G4LogicalVolume*> VOL_LIST)
{
  G4String::caseCompare cmpmode = G4String::ignoreCase;
  for(G4int k=0; k<NVariables; k++) // Now run through and place according to
    { 
      SetPlacementParams(aSQLTable, k);
      G4int PARENTID=0;
      if(PARENTNAME!=""){
	PARENTNAME+="_LogVol";
	for(G4int i=0; i<(G4int)VOL_LIST.size(); i++)
	  {
	    if(PARENTNAME.compareTo(VOL_LIST[i]->GetName(),cmpmode)==0)
	      {
		PARENTID = i;
		continue;
	      }
	  }
      }
      
      // to being in line with logvol names (needed for name checking loop
      G4String tmpname = Name+"_LogVol";
      G4int ID=0;
      for(G4int i=0; i<(G4int)VOL_LIST.size(); i++)
	{
	  if(tmpname.compareTo(VOL_LIST[i]->GetName(),cmpmode)==0)
	    {
	      ID = i;
	      continue;
	    }
	}

      if(SetSensitive) sensitiveComponents.push_back(VOL_LIST[ID]);
      G4ThreeVector PlacementPoint(PosX,PosY,PosZ);

      if(InheritStyle.compareTo("",cmpmode)){ //True if InheritStyle is set
	if(InheritStyle.compareTo("SUBTRACT",cmpmode)==0)
	  {
	    G4VSolid* original = VOL_LIST[PARENTID]->GetSolid();
	    G4VSolid* sub = VOL_LIST[ID]->GetSolid();
	    VOL_LIST[PARENTID]->SetSolid(new G4SubtractionSolid(VOL_LIST[PARENTID]->GetName(),
								original,
								sub,
								RotateComponent(RotPsi,RotPhi,RotTheta),
								PlacementPoint));
	    
	  }else if(InheritStyle.compareTo("INTERSECT",cmpmode)==0){
	  G4VSolid* original = VOL_LIST[PARENTID]->GetSolid();
	  G4VSolid* sub = VOL_LIST[ID]->GetSolid();
	  VOL_LIST[PARENTID]->SetSolid(new G4IntersectionSolid(VOL_LIST[PARENTID]->GetName(),
							       original,
							       sub,
							       RotateComponent(RotPsi,RotPhi,RotTheta),
							       PlacementPoint));
	
	} else if(InheritStyle.compareTo("UNION",cmpmode)==0)
	  {
	    G4VSolid* original = VOL_LIST[PARENTID]->GetSolid();
	    G4VSolid* sub = VOL_LIST[ID]->GetSolid();
	    VOL_LIST[PARENTID]->SetSolid(new G4UnionSolid(VOL_LIST[PARENTID]->GetName(),
							  original,
							  sub,
							  RotateComponent(RotPsi,RotPhi,RotTheta),
							  PlacementPoint));
	  }
      }

      if(Parameterisation.compareTo("GFLASH",cmpmode)==0){       
	gFlashComponents.push_back(VOL_LIST[ID]);
      }

#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << " k = " << k << ", volume = " << VOL_LIST[ID]->GetName() << G4endl;
#endif

      G4LogicalVolume* volume = VOL_LIST[ID];
      G4int copyNumber = 0;
      auto result = samplerIDs.find(volume);
      if (result != samplerIDs.end())
	{copyNumber = result->second;}
      
      G4VPhysicalVolume* PhysiComp = 
	new G4PVPlacement(RotateComponent(RotPsi,RotPhi,RotTheta),
			  PlacementPoint,
			  VOL_LIST[ID],
			  Name,
			  VOL_LIST[PARENTID],
			  false,
			  copyNumber,
                      BDSGlobalConstants::Instance()->CheckOverlaps());
      if(align_in)
	{
	  // Make sure program stops and informs user if more than one alignment vol.
	  if(GetAlignInVolume())
	    {
	      G4cerr<<"\nBDSGeometrySQL.cc:486: Trying to align in-beam to SQL volume to " << PhysiComp->GetName() << " but alignment already set to " << GetAlignInVolume()->GetName() << G4endl;
	      G4Exception("Aborting Program", "-1", FatalException, "");

	    }

	  else
	    {alignInVolume = PhysiComp;}
	}

      if(align_out)
	{
	  if(GetAlignOutVolume())
	    {
	      G4cerr<<"\nBDSGeometrySQL.cc:486: Trying to align out-beam to SQL volume to " << PhysiComp->GetName() << " but alignment already set to " << GetAlignOutVolume()->GetName() << G4endl;
	      G4Exception("Aborting Program", "-1", FatalException, "");
	    }

	  else
	    {alignOutVolume = PhysiComp;}
	}

//      G4double P0 = BDSGlobalConstants::Instance()->BeamTotalEnergy();
//      G4double brho=
//	sqrt(pow(P0,2)- pow(electron_mass_c2,2))/(0.299792458 * (CLHEP::GeV/(CLHEP::tesla*CLHEP::m)));

      // compute magnetic rigidity brho
      // formula: B(Tesla)*rho(m) = p(GeV)/(0.299792458 * |charge(e)|)
      //
      // charge (in |e| units)
      G4double charge = BDSGlobalConstants::Instance()->GetParticleDefinition()->GetPDGCharge();  
      // momentum (in GeV/c)   
      G4double momentum = BDSGlobalConstants::Instance()->BeamMomentum();
      // rigidity (in T*m)
      G4double brho = ( (momentum/CLHEP::GeV) / (0.299792458 * charge));
      // rigidity (in Geant4 units)
      brho *= (CLHEP::tesla*CLHEP::m);

      if(MagType.compareTo("QUAD",cmpmode)==0)
	{
	  hasFields = true;
	  nPoleField = 1;
	  QuadBgrad.push_back(- brho * K1 / CLHEP::m2);
	  Quadvol.push_back(PhysiComp->GetName());
	  quadVolBgrad[PhysiComp->GetName()]=(- brho * K1 / CLHEP::m2);
	}

      if(MagType.compareTo("SEXT",cmpmode)==0)
	{
	  hasFields = true;
	  nPoleField = 2;
	  SextBgrad.push_back(- brho * K2 / CLHEP::m3);
	  Sextvol.push_back(PhysiComp->GetName());
	  sextVolBgrad[PhysiComp->GetName()]=(- brho * K2 / CLHEP::m3);
	}

      if(MagType.compareTo("OCT",cmpmode)==0)
	{
	  hasFields = true;
	  nPoleField = 3;
	  OctBgrad.push_back(- brho * K3 / (CLHEP::m2*CLHEP::m2));
	  Octvol.push_back(PhysiComp->GetName());
	  octVolBgrad[PhysiComp->GetName()]=(- brho * K3 / (CLHEP::m2*CLHEP::m2));
	}

      if(FieldX || FieldY || FieldZ) //if any vols have non-zero field components
	{
	  hasFields = true;
	  hasUniformField=true;
#ifdef BDSDEBUG
	  G4cout << "BDSGeometrySQL> volume " << PhysiComp->GetName() << " has the following uniform field: " << FieldX << " " << FieldY << " " << FieldZ << " " << G4endl;
#endif
	  UniformField.push_back(G4ThreeVector(FieldX*CLHEP::tesla,
					       FieldY*CLHEP::tesla,
					       FieldZ*CLHEP::tesla));
	  Fieldvol.push_back(PhysiComp->GetName());
	  uniformFieldVolField[PhysiComp->GetName()]=G4ThreeVector(FieldX*CLHEP::tesla,
								   FieldY*CLHEP::tesla,
								   FieldZ*CLHEP::tesla);
	}
  }
}
