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

//============================================================

BDSPCLDrift::BDSPCLDrift (G4String aName, G4double aLength, 
			  std::list<G4double> blmLocZ, std::list<G4double> blmLocTheta, G4double aperX, G4double aperYUp, G4double aperYDown, G4double aperDy, G4String tunnelMaterial, G4double aper, G4double tunnelRadius, G4double tunnelOffsetX, G4String aMaterial):
  BDSMultipole(aName, aLength, aper, aper, blmLocZ, blmLocTheta, tunnelMaterial, aMaterial, aper, aper, 0, tunnelRadius, tunnelOffsetX),
  itsYAperUp(aperYUp), itsYAperDown(aperYDown), itsDyAper(aperDy),
  outer_solid(NULL),inner_solid(NULL),itsOuterBeamPipeLogicalVolume(NULL),
  itsInnerBeamPipeLogicalVolume(NULL),itsPhysiInner(NULL),itsPhysiOuter(NULL),
  itsBeampipeVisAtt(NULL),itsInnerBeampipeVisAtt(NULL)
{
  itsXAper=aperX;
}

void BDSPCLDrift::BuildBeampipe(){
  G4String materialName = "";
  G4Material* material;
  if(materialName != ""){
    material = BDSMaterials::Instance()->GetMaterial( materialName );
  } else {
    material = BDSMaterials::Instance()->GetMaterial( BDSGlobalConstants::Instance()->GetBeamPipeMaterialName());
  }
  
  // build beampipe

#ifdef BDSDEBUG 
  G4cout << "Outer pipe :"
         << " r= " << itsBpRadius/CLHEP::m << " m"
         << " l= " << itsLength/(2.)/CLHEP::m << " m"
         << G4endl;
  G4cout << "PCLDrift aperX: " << itsXAper/CLHEP::m << " m" << G4endl;
  G4cout << "PCLDrift aperYUp: " << itsYAperUp/CLHEP::m << " m" << G4endl;
  G4cout << "PCLDrift aperYDown: " << itsYAperDown/CLHEP::m << " m" << G4endl;
  G4cout << "PCLDrift Dy: " << itsDyAper/CLHEP::m << " m" << G4endl;
#endif

  G4double ts = BDSGlobalConstants::Instance()->LengthSafety()+itsBeampipeThickness/2;

  BDSPCLTube* innerTube = new BDSPCLTube(itsXAper-ts, itsYAperUp-ts, itsYAperDown-ts, itsDyAper, -1, itsLength, itsName+"_inner");

  BDSPCLTube* outerTube = new BDSPCLTube(itsXAper, itsYAperUp, itsYAperDown, itsDyAper, itsBeampipeThickness, itsLength, itsName+"_outer");
  
  //The inner beam pipe solids need to be fused together to form one seamless shape in order to create a vacuum and field without gaps in it.

  inner_solid = innerTube->GetSolid();
  outer_solid = outerTube->GetSolid();

  delete innerTube;
  delete outerTube;

#ifdef BDSDEBUG
  G4cout << "BDSPCLDrift.cc: Making logical..." << G4endl;
#endif

  itsInnerBeamPipeLogicalVolume=	
    new G4LogicalVolume(inner_solid,
			BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->VacuumMaterial()),
			itsName+"_inner_bmp_log");

  itsOuterBeamPipeLogicalVolume=	
    new G4LogicalVolume(outer_solid,
			material,
			itsName+"_outer_bmp_log");
  
  //
  // set visualization attributes
  //
  itsInnerBeampipeVisAtt =  new G4VisAttributes(G4Colour(0.1, 0.1, 0.1));
  itsInnerBeampipeVisAtt->SetVisibility(true);
  itsInnerBeampipeVisAtt->SetForceSolid(true);

  itsBeampipeVisAtt = new G4VisAttributes(G4Colour(0.5, 0.5, 0.5));
  itsBeampipeVisAtt->SetForceSolid(true);
  itsBeampipeVisAtt->SetVisibility(true);

  itsOuterBeamPipeLogicalVolume->SetVisAttributes(itsBeampipeVisAtt);
  itsInnerBeamPipeLogicalVolume->SetVisAttributes(itsInnerBeampipeVisAtt);

#ifdef BDSDEBUG  
  G4cout << "BDSPCLDrift.cc: Placing..." << G4endl;
#endif

  G4ThreeVector threeVector1;
  threeVector1.setY(0);
  itsPhysiInner = new G4PVPlacement(
				    (G4RotationMatrix*)0,		        // no rotation
				    threeVector1,	               
				    itsInnerBeamPipeLogicalVolume,  // its logical volume
				    itsName+"_inner_bmp_phys",// its name
				    itsMarkerLogicalVolume,   // its mother  volume
				    false,		        // no boolean operation
				    0, BDSGlobalConstants::Instance()->CheckOverlaps());		        // copy number


  
  itsPhysiOuter = new G4PVPlacement(
				    (G4RotationMatrix*)0,		        // no rotation
				    threeVector1,	               
				    itsOuterBeamPipeLogicalVolume,  // its logical volume
				    itsName+"_inner_bmp_phys",// its name
				    itsMarkerLogicalVolume,   // its mother  volume
				    false,		        // no boolean operation
				    0, BDSGlobalConstants::Instance()->CheckOverlaps());		        // copy number




  //Add the physical volumes to a vector which can be used for e.g. geometrical biasing
  SetMultiplePhysicalVolumes(itsPhysiInner);
  SetMultiplePhysicalVolumes(itsPhysiOuter);
  //
  // define sensitive volumes for hit generation
  //
  if(BDSGlobalConstants::Instance()->SensitiveBeamPipe()){
    AddSensitiveVolume(itsOuterBeamPipeLogicalVolume);	
  }

#ifndef NOUSERLIMITS
  itsBeampipeUserLimits =  new G4UserLimits("beampipe cuts");
  itsBeampipeUserLimits->SetUserMinEkine(BDSGlobalConstants::Instance()->ThresholdCutCharged());
  itsBeampipeUserLimits->SetUserMaxTime(BDSGlobalConstants::Instance()->MaxTime());

  itsInnerBeampipeUserLimits =  new G4UserLimits("inner beampipe cuts");
  itsInnerBeampipeUserLimits->SetUserMinEkine(BDSGlobalConstants::Instance()->ThresholdCutCharged());
  itsInnerBeampipeUserLimits->SetUserMaxTime(BDSGlobalConstants::Instance()->MaxTime());

  G4double stepfactor = 5;

  itsBeampipeUserLimits->SetMaxAllowedStep(itsLength*stepfactor);
  itsInnerBeampipeUserLimits->SetMaxAllowedStep(itsLength*stepfactor);

  itsInnerBeamPipeLogicalVolume->SetUserLimits(itsInnerBeampipeUserLimits);
  itsOuterBeamPipeLogicalVolume->SetUserLimits(itsBeampipeUserLimits);


#endif

   itsInnerBeamPipeLogicalVolume->SetFieldManager(itsFieldMgr,false);
   itsOuterBeamPipeLogicalVolume->SetFieldManager(itsFieldMgr,false);

  
  // now protect the fields inside the marker volume by giving the
  // marker a null magnetic field (otherwise G4VPlacement can
  // over-ride the already-created fields, by calling 
  // G4LogicalVolume::AddDaughter, which calls 
  // pDaughterLogical->SetFieldManager(fFieldManager, true) - the
  // latter 'true' over-writes all the other fields
  
  itsMarkerLogicalVolume->
    SetFieldManager(BDSGlobalConstants::Instance()->GetZeroFieldManager(),false);
  
#ifdef BDSDEBUG
  G4cout << "BDSPCLDrift.cc: Finished making beam pipe..." << G4endl;
#endif
}

void BDSPCLDrift::BuildBPFieldAndStepper(){
    // set up the magnetic field and stepper
  itsMagField=new BDSMagField(); //Zero magnetic field.
  itsEqRhs=new G4Mag_UsualEqRhs(itsMagField);
  itsStepper = new BDSDriftStepper(itsEqRhs);
}

void BDSPCLDrift::BuildBLMs(){
  BDSAcceleratorComponent::BuildBLMs();
}

void BDSPCLDrift::SetVisAttributes()
{
  itsVisAttributes=new G4VisAttributes(G4Colour(0.5,0.5,0.5));
}

BDSPCLDrift::~BDSPCLDrift()
{
  delete itsBeampipeVisAtt;
  delete itsInnerBeampipeVisAtt;
}

void BDSPCLDrift::BuildOuterVolume()
{
  return;
}
