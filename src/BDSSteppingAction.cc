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
#include "BDSSteppingAction.hh"
#include "BDSUtilities.hh"

#include "globals.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4Track.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VProcess.hh"


BDSSteppingAction::BDSSteppingAction():
  verboseStep(false),
  verboseEventStart(false),
  verboseEventStop(false)
{;}

BDSSteppingAction::BDSSteppingAction(G4bool verboseStepIn,
				     G4int  verboseEventStartIn,
				     G4int  verboseEventStopIn):
  verboseStep(verboseStepIn),
  verboseEventStart(verboseEventStartIn),
  verboseEventStop(verboseEventStopIn)
{;}

BDSSteppingAction::~BDSSteppingAction()
{;}

void BDSSteppingAction::UserSteppingAction(const G4Step* step)
{
  if (!verboseStep)
    {return;}
  G4int eventID = G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID();
  if (BDS::VerboseThisEvent(eventID, verboseEventStart, verboseEventStop) || verboseStep)
    {VerboseSteppingAction(step);}
}

void BDSSteppingAction::VerboseSteppingAction(const G4Step* step)
{ 
  //output in case of verbose step
  G4Track* track        = step->GetTrack();
  int trackID           = track->GetTrackID();
  G4VPhysicalVolume* pv = track->GetVolume();
  G4LogicalVolume* lv   = pv->GetLogicalVolume();
  G4ThreeVector pos     = track->GetPosition();
  G4ThreeVector mom     = track->GetMomentum() / CLHEP::GeV;
  G4String materialName = track->GetMaterial()->GetName();
  
  int G4precision = G4cout.precision();
  G4cout.precision(10);
  G4cout << "pv: " << pv->GetName() << " lv: " << lv->GetName() << G4endl;
  G4cout << "trackID: "  << trackID
	 << " pdgID: " << track->GetDefinition()->GetParticleName()
	 << " Energy: "   << track->GetTotalEnergy()/CLHEP::GeV
	 << " position: " << pos << "mm "
	 << " momentum: " << mom
	 << " material: " << materialName
	 << G4endl;
	    
  auto preProcess  = step->GetPreStepPoint()->GetProcessDefinedStep();
  auto postProcess = step->GetPostStepPoint()->GetProcessDefinedStep();
  
  if (preProcess)
    {G4cout << "Pre-step process: " << preProcess->GetProcessName() << G4endl;}
  if (postProcess)
    {G4cout << "Post-step process: " << postProcess->GetProcessName() << G4endl;}

  // set precision back
  G4cout.precision(G4precision);
}
