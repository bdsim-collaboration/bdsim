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
//  
//   BDSIM, (C) 2001-2007
//    
//   version 0.5 
//   last modified : 12 Mar 2009 by malton@pp.rhul.ac.uk
//  


//
//    beam dumper/reader for online exchange - Sensitive Detector
//

#include "BDSGlobalConstants.hh" 

#include "BDSDumpSD.hh"
#include "BDSDebug.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4ParticleDefinition.hh"
#include "G4VTouchable.hh"
#include "G4TouchableHistory.hh"
#include "G4ios.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"

#include "G4AffineTransform.hh"

BDSDumpSD::BDSDumpSD(G4String name, G4String type):
  G4VSensitiveDetector(name),
  itsType(type)
{
}

BDSDumpSD::~BDSDumpSD()
{;}

void BDSDumpSD::Initialize(G4HCofThisEvent*)
{
}

G4bool BDSDumpSD::ProcessHits(G4Step*aStep,G4TouchableHistory*)
{
  G4cout << __METHOD_NAME__  <<G4endl;
  G4Track* theTrack=aStep->GetTrack();
  G4StepPoint* preStepPoint = aStep->GetPreStepPoint();
  
 
  //Check if the particle is on the boundary 
  if(preStepPoint->GetStepStatus()==fGeomBoundary){
    if(abs(theTrack->GetDefinition()->GetPDGEncoding()) == 11){
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ <<"Dump: postponing track..."<<G4endl;
#endif
      BDSGlobalConstants::Instance()->setWaitingForDump(true);
      //Don't postpone to next event if this is the last event.
      //      if(BDSRunManager::GetRunManager()->GetCurrentEvent()->GetEventID()!=(BDSGlobalConstants::Instance()->GetNumberToGenerate()-1)){
	theTrack->SetTrackStatus(fPostponeToNextEvent);
	//      }
      
      G4AffineTransform tf(aStep->GetPreStepPoint()->GetTouchableHandle()->
			   GetHistory()->GetTopTransform().Inverse());
      BDSGlobalConstants::Instance()->SetDumpTransform(tf);
    }
  }
  return true;
}
