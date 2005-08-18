/* BDSIM code.    Version beta
   Author: Grahame I. Agapov, Royal Holloway, Univ. of London.

*/
#include "BDSGlobalConstants.hh" // must be first in include list
#include "BDSCollimator.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"               
#include "G4UserLimits.hh"
#include "G4TransportationManager.hh"

#include "G4SDManager.hh"
#include "G4UserLimits.hh"

#include <map>

//============================================================
//typedef std::map<G4String,MagFieldFunction*> PhysFieldMap;
//extern PhysFieldMap* MagFieldMap;

typedef std::map<G4String,int> LogVolCountMap;
extern LogVolCountMap* LogVolCount;

typedef std::map<G4String,G4LogicalVolume*> LogVolMap;
extern LogVolMap* LogVol;
//extern G4double BDS_Threshold_Energy;
extern BDSMaterials* theMaterials;
//============================================================

BDSCollimator::BDSCollimator (G4String aName,G4double aLength,G4double bpRad,
			      G4double xAper,G4double yAper, G4int type,
			  G4Material *CollimatorMaterial):
  BDSAcceleratorComponent(aName,
			  aLength,bpRad,xAper,yAper,
			  SetVisAttributes()),
  itsCollimatorMaterial(CollimatorMaterial), itsType(type)
{
  
  if ( (*LogVolCount)[itsName]==0)
    {
      itsMarkerLogicalVolume=
	new G4LogicalVolume(
			    new G4Box(itsName,
				      BDSGlobals->GetComponentBoxSize()/2,
				      BDSGlobals->GetComponentBoxSize()/2,
				      itsLength/2),
			    theMaterials->LCVacuum,
			    itsName);
      BuildInnerCollimator();

      (*LogVolCount)[itsName]=1;
      (*LogVol)[itsName]=itsMarkerLogicalVolume;
    }
  else
    {
      (*LogVolCount)[itsName]++;
      itsMarkerLogicalVolume=(*LogVol)[itsName];
    }  
}


G4VisAttributes* BDSCollimator::SetVisAttributes()
{
  itsVisAttributes=new G4VisAttributes(G4Colour(0.3,0.4,0.2));
  return itsVisAttributes;
}


void BDSCollimator::BuildInnerCollimator()
{

  // zero aperture --> no aperture

  if(itsXAper <= 0) itsXAper = BDSGlobals->GetComponentBoxSize()/2;
  if(itsYAper <= 0) itsYAper = BDSGlobals->GetComponentBoxSize()/2;
  
  itsSolidLogVol=
    new G4LogicalVolume(new G4Box(itsName+"_solid",
				  BDSGlobals->GetComponentBoxSize()/2,
				  BDSGlobals->GetComponentBoxSize()/2,
				  itsLength/2),
			//itsCollimatorMaterial,
			itsCollimatorMaterial,
			itsName+"_solid");

  if(itsType == _RCOL)
    {
      itsInnerLogVol=
	new G4LogicalVolume(new G4Box(itsName+"_inner",
				      itsXAper,
				      itsYAper,
				      itsLength/2),
			    theMaterials->LCVacuum,
			    itsName+"_inner");
    }
  
  if(itsType == _ECOL)
    {
      itsInnerLogVol=
	new G4LogicalVolume(new G4EllipticalTube(itsName+"_inner",
				      itsXAper,
				      itsYAper,
				      itsLength/2),
			    theMaterials->LCVacuum,
			    itsName+"_inner");
    }
  
  itsPhysiComp2 = 
    new G4PVPlacement(
		      0,		   // no rotation
		      0,                   // its position
		      itsInnerLogVol,      // its logical volume
		      itsName+"_combined", // its name
		      itsSolidLogVol,      // its mother  volume
		      false,		   // no boolean operation
		      0);		   // copy number 

  SetSensitiveVolume(itsSolidLogVol);

  itsSolidLogVol->
    SetUserLimits(new G4UserLimits(DBL_MAX,DBL_MAX,DBL_MAX,
				       BDSGlobals-> GetThresholdCutCharged()));
  
  itsPhysiComp = 
    new G4PVPlacement(
		      0,		     // no rotation
		      0,                     // its position
		      itsSolidLogVol,    // its logical volume
		      itsName+"_solid",	     // its name
		      itsMarkerLogicalVolume, // its mother  volume
		      false,		     // no boolean operation
		      0);		     // copy number  
}


BDSCollimator::~BDSCollimator()
{
  if(itsVisAttributes) delete itsVisAttributes;
  if(itsUserLimits) delete itsUserLimits;

  if(itsMarkerLogicalVolume)delete itsMarkerLogicalVolume;
  if(itsSolidLogVol)delete itsSolidLogVol;
  if(itsInnerLogVol)delete itsInnerLogVol;

  if(itsPhysiComp) delete itsPhysiComp;
  if(itsPhysiComp2) delete itsPhysiComp2;
}
