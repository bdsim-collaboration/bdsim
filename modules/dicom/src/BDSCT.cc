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
#include "BDSAcceleratorComponent.hh"
#include "BDSColours.hh"
#include "BDSCT.hh"
#include "BDSDicomFileMgr.hh"
#include "BDSDicomIntersectVolume.hh"
#include "BDSDicomPhantomParameterisationColour.hh"
#include "BDSExtent.hh"

#include "G4Box.hh"
#include "G4Colour.hh"
#include "G4LogicalVolume.hh"
#include "G4MultiFunctionalDetector.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4PVParameterised.hh"
#include "G4String.hh"
#include "G4Tubs.hh"
#include "G4Types.hh"
#include "G4UserLimits.hh"
#include "G4Version.hh"
#include "G4VisAttributes.hh"

#include "CLHEP/Units/SystemOfUnits.h"

BDSCT::BDSCT(const G4String& nameIn,
             const G4String& dataFilePath,
             const G4String& dataFileName):
  BDSAcceleratorComponent(nameIn, 1, 0, "ct"),
  dicomDataPath(dataFilePath),
  dicomDataFile(dataFileName)
{
  //--- As soon as the object is constructed, we retrieve data from the CT files
  theFileMgr = BDSDicomFileMgr::GetInstance();
  theFileMgr->Convert(dicomDataPath, dicomDataFile);
  new BDSDicomIntersectVolume();
}

BDSCT::~BDSCT()
{;}

void BDSCT::Build()
{
  if (!fConstructed || fWorld_phys == nullptr)
    {
      fConstructed = true;
      InitialisationOfMaterials();

      //----- Build world
      G4double worldXDimension = 1. * CLHEP::m;
      G4double worldYDimension = 1. * CLHEP::m;
      G4double worldZDimension = 1. * CLHEP::m;
      
      fWorld_solid = new G4Box("WorldSolid",
			       worldXDimension,
			       worldYDimension,
			       worldZDimension);
      
      fWorld_logic = new G4LogicalVolume(fWorld_solid,
					 fAir,
					 "WorldLogical",
					 nullptr, nullptr, nullptr);
      
      fWorld_phys = new G4PVPlacement(nullptr,
				      G4ThreeVector(0, 0, 0),
				      "World",
				      fWorld_logic,
				      nullptr,
				      false,
				      0);
      
      //--- We retrieve data stored in out file and analyse it in order to convert it with vectors
      ReadPhantomData();
      BuildContainerLogicalVolume();
      BuildPhantom();
    }
}

void BDSCT::BuildPhantom()
{
  //----- Create parameterisation
  BDSDicomPhantomParameterisationColour* param = new BDSDicomPhantomParameterisationColour(dicomDataPath + "ColourMap.dat");
  
  //----- Set voxel dimensions
  param->SetVoxelDimensions(fVoxelHalfDimX, fVoxelHalfDimY, fVoxelHalfDimZ);
  
  //----- Set number of voxels
#if G4VERSION_NUMBER < 1100
  param->SetNoVoxel(fNVoxelX, fNVoxelY, fNVoxelZ);
#else
  param->SetNoVoxels(fNVoxelX, fNVoxelY, fNVoxelZ);
#endif

  //----- Set list of materials
  param->SetMaterials(fMaterials);
  
  //----- Set list of material indices: for each voxel it is a number that
  // correspond to the index of its material in the vector of materials
  // defined above
  param->SetMaterialIndices(fMateIDs);
  
  //----- Define voxel logical volume
  G4Box *voxel_solid = new G4Box("Voxel", fVoxelHalfDimX, fVoxelHalfDimY, fVoxelHalfDimZ);
  G4LogicalVolume *voxel_logic = new G4LogicalVolume(voxel_solid, fMaterials[0], "VoxelLogical", 0, 0, 0);
  // material is not relevant for the moment, it will be changed later on by the
  // ComputeMaterial method of the parameterisation
  
  voxel_logic->SetVisAttributes(new G4VisAttributes(G4VisAttributes::GetInvisible()));
  
  //--- Assign the fContainer volume of the parameterisation
  param->BuildContainerSolid(fContainer_phys);
  
  //--- Assure yourself that the voxels are completely filling the
  // fContainer volume
  param->CheckVoxelsFillContainer(fContainer_solid->GetXHalfLength(),
				  fContainer_solid->GetYHalfLength(),
				  fContainer_solid->GetZHalfLength());
  
  //----- The G4PVParameterised object that uses the created parameterisation
  // should be placed in the fContainer logical volume
  G4PVParameterised *phantom_phys = new G4PVParameterised("phantom",
							  voxel_logic,
							  containerLogicalVolume,
							  kXAxis,
							  fNVoxelX * fNVoxelY * fNVoxelZ,
							  param);
  // if axis is set as kUndefined instead of kXAxis, GEANT4 will
  // do a smart voxel optimisation
  // (not needed if G4RegularNavigation is used)
  
  //----- Set this physical volume as having a regular structure of type 1,
  // so that G4RegularNavigation is used
  phantom_phys->SetRegularStructureId(1); // if not set, G4VoxelNavigation
  // will be used instead
  
  SetScorer(voxel_logic);
}

void BDSCT::InitialisationOfMaterials()
{
  // Creating elements :
  G4double z, a, density;
  G4String mname, symbol;
  
  auto *elC = new G4Element(mname = "Carbon",
			    symbol = "C",
			    z = 6.0, a = 12.011 * CLHEP::g / CLHEP::mole);
  auto *elH = new G4Element(mname = "Hydrogen",
			    symbol = "H",
			    z = 1.0, a = 1.008 * CLHEP::g / CLHEP::mole);
  auto *elN = new G4Element(mname = "Nitrogen",
			    symbol = "N",
			    z = 7.0, a = 14.007 * CLHEP::g / CLHEP::mole);
  auto *elO = new G4Element(mname = "Oxygen",
			    symbol = "O",
			    z = 8.0, a = 16.00 * CLHEP::g / CLHEP::mole);
  auto *elNa = new G4Element(mname = "Sodium",
			     symbol = "Na",
			     z = 11.0, a = 22.98977 * CLHEP::g / CLHEP::mole);
  auto *elMg = new G4Element(mname = "Magnesium",
			     symbol = "Mg",
			     z = 12.0, a = 24.3050 * CLHEP::g / CLHEP::mole);
  auto *elP = new G4Element(mname = "Phosphorus",
			    symbol = "P",
			    z = 15.0, a = 30.973976 * CLHEP::g / CLHEP::mole);
  auto *elS = new G4Element(mname = "Sulfur",
			    symbol = "S",
			    z = 16.0, a = 32.065 * CLHEP::g / CLHEP::mole);
  auto *elCl = new G4Element(mname = "Chlorine",
			     symbol = "P",
			     z = 17.0, a = 35.453 * CLHEP::g / CLHEP::mole);
  auto *elK = new G4Element(mname = "Potassium",
			    symbol = "P",
			    z = 19.0, a = 30.0983 * CLHEP::g / CLHEP::mole);
  
  auto *elFe = new G4Element(mname = "Iron",
			     symbol = "Fe",
			     z = 26, a = 56.845 * CLHEP::g / CLHEP::mole);
  
  auto *elCa = new G4Element(mname = "Calcium",
			     symbol = "Ca",
			     z = 20.0, a = 40.078 * CLHEP::g / CLHEP::mole);
  
  auto *elZn = new G4Element(mname = "Zinc",
			     symbol = "Zn",
			     z = 30.0, a = 65.382 * CLHEP::g / CLHEP::mole);
  
  // Creating Materials :
  G4int numberofElements;
  
  // Air
  fAir = new G4Material("Air",
			0.001290 * CLHEP::g / CLHEP::cm3,
			numberofElements = 2);
  fAir->AddElement(elN, 0.7);
  fAir->AddElement(elO, 0.3);
  
  // Soft tissue (ICRP - NIST)
  auto *softTissue = new G4Material("SoftTissue", 1.00 * CLHEP::g / CLHEP::cm3,
				    numberofElements = 13);
  softTissue->AddElement(elH, 10.4472 * CLHEP::perCent);
  softTissue->AddElement(elC, 23.219 * CLHEP::perCent);
  softTissue->AddElement(elN, 2.488 * CLHEP::perCent);
  softTissue->AddElement(elO, 63.0238 * CLHEP::perCent);
  softTissue->AddElement(elNa, 0.113 * CLHEP::perCent);
  softTissue->AddElement(elMg, 0.0113 * CLHEP::perCent);
  softTissue->AddElement(elP, 0.113 * CLHEP::perCent);
  softTissue->AddElement(elS, 0.199 * CLHEP::perCent);
  softTissue->AddElement(elCl, 0.134 * CLHEP::perCent);
  softTissue->AddElement(elK, 0.199 * CLHEP::perCent);
  softTissue->AddElement(elCa, 0.023 * CLHEP::perCent);
  softTissue->AddElement(elFe, 0.005 * CLHEP::perCent);
  softTissue->AddElement(elZn, 0.003 * CLHEP::perCent);
  
  //  Lung Inhale
  auto *lunginhale = new G4Material("LungInhale",
				    density = 0.217 * CLHEP::g / CLHEP::cm3,
				    numberofElements = 9);
  lunginhale->AddElement(elH, 0.103);
  lunginhale->AddElement(elC, 0.105);
  lunginhale->AddElement(elN, 0.031);
  lunginhale->AddElement(elO, 0.749);
  lunginhale->AddElement(elNa, 0.002);
  lunginhale->AddElement(elP, 0.002);
  lunginhale->AddElement(elS, 0.003);
  lunginhale->AddElement(elCl, 0.002);
  lunginhale->AddElement(elK, 0.003);
  
  // Lung exhale
  G4Material *lungexhale = new G4Material("LungExhale",
					  density = 0.508 * CLHEP::g / CLHEP::cm3,
					  numberofElements = 9);
  lungexhale->AddElement(elH, 0.103);
  lungexhale->AddElement(elC, 0.105);
  lungexhale->AddElement(elN, 0.031);
  lungexhale->AddElement(elO, 0.749);
  lungexhale->AddElement(elNa, 0.002);
  lungexhale->AddElement(elP, 0.002);
  lungexhale->AddElement(elS, 0.003);
  lungexhale->AddElement(elCl, 0.002);
  lungexhale->AddElement(elK, 0.003);
  
  // Adipose tissue
  auto *adiposeTissue = new G4Material("AdiposeTissue",
				       density = 0.967 * CLHEP::g / CLHEP::cm3,
				       numberofElements = 7);
  adiposeTissue->AddElement(elH, 0.114);
  adiposeTissue->AddElement(elC, 0.598);
  adiposeTissue->AddElement(elN, 0.007);
  adiposeTissue->AddElement(elO, 0.278);
  adiposeTissue->AddElement(elNa, 0.001);
  adiposeTissue->AddElement(elS, 0.001);
  adiposeTissue->AddElement(elCl, 0.001);
  
  // Brain (ICRP - NIST)
  auto *brainTissue = new G4Material("BrainTissue", 1.03 * CLHEP::g / CLHEP::cm3,
				     numberofElements = 13);
  brainTissue->AddElement(elH, 11.0667 * CLHEP::perCent);
  brainTissue->AddElement(elC, 12.542 * CLHEP::perCent);
  brainTissue->AddElement(elN, 1.328 * CLHEP::perCent);
  brainTissue->AddElement(elO, 73.7723 * CLHEP::perCent);
  brainTissue->AddElement(elNa, 0.1840 * CLHEP::perCent);
  brainTissue->AddElement(elMg, 0.015 * CLHEP::perCent);
  brainTissue->AddElement(elP, 0.356 * CLHEP::perCent);
  brainTissue->AddElement(elS, 0.177 * CLHEP::perCent);
  brainTissue->AddElement(elCl, 0.236 * CLHEP::perCent);
  brainTissue->AddElement(elK, 0.31 * CLHEP::perCent);
  brainTissue->AddElement(elCa, 0.009 * CLHEP::perCent);
  brainTissue->AddElement(elFe, 0.005 * CLHEP::perCent);
  brainTissue->AddElement(elZn, 0.001 * CLHEP::perCent);
  
  // Breast
  auto *breast = new G4Material("Breast",
				density = 0.990 * CLHEP::g / CLHEP::cm3,
				numberofElements = 8);
  breast->AddElement(elH, 0.109);
  breast->AddElement(elC, 0.506);
  breast->AddElement(elN, 0.023);
  breast->AddElement(elO, 0.358);
  breast->AddElement(elNa, 0.001);
  breast->AddElement(elP, 0.001);
  breast->AddElement(elS, 0.001);
  breast->AddElement(elCl, 0.001);
  
  // Spinal Disc
  auto *spinalDisc = new G4Material("SpinalDisc", 1.10 * CLHEP::g / CLHEP::cm3,
				    numberofElements = 8);
  spinalDisc->AddElement(elH, 9.60 * CLHEP::perCent);
  spinalDisc->AddElement(elC, 9.90 * CLHEP::perCent);
  spinalDisc->AddElement(elN, 2.20 * CLHEP::perCent);
  spinalDisc->AddElement(elO, 74.40 * CLHEP::perCent);
  spinalDisc->AddElement(elNa, 0.50 * CLHEP::perCent);
  spinalDisc->AddElement(elP, 2.20 * CLHEP::perCent);
  spinalDisc->AddElement(elS, 0.90 * CLHEP::perCent);
  spinalDisc->AddElement(elCl, 0.30 * CLHEP::perCent);
  
  // Water
  auto *water = new G4Material("Water",
			       density = 1.0 * CLHEP::g / CLHEP::cm3,
			       numberofElements = 2);
  water->AddElement(elH, 0.112);
  water->AddElement(elO, 0.888);
  
  // Muscle
  auto *muscle = new G4Material("Muscle",
				density = 1.061 * CLHEP::g / CLHEP::cm3,
				numberofElements = 9);
  muscle->AddElement(elH, 0.102);
  muscle->AddElement(elC, 0.143);
  muscle->AddElement(elN, 0.034);
  muscle->AddElement(elO, 0.710);
  muscle->AddElement(elNa, 0.001);
  muscle->AddElement(elP, 0.002);
  muscle->AddElement(elS, 0.003);
  muscle->AddElement(elCl, 0.001);
  muscle->AddElement(elK, 0.004);
  
  // Liver
  auto *liver = new G4Material("Liver",
			       density = 1.071 * CLHEP::g / CLHEP::cm3,
			       numberofElements = 9);
  liver->AddElement(elH, 0.102);
  liver->AddElement(elC, 0.139);
  liver->AddElement(elN, 0.030);
  liver->AddElement(elO, 0.716);
  liver->AddElement(elNa, 0.002);
  liver->AddElement(elP, 0.003);
  liver->AddElement(elS, 0.003);
  liver->AddElement(elCl, 0.002);
  liver->AddElement(elK, 0.003);
  
  // Tooth Dentin
  auto *toothDentin = new G4Material("ToothDentin", 2.14 * CLHEP::g / CLHEP::cm3,
				     numberofElements = 10);
  toothDentin->AddElement(elH, 2.67 * CLHEP::perCent);
  toothDentin->AddElement(elC, 12.77 * CLHEP::perCent);
  toothDentin->AddElement(elN, 4.27 * CLHEP::perCent);
  toothDentin->AddElement(elO, 40.40 * CLHEP::perCent);
  toothDentin->AddElement(elNa, 0.65 * CLHEP::perCent);
  toothDentin->AddElement(elMg, 0.59 * CLHEP::perCent);
  toothDentin->AddElement(elP, 11.86 * CLHEP::perCent);
  toothDentin->AddElement(elCl, 0.04 * CLHEP::perCent);
  toothDentin->AddElement(elCa, 26.74 * CLHEP::perCent);
  toothDentin->AddElement(elZn, 0.01 * CLHEP::perCent);
  
  // Trabecular Bone
  auto *trabecularBone = new G4Material("TrabecularBone",
					density = 1.159 * CLHEP::g / CLHEP::cm3,
					numberofElements = 12);
  trabecularBone->AddElement(elH, 0.085);
  trabecularBone->AddElement(elC, 0.404);
  trabecularBone->AddElement(elN, 0.058);
  trabecularBone->AddElement(elO, 0.367);
  trabecularBone->AddElement(elNa, 0.001);
  trabecularBone->AddElement(elMg, 0.001);
  trabecularBone->AddElement(elP, 0.034);
  trabecularBone->AddElement(elS, 0.002);
  trabecularBone->AddElement(elCl, 0.002);
  trabecularBone->AddElement(elK, 0.001);
  trabecularBone->AddElement(elCa, 0.044);
  trabecularBone->AddElement(elFe, 0.001);
  
  // Trabecular bone used in the DICOM Head
  
  auto *trabecularBone_head = new G4Material("TrabecularBone_HEAD",
					     1.18 * CLHEP::g / CLHEP::cm3,
					     numberofElements = 12);
  trabecularBone_head->AddElement(elH, 8.50 * CLHEP::perCent);
  trabecularBone_head->AddElement(elC, 40.40 * CLHEP::perCent);
  trabecularBone_head->AddElement(elN, 2.80 * CLHEP::perCent);
  trabecularBone_head->AddElement(elO, 36.70 * CLHEP::perCent);
  trabecularBone_head->AddElement(elNa, 0.10 * CLHEP::perCent);
  trabecularBone_head->AddElement(elMg, 0.10 * CLHEP::perCent);
  trabecularBone_head->AddElement(elP, 3.40 * CLHEP::perCent);
  trabecularBone_head->AddElement(elS, 0.20 * CLHEP::perCent);
  trabecularBone_head->AddElement(elCl, 0.20 * CLHEP::perCent);
  trabecularBone_head->AddElement(elK, 0.10 * CLHEP::perCent);
  trabecularBone_head->AddElement(elCa, 7.40 * CLHEP::perCent);
  trabecularBone_head->AddElement(elFe, 0.10 * CLHEP::perCent);
  
  // Dense Bone
  auto *denseBone = new G4Material("DenseBone",
				   density = 1.575 * CLHEP::g / CLHEP::cm3,
				   numberofElements = 11);
  denseBone->AddElement(elH, 0.056);
  denseBone->AddElement(elC, 0.235);
  denseBone->AddElement(elN, 0.050);
  denseBone->AddElement(elO, 0.434);
  denseBone->AddElement(elNa, 0.001);
  denseBone->AddElement(elMg, 0.001);
  denseBone->AddElement(elP, 0.072);
  denseBone->AddElement(elS, 0.003);
  denseBone->AddElement(elCl, 0.001);
  denseBone->AddElement(elK, 0.001);
  denseBone->AddElement(elCa, 0.146);
  
  // Cortical Bone 1 (ICRP - NIST)
  auto *corticalBone1 = new G4Material("CorticalBone1", 1.85 * CLHEP::g / CLHEP::cm3,
				       numberofElements = 9);
  corticalBone1->AddElement(elH, 4.7234 * CLHEP::perCent);
  corticalBone1->AddElement(elC, 14.4330 * CLHEP::perCent);
  corticalBone1->AddElement(elN, 4.199 * CLHEP::perCent);
  corticalBone1->AddElement(elO, 44.6096 * CLHEP::perCent);
  corticalBone1->AddElement(elMg, 0.22 * CLHEP::perCent);
  corticalBone1->AddElement(elP, 10.497 * CLHEP::perCent);
  corticalBone1->AddElement(elS, 0.315 * CLHEP::perCent);
  corticalBone1->AddElement(elCa, 20.993 * CLHEP::perCent);
  corticalBone1->AddElement(elZn, 0.01 * CLHEP::perCent);
  
  // Tooth enamel
  auto *toothEnamel = new G4Material("ToothEnamel", 2.89 * CLHEP::g / CLHEP::cm3,
				     numberofElements = 10);
  toothEnamel->AddElement(elH, 0.95 * CLHEP::perCent);
  toothEnamel->AddElement(elC, 1.11 * CLHEP::perCent);
  toothEnamel->AddElement(elN, 0.23 * CLHEP::perCent);
  toothEnamel->AddElement(elO, 41.66 * CLHEP::perCent);
  toothEnamel->AddElement(elNa, 0.79 * CLHEP::perCent);
  toothEnamel->AddElement(elMg, 0.23 * CLHEP::perCent);
  toothEnamel->AddElement(elP, 18.71 * CLHEP::perCent);
  toothEnamel->AddElement(elCl, 0.34 * CLHEP::perCent);
  toothEnamel->AddElement(elCa, 35.97 * CLHEP::perCent);
  toothEnamel->AddElement(elZn, 0.02 * CLHEP::perCent);
  
  // Lung blood-filled (Schneider et al.)
  auto *lungBloodFilled = new G4Material("LungBloodFilled", 0.26 * CLHEP::g / CLHEP::cm3, numberofElements = 6);
  lungBloodFilled->AddElement(elH, 10.3 * CLHEP::perCent);
  lungBloodFilled->AddElement(elC, 10.5 * CLHEP::perCent);
  lungBloodFilled->AddElement(elN, 3.1 * CLHEP::perCent);
  lungBloodFilled->AddElement(elO, 74.9 * CLHEP::perCent);
  lungBloodFilled->AddElement(elP, 0.2 * CLHEP::perCent);
  lungBloodFilled->AddElement(elK, 1.0 * CLHEP::perCent);
  
  // Adipose Tissue 1 (Schneider et al.)
  auto *adiposeTissue1 = new G4Material("AdiposeTissue1", 0.97 * CLHEP::g / CLHEP::cm3, numberofElements = 5);
  adiposeTissue1->AddElement(elH, 11.2 * CLHEP::perCent);
  adiposeTissue1->AddElement(elC, 51.7 * CLHEP::perCent);
  adiposeTissue1->AddElement(elN, 1.3 * CLHEP::perCent);
  adiposeTissue1->AddElement(elO, 35.5 * CLHEP::perCent);
  adiposeTissue1->AddElement(elK, 0.3 * CLHEP::perCent);
  
  // Adipose Tissue 2 (Schneider et al.)
  auto *adiposeTissue2 = new G4Material("AdiposeTissue2", 0.95 * CLHEP::g / CLHEP::cm3, numberofElements = 5);
  adiposeTissue2->AddElement(elH, 11.4 * CLHEP::perCent);
  adiposeTissue2->AddElement(elC, 59.8 * CLHEP::perCent);
  adiposeTissue2->AddElement(elN, 0.7 * CLHEP::perCent);
  adiposeTissue2->AddElement(elO, 27.8 * CLHEP::perCent);
  adiposeTissue2->AddElement(elK, 0.3 * CLHEP::perCent);
  
  // Adipose Tissue 3 (Schneider et al.)
  auto *adiposeTissue3 = new G4Material("AdiposeTissue3", 0.93 * CLHEP::g / CLHEP::cm3, numberofElements = 5);
  adiposeTissue3->AddElement(elH, 11.6 * CLHEP::perCent);
  adiposeTissue3->AddElement(elC, 68.1 * CLHEP::perCent);
  adiposeTissue3->AddElement(elN, 0.2 * CLHEP::perCent);
  adiposeTissue3->AddElement(elO, 19.8 * CLHEP::perCent);
  adiposeTissue3->AddElement(elK, 0.3 * CLHEP::perCent);
  
  // Mammary gland 1 (Schneider et al.)
  auto *mammaryGland1 = new G4Material("MammaryGland1", 0.99 * CLHEP::g / CLHEP::cm3, numberofElements = 6);
  mammaryGland1->AddElement(elH, 10.9 * CLHEP::perCent);
  mammaryGland1->AddElement(elC, 50.6 * CLHEP::perCent);
  mammaryGland1->AddElement(elN, 2.3 * CLHEP::perCent);
  mammaryGland1->AddElement(elO, 35.8 * CLHEP::perCent);
  mammaryGland1->AddElement(elP, 0.1 * CLHEP::perCent);
  mammaryGland1->AddElement(elK, 0.3 * CLHEP::perCent);
  
  // Mammary gland 2 (Schneider et al.)
  auto *mammaryGland2 = new G4Material("MammaryGland2", 1.02 * CLHEP::g / CLHEP::cm3, numberofElements = 6);
  mammaryGland2->AddElement(elH, 10.6 * CLHEP::perCent);
  mammaryGland2->AddElement(elC, 33.2 * CLHEP::perCent);
  mammaryGland2->AddElement(elN, 3.0 * CLHEP::perCent);
  mammaryGland2->AddElement(elO, 52.8 * CLHEP::perCent);
  mammaryGland2->AddElement(elP, 0.1 * CLHEP::perCent);
  mammaryGland2->AddElement(elK, 0.3 * CLHEP::perCent);
  
  // Adrenal gland (Schneider et al.)
  auto *adrenalGland = new G4Material("AdrenalGland", 1.03 * CLHEP::g / CLHEP::cm3, numberofElements = 6);
  adrenalGland->AddElement(elH, 10.6 * CLHEP::perCent);
  adrenalGland->AddElement(elC, 28.4 * CLHEP::perCent);
  adrenalGland->AddElement(elN, 2.6 * CLHEP::perCent);
  adrenalGland->AddElement(elO, 57.8 * CLHEP::perCent);
  adrenalGland->AddElement(elP, 0.1 * CLHEP::perCent);
  adrenalGland->AddElement(elK, 0.5 * CLHEP::perCent);
  
  // Small intestine wall (Schneider et al.)
  auto *smallInstestineWall = new G4Material("SmallIntestineWall", 1.03 * CLHEP::g / CLHEP::cm3, numberofElements = 6);
  smallInstestineWall->AddElement(elH, 10.6 * CLHEP::perCent);
  smallInstestineWall->AddElement(elC, 11.5 * CLHEP::perCent);
  smallInstestineWall->AddElement(elN, 2.2 * CLHEP::perCent);
  smallInstestineWall->AddElement(elO, 75.1 * CLHEP::perCent);
  smallInstestineWall->AddElement(elP, 0.1 * CLHEP::perCent);
  smallInstestineWall->AddElement(elK, 0.5 * CLHEP::perCent);
  
  // Cerebrospinal fluid (Schneider et al.)
  auto *cerebrospinalFluid = new G4Material("CerebrospinalFluid", 1.01 * CLHEP::g / CLHEP::cm3, numberofElements = 3);
  cerebrospinalFluid->AddElement(elH, 11.1 * CLHEP::perCent);
  cerebrospinalFluid->AddElement(elO, 88.0 * CLHEP::perCent);
  cerebrospinalFluid->AddElement(elK, 0.9 * CLHEP::perCent);
  
  // Urine (Schneider et al.)
  auto *urine = new G4Material("Urine", 1.02 * CLHEP::g / CLHEP::cm3, numberofElements = 7);
  urine->AddElement(elH, 11.0 * CLHEP::perCent);
  urine->AddElement(elC, 0.5 * CLHEP::perCent);
  urine->AddElement(elN, 1.0 * CLHEP::perCent);
  urine->AddElement(elO, 86.2 * CLHEP::perCent);
  urine->AddElement(elP, 0.1 * CLHEP::perCent);
  urine->AddElement(elK, 0.2 * CLHEP::perCent);
  urine->AddElement(elNa, 1.0 * CLHEP::perCent);
  
  // Gallbladder bile (Schneider et al.)
  auto *gallbladderBile = new G4Material("GallbladderBile", 1.03 * CLHEP::g / CLHEP::cm3, numberofElements = 5);
  gallbladderBile->AddElement(elH, 10.8 * CLHEP::perCent);
  gallbladderBile->AddElement(elC, 6.1 * CLHEP::perCent);
  gallbladderBile->AddElement(elN, 0.1 * CLHEP::perCent);
  gallbladderBile->AddElement(elO, 82.2 * CLHEP::perCent);
  gallbladderBile->AddElement(elK, 0.8 * CLHEP::perCent);
  
  // Lymph (Schneider et al.)
  auto *lymph = new G4Material("Lymph", 1.03 * CLHEP::g / CLHEP::cm3, numberofElements = 5);
  lymph->AddElement(elH, 10.8 * CLHEP::perCent);
  lymph->AddElement(elC, 4.1 * CLHEP::perCent);
  lymph->AddElement(elN, 1.1 * CLHEP::perCent);
  lymph->AddElement(elO, 83.2 * CLHEP::perCent);
  lymph->AddElement(elK, 0.8 * CLHEP::perCent);
  
  // Pancreas (Schneider et al.)
  auto *pancreas = new G4Material("Pancreas", 1.04 * CLHEP::g / CLHEP::cm3, numberofElements = 6);
  pancreas->AddElement(elH, 10.6 * CLHEP::perCent);
  pancreas->AddElement(elC, 16.9 * CLHEP::perCent);
  pancreas->AddElement(elN, 2.2 * CLHEP::perCent);
  pancreas->AddElement(elO, 69.4 * CLHEP::perCent);
  pancreas->AddElement(elP, 0.2 * CLHEP::perCent);
  pancreas->AddElement(elK, 0.7 * CLHEP::perCent);
  
  // Prostate (Schneider et al.)
  auto *prostate = new G4Material("Prostate", 1.04 * CLHEP::g / CLHEP::cm3, numberofElements = 6);
  prostate->AddElement(elH, 10.5 * CLHEP::perCent);
  prostate->AddElement(elC, 8.9 * CLHEP::perCent);
  prostate->AddElement(elN, 2.5 * CLHEP::perCent);
  prostate->AddElement(elO, 77.4 * CLHEP::perCent);
  prostate->AddElement(elP, 0.1 * CLHEP::perCent);
  prostate->AddElement(elK, 0.6 * CLHEP::perCent);
  
  // White matter (Schneider et al.)
  auto *whiteMatter = new G4Material("WhiteMatter", 1.04 * CLHEP::g / CLHEP::cm3, numberofElements = 6);
  whiteMatter->AddElement(elH, 10.6 * CLHEP::perCent);
  whiteMatter->AddElement(elC, 19.4 * CLHEP::perCent);
  whiteMatter->AddElement(elN, 2.5 * CLHEP::perCent);
  whiteMatter->AddElement(elO, 66.1 * CLHEP::perCent);
  whiteMatter->AddElement(elP, 0.4 * CLHEP::perCent);
  whiteMatter->AddElement(elK, 1.0 * CLHEP::perCent);
  
  // Testis (Schneider et al.)
  auto *testis = new G4Material("Testis", 1.04 * CLHEP::g / CLHEP::cm3, numberofElements = 6);
  testis->AddElement(elH, 10.6 * CLHEP::perCent);
  testis->AddElement(elC, 9.9 * CLHEP::perCent);
  testis->AddElement(elN, 2.0 * CLHEP::perCent);
  testis->AddElement(elO, 76.6 * CLHEP::perCent);
  testis->AddElement(elP, 0.1 * CLHEP::perCent);
  testis->AddElement(elK, 0.8 * CLHEP::perCent);
  
  // Grey Matter (Schneider et al.)
  auto *greyMatter = new G4Material("GreyMatter", 1.04 * CLHEP::g / CLHEP::cm3, numberofElements = 6);
  greyMatter->AddElement(elH, 10.7 * CLHEP::perCent);
  greyMatter->AddElement(elC, 9.5 * CLHEP::perCent);
  greyMatter->AddElement(elN, 1.8 * CLHEP::perCent);
  greyMatter->AddElement(elO, 76.7 * CLHEP::perCent);
  greyMatter->AddElement(elP, 0.3 * CLHEP::perCent);
  greyMatter->AddElement(elK, 1.0 * CLHEP::perCent);
  
  // Muscle Skeletal 1 (Schneider et al.)
  auto *muscleSkeletal1 = new G4Material("MuscleSkeletal1", 1.05 * CLHEP::g / CLHEP::cm3, numberofElements = 6);
  muscleSkeletal1->AddElement(elH, 10.1 * CLHEP::perCent);
  muscleSkeletal1->AddElement(elC, 17.1 * CLHEP::perCent);
  muscleSkeletal1->AddElement(elN, 3.6 * CLHEP::perCent);
  muscleSkeletal1->AddElement(elO, 68.1 * CLHEP::perCent);
  muscleSkeletal1->AddElement(elP, 0.2 * CLHEP::perCent);
  muscleSkeletal1->AddElement(elK, 0.9 * CLHEP::perCent);
  
  // Stomach (Schneider et al.)
  auto *stomach = new G4Material("Stomach", 1.05 * CLHEP::g / CLHEP::cm3, numberofElements = 6);
  stomach->AddElement(elH, 10.4 * CLHEP::perCent);
  stomach->AddElement(elC, 13.9 * CLHEP::perCent);
  stomach->AddElement(elN, 2.9 * CLHEP::perCent);
  stomach->AddElement(elO, 72.1 * CLHEP::perCent);
  stomach->AddElement(elP, 0.1 * CLHEP::perCent);
  stomach->AddElement(elK, 0.6 * CLHEP::perCent);
  
  // Heart 1 (Schneider et al.)
  auto *heart1 = new G4Material("Heart1", 1.05 * CLHEP::g / CLHEP::cm3, numberofElements = 6);
  heart1->AddElement(elH, 10.3 * CLHEP::perCent);
  heart1->AddElement(elC, 17.5 * CLHEP::perCent);
  heart1->AddElement(elN, 3.1 * CLHEP::perCent);
  heart1->AddElement(elO, 68.1 * CLHEP::perCent);
  heart1->AddElement(elP, 0.2 * CLHEP::perCent);
  heart1->AddElement(elK, 0.8 * CLHEP::perCent);
  
  // Kidney 1 (Schneider et al.)
  auto *kidney1 = new G4Material("Kidney1", 1.05 * CLHEP::g / CLHEP::cm3, numberofElements = 7);
  kidney1->AddElement(elH, 10.2 * CLHEP::perCent);
  kidney1->AddElement(elC, 16.0 * CLHEP::perCent);
  kidney1->AddElement(elN, 3.4 * CLHEP::perCent);
  kidney1->AddElement(elO, 69.3 * CLHEP::perCent);
  kidney1->AddElement(elP, 0.2 * CLHEP::perCent);
  kidney1->AddElement(elCa, 0.1 * CLHEP::perCent);
  kidney1->AddElement(elK, 0.8 * CLHEP::perCent);
  
  // Thyroid (Schneider et al.)
  auto *thyroid = new G4Material("Thyroid", 1.05 * CLHEP::g / CLHEP::cm3, numberofElements = 6);
  thyroid->AddElement(elH, 10.4 * CLHEP::perCent);
  thyroid->AddElement(elC, 11.9 * CLHEP::perCent);
  thyroid->AddElement(elN, 2.4 * CLHEP::perCent);
  thyroid->AddElement(elO, 74.5 * CLHEP::perCent);
  thyroid->AddElement(elP, 0.1 * CLHEP::perCent);
  thyroid->AddElement(elK, 0.7 * CLHEP::perCent);
  
  // Aorta (Schneider et al.)
  auto *aorta = new G4Material("Aorta", 1.05 * CLHEP::g / CLHEP::cm3, numberofElements = 7);
  aorta->AddElement(elH, 9.9 * CLHEP::perCent);
  aorta->AddElement(elC, 14.7 * CLHEP::perCent);
  aorta->AddElement(elN, 4.2 * CLHEP::perCent);
  aorta->AddElement(elO, 69.8 * CLHEP::perCent);
  aorta->AddElement(elP, 0.4 * CLHEP::perCent);
  aorta->AddElement(elCa, 0.4 * CLHEP::perCent);
  aorta->AddElement(elK, 0.6 * CLHEP::perCent);
  
  // Heart 2 (Schneider et al.)
  auto *heart2 = new G4Material("Heart2", 1.05 * CLHEP::g / CLHEP::cm3, numberofElements = 6);
  heart2->AddElement(elH, 10.4 * CLHEP::perCent);
  heart2->AddElement(elC, 13.9 * CLHEP::perCent);
  heart2->AddElement(elN, 2.9 * CLHEP::perCent);
  heart2->AddElement(elO, 71.8 * CLHEP::perCent);
  heart2->AddElement(elP, 0.2 * CLHEP::perCent);
  heart2->AddElement(elK, 0.8 * CLHEP::perCent);
    
  // Kidney 2 (Schneider et al.)
  auto *kidney2 = new G4Material("Kidney2", 1.05 * CLHEP::g / CLHEP::cm3, numberofElements = 7);
  kidney2->AddElement(elH, 10.3 * CLHEP::perCent);
  kidney2->AddElement(elC, 13.2 * CLHEP::perCent);
  kidney2->AddElement(elN, 3.0 * CLHEP::perCent);
  kidney2->AddElement(elO, 72.4 * CLHEP::perCent);
  kidney2->AddElement(elP, 0.2 * CLHEP::perCent);
  kidney2->AddElement(elCa, 0.1 * CLHEP::perCent);
  kidney2->AddElement(elK, 0.8 * CLHEP::perCent);

    // Liver 1 (Schneider et al.)
    auto *liver1 = new G4Material("Liver1", 1.05 * CLHEP::g / CLHEP::cm3, numberofElements = 6);
    liver1->AddElement(elH, 10.3 * CLHEP::perCent);
    liver1->AddElement(elC, 15.6 * CLHEP::perCent);
    liver1->AddElement(elN, 2.7 * CLHEP::perCent);
    liver1->AddElement(elO, 70.1 * CLHEP::perCent);
    liver1->AddElement(elP, 0.3 * CLHEP::perCent);
    liver1->AddElement(elK, 1.0 * CLHEP::perCent);

    // Muscle Skeletal 2 (Schneider et al.)
    auto *muscleSkeletal2 = new G4Material("MuscleSkeletal2", 1.05 * CLHEP::g / CLHEP::cm3, numberofElements = 6);
    muscleSkeletal2->AddElement(elH, 10.2 * CLHEP::perCent);
    muscleSkeletal2->AddElement(elC, 14.3 * CLHEP::perCent);
    muscleSkeletal2->AddElement(elN, 3.4 * CLHEP::perCent);
    muscleSkeletal2->AddElement(elO, 71.0 * CLHEP::perCent);
    muscleSkeletal2->AddElement(elP, 0.2 * CLHEP::perCent);
    muscleSkeletal2->AddElement(elK, 0.9 * CLHEP::perCent);

    // Muscle Skeletal 3 (Schneider et al.)
    auto *muscleSkeletal3 = new G4Material("MuscleSkeletal3", 1.05 * CLHEP::g / CLHEP::cm3, numberofElements = 6);
    muscleSkeletal3->AddElement(elH, 10.2 * CLHEP::perCent);
    muscleSkeletal3->AddElement(elC, 11.2 * CLHEP::perCent);
    muscleSkeletal3->AddElement(elN, 3.0 * CLHEP::perCent);
    muscleSkeletal3->AddElement(elO, 74.5 * CLHEP::perCent);
    muscleSkeletal3->AddElement(elP, 0.2 * CLHEP::perCent);
    muscleSkeletal3->AddElement(elK, 0.9 * CLHEP::perCent);

    // Heart 3 (Schneider et al.)
    auto *heart3 = new G4Material("Heart3", 1.05 * CLHEP::g / CLHEP::cm3, numberofElements = 6);
    heart3->AddElement(elH, 10.4 * CLHEP::perCent);
    heart3->AddElement(elC, 10.3 * CLHEP::perCent);
    heart3->AddElement(elN, 2.7 * CLHEP::perCent);
    heart3->AddElement(elO, 75.6 * CLHEP::perCent);
    heart3->AddElement(elP, 0.2 * CLHEP::perCent);
    heart3->AddElement(elK, 0.8 * CLHEP::perCent);

    // Mammary Gland 3 (Schneider et al.)
    auto *mammaryGland3 = new G4Material("MammaryGland3", 1.06 * CLHEP::g / CLHEP::cm3, numberofElements = 6);
    mammaryGland3->AddElement(elH, 10.2 * CLHEP::perCent);
    mammaryGland3->AddElement(elC, 15.8 * CLHEP::perCent);
    mammaryGland3->AddElement(elN, 3.7 * CLHEP::perCent);
    mammaryGland3->AddElement(elO, 69.8 * CLHEP::perCent);
    mammaryGland3->AddElement(elP, 0.1 * CLHEP::perCent);
    mammaryGland3->AddElement(elK, 0.4 * CLHEP::perCent);

    // Kidney 3 (Schneider et al.)
    auto *kidney3 = new G4Material("Kidney3", 1.05 * CLHEP::g / CLHEP::cm3, numberofElements = 7);
    kidney3->AddElement(elH, 10.4 * CLHEP::perCent);
    kidney3->AddElement(elC, 10.6 * CLHEP::perCent);
    kidney3->AddElement(elN, 2.7 * CLHEP::perCent);
    kidney3->AddElement(elO, 75.2 * CLHEP::perCent);
    kidney3->AddElement(elP, 0.2 * CLHEP::perCent);
    kidney3->AddElement(elCa, 0.1 * CLHEP::perCent);
    kidney3->AddElement(elK, 0.8 * CLHEP::perCent);

    // Ovary (Schneider et al.)
    auto *ovary = new G4Material("Ovary", 1.05 * CLHEP::g / CLHEP::cm3, numberofElements = 6);
    ovary->AddElement(elH, 10.5 * CLHEP::perCent);
    ovary->AddElement(elC, 9.3 * CLHEP::perCent);
    ovary->AddElement(elN, 2.4 * CLHEP::perCent);
    ovary->AddElement(elO, 76.8 * CLHEP::perCent);
    ovary->AddElement(elP, 0.2 * CLHEP::perCent);
    ovary->AddElement(elK, 0.8 * CLHEP::perCent);

    // Eye lens (Schneider et al.)
    auto *eyeLens = new G4Material("EyeLens", 1.07 * CLHEP::g / CLHEP::cm3, numberofElements = 6);
    eyeLens->AddElement(elH, 9.6 * CLHEP::perCent);
    eyeLens->AddElement(elC, 19.5 * CLHEP::perCent);
    eyeLens->AddElement(elN, 5.7 * CLHEP::perCent);
    eyeLens->AddElement(elO, 64.6 * CLHEP::perCent);
    eyeLens->AddElement(elP, 0.1 * CLHEP::perCent);
    eyeLens->AddElement(elK, 0.5 * CLHEP::perCent);

    // Liver 2 (Schneider et al.)
    auto *liver2 = new G4Material("Liver2", 1.06 * CLHEP::g / CLHEP::cm3, numberofElements = 6);
    liver2->AddElement(elH, 10.2 * CLHEP::perCent);
    liver2->AddElement(elC, 13.9 * CLHEP::perCent);
    liver2->AddElement(elN, 3.0 * CLHEP::perCent);
    liver2->AddElement(elO, 71.6 * CLHEP::perCent);
    liver2->AddElement(elP, 0.3 * CLHEP::perCent);
    liver2->AddElement(elK, 1.0 * CLHEP::perCent);

    // Trachea (Schneider et al.)
    auto *trachea = new G4Material("Trachea", 1.06 * CLHEP::g / CLHEP::cm3, numberofElements = 6);
    trachea->AddElement(elH, 10.2 * CLHEP::perCent);
    trachea->AddElement(elC, 13.9 * CLHEP::perCent);
    trachea->AddElement(elN, 3.0 * CLHEP::perCent);
    trachea->AddElement(elO, 71.6 * CLHEP::perCent);
    trachea->AddElement(elP, 0.3 * CLHEP::perCent);
    trachea->AddElement(elK, 1.0 * CLHEP::perCent);

    // Spleen (Schneider et al.)
    auto *spleen = new G4Material("Spleen", 1.06 * CLHEP::g / CLHEP::cm3, numberofElements = 6);
    spleen->AddElement(elH, 10.3 * CLHEP::perCent);
    spleen->AddElement(elC, 11.3 * CLHEP::perCent);
    spleen->AddElement(elN, 3.2 * CLHEP::perCent);
    spleen->AddElement(elO, 74.1 * CLHEP::perCent);
    spleen->AddElement(elP, 0.3 * CLHEP::perCent);
    spleen->AddElement(elK, 0.8 * CLHEP::perCent);

    // Heart Blood Filled (Schneider et al.)
    auto *heartBloodFilled = new G4Material("HeartBloodFilled", 1.06 * CLHEP::g / CLHEP::cm3, numberofElements = 6);
    heartBloodFilled->AddElement(elH, 10.3 * CLHEP::perCent);
    heartBloodFilled->AddElement(elC, 12.1 * CLHEP::perCent);
    heartBloodFilled->AddElement(elN, 3.2 * CLHEP::perCent);
    heartBloodFilled->AddElement(elO, 73.4 * CLHEP::perCent);
    heartBloodFilled->AddElement(elP, 0.1 * CLHEP::perCent);
    heartBloodFilled->AddElement(elK, 0.9 * CLHEP::perCent);

    // Blood whole (Schneider et al.)
    auto *bloodWhole = new G4Material("BloodWhole", 1.06 * CLHEP::g / CLHEP::cm3, numberofElements = 6);
    bloodWhole->AddElement(elH, 10.2 * CLHEP::perCent);
    bloodWhole->AddElement(elC, 11.0 * CLHEP::perCent);
    bloodWhole->AddElement(elN, 3.3 * CLHEP::perCent);
    bloodWhole->AddElement(elO, 74.5 * CLHEP::perCent);
    bloodWhole->AddElement(elP, 0.1 * CLHEP::perCent);
    bloodWhole->AddElement(elK, 0.9 * CLHEP::perCent);

    // Liver 3 (Schneider et al.)
    auto *liver3 = new G4Material("Liver3", 1.07 * CLHEP::g / CLHEP::cm3, numberofElements = 6);
    liver3->AddElement(elH, 10.1 * CLHEP::perCent);
    liver3->AddElement(elC, 12.6 * CLHEP::perCent);
    liver3->AddElement(elN, 3.3 * CLHEP::perCent);
    liver3->AddElement(elO, 72.7 * CLHEP::perCent);
    liver3->AddElement(elP, 0.3 * CLHEP::perCent);
    liver3->AddElement(elK, 1.0 * CLHEP::perCent);

    // Skin 1 (Schneider et al.)
    auto *skin1 = new G4Material("Skin1", 1.09 * CLHEP::g / CLHEP::cm3, numberofElements = 6);
    skin1->AddElement(elH, 10.0 * CLHEP::perCent);
    skin1->AddElement(elC, 25.0 * CLHEP::perCent);
    skin1->AddElement(elN, 4.6 * CLHEP::perCent);
    skin1->AddElement(elO, 59.4 * CLHEP::perCent);
    skin1->AddElement(elP, 0.1 * CLHEP::perCent);
    skin1->AddElement(elK, 0.9 * CLHEP::perCent);

    // Skin 2 (Schneider et al.)
    auto *skin2 = new G4Material("Skin2", 1.09 * CLHEP::g / CLHEP::cm3, numberofElements = 6);
    skin2->AddElement(elH, 10.0 * CLHEP::perCent);
    skin2->AddElement(elC, 20.4 * CLHEP::perCent);
    skin2->AddElement(elN, 4.2 * CLHEP::perCent);
    skin2->AddElement(elO, 64.5 * CLHEP::perCent);
    skin2->AddElement(elP, 0.1 * CLHEP::perCent);
    skin2->AddElement(elK, 0.8 * CLHEP::perCent);

    // Skin 3 (Schneider et al.)
    auto *skin3 = new G4Material("Skin3", 1.09 * CLHEP::g / CLHEP::cm3, numberofElements = 6);
    skin3->AddElement(elH, 10.1 * CLHEP::perCent);
    skin3->AddElement(elC, 15.8 * CLHEP::perCent);
    skin3->AddElement(elN, 3.7 * CLHEP::perCent);
    skin3->AddElement(elO, 69.5 * CLHEP::perCent);
    skin3->AddElement(elP, 0.1 * CLHEP::perCent);
    skin3->AddElement(elK, 0.8 * CLHEP::perCent);

    // Connective tissue (Schneider et al.)
    auto *connectiveTissue = new G4Material("ConnectiveTissue", 1.09 * CLHEP::g / CLHEP::cm3, numberofElements = 7);
    connectiveTissue->AddElement(elH, 9.4 * CLHEP::perCent);
    connectiveTissue->AddElement(elC, 20.7 * CLHEP::perCent);
    connectiveTissue->AddElement(elN, 6.2 * CLHEP::perCent);
    connectiveTissue->AddElement(elO, 62.2 * CLHEP::perCent);
    connectiveTissue->AddElement(elK, 0.8 * CLHEP::perCent);
    connectiveTissue->AddElement(elNa, 0.2 * CLHEP::perCent);
    connectiveTissue->AddElement(elCl, 0.5 * CLHEP::perCent);

    // Yellow marrow (Schneider et al.)
    auto *yellowMarrow = new G4Material("YellowMarrow", 0.98 * CLHEP::g / CLHEP::cm3, numberofElements = 5);
    yellowMarrow->AddElement(elH, 11.5 * CLHEP::perCent);
    yellowMarrow->AddElement(elC, 64.4 * CLHEP::perCent);
    yellowMarrow->AddElement(elN, 0.7 * CLHEP::perCent);
    yellowMarrow->AddElement(elO, 23.1 * CLHEP::perCent);
    yellowMarrow->AddElement(elK, 0.3 * CLHEP::perCent);

    // Yellow red marrow (Schneider et al.)
    auto *yellowRedMarrow = new G4Material("YellowRedMarrow", 1.00 * CLHEP::g / CLHEP::cm3, numberofElements = 6);
    yellowRedMarrow->AddElement(elH, 11.0 * CLHEP::perCent);
    yellowRedMarrow->AddElement(elC, 52.9 * CLHEP::perCent);
    yellowRedMarrow->AddElement(elN, 2.1 * CLHEP::perCent);
    yellowRedMarrow->AddElement(elO, 33.5 * CLHEP::perCent);
    yellowRedMarrow->AddElement(elP, 0.1 * CLHEP::perCent);
    yellowRedMarrow->AddElement(elK, 0.4 * CLHEP::perCent);

    // Red marrow (Schneider et al.)
    auto *redMarrow = new G4Material("RedMarrow", 1.03 * CLHEP::g / CLHEP::cm3, numberofElements = 6);
    redMarrow->AddElement(elH, 11.0 * CLHEP::perCent);
    redMarrow->AddElement(elC, 52.9 * CLHEP::perCent);
    redMarrow->AddElement(elN, 2.1 * CLHEP::perCent);
    redMarrow->AddElement(elO, 33.5 * CLHEP::perCent);
    redMarrow->AddElement(elP, 0.1 * CLHEP::perCent);
    redMarrow->AddElement(elK, 0.4 * CLHEP::perCent);

    // Cartilage (Schneider et al.)
    auto *cartilage = new G4Material("Cartilage", 1.1 * CLHEP::g / CLHEP::cm3, numberofElements = 9);
    cartilage->AddElement(elH, 9.6 * CLHEP::perCent);
    cartilage->AddElement(elC, 9.9 * CLHEP::perCent);
    cartilage->AddElement(elN, 2.2 * CLHEP::perCent);
    cartilage->AddElement(elO, 74.4 * CLHEP::perCent);
    cartilage->AddElement(elP, 2.2 * CLHEP::perCent);
    cartilage->AddElement(elK, 0.4 * CLHEP::perCent);
    cartilage->AddElement(elMg, 0.5 * CLHEP::perCent);
    cartilage->AddElement(elS, 0.5 * CLHEP::perCent);
    cartilage->AddElement(elCl, 0.3 * CLHEP::perCent);

    // Sternum (Schneider et al.)
    auto *sternum = new G4Material("Sternum", 1.25 * CLHEP::g / CLHEP::cm3, numberofElements = 7);
    sternum->AddElement(elH, 7.8 * CLHEP::perCent);
    sternum->AddElement(elC, 31.6 * CLHEP::perCent);
    sternum->AddElement(elN, 3.7 * CLHEP::perCent);
    sternum->AddElement(elO, 43.8 * CLHEP::perCent);
    sternum->AddElement(elP, 4.0 * CLHEP::perCent);
    sternum->AddElement(elCa, 8.5 * CLHEP::perCent);
    sternum->AddElement(elK, 0.6 * CLHEP::perCent);

    // Sacrum male (Schneider et al.)
    auto *sacrumMale = new G4Material("SacrumMale", 1.29 * CLHEP::g / CLHEP::cm3, numberofElements = 7);
    sacrumMale->AddElement(elH, 7.4 * CLHEP::perCent);
    sacrumMale->AddElement(elC, 30.2 * CLHEP::perCent);
    sacrumMale->AddElement(elN, 3.7 * CLHEP::perCent);
    sacrumMale->AddElement(elO, 43.8 * CLHEP::perCent);
    sacrumMale->AddElement(elP, 4.5 * CLHEP::perCent);
    sacrumMale->AddElement(elCa, 9.8 * CLHEP::perCent);
    sacrumMale->AddElement(elK, 0.6 * CLHEP::perCent);

    // D6 L3 included cartilage male (Schneider et al.)
    auto *d6L3InclCartilageMale = new G4Material("D6L3IncludedCartilageMale", 1.3 * CLHEP::g / CLHEP::cm3, numberofElements = 7);
    d6L3InclCartilageMale->AddElement(elH, 7.3 * CLHEP::perCent);
    d6L3InclCartilageMale->AddElement(elC, 26.5 * CLHEP::perCent);
    d6L3InclCartilageMale->AddElement(elN, 3.6 * CLHEP::perCent);
    d6L3InclCartilageMale->AddElement(elO, 47.3 * CLHEP::perCent);
    d6L3InclCartilageMale->AddElement(elP, 4.8 * CLHEP::perCent);
    d6L3InclCartilageMale->AddElement(elCa, 9.8 * CLHEP::perCent);
    d6L3InclCartilageMale->AddElement(elK, 0.7 * CLHEP::perCent);

    // Whole vertebral column male (Schneider et al.)
    auto *wholeVertebralColumnMale = new G4Material("WholeVertebralColumnMale", 1.33 * CLHEP::g / CLHEP::cm3, numberofElements = 7);
    wholeVertebralColumnMale->AddElement(elH, 7.1 * CLHEP::perCent);
    wholeVertebralColumnMale->AddElement(elC, 25.8 * CLHEP::perCent);
    wholeVertebralColumnMale->AddElement(elN, 3.6 * CLHEP::perCent);
    wholeVertebralColumnMale->AddElement(elO, 47.2 * CLHEP::perCent);
    wholeVertebralColumnMale->AddElement(elP, 5.1 * CLHEP::perCent);
    wholeVertebralColumnMale->AddElement(elCa, 10.5 * CLHEP::perCent);
    wholeVertebralColumnMale->AddElement(elK, 0.7 * CLHEP::perCent);

    // D6 L3 excluded cartilage male (Schneider et al.)
    auto *d6L3ExclCartilageMale = new G4Material("D6L3ExcludedCartilageMale", 1.33 * CLHEP::g / CLHEP::cm3, numberofElements = 7);
    d6L3ExclCartilageMale->AddElement(elH, 7.0 * CLHEP::perCent);
    d6L3ExclCartilageMale->AddElement(elC, 28.7 * CLHEP::perCent);
    d6L3ExclCartilageMale->AddElement(elN, 3.8 * CLHEP::perCent);
    d6L3ExclCartilageMale->AddElement(elO, 43.7 * CLHEP::perCent);
    d6L3ExclCartilageMale->AddElement(elP, 5.1 * CLHEP::perCent);
    d6L3ExclCartilageMale->AddElement(elCa, 11.1 * CLHEP::perCent);
    d6L3ExclCartilageMale->AddElement(elK, 0.6 * CLHEP::perCent);

    // Humerus spherical head (Schneider et al.)
    auto *humerusSphericalHead = new G4Material("HumerusSphericalHead", 1.33 * CLHEP::g / CLHEP::cm3, numberofElements = 7);
    humerusSphericalHead->AddElement(elH, 7.1 * CLHEP::perCent);
    humerusSphericalHead->AddElement(elC, 37.9 * CLHEP::perCent);
    humerusSphericalHead->AddElement(elN, 2.6 * CLHEP::perCent);
    humerusSphericalHead->AddElement(elO, 34.2 * CLHEP::perCent);
    humerusSphericalHead->AddElement(elP, 5.6 * CLHEP::perCent);
    humerusSphericalHead->AddElement(elCa, 12.2 * CLHEP::perCent);
    humerusSphericalHead->AddElement(elK, 0.4 * CLHEP::perCent);

    // Femur spherical head (Schneider et al.)
    auto *femurSphericalHead = new G4Material("FemurSphericalHead", 1.33 * CLHEP::g / CLHEP::cm3, numberofElements = 7);
    femurSphericalHead->AddElement(elH, 7.1 * CLHEP::perCent);
    femurSphericalHead->AddElement(elC, 37.9 * CLHEP::perCent);
    femurSphericalHead->AddElement(elN, 2.6 * CLHEP::perCent);
    femurSphericalHead->AddElement(elO, 34.2 * CLHEP::perCent);
    femurSphericalHead->AddElement(elP, 5.6 * CLHEP::perCent);
    femurSphericalHead->AddElement(elCa, 12.2 * CLHEP::perCent);
    femurSphericalHead->AddElement(elK, 0.4 * CLHEP::perCent);

    // Femur conical trochanter (Schneider et al.)
    auto *femurConicalTrochanter = new G4Material("FemurConicalTrochanter", 1.36 * CLHEP::g / CLHEP::cm3, numberofElements = 7);
    femurConicalTrochanter->AddElement(elH, 6.9 * CLHEP::perCent);
    femurConicalTrochanter->AddElement(elC, 36.6 * CLHEP::perCent);
    femurConicalTrochanter->AddElement(elN, 2.7 * CLHEP::perCent);
    femurConicalTrochanter->AddElement(elO, 34.7 * CLHEP::perCent);
    femurConicalTrochanter->AddElement(elP, 5.9 * CLHEP::perCent);
    femurConicalTrochanter->AddElement(elCa, 12.8 * CLHEP::perCent);
    femurConicalTrochanter->AddElement(elK, 0.4 * CLHEP::perCent);

    // C4 included cartilage male (Schneider et al.)
    auto *c4InclCartilageMale = new G4Material("C4IncludedCartilageMale", 1.38 * CLHEP::g / CLHEP::cm3, numberofElements = 7);
    c4InclCartilageMale->AddElement(elH, 6.6 * CLHEP::perCent);
    c4InclCartilageMale->AddElement(elC, 24.3 * CLHEP::perCent);
    c4InclCartilageMale->AddElement(elN, 3.7 * CLHEP::perCent);
    c4InclCartilageMale->AddElement(elO, 47.1 * CLHEP::perCent);
    c4InclCartilageMale->AddElement(elP, 5.7 * CLHEP::perCent);
    c4InclCartilageMale->AddElement(elCa, 11.9 * CLHEP::perCent);
    c4InclCartilageMale->AddElement(elK, 0.7 * CLHEP::perCent);

    // Sacrum female (Schneider et al.)
    auto *sacrumFemale = new G4Material("SacrumFemale", 1.39 * CLHEP::g / CLHEP::cm3, numberofElements = 7);
    sacrumFemale->AddElement(elH, 6.6 * CLHEP::perCent);
    sacrumFemale->AddElement(elC, 27.1 * CLHEP::perCent);
    sacrumFemale->AddElement(elN, 3.8 * CLHEP::perCent);
    sacrumFemale->AddElement(elO, 43.5 * CLHEP::perCent);
    sacrumFemale->AddElement(elP, 5.8 * CLHEP::perCent);
    sacrumFemale->AddElement(elCa, 12.5 * CLHEP::perCent);
    sacrumFemale->AddElement(elK, 0.7 * CLHEP::perCent);

    // Humerus whole specimen (Schneider et al.)
    auto *humerusWholeSpecimen = new G4Material("HumerusWholeSpecimen", 1.39 * CLHEP::g / CLHEP::cm3, numberofElements = 7);
    humerusWholeSpecimen->AddElement(elH, 6.7 * CLHEP::perCent);
    humerusWholeSpecimen->AddElement(elC, 35.2 * CLHEP::perCent);
    humerusWholeSpecimen->AddElement(elN, 2.8 * CLHEP::perCent);
    humerusWholeSpecimen->AddElement(elO, 35.2 * CLHEP::perCent);
    humerusWholeSpecimen->AddElement(elP, 6.2 * CLHEP::perCent);
    humerusWholeSpecimen->AddElement(elCa, 13.5 * CLHEP::perCent);
    humerusWholeSpecimen->AddElement(elK, 0.4 * CLHEP::perCent);

    // Ribs 2nd, 6th male (Schneider et al.)
    auto *ribs2nd6thMale = new G4Material("Ribs2nd6thMale", 1.41 * CLHEP::g / CLHEP::cm3, numberofElements = 7);
    ribs2nd6thMale->AddElement(elH, 6.4 * CLHEP::perCent);
    ribs2nd6thMale->AddElement(elC, 26.3 * CLHEP::perCent);
    ribs2nd6thMale->AddElement(elN, 3.9 * CLHEP::perCent);
    ribs2nd6thMale->AddElement(elO, 43.6 * CLHEP::perCent);
    ribs2nd6thMale->AddElement(elP, 6.0 * CLHEP::perCent);
    ribs2nd6thMale->AddElement(elCa, 13.1 * CLHEP::perCent);
    ribs2nd6thMale->AddElement(elK, 0.7 * CLHEP::perCent);

    // Innominate male (Schneider et al.)
    auto *innominateMale = new G4Material("InnominateMale", 1.41 * CLHEP::g / CLHEP::cm3, numberofElements = 7);
    innominateMale->AddElement(elH, 6.3 * CLHEP::perCent);
    innominateMale->AddElement(elC, 26.2 * CLHEP::perCent);
    innominateMale->AddElement(elN, 3.9 * CLHEP::perCent);
    innominateMale->AddElement(elO, 43.6 * CLHEP::perCent);
    innominateMale->AddElement(elP, 6.1 * CLHEP::perCent);
    innominateMale->AddElement(elCa, 13.2 * CLHEP::perCent);
    innominateMale->AddElement(elK, 0.7 * CLHEP::perCent);

    // C4 excluded cartilage male (Schneider et al.)
    auto *c4ExclCartilageMale = new G4Material("C4ExcludedCartilageMale", 1.42 * CLHEP::g / CLHEP::cm3, numberofElements = 7);
    c4ExclCartilageMale->AddElement(elH, 6.3 * CLHEP::perCent);
    c4ExclCartilageMale->AddElement(elC, 26.1 * CLHEP::perCent);
    c4ExclCartilageMale->AddElement(elN, 3.9 * CLHEP::perCent);
    c4ExclCartilageMale->AddElement(elO, 43.6 * CLHEP::perCent);
    c4ExclCartilageMale->AddElement(elP, 6.1 * CLHEP::perCent);
    c4ExclCartilageMale->AddElement(elCa, 13.3 * CLHEP::perCent);
    c4ExclCartilageMale->AddElement(elK, 0.7 * CLHEP::perCent);

    // Femur total bone (Schneider et al.)
    auto *femurTotalBone = new G4Material("FemurTotalBone", 1.42 * CLHEP::g / CLHEP::cm3, numberofElements = 7);
    femurTotalBone->AddElement(elH, 6.3 * CLHEP::perCent);
    femurTotalBone->AddElement(elC, 33.3 * CLHEP::perCent);
    femurTotalBone->AddElement(elN, 2.9 * CLHEP::perCent);
    femurTotalBone->AddElement(elO, 36.2 * CLHEP::perCent);
    femurTotalBone->AddElement(elP, 6.6 * CLHEP::perCent);
    femurTotalBone->AddElement(elCa, 14.3 * CLHEP::perCent);
    femurTotalBone->AddElement(elK, 0.4 * CLHEP::perCent);

    // Femur whole specimen (Schneider et al.)
    auto *femurWholeSpecimen = new G4Material("FemurWholeSpecimen", 1.43 * CLHEP::g / CLHEP::cm3, numberofElements = 7);
    femurWholeSpecimen->AddElement(elH, 6.3 * CLHEP::perCent);
    femurWholeSpecimen->AddElement(elC, 33.1 * CLHEP::perCent);
    femurWholeSpecimen->AddElement(elN, 2.9 * CLHEP::perCent);
    femurWholeSpecimen->AddElement(elO, 36.3 * CLHEP::perCent);
    femurWholeSpecimen->AddElement(elP, 6.6 * CLHEP::perCent);
    femurWholeSpecimen->AddElement(elCa, 14.4 * CLHEP::perCent);
    femurWholeSpecimen->AddElement(elK, 0.4 * CLHEP::perCent);

    // Innominate female (Schneider et al.)
    auto *innominateFemale = new G4Material("InnominateFemale", 1.46 * CLHEP::g / CLHEP::cm3, numberofElements = 7);
    innominateFemale->AddElement(elH, 6.0 * CLHEP::perCent);
    innominateFemale->AddElement(elC, 25.0 * CLHEP::perCent);
    innominateFemale->AddElement(elN, 3.9 * CLHEP::perCent);
    innominateFemale->AddElement(elO, 43.5 * CLHEP::perCent);
    innominateFemale->AddElement(elP, 6.6 * CLHEP::perCent);
    innominateFemale->AddElement(elCa, 14.3 * CLHEP::perCent);
    innominateFemale->AddElement(elK, 0.7 * CLHEP::perCent);

    // Clavicle scapula (Schneider et al.)
    auto *clavicleScapula = new G4Material("ClavicleScapula", 1.46 * CLHEP::g / CLHEP::cm3, numberofElements = 7);
    clavicleScapula->AddElement(elH, 6.0 * CLHEP::perCent);
    clavicleScapula->AddElement(elC, 31.3 * CLHEP::perCent);
    clavicleScapula->AddElement(elN, 3.1 * CLHEP::perCent);
    clavicleScapula->AddElement(elO, 37.0 * CLHEP::perCent);
    clavicleScapula->AddElement(elP, 7.0 * CLHEP::perCent);
    clavicleScapula->AddElement(elCa, 15.2 * CLHEP::perCent);
    clavicleScapula->AddElement(elK, 0.4 * CLHEP::perCent);

    // Humerus total bone (Schneider et al.)
    auto *humerusTotalBone = new G4Material("HumerusTotalBone", 1.46 * CLHEP::g / CLHEP::cm3, numberofElements = 7);
    humerusTotalBone->AddElement(elH, 6.0 * CLHEP::perCent);
    humerusTotalBone->AddElement(elC, 31.4 * CLHEP::perCent);
    humerusTotalBone->AddElement(elN, 3.1 * CLHEP::perCent);
    humerusTotalBone->AddElement(elO, 36.9 * CLHEP::perCent);
    humerusTotalBone->AddElement(elP, 7.0 * CLHEP::perCent);
    humerusTotalBone->AddElement(elCa, 15.2 * CLHEP::perCent);
    humerusTotalBone->AddElement(elK, 0.4 * CLHEP::perCent);

    // Humerus cylindrical shaft (Schneider et al.)
    auto *humerusCylindricalShaft = new G4Material("HumerusCylindricalShaft", 1.49 * CLHEP::g / CLHEP::cm3, numberofElements = 7);
    humerusCylindricalShaft->AddElement(elH, 5.8 * CLHEP::perCent);
    humerusCylindricalShaft->AddElement(elC, 30.1 * CLHEP::perCent);
    humerusCylindricalShaft->AddElement(elN, 3.2 * CLHEP::perCent);
    humerusCylindricalShaft->AddElement(elO, 37.4 * CLHEP::perCent);
    humerusCylindricalShaft->AddElement(elP, 7.2 * CLHEP::perCent);
    humerusCylindricalShaft->AddElement(elCa, 15.8 * CLHEP::perCent);
    humerusCylindricalShaft->AddElement(elK, 0.5 * CLHEP::perCent);

    // Ribs 10th male (Schneider et al.)
    auto *ribs10thMale = new G4Material("Ribs10thMale", 1.52 * CLHEP::g / CLHEP::cm3, numberofElements = 7);
    ribs10thMale->AddElement(elH, 5.6 * CLHEP::perCent);
    ribs10thMale->AddElement(elC, 23.5 * CLHEP::perCent);
    ribs10thMale->AddElement(elN, 4.0 * CLHEP::perCent);
    ribs10thMale->AddElement(elO, 43.4 * CLHEP::perCent);
    ribs10thMale->AddElement(elP, 7.2 * CLHEP::perCent);
    ribs10thMale->AddElement(elCa, 15.6 * CLHEP::perCent);
    ribs10thMale->AddElement(elK, 0.7 * CLHEP::perCent);

    // Cranium (Schneider et al.)
    auto *cranium = new G4Material("Cranium", 1.61 * CLHEP::g / CLHEP::cm3, numberofElements = 7);
    cranium->AddElement(elH, 5.0 * CLHEP::perCent);
    cranium->AddElement(elC, 21.2 * CLHEP::perCent);
    cranium->AddElement(elN, 4.0 * CLHEP::perCent);
    cranium->AddElement(elO, 43.5 * CLHEP::perCent);
    cranium->AddElement(elP, 8.1 * CLHEP::perCent);
    cranium->AddElement(elCa, 17.6 * CLHEP::perCent);
    cranium->AddElement(elK, 0.6 * CLHEP::perCent);

    // Mandible (Schneider et al.)
    auto *mandible = new G4Material("Mandible", 1.68 * CLHEP::g / CLHEP::cm3, numberofElements = 7);
    mandible->AddElement(elH, 4.6 * CLHEP::perCent);
    mandible->AddElement(elC, 19.9 * CLHEP::perCent);
    mandible->AddElement(elN, 4.1 * CLHEP::perCent);
    mandible->AddElement(elO, 43.5 * CLHEP::perCent);
    mandible->AddElement(elP, 8.6 * CLHEP::perCent);
    mandible->AddElement(elCa, 18.7 * CLHEP::perCent);
    mandible->AddElement(elK, 0.6 * CLHEP::perCent);

    // Femur cylindrical shaft (Schneider et al.)
    auto *femurCylindricalShaft = new G4Material("FemurCylindricalShaft", 1.75 * CLHEP::g / CLHEP::cm3, numberofElements = 7);
    femurCylindricalShaft->AddElement(elH, 4.2 * CLHEP::perCent);
    femurCylindricalShaft->AddElement(elC, 20.4 * CLHEP::perCent);
    femurCylindricalShaft->AddElement(elN, 3.8 * CLHEP::perCent);
    femurCylindricalShaft->AddElement(elO, 41.5 * CLHEP::perCent);
    femurCylindricalShaft->AddElement(elP, 9.3 * CLHEP::perCent);
    femurCylindricalShaft->AddElement(elCa, 20.2 * CLHEP::perCent);
    femurCylindricalShaft->AddElement(elK, 0.6 * CLHEP::perCent);

    // Cortical bone 2 (Schneider et al.)
    auto *corticalBone2 = new G4Material("CorticalBone2", 1.92 * CLHEP::g / CLHEP::cm3, numberofElements = 7);
    corticalBone2->AddElement(elH, 3.4 * CLHEP::perCent);
    corticalBone2->AddElement(elC, 15.5 * CLHEP::perCent);
    corticalBone2->AddElement(elN, 4.2 * CLHEP::perCent);
    corticalBone2->AddElement(elO, 43.5 * CLHEP::perCent);
    corticalBone2->AddElement(elP, 10.3 * CLHEP::perCent);
    corticalBone2->AddElement(elCa, 22.5 * CLHEP::perCent);
    corticalBone2->AddElement(elK, 0.6 * CLHEP::perCent);

    fOriginalMaterials.push_back(corticalBone2);            // rho = 1.92
    fOriginalMaterials.push_back(corticalBone1);            // rho = 1.85
    fOriginalMaterials.push_back(femurCylindricalShaft);    // rho = 1.75
    fOriginalMaterials.push_back(mandible);                 // rho = 1.68
    fOriginalMaterials.push_back(cranium);                  // rho = 1.61
    fOriginalMaterials.push_back(ribs10thMale);             // rho = 1.52
    fOriginalMaterials.push_back(humerusCylindricalShaft);  // rho = 1.49
    fOriginalMaterials.push_back(humerusTotalBone);         // rho = 1.46
    fOriginalMaterials.push_back(clavicleScapula);          // rho = 1.46
    fOriginalMaterials.push_back(innominateFemale);         // rho = 1.46
    fOriginalMaterials.push_back(femurWholeSpecimen);       // rho = 1.43
    fOriginalMaterials.push_back(femurTotalBone);           // rho = 1.42
    fOriginalMaterials.push_back(c4ExclCartilageMale);      // rho = 1.42
    fOriginalMaterials.push_back(innominateMale);           // rho = 1.41
    fOriginalMaterials.push_back(ribs2nd6thMale);           // rho = 1.41
    fOriginalMaterials.push_back(humerusWholeSpecimen);     // rho = 1.39
    fOriginalMaterials.push_back(sacrumFemale);             // rho = 1.39
    fOriginalMaterials.push_back(c4InclCartilageMale);      // rho = 1.38
    fOriginalMaterials.push_back(femurConicalTrochanter);   // rho = 1.36
    fOriginalMaterials.push_back(femurSphericalHead);       // rho = 1.33
    fOriginalMaterials.push_back(humerusSphericalHead);     // rho = 1.33
    fOriginalMaterials.push_back(d6L3ExclCartilageMale);    // rho = 1.33
    fOriginalMaterials.push_back(wholeVertebralColumnMale); // rho = 1.33
    fOriginalMaterials.push_back(d6L3InclCartilageMale);    // rho = 1.3
    fOriginalMaterials.push_back(sacrumMale);               // rho = 1.29
    fOriginalMaterials.push_back(sternum);                  // rho = 1.25
    fOriginalMaterials.push_back(cartilage);                // rho = 1.1
    fOriginalMaterials.push_back(fAir);                     // rho = 0.00129
    fOriginalMaterials.push_back(lunginhale);               // rho = 0.217
    fOriginalMaterials.push_back(lungexhale);               // rho = 0.508
    fOriginalMaterials.push_back(adiposeTissue);            // rho = 0.967
    fOriginalMaterials.push_back(breast);                   // rho = 0.990
    fOriginalMaterials.push_back(water);                    // rho = 1.018
    fOriginalMaterials.push_back(muscle);                   // rho = 1.061
    fOriginalMaterials.push_back(liver);                    // rho = 1.071
    fOriginalMaterials.push_back(trabecularBone);           // rho = 1.159 - HEAD PHANTOM
    fOriginalMaterials.push_back(denseBone);                // rho = 1.575
    fOriginalMaterials.push_back(lungBloodFilled);          // rho = 0.26
    fOriginalMaterials.push_back(adiposeTissue1);           // rho = 0.97
    fOriginalMaterials.push_back(adiposeTissue2);           // rho = 0.95
    fOriginalMaterials.push_back(adiposeTissue3);           // rho = 0.93
    fOriginalMaterials.push_back(mammaryGland1);            // rho = 0.99
    fOriginalMaterials.push_back(mammaryGland2);            // rho = 1.02
    fOriginalMaterials.push_back(adrenalGland);             // rho = 1.03
    fOriginalMaterials.push_back(smallInstestineWall);      // rho = 1.03
    fOriginalMaterials.push_back(cerebrospinalFluid);       // rho = 1.01
    fOriginalMaterials.push_back(urine);                    // rho = 1.02
    fOriginalMaterials.push_back(gallbladderBile);          // rho = 1.03
    fOriginalMaterials.push_back(lymph);                    // rho = 1.03
    fOriginalMaterials.push_back(pancreas);                 // rho = 1.04
    fOriginalMaterials.push_back(prostate);                 // rho = 1.04
    fOriginalMaterials.push_back(whiteMatter);              // rho = 1.04
    fOriginalMaterials.push_back(testis);                   // rho = 1.04
    fOriginalMaterials.push_back(greyMatter);               // rho = 1.04
    fOriginalMaterials.push_back(muscleSkeletal1);          // rho = 1.05
    fOriginalMaterials.push_back(stomach);                  // rho = 1.05
    fOriginalMaterials.push_back(heart1);                   // rho = 1.05
    fOriginalMaterials.push_back(kidney1);                  // rho = 1.05
    fOriginalMaterials.push_back(thyroid);                  // rho = 1.05
    fOriginalMaterials.push_back(aorta);                    // rho = 1.05
    fOriginalMaterials.push_back(heart2);                   // rho = 1.05
    fOriginalMaterials.push_back(kidney2);                  // rho = 1.05
    fOriginalMaterials.push_back(liver1);                   // rho = 1.05
    fOriginalMaterials.push_back(muscleSkeletal2);          // rho = 1.05
    fOriginalMaterials.push_back(muscleSkeletal3);          // rho = 1.05
    fOriginalMaterials.push_back(heart3);                   // rho = 1.05
    fOriginalMaterials.push_back(kidney3);                  // rho = 1.06
    fOriginalMaterials.push_back(ovary);                    // rho = 1.05
    fOriginalMaterials.push_back(eyeLens);                  // rho = 1.07
    fOriginalMaterials.push_back(liver2);                   // rho = 1.06
    fOriginalMaterials.push_back(trachea);                  // rho = 1.06
    fOriginalMaterials.push_back(spleen);                   // rho = 1.06
    fOriginalMaterials.push_back(heartBloodFilled);         // rho = 1.06
    fOriginalMaterials.push_back(bloodWhole);               // rho = 1.06
    fOriginalMaterials.push_back(liver3);                   // rho = 1.07
    fOriginalMaterials.push_back(skin1);                    // rho = 1.09
    fOriginalMaterials.push_back(skin2);                    // rho = 1.09
    fOriginalMaterials.push_back(skin3);                    // rho = 1.09
    fOriginalMaterials.push_back(connectiveTissue);         // rho = 1.12
    fOriginalMaterials.push_back(yellowMarrow);             // rho = 0.98
    fOriginalMaterials.push_back(yellowRedMarrow);          // rho = 1.00
    fOriginalMaterials.push_back(redMarrow);                // rho = 1.03
    G4cout << "Default materials of the DICOM Extended examples have been used"
           << G4endl;
}

void BDSCT::ReadPhantomData()
{
  //---- We read data stored in the out file (.g4dcm) and treat it
  G4String fileName = BDSDicomFileMgr::GetInstance()->GetFileOutName();
  
  // Preparing the g4dcm file reading
  std::ifstream fin(fileName);
  std::vector<G4String> wl;
  G4int nMaterials;
  
  // The first line of the g4dcm file is the number of materials prompted by the user
  fin >> nMaterials;
  G4String mateName;
  G4int nmate;
  
  for (G4int ii = 0; ii < nMaterials; ++ii)
    {
      // Retrieving user-chosen Materials from data.dat
      fin >> nmate;
      fin >> mateName;
      if (mateName[0] == '"' && mateName[mateName.length() - 1] == '"')
        {
	  mateName = mateName.substr(1, mateName.length() - 2);
        }
      G4cout << "GmReadPhantomG4Geometry::ReadPhantomData reading nmate "
	     << ii << " = " << nmate
	     << " mate " << mateName << G4endl;
      if (ii != nmate)
	G4Exception("GmReadPhantomG4Geometry::ReadPhantomData",
		    "Wrong argument", FatalErrorInArgument,
		    "Material number should be in increasing order:wrong material number");
      
      // Finding corresponding material in Geant4 table from name
      G4Material *mate = 0;
      const G4MaterialTable *matTab = G4Material::GetMaterialTable();
      for (auto matite = matTab->cbegin(); matite != matTab->cend(); ++matite)
        {
	  if ((*matite)->GetName() == mateName)
	    mate = *matite;
        }
      
      // If not found in Geant4 table, custom build this material from name
      if (mate == 0)
	mate = G4NistManager::Instance()->FindOrBuildMaterial(mateName);
      
      if (!mate)
	G4Exception("GmReadPhantomG4Geometry::ReadPhantomData",
		    "Wrong argument",
		    FatalErrorInArgument,
		    ("Material not found" + mateName).c_str());
      thePhantomMaterialsOriginal[nmate] = mate;
    }
  // All materials required by the user are now ready for use
  // Now it should be associated with voxels and densities
  
  fin >> fNVoxelX >> fNVoxelY >> fNVoxelZ;
  G4cout << "GmReadPhantomG4Geometry::ReadPhantomData fNVoxel X/Y/Z "
	 << fNVoxelX << " "
	 << fNVoxelY << " " << fNVoxelZ << G4endl;
  fin >> fMinX >> fMaxX;
  fin >> fMinY >> fMaxY;
  fin >> fMinZ >> fMaxZ;
  fVoxelHalfDimX = (fMaxX - fMinX) / fNVoxelX / 2.;
  fVoxelHalfDimY = (fMaxY - fMinY) / fNVoxelY / 2.;
  fVoxelHalfDimZ = (fMaxZ - fMinZ) / fNVoxelZ / 2.;
  
  // Reading material IDs from g4dcm file
  // The IDs are associated to materials
  // Creating the iterator
  fMateIDs = new size_t[fNVoxelX * fNVoxelY * fNVoxelZ];
  // Loop scanning all voxels and retrieving the corresponding material
  for (G4int iz = 0; iz < fNVoxelZ; ++iz)
    {
      for (G4int iy = 0; iy < fNVoxelY; ++iy)
        {
	  for (G4int ix = 0; ix < fNVoxelX; ++ix)
            {
	      G4int mateID;
	      // Getting material ID from out file
	      fin >> mateID;
	      G4int copyNo = ix + (iy)*fNVoxelX + (iz)*fNVoxelX * fNVoxelY;
	      if (mateID < 0 || mateID >= nMaterials)
                {
		  G4Exception("GmReadPhantomG4Geometry::ReadPhantomData",
			      "Wrong index in phantom file",
			      FatalException,
			      G4String("It should be between 0 and " + G4UIcommand::ConvertToString(nMaterials - 1) + ", while it is " + G4UIcommand::ConvertToString(mateID)).c_str());
                }
	      // Storing the IDs in a vector where indices are copy numbers and contents are the materials IDs
	      fMateIDs[copyNo] = mateID;
            }
        }
    }
  // We now have a vector containing all material IDs. The index of this vector corresponds to the copy number of the voxel
  ReadVoxelDensities(fin);
  
  fin.close();
}

void BDSCT::ReadVoxelDensities(std::ifstream& fin)
{
  // We now retrieve densities from the out file
  // G4String stemp;
  std::map<G4int, std::pair<G4double, G4double>> densiMinMax;
  std::map<G4int, std::pair<G4double, G4double>>::iterator mpite;
  for (G4int ii = 0; ii < G4int(thePhantomMaterialsOriginal.size()); ++ii)
    {
      // DBL_MAX is defined in float.h and is the value of the maximum representable finite floating-point number in C++
      // Initialisation of the minimum and maximum densities
      // Min and Max are inverted in this initialisation in order for the modification to take place systematically
      densiMinMax[ii] = std::pair<G4double, G4double>(DBL_MAX, -DBL_MAX);
    }
  
  char *part = std::getenv("DICOM_CHANGE_MATERIAL_DENSITY");
  G4double densityDiff = -1.;
  if (part)
    densityDiff = G4UIcommand::ConvertToDouble(part);
  
  std::map<G4int, G4double> densityDiffs;
  for (G4int ii = 0; ii < G4int(thePhantomMaterialsOriginal.size()); ++ii)
    {
      // Currently all materials with same step = -1.
      densityDiffs[ii] = densityDiff;
    }
  //  densityDiffs[0] = 0.0001; //air
  
  //--- Calculate the average material density for each material/density bin
  // First we create a map that will contain all new materials and their information
  std::map<std::pair<G4Material *, G4int>, matInfo *> newMateDens;
  
  //---- Read the material densities
  G4double dens;
  for (G4int iz = 0; iz < fNVoxelZ; ++iz)
    {
      for (G4int iy = 0; iy < fNVoxelY; ++iy)
        {
	  for (G4int ix = 0; ix < fNVoxelX; ++ix)
            {
	      // Getting the density from file
	      fin >> dens;
	      
	      // Computing the copy number from the position in the phantom
	      G4int copyNo = ix + (iy)*fNVoxelX + (iz)*fNVoxelX * fNVoxelY;
	      
	      // If the statement is True, ignore the rest of this iteration (for all loops)
	      // In other words, if the user defines another density difference (variable part), this triple loop is ignored
	      if (densityDiff != -1.)
		{continue;}
	      
	      //--- We retrieve the material ID for this voxel
	      G4int mateID = G4int(fMateIDs[copyNo]);
	      
	      // First, find the entry of the map of density intervals that correspond to the material ID
	      mpite = densiMinMax.find(mateID);
	      // If the density is outside the density range of the voxel we set the limits of the range as equal to this density
	      // In other words, for a particular material, we extend the range of densities if we find a density outside of it
	      // Reminder: because of the way the initialisation was performed, this interval is automatically updated
	      if (dens < (*mpite).second.first)
		(*mpite).second.first = dens;
	      if (dens > (*mpite).second.second)
		(*mpite).second.second = dens;
	      
	      //--- Get material from original list of material in file
	      std::map<G4int, G4Material *>::const_iterator imite = thePhantomMaterialsOriginal.find(mateID);
	      
	      //--- Check if density is equal to the original material density (up to 1e-9 g/cm3), if it is, ignore this voxel
	      //--- as it is already at the right density
	      if (std::fabs(dens - (*imite).second->GetDensity() / (CLHEP::g / CLHEP::cm3)) < 1.e-9)
		continue;
	      
	      //--- If the density is NOT equal to the original material density (up to 1e-9 g/cm3),
	      //--- Build material name with thePhantomMaterialsOriginal name+density
	      G4int densityBin = (G4int(dens / densityDiffs[mateID]));
	      
	      // The new material name (unnecessary ???)
	      //G4String mateName = (*imite).second->GetName() + G4UIcommand::ConvertToString(densityBin);
	      
	      //--- Look if it is the first voxel with this material/densityBin
	      // Create a pair that stores the material and the density bin associated
	      // This will serve as a new entry
	      std::pair<G4Material *, G4int> matdens((*imite).second, densityBin);
	      
	      //--- Filling a map containing a new material with the corresponding ID, the density bin and all info about this material
	      // First, find if the material with this density bin already exists in the new material density
	      // If it is not, it will put it at the end of this map
	      auto mppite = newMateDens.find(matdens);
	      if (mppite != newMateDens.cend())
                {
		  // The material was found in the new material density, we retrieve it
		  matInfo *mi = (*mppite).second;
		  // We add the density
		  mi->sumOfDensities += dens;
		  // Increase the number of voxels that are concerned by this material
		  mi->nVoxels++;
		  // Update the material ID of the voxel
		  fMateIDs[copyNo] = thePhantomMaterialsOriginal.size() - 1 + mi->materialID;
                }
	      else
                {
		  // We are at the end of the map, therefore, the material is not yet in the new material density, thus we create the data structure that will include it
		  // Initialisation of the data structure
		  matInfo *mi = new matInfo;
		  // As it is the first voxel to have this new material, we must initialise its density
		  mi->sumOfDensities = dens;
		  // This is the first voxel that has this material
		  mi->nVoxels = 1;
		  mi->materialID = G4int(newMateDens.size() + 1);
		  // The new material now appears in the new material densities
		  newMateDens[matdens] = mi;
		  // Update the material ID of the voxel
		  fMateIDs[copyNo] = thePhantomMaterialsOriginal.size() - 1 + mi->materialID;
                }
            }
        }
    }
  
  //----- Build the list of phantom materials that go to Parameterisation
  //--- Add original materials
  for (auto mimite = thePhantomMaterialsOriginal.cbegin(); mimite != thePhantomMaterialsOriginal.cend(); ++mimite)
    {fMaterials.push_back((*mimite).second);}

  std::map<G4Material*, G4String> thePhantomMaterialsNew;
  auto fMaterials_copy = fMaterials;
  
  //---- Build and add new materials
    // The data structure that is used contains the average density of the new material
  for (auto mppite = newMateDens.cbegin(); mppite != newMateDens.cend(); ++mppite)
    {
      // We retrieve the average density for the new material considering all voxels that  contain this material
      G4double averdens = (*mppite).second->sumOfDensities / (*mppite).second->nVoxels;
      
      // Saving the average density in a format fit for naming
      G4double saverdens = G4int(1000.001 * averdens) / 1000.;
      // Creating a new name for the new material
      G4String mateName = ((*mppite).first).first->GetName() + "_" + G4UIcommand::ConvertToString(saverdens);
      // Creating a new material using this average density
      auto *newMate = BuildMaterialWithChangingDensity((*mppite).first.first, G4float(averdens), mateName);
      thePhantomMaterialsNew.insert(std::pair<G4Material *, G4String>(newMate, ((*mppite).first).first->GetName()));
      //fMaterials.push_back( newMate );
    }
  
  for (auto ite = fMaterials_copy.cbegin(); ite != fMaterials_copy.cend(); ++ite)
    {
      for (auto ite1 = thePhantomMaterialsNew.cbegin(); ite1 != thePhantomMaterialsNew.cend(); ++ite1)
        {
	  if ((*ite)->GetName() == (*ite1).second)
	    fMaterials.push_back((*ite1).first);
        }
    }
  
  for (auto ite = fMaterials.cbegin(); ite != fMaterials.cend(); ++ite)
    {
      std::cout << "The phantom will be built with material " << (*ite)->GetName() << " with density " << (*ite)->GetDensity() / (CLHEP::g / CLHEP::cm3) << " g/cm3" << std::endl;
    }
}

G4Material* BDSCT::BuildMaterialWithChangingDensity(const G4Material* origMate,
						    G4float density,
						    G4String newMateName)
{
  //----- Copy original material, but with new density
  G4int nelem = G4int(origMate->GetNumberOfElements());
  G4Material *mate = new G4Material(newMateName,
				    density * CLHEP::g / CLHEP::cm3,
				    nelem,
				    kStateUndefined,
				    CLHEP::STP_Temperature);
  
  for (G4int ii = 0; ii < nelem; ++ii)
    {
      G4double frac = origMate->GetFractionVector()[ii];
      G4Element *elem = const_cast<G4Element *>(origMate->GetElement(ii));
      mate->AddElement(elem, frac);
    }
  
  return mate;
}

void BDSCT::BuildContainerLogicalVolume()
{
  //---- Extract number of voxels and voxel dimensions
  
  //----- Define the volume that contains all the voxels
  fContainer_solid = new G4Box("phantomContainer", fNVoxelX * fVoxelHalfDimX,
			       fNVoxelY * fVoxelHalfDimY,
			       fNVoxelZ * fVoxelHalfDimZ);

  //the material is not important, it will be fully filled by the voxels
  containerLogicalVolume = new G4LogicalVolume(fContainer_solid,
					       fMaterials[0],
					       "phantomContainer",
					       0, 0, 0);
  
  G4ThreeVector posCentreVoxels((fMinX + fMaxX) / 2.,
				(fMinY + fMaxY) / 2.,
				(fMinZ + fMaxZ) / 2.);
  
  fContainer_phys = new G4PVPlacement(0, // rotation
				      posCentreVoxels,
				      containerLogicalVolume, // The logic volume
				      "phantomContainer",     // Name
				      fWorld_logic,           // Mother
				      false,                  // No op. bool.
				      1);                     // Copy number
}

void BDSCT::BuildUserLimits()
{
  userLimits = new G4UserLimits(1, // max 1mm step into dump
				0, // max track length
				0, // max time
				0, // max kinetic energy
				std::numeric_limits<double>::max());

  RegisterUserLimits(userLimits);
}

void BDSCT::SetScorer(G4LogicalVolume *voxel_logic)
{
  fScorers.insert(voxel_logic);
}
