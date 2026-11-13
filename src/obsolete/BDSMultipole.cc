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
void BDSMultipole::BuildEllipticalOuterLogicalVolume(G4bool OuterMaterialIsVacuum)
{
  //OuterMaterialIsVacuum parameter is useless: one can set
  //itsMaterial = BDSGlobalConstants::Instance()->VacuumMaterial() and obtain the same result. Or cannot?

  G4Material* material;
  if(itsMaterial != "") material = BDSMaterials::Instance()->GetMaterial(itsMaterial);
  else material = BDSMaterials::Instance()->GetMaterial("Iron");
  if(OuterMaterialIsVacuum) material = BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetVacuumMaterial());
  G4double outerRadius = itsOuterR;
  if(itsOuterR==0) outerRadius = BDSGlobalConstants::Instance()->GetComponentBoxSize()/2;

  G4Tubs* tubs_tmp= new G4Tubs(itsName+"_tubs_tmp",
			       0,
			       outerRadius,
			       itsLength/2-BDSGlobalConstants::Instance()->GetLengthSafety(),
			       0,CLHEP::twopi*CLHEP::radian);

  G4EllipticalTube* etube_tmp= new G4EllipticalTube(itsName+"_etube_tmp",
                                                    this->GetAperX()+1*CLHEP::nm,
                                                    this->GetAperY()+1*CLHEP::nm,
                                                    itsLength/2-BDSGlobalConstants::Instance()->GetLengthSafety());
  
 

  G4LogicalVolume* itsOuterLogicalVolume=
    new G4LogicalVolume(new G4SubtractionSolid(itsName+"_outer_solid",
					       tubs_tmp, etube_tmp),
			material,
			itsName+"_outer_log");
  
  itsPhysiComp = new G4PVPlacement(
				   (G4RotationMatrix*)0,		      // no rotation
				   (G4ThreeVector)0,                      // its position
				   itsOuterLogicalVolume,  // its logical volume
				   itsName+"_outer_phys",  // its name
				   itsMarkerLogicalVolume, // its mother  volume
				   false,		      // no boolean operation
				   0, BDSGlobalConstants::Instance()->GetCheckOverlaps());		      // copy number
  
  //Add the physical volumes to a vector which can be used for e.g. geometrical biasing
  SetMultiplePhysicalVolumes(itsPhysiComp);
  
#ifndef NOUSERLIMITS
  G4double maxStepFactor=0.5;
  itsOuterUserLimits = new G4UserLimits("multipole cut");
  itsOuterUserLimits->SetUserMinEkine(BDSGlobalConstants::Instance()->GetThresholdCutCharged());
  itsOuterUserLimits->SetMaxAllowedStep(itsLength*maxStepFactor);
  itsOuterLogicalVolume->SetUserLimits(itsOuterUserLimits);
#endif
}
