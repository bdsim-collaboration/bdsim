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
//=================================================================
//=================================================================

#include "BDSDetectorMessenger.hh"

#include "BDSDetectorConstruction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithoutParameter.hh"

//=================================================================

BDSDetectorMessenger::BDSDetectorMessenger(BDSDetectorConstruction * BDSDet)
:BDSDetector(BDSDet)
{ 
  /*
  BDSdetDir = new G4UIdirectory("/quad/");
  BDSdetDir->SetGuidance("BDS detector control.");
  */  
  //  AbsMaterCmd = new G4UIcmdWithAString("/quad/setAbsMat",this);
  // AbsMaterCmd->SetGuidance("Select Material of the Absorber.");
  //AbsMaterCmd->SetParameterName("choice",false);
  //AbsMaterCmd->AvailableForStates(Idle);
  
  //  GapMaterCmd = new G4UIcmdWithAString("/quad/setGapMat",this);
  //  GapMaterCmd->SetGuidance("Select Material of the Gap.");
  //  GapMaterCmd->SetParameterName("choice",false);
  //  GapMaterCmd->AvailableForStates(Idle);
    
  //  AbsThickCmd = new G4UIcmdWithADoubleAndUnit("/quad/setAbsThick",this);
  //  AbsThickCmd->SetGuidance("Set Thickness of the Absorber");
  //  AbsThickCmd->SetParameterName("Size",false);
  //  AbsThickCmd->SetRange("Size>=0.");
  //  AbsThickCmd->SetUnitCategory("Length");
  //  AbsThickCmd->AvailableForStates(Idle);
  
  //  GapThickCmd = new G4UIcmdWithADoubleAndUnit("/quad/setGapThick",this);
  //  GapThickCmd->SetGuidance("Set Thickness of the Gap");
  //  GapThickCmd->SetParameterName("Size",false);
  //  GapThickCmd->SetRange("Size>=0.");
  //  GapThickCmd->SetUnitCategory("Length");  
  //  GapThickCmd->AvailableForStates(Idle);
  
  //  SizeYZCmd = new G4UIcmdWithADoubleAndUnit("/quad/setSizeYZ",this);
  //  SizeYZCmd->SetGuidance("Set tranverse size of the quadimeter");
  //  SizeYZCmd->SetParameterName("Size",false);
  //  SizeYZCmd->SetRange("Size>0.");
  //  SizeYZCmd->SetUnitCategory("Length");    
  //  SizeYZCmd->AvailableForStates(Idle);
  
  //  NbLayersCmd = new G4UIcmdWithAnInteger("/quad/setNbOfLayers",this);
  //  NbLayersCmd->SetGuidance("Set number of layers.");
  //  NbLayersCmd->SetParameterName("NbLayers",false);
  //  NbLayersCmd->SetRange("NbLayers>0 && NbLayers<500");
  //  NbLayersCmd->AvailableForStates(Idle);

  //  UpdateCmd = new G4UIcmdWithoutParameter("/quad/update",this);
  //  UpdateCmd->SetGuidance("Update quadimeter geometry.");
  //  UpdateCmd->SetGuidance("This command MUST be applied before \"beamOn\" ");
  //  UpdateCmd->SetGuidance("if you changed geometrical value(s).");
  //  UpdateCmd->AvailableForStates(Idle);
  /*
  MagFieldCmd = new G4UIcmdWithADoubleAndUnit("/quad/setField",this);  
  MagFieldCmd->SetGuidance("Define magnetic field.");
  MagFieldCmd->SetGuidance("Magnetic field will be in Y direction.");
  MagFieldCmd->SetParameterName("By",false);
  MagFieldCmd->SetUnitCategory("Magnetic flux density");
  MagFieldCmd->AvailableForStates(Idle);  
  */
}

//=================================================================

BDSDetectorMessenger::~BDSDetectorMessenger()
{
  //  delete NbLayersCmd;
  //  delete AbsMaterCmd; delete GapMaterCmd;
  //  delete AbsThickCmd; delete GapThickCmd;
  //  delete SizeYZCmd;   delete UpdateCmd;
  /*
  delete MagFieldCmd;
  delete BDSdetDir;
  */
}

//=================================================================

void BDSDetectorMessenger::SetNewValue(G4UIcommand* /*command*/,G4String /*newValue*/)
{ 
  //  if( command == AbsMaterCmd )
  //   { BDSDetector->SetAbsorberMaterial(newValue);}
   
  //  if( command == GapMaterCmd )
  //   { BDSDetector->SetGapMaterial(newValue);}
  
  //  if( command == AbsThickCmd )
  //   { BDSDetector->SetAbsorberThickness(AbsThickCmd->GetNewDoubleValue(newValue));}
   
  //  if( command == GapThickCmd )
  //   { BDSDetector->SetGapThickness(GapThickCmd->GetNewDoubleValue(newValue));}
   
  //  if( command == SizeYZCmd )
  //   { BDSDetector->SetQuadSizeYZ(SizeYZCmd->GetNewDoubleValue(newValue));}
   
  //  if( command == NbLayersCmd )
  //   { BDSDetector->SetNbOfLayers(NbLayersCmd->GetNewIntValue(newValue));}
  
  //  if( command == UpdateCmd )
  //   { BDSDetector->UpdateGeometry(); }

  /*
  if( command == MagFieldCmd )
   { BDSDetector->SetMagField(MagFieldCmd->GetNewDoubleValue(newValue));}
  */
}

//=================================================================
