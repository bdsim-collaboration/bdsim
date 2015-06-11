/* BDSIM code.    Version 1.0
   Author: John C. Carter, Royal Holloway, Univ. of London.
   Last modified 02.12.2004
   Copyright (c) 2004 by J.C.Carter.  ALL RIGHTS RESERVED. 
*/

#include "BDSGlobalConstants.hh"
#include "BDSExecOptions.hh"
#include "BDSElement.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Torus.hh"
#include "G4Trd.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4UserLimits.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "BDSAcceleratorComponent.hh"
#include "BDSXYMagField.hh"
#include "BDSMagFieldSQL.hh"
#include "G4NystromRK4.hh"
#include "BDS3DMagField.hh"
#include "BDSMagFieldFactory.hh"
#include "BDSGeometryFactory.hh"
#include "BDSDebug.hh"

// geometry drivers
#include "ggmad.hh"
#include "BDSGeometrySQL.hh"

#ifdef USE_LCDD
#include "BDSGeometryLCDD.hh"
#endif

#ifdef USE_GDML
#include "BDSGeometryGDML.hh"
#endif

#include <vector>

//============================================================

BDSElement::BDSElement(G4String aName, G4String geometry, G4String bmap, G4double bmapZOffset,
		       G4double aLength, G4double bpRad, G4double outR, G4String aTunnelMaterial, G4double aTunnelRadius, G4double aTunnelOffsetX, G4String aTunnelCavityMaterial):
  BDSAcceleratorComponent(
			  aName,
			  aLength,bpRad,0,0,
			  aTunnelMaterial, "", 0., 0., 0., 0., aTunnelRadius*CLHEP::m, aTunnelOffsetX*CLHEP::m, aTunnelCavityMaterial, bmap, bmapZOffset)
  
{
  itsGeometry=geometry;
  itsFieldVolName="";
  itsOuterR = outR;

  // WARNING: ALign in and out will only apply to the first instance of the
  //          element. Subsequent copies will have no alignment set.
  align_in_volume = NULL;
  align_out_volume = NULL;
}

void BDSElement::Build(){
  BDSAcceleratorComponent::Build();

  _visAttDebug = new G4VisAttributes(G4Colour(0.0, 1.0, 0.0));
  _visAttDebug->SetForceSolid(true);  
  _visAttDebug->SetVisibility(true);
  itsMarkerLogicalVolume->SetVisAttributes(_visAttDebug);
  
  PlaceComponents(); // place components (from file) and build field maps
  BuildTunnel();
}

// place components 
void BDSElement::PlaceComponents()
{
  
  BDSGeometryFactory* gFact = new BDSGeometryFactory();
  
  _geom = gFact->buildGeometry(itsGeometry);
  _geom->Construct(itsMarkerLogicalVolume);
  AddSensitiveVolume(itsMarkerLogicalVolume);
    
  for(G4int id=0; id<(G4int)_geom->GetSensitiveComponents().size(); id++){
    AddSensitiveVolume(_geom->GetSensitiveComponents()[id]);
  }
  
  for(unsigned int i=0; i<_geom->GetMultiplePhysicalVolumes().size(); i++){
    SetMultiplePhysicalVolumes(_geom->GetMultiplePhysicalVolumes().at(i));
  }

  for(G4int id=0; id<(G4int)_geom->GetGFlashComponents().size(); id++){
    SetGFlashVolumes(_geom->GetGFlashComponents()[id]);
  }
  
  align_in_volume = _geom->align_in_volume();
  align_out_volume = _geom->align_out_volume();
  
#ifdef USE_LCDD
  /*
  G4cout << __METHOD_NAME__ << " - setting lcdd vis attributes." << G4endl;
  if(geom->format()->spec()==(G4String)"lcdd") {
    //Make marker visible (temp debug)
    G4VisAttributes* VisAttLCDD = new G4VisAttributes(G4Colour(0.0, 1.0, 0.0));
    VisAttLCDD->SetForceSolid(true);  
    VisAttLCDD->SetVisibility(false);
    itsMarkerLogicalVolume->SetVisAttributes(VisAttLCDD);
  }
  */
#endif 


  BuildFieldAndStepper();
}

void BDSElement::SetVisAttributes()
{
  itsVisAttributes=new G4VisAttributes(G4Colour(0.5,0.5,1));
}

/*
void BDSElement::BuildFieldAndStepper(){
  BuildBmapFieldAndStepper();
}
*/


// Rotates and positions the marker volume before it is placed in
// BDSDetectorConstruction. It aligns the marker volume so that the
// the beamline goes through the specified daugther volume (e.g. for mokka)
void BDSElement::AlignComponent(G4ThreeVector& TargetPos, 
				G4RotationMatrix *TargetRot, 
				G4RotationMatrix& globalRotation,
				G4ThreeVector& rtot,
				G4ThreeVector& rlast,
				G4ThreeVector& localX,
				G4ThreeVector& localY,
				G4ThreeVector& localZ)
{
  
  
  if(align_in_volume == NULL)
    {
      if(align_out_volume == NULL)
	{
	  // advance co-ords in usual way if no alignment volumes found
	  
	  rtot = rlast + localZ*(itsLength/2);
	  rlast = rtot + localZ*(itsLength/2);
	  return;
	}
      else 
	{
#ifdef BDSDEBUG
	  G4cout << "BDSElement : Aligning outgoing to SQL element " 
		 << align_out_volume->GetName() << G4endl;
#endif
	  G4RotationMatrix Trot = *TargetRot;
	  G4RotationMatrix trackedRot;
	  G4RotationMatrix outRot = *(align_out_volume->GetFrameRotation());
	  trackedRot.transform(outRot.inverse());
	  trackedRot.transform(Trot.inverse());
	  globalRotation = trackedRot;

	  G4ThreeVector outPos = align_out_volume->GetFrameTranslation();
	  G4ThreeVector diff = outPos;

	  G4ThreeVector zHalfAngle = G4ThreeVector(0.,0.,1.);

	  zHalfAngle.transform(globalRotation);

	  //moving positioning to outgoing alignment volume
	  rlast = TargetPos - ((outPos.unit()).transform(Trot.inverse()) )*diff.mag();
	  localX.transform(outRot.inverse());
	  localY.transform(outRot.inverse());
	  localZ.transform(outRot.inverse());

	  localX.transform(Trot.inverse());
	  localY.transform(Trot.inverse());
	  localZ.transform(Trot.inverse());

	  //moving position in Z be at least itsLength/2 away
	  rlast +=zHalfAngle*(itsLength/2 + diff.z());
	  return;
	}
    }

  if(align_in_volume != NULL)
    {
#ifdef BDSDEBUG
      G4cout << "BDSElement : Aligning incoming to SQL element " 
      	     << align_in_volume->GetName() << G4endl;
#endif
      
      const G4RotationMatrix* inRot = align_in_volume->GetFrameRotation();
      TargetRot->transform((*inRot).inverse());
      
      G4ThreeVector inPos = align_in_volume->GetFrameTranslation();
      inPos.transform((*TargetRot).inverse());
      TargetPos+=G4ThreeVector(inPos.x(), inPos.y(), 0.0);
      
      if(align_out_volume == NULL)
	{
	  // align outgoing (i.e. next component) to Marker Volume
	  
	  G4RotationMatrix Trot = *TargetRot;
	  globalRotation.transform(Trot.inverse());
	  
	  G4ThreeVector zHalfAngle = G4ThreeVector(0.,0.,1.);
	  zHalfAngle.transform(Trot.inverse());
	  
	  rlast = TargetPos + zHalfAngle*(itsLength/2);
	  localX.transform(Trot.inverse());
	  localY.transform(Trot.inverse());
	  localZ.transform(Trot.inverse());
	  return;
	}

      else
	{
#ifdef BDSDEBUG
	  G4cout << "BDSElement : Aligning outgoing to SQL element " 
		 << align_out_volume->GetName() << G4endl;
#endif
	  G4RotationMatrix Trot = *TargetRot;
	  G4RotationMatrix trackedRot;
	  G4RotationMatrix outRot = *(align_out_volume->GetFrameRotation());
	  trackedRot.transform(outRot.inverse());
	  trackedRot.transform(Trot.inverse());
	  globalRotation = trackedRot;

	  G4ThreeVector outPos = align_out_volume->GetFrameTranslation();
	  G4ThreeVector diff = outPos;

	  G4ThreeVector zHalfAngle = G4ThreeVector(0.,0.,1.);

	  zHalfAngle.transform(globalRotation);

	  //moving positioning to outgoing alignment volume
	  rlast = TargetPos - ((outPos.unit()).transform(Trot.inverse()) )*diff.mag();
	  localX.transform(outRot.inverse());
	  localY.transform(outRot.inverse());
	  localZ.transform(outRot.inverse());

	  localX.transform(Trot.inverse());
	  localY.transform(Trot.inverse());
	  localZ.transform(Trot.inverse());

	  //moving position in Z be at least itsLength/2 away
	  rlast +=zHalfAngle*(itsLength/2 + diff.z());
	  return;
	}
    }
  
}

BDSElement::~BDSElement()
{
}
