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
#include <map>

//============================================================

typedef std::map<G4String,int> LogVolCountMap;
extern LogVolCountMap* LogVolCount;

typedef std::map<G4String,G4LogicalVolume*> LogVolMap;
extern LogVolMap* LogVol;

//============================================================

BDSSkewSextupole::BDSSkewSextupole(G4String& aName,G4double aLength, 
				   G4double bpRad,G4double FeRad,
                                   std::list<G4double> blmLocZ, std::list<G4double> blmLocTheta,
                                   G4String aTunnelMaterial, G4String aMaterial,
				   G4double BDblPrime):
  BDSMultipole(aName,aLength, bpRad, FeRad,SetVisAttributes(),blmLocZ, blmLocTheta, aTunnelMaterial,aMaterial),
  itsBDblPrime(BDblPrime),
  itsStepper(NULL),itsMagField(NULL),itsEqRhs(NULL)
{
  if (!(*LogVolCount)[itsName])
    {
      BuildBPFieldAndStepper();
      BuildBPFieldMgr(itsStepper,itsMagField);
      BuildDefaultMarkerLogicalVolume();

      BuildBeampipe();

      BuildDefaultOuterLogicalVolume(itsLength);

      if(BDSGlobalConstants::Instance()->SensitiveBeamPipe()){
        SetMultipleSensitiveVolumes(itsBeampipeLogicalVolume);
      }
      if(BDSGlobalConstants::Instance()->SensitiveComponents()){
        SetMultipleSensitiveVolumes(itsOuterLogicalVolume);
      }

      if(BDSGlobalConstants::Instance()->IncludeIronMagFields())
	{
	  G4double polePos[4];
	  G4double Bfield[3];

	  polePos[0]=-BDSGlobalConstants::Instance()->MagnetPoleRadius()*sin(CLHEP::pi/6);
	  polePos[1]= BDSGlobalConstants::Instance()->MagnetPoleRadius()*cos(CLHEP::pi/6);
	  polePos[2]=0.;
	  polePos[3]=-999.;//flag to use polePos rather than local track
	                   //coordinate in GetFieldValue	    
	    
	  itsMagField->GetFieldValue(polePos,Bfield);
	  G4double BFldIron=
	    sqrt(Bfield[0]*Bfield[0]+Bfield[1]*Bfield[1])*
                BDSGlobalConstants::Instance()->MagnetPoleSize()/
	    (BDSGlobalConstants::Instance()->ComponentBoxSize()/2-
                BDSGlobalConstants::Instance()->MagnetPoleRadius());
	  // Magnetic flux from a pole is divided in two directions
	  BFldIron/=2.;

	  BuildOuterFieldManager(6, BFldIron,CLHEP::pi/6);
	}


      (*LogVolCount)[itsName]=1;
      (*LogVol)[itsName]=itsMarkerLogicalVolume;
    }
  else
    {
      (*LogVolCount)[itsName]++;
	  itsMarkerLogicalVolume=(*LogVol)[itsName];
    }
  
}


G4VisAttributes* BDSSkewSextupole::SetVisAttributes()
{
  itsVisAttributes=new G4VisAttributes(G4Colour(1,1,0));
  return itsVisAttributes;
}


void BDSSkewSextupole::BuildBPFieldAndStepper()
{
  // set up the magnetic field and stepper
  itsMagField=new BDSSkewSextMagField(itsBDblPrime);
  itsEqRhs=new G4Mag_UsualEqRhs(itsMagField);
  
  itsStepper=new BDSSkewSextStepper(itsEqRhs);
  itsStepper->SetBDblPrime(itsBDblPrime);

}

BDSSkewSextupole::~BDSSkewSextupole()
{
  delete itsMagField;
  delete itsEqRhs;
  delete itsStepper;
}
