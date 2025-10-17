/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2024.

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
#include "BDSAuxiliaryNavigator.hh"
#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh"
#include "BDSHitSamplerLink.hh"
#include "BDSLinkEventAction.hh"
#include "BDSLinkEventInfo.hh"
#include "BDSLinkRunAction.hh"
#include "BDSOutput.hh"
#include "BDSSDSampler.hh"
#include "BDSSDSamplerLink.hh"
#include "BDSSDManager.hh"
#include "BDSUtilities.hh"

#include "globals.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4PropagatorInField.hh"
#include "G4Run.hh"
#include "G4SDManager.hh"
#include "G4THitsMap.hh"
#include "G4TransportationManager.hh"
#include "G4VUserEventInformation.hh"

#include <map>


BDSLinkEventAction::BDSLinkEventAction(BDSOutput*        outputIn,
                                       BDSLinkRunAction* runActionIn,
                                       G4bool            debugIn):
  output(outputIn),
  runAction(runActionIn),
  debug(debugIn),
  collIDSamplerLink(-1),
  collIDSampler(-1),
  currentEventIndex(0),
  primaryAbsorbedInCollimator(false)
{
  BDSGlobalConstants* globals = BDSGlobalConstants::Instance();
  verboseEventBDSIM         = globals->VerboseEventBDSIM();
  verboseEventStart         = globals->VerboseEventStart();
  verboseEventStop          = BDS::VerboseEventStop(verboseEventStart, globals->VerboseEventContinueFor());
  printModulo               = globals->PrintModuloEvents();
}

BDSLinkEventAction::~BDSLinkEventAction()
{;}

void BDSLinkEventAction::BeginOfEventAction(const G4Event* evt)
{
  currentEventIndex = evt->GetEventID();
  primaryAbsorbedInCollimator = false;
  
  // reset navigators to ensure no mis-navigating and that events are truly independent
  BDSAuxiliaryNavigator::ResetNavigatorStates();
  // make further attempts to clear Geant4's tracking history between events to make them
  // truly independent.
  G4Navigator* trackingNavigator = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
  trackingNavigator->ResetStackAndState();
  G4TransportationManager* tm = G4TransportationManager::GetTransportationManager();
  int i = 0;
  for (auto it = tm->GetActiveNavigatorsIterator(); i < (int)tm->GetNoActiveNavigators(); it++)
    {(*it)->ResetStackAndState(); i++;}
  tm->GetPropagatorInField()->ClearPropagatorState(); // <- this one really makes a difference

  // number feedback
  if (debug)
    {
      if (currentEventIndex % printModulo == 0)
        {G4cout << "---> Begin of event: " << currentEventIndex << G4endl;}
    }
  if (verboseEventBDSIM) // always print this out
    {G4cout << __METHOD_NAME__ << "event #" << currentEventIndex << G4endl;}

  // cache hit collection IDs for quicker access
  if (collIDSamplerLink < 0)
    {// if one is -1 then all need initialised.
      G4SDManager*  g4SDMan  = G4SDManager::GetSDMpointer();
      BDSSDManager* bdsSDMan = BDSSDManager::Instance();
      collIDSamplerLink = g4SDMan->GetCollectionID(bdsSDMan->SamplerLink()->GetName());
      collIDSampler     = g4SDMan->GetCollectionID(bdsSDMan->SamplerPlane()->GetName());
    }
}

void BDSLinkEventAction::EndOfEventAction(const G4Event* evt)
{
  // Get the hits collection of this event - all hits from different SDs.
  G4HCofThisEvent* HCE = evt->GetHCofThisEvent();
  typedef BDSHitsCollectionSamplerLink slhc;
  slhc* samplerLink = HCE ? dynamic_cast<slhc*>(HCE->GetHC(collIDSamplerLink)) : nullptr;
  typedef BDSHitsCollectionSampler shc;
  shc* sampHC = HCE ? dynamic_cast<shc*>(HCE->GetHC(collIDSampler)) : nullptr;
  std::vector<shc*> allSamplerHits = {sampHC};
  
  G4VUserEventInformation* evtInfoG4 = evt->GetUserInformation();
  BDSLinkEventInfo* evtInfo = dynamic_cast<BDSLinkEventInfo*>(evtInfoG4);
  G4int primaryExternalParticleID = 0;
  G4int primaryExternalParentID   = 0;
  if (evtInfo)
    {
      primaryExternalParticleID = evtInfo->externalParticleIDofPrimary;
      primaryExternalParentID   = evtInfo->externalParentIDofPrimary;
    }
  
  if (!samplerLink)
    {return;}
  if (samplerLink->entries() <= 0)
    {return;}
  else
    {runAction->AppendHits(currentEventIndex, primaryExternalParticleID, primaryExternalParentID, samplerLink);}

  output->FillEvent(nullptr,
                    evt->GetPrimaryVertex(),
                    allSamplerHits,
                    std::vector<BDSHitsCollectionSamplerCylinder*>(),
                    std::vector<BDSHitsCollectionSamplerSphere*>(),
                    samplerLink,
                    nullptr,
                    nullptr,
                    nullptr,
                    nullptr,
                    nullptr,
                    nullptr,
                    nullptr,
                    std::vector<const BDSTrajectoryPointHit*>(),
                    std::vector<const BDSTrajectoryPointHit*>(),
                    nullptr,
                    nullptr,
                    nullptr,
                    std::map<G4String, G4THitsMap<G4double>*>(),
                    BDSGlobalConstants::Instance()->TurnsTaken());
}
