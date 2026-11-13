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
#include "UDipole.hh"

#include "BDSAcceleratorComponent.hh"
#include "BDSBeamPipe.hh"
#include "BDSBeamPipeFactory.hh"
#include "BDSBeamPipeInfo.hh"
#include "BDSBeamPipeType.hh"
#include "BDSColours.hh"
#include "BDSExtent.hh"
#include "BDSFieldBuilder.hh"
#include "BDSFieldInfo.hh"
#include "BDSFieldType.hh"
#include "BDSIntegratorType.hh"
#include "BDSMagnetGeometryType.hh"
#include "BDSMagnetOuter.hh"
#include "BDSMagnetOuterFactory.hh"
#include "BDSMagnetOuterInfo.hh"
#include "BDSMagnetStrength.hh"
#include "BDSMagnetType.hh"
#include "BDSMaterials.hh"
#include "BDSSamplerPlane.hh"
#include "BDSSamplerRegistry.hh"
#include "BDSSDSampler.hh" // so we can convert BDSSamplerSD* to G4VSensitiveDetector*
#include "BDSSDManager.hh"
#include "BDSSimpleComponent.hh"
#include "BDSUtilities.hh"

#include "globals.hh" // geant4 globals / types
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4VisAttributes.hh"

UDipole::UDipole(G4String name,
		 G4double bFieldIn,
		 G4String params):
  BDSAcceleratorComponent(name, 1.57*CLHEP::m, /*angle*/0, "udipole"),
  bField(bFieldIn),
  horizontalWidth(1*CLHEP::m),
  pipe1Info(nullptr),
  volumeForField(nullptr)
{
  // these can be used anywhere in the class now
  vacuum = BDSMaterials::Instance()->GetMaterial("vacuum");
  air    = BDSMaterials::Instance()->GetMaterial("air");
  steel  = BDSMaterials::Instance()->GetMaterial("stainlesssteel");
  iron   = BDSMaterials::Instance()->GetMaterial("G4_Fe");
  pipe1Length = 25*CLHEP::cm;
  yokeLength  = 0.98*CLHEP::m;
  chamberLength = chordLength - 2*pipe1Length - 2*lengthSafety;

  // use function from BDSUtilities to process user params string into
  // map of strings and values.
  std::map<G4String, G4String> map = BDS::GetUserParametersMap(params);

  // the map is supplied at run time so we must check it actually has the key
  // to avoid throwing an exception.
  auto colourSearch = map.find("colour");
  if (colourSearch != map.end())
    {colour = colourSearch->second;}
  else
    {colour = "rectangularbend";}
}

UDipole::~UDipole()
{
  delete pipe1Info;
}

void UDipole::BuildContainerLogicalVolume()
{
  // containerSolid is a member of BDSAcceleratorComponent we should fill in
  containerSolid = new G4Box(name + "_container_solid",
			     horizontalWidth * 0.5,
			     horizontalWidth * 0.5,
			     chordLength * 0.5);

  // containerLogicalVolume is a member of BDSAcceleratorComponent we should fill in
  containerLogicalVolume = new G4LogicalVolume(containerSolid,
					       vacuum,
					       name + "_container_lv");
}

void UDipole::Build()
{
  // Call the base class implementation of this function. This builds the container
  // logical volume then sets some visualisation attributes for it.
  BDSAcceleratorComponent::Build();

  BuildBeamPipe();
  BuildMagnetYoke();
  BuildChamber();
  BuildField();
  SetExtents();
}

void UDipole::BuildBeamPipe()
{
  // create the recipe for a small section of beam pipe then use the BDSIM
  // beam pipe factory to make it for us.
  pipe1Info = new BDSBeamPipeInfo(BDSBeamPipeType::circular,
				  3*CLHEP::cm,   //radius
				  0, 0, 0,       // other aperture params not needed
				  vacuum,        // vacuum material
				  0.5*CLHEP::cm, // thickness
				  steel);        // material
  
  // get the beam pipe factory and use it to build a piece of beam pipe
  BDSBeamPipeFactory* factory = BDSBeamPipeFactory::Instance();
  BDSBeamPipe* pipe1 = factory->CreateBeamPipe(name + "_bp_1",
					       pipe1Length,
					       pipe1Info);
  RegisterDaughter(pipe1); // for deletion by bdsim

  // place the beam pipe in the container
  G4double zPos = -(0.5*chordLength - 0.5*pipe1Length - lengthSafety);
  G4ThreeVector pipe1Pos = G4ThreeVector(0, 0, zPos);
  auto pipe1PV = new G4PVPlacement(nullptr, // no rotation matrix,
				   pipe1Pos,
				   pipe1->GetContainerLogicalVolume(),
				   name + "_bp_1_pv",
				   containerLogicalVolume,
				   false,
				   0,
				   checkOverlaps);
  RegisterPhysicalVolume(pipe1PV); // for deletion by bdsim

  // place it again at the other end
  // use the same piece of geometry twice
  G4ThreeVector pipe2Pos = G4ThreeVector(0,0,-zPos);
  auto pipe2PV = new G4PVPlacement(nullptr, // no rotation matrix,
				   pipe2Pos,
				   pipe1->GetContainerLogicalVolume(),
				   name + "_bp_2_pv",
				   containerLogicalVolume,
				   false,
				   0,
				   checkOverlaps);
  RegisterPhysicalVolume(pipe2PV); // for deletion by bdsim
}

void UDipole::BuildMagnetYoke()
{
  // create recipe for a vertical kicker dipole (ie doesn't bend)
  BDSMagnetOuterInfo* outerInfo = new BDSMagnetOuterInfo(name + "_yoke",
							 BDSMagnetGeometryType::polescircular,
							 0.5*CLHEP::m,
							 iron,
							 5*CLHEP::cm,
							 1.0);
  outerInfo->yokeOnLeft = true; // put C yoke on top
  outerInfo->colour = BDSColours::Instance()->GetColour(colour); // update colour

  // create a fake piece of beam pipe to build the yoke w.r.t. because currently
  // the magnet factory always builds a tight fitting yoke to a piece of beam pipe
  // this will be fixed in future.
  BDSBeamPipeInfo* fakePipeInfo = new BDSBeamPipeInfo(*pipe1Info);
  fakePipeInfo->aper1 = 5*CLHEP::cm;
  BDSBeamPipeFactory* factory = BDSBeamPipeFactory::Instance();
  BDSBeamPipe* fakePipe = factory->CreateBeamPipe(name + "_bp_fake",
						  pipe1Length,
						  fakePipeInfo);
  
  BDSMagnetOuter* outer = BDSMagnetOuterFactory::Instance()->CreateMagnetOuter(BDSMagnetType::vkicker,
									       outerInfo,
									       yokeLength,
									       chordLength,
									       fakePipe);
  delete fakePipe; // get rid of it now we're done with it
  
  // place the yoke in the midle
  auto yokePV = new G4PVPlacement(nullptr,         // no rotation matrix,
				  G4ThreeVector(), // no translation - in the middle
				  outer->GetContainerLogicalVolume(),
				  name + "_yoke_pv",
				  containerLogicalVolume,
				  false,
				  0,
				  checkOverlaps);
  RegisterPhysicalVolume(yokePV); // for deletion by bdsim

  // place end pieces for coils
  BDSSimpleComponent* endPieceBefore = outer->EndPieceBefore();
  BDSSimpleComponent* endPieceAfter  = outer->EndPieceAfter();

  G4double coilZOffset = 0.5*yokeLength + 0.5*endPieceBefore->GetChordLength() + lengthSafetyLarge;
  auto endPieceBeforePV = new G4PVPlacement(nullptr,
					    G4ThreeVector(0,0,-coilZOffset),
					    endPieceBefore->GetContainerLogicalVolume(),
					    name + "_endpiece_in_pv",
					    containerLogicalVolume,
					    false,
					    0,
					    checkOverlaps);
  RegisterPhysicalVolume(endPieceBeforePV);
  auto endPieceAfterPV = new G4PVPlacement(nullptr,
					   G4ThreeVector(0,0,coilZOffset),
					   endPieceAfter->GetContainerLogicalVolume(),
					   name + "_endpiece_out_pv",
					   containerLogicalVolume,
					   false,
					   0,
					   checkOverlaps);
  RegisterPhysicalVolume(endPieceAfterPV);
}

void UDipole::BuildChamber()
{  
  BDSBeamPipeInfo* chamberInfo = new BDSBeamPipeInfo(BDSBeamPipeType::rectangular,
						     4*CLHEP::cm,   // horizonal half width
						     20*CLHEP::cm   // vertical half width
						     , 0, 0,        // other aperture params not needed
						     vacuum,        // vacuum material
						     0.5*CLHEP::cm, // thickness
						     steel);        // material
  
  // get the beam pipe factory and use it to build a piece of beam pipe
  BDSBeamPipeFactory* factory = BDSBeamPipeFactory::Instance();
  BDSBeamPipe* chamber = factory->CreateBeamPipe(name + "_chamber",
						 chamberLength,
						 chamberInfo);
  RegisterDaughter(chamber); // for deletion by bdsim

  // update the volume we keep a pointer to to attach the field (specific to this class)
  volumeForField = chamber->GetContainerLogicalVolume();

  // place the beam pipe in the container
  G4ThreeVector chamberPos = G4ThreeVector(0, -15*CLHEP::cm, 0);
  auto chamberPV = new G4PVPlacement(nullptr, // no rotation matrix,
				     chamberPos,
				     chamber->GetContainerLogicalVolume(),
				     name + "_chamber_pv",
				     containerLogicalVolume,
				     false,
				     0,
				     checkOverlaps);
  RegisterPhysicalVolume(chamberPV); // for deletion by bdsim

  // no we build a box to act as a sampler inside the chamber
  // we use the suggested minimal length for a plan sampler taken from the
  // public static member chordLength of BDSSamplerPlane.
  G4Box* screenSolid = new G4Box(name + "_screen_solid",
				 3*CLHEP::cm,
				 20*CLHEP::cm,
				 BDSSamplerPlane::chordLength*0.5);
  RegisterSolid(screenSolid); // for deletion by bdsim

  // make a logical volume for the screen
  G4LogicalVolume* screenLV = new G4LogicalVolume(screenSolid,
						  vacuum,
						  name + "_screen_lv");
  // attach a sensitive detector to record output information from this logical volume
  screenLV->SetSensitiveDetector(BDSSDManager::Instance()->SamplerPlane());

  // visualisation attributes - make it nicely visible
  G4VisAttributes* screenVis = new G4VisAttributes(*BDSColours::Instance()->GetColour("crystal"));
  screenVis->SetVisibility(true);
  screenLV->SetVisAttributes(screenVis);
  RegisterVisAttributes(screenVis);// for deletion by bdsim
  RegisterLogicalVolume(screenLV); // for deletion by bdsim
  
  G4RotationMatrix* screenRM = new G4RotationMatrix();
  screenRM->rotateX(-CLHEP::pi/3.0);
  RegisterRotationMatrix(screenRM); // for deletion by bdsim
  G4double screenZPos = 15*CLHEP::cm;
  G4ThreeVector screenPos = G4ThreeVector(0,-5*CLHEP::cm, screenZPos);

  // We register a sampler name in the sampler registry which create the branch in the
  // output with that name. The registry returns an integer that we must use as the placement
  // copy number for the information to be put in the correct place in the output.
  G4String samplerName = name + "_screen";
  G4int samplerID = BDSSamplerRegistry::Instance()->RegisterSampler(samplerName,
								    nullptr);

  // see BDSBeamPipe class for GetVacuumLogicalVolume - we want to place it 'in' the vacuum.
  auto screenPV = new G4PVPlacement(screenRM,
				    screenPos,
				    screenLV,
				    samplerName,
				    chamber->GetVacuumLogicalVolume(),
				    false,
				    samplerID,
				    checkOverlaps);
  RegisterPhysicalVolume(screenPV); // for deletion by bdsim		      
}

void UDipole::BuildField()
{
  // We build a strength object. We specify the field magnitude and unit direction components.
  BDSMagnetStrength* st = new BDSMagnetStrength();
  (*st)["field"] = bField;
  (*st)["bx"] = 1;
  (*st)["by"] = 0;
  (*st)["bz"] = 0;

  // we build a recipe for the field - pure dipole using a G4ClassicalRK4 integrator
  BDSFieldInfo* vacuumField = new BDSFieldInfo(BDSFieldType::dipole,
					       0, // brho - not needed for a pure dipole field
					       BDSIntegratorType::g4classicalrk4,
					       st,
					       true);

  // We 'register' the field to be constructed on a specific logical volume. All fields
  // are constructed and attached at once as per the Geant4 way of doing things. true
  // means propagate to all daughter volumes.
  BDSFieldBuilder::Instance()->RegisterFieldForConstruction(vacuumField,
							    volumeForField,
							    true);

}

void UDipole::SetExtents()
{
  // record extents so bdsim can check when placing stuff beside this magnet if it overlaps
  BDSExtent ext = BDSExtent(horizontalWidth * 0.5,
			    horizontalWidth * 0.5,
			    chordLength * 0.5);
  SetExtent(ext); // BDSGeometryComponent base class method
}
