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
#include "BDSDebug.hh"
#include "BDSTrajectory.hh"
#include "BDSTrajectoryPoint.hh"

#include "globals.hh" // geant4 globals / types
#include "G4Allocator.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4VProcess.hh"
#include "G4TrajectoryContainer.hh"  // also provides TrajectoryVector type(def)

#include <map>
#include <ostream>

G4Allocator<BDSTrajectory> bdsTrajectoryAllocator;

BDSTrajectory::BDSTrajectory(const G4Track* aTrack,
                             G4bool         interactiveIn,
                             const BDS::TrajectoryOptions& storageOptionsIn):
  G4Trajectory(aTrack),
  interactive(interactiveIn),
  storageOptions(storageOptionsIn),
  parent(nullptr),
  trajIndex(0),
  parentIndex(0),
  parentStepIndex(0),
  depth(-1)
{
  suppressTransportationAndNotInteractive = storageOptionsIn.suppressTransportationSteps && !interactiveIn;
  const G4VProcess* proc = aTrack->GetCreatorProcess();
  if (proc)
    {
      creatorProcessType    = aTrack->GetCreatorProcess()->GetProcessType();
      creatorProcessSubType = aTrack->GetCreatorProcess()->GetProcessSubType();
    }
  else
    {
      creatorProcessType    = -1;
      creatorProcessSubType = -1;
    }
  weight = aTrack->GetWeight();

  parentIndex = -1;
  fpBDSPointsContainer = new BDSTrajectoryPointsContainer();
  // this is for the first point of the track
  (*fpBDSPointsContainer).push_back(new BDSTrajectoryPoint(aTrack,
                                                           storageOptions.storeLocal,
                                                           storageOptions.storeLinks,
                                                           storageOptions.storeIon));
}

BDSTrajectory::~BDSTrajectory()
{
  // clean points container
  for (auto i : *fpBDSPointsContainer)
    {delete i;}
  fpBDSPointsContainer->clear();
  delete fpBDSPointsContainer;
}

void BDSTrajectory::AppendStep(const BDSTrajectoryPoint* pointIn)
{
  if (suppressTransportationAndNotInteractive)
    {
      if (pointIn->NotTransportationLimitedStep())
        {
          auto r = new BDSTrajectoryPoint(*pointIn);
          if (fpBDSPointsContainer->size() == 1)
            {(*fpBDSPointsContainer)[0]->SetMaterial(pointIn->GetMaterial());}
          CleanPoint(r);
          fpBDSPointsContainer->push_back(r);
        }
    }
  else
    {
      auto r = new BDSTrajectoryPoint(*pointIn);
      if (fpBDSPointsContainer->size() == 1)
        {(*fpBDSPointsContainer)[0]->SetMaterial(pointIn->GetMaterial());}
      CleanPoint(r);
      fpBDSPointsContainer->push_back(r);
    }
}

void BDSTrajectory::CleanPoint(BDSTrajectoryPoint* point) const
{
  if (!storageOptions.storeIon)
    {point->DeleteExtraIon();}
  if (!storageOptions.storeLinks)
    {point->DeleteExtraLinks();}
  if (!storageOptions.storeLocal)
    {point->DeleteExtraLocal();}
}

void BDSTrajectory::AppendStep(const G4Step* aStep)
{
  // we do not use G4Trajectory::AppendStep here as that would
  // duplicate position information in its own vector of positions
  // which we prevent access to be overriding GetPoint
  
  // if the first step, we update the material of the 0th point which was
  // constructed from the track before geometry tracking and we didn't know
  // the material
  if (fpBDSPointsContainer->size() == 1)
    {(*fpBDSPointsContainer)[0]->SetMaterial(aStep->GetTrack()->GetMaterial());}
  if (suppressTransportationAndNotInteractive)
    {
      // note for a first step of a track, the prestep point process
      // may be nullptr, but if we're appending a step we really care
      // about what the post process is - test on that
      auto postStepPoint = aStep->GetPostStepPoint(); 
      const G4VProcess* postProcess = postStepPoint->GetProcessDefinedStep();
      if (postProcess)
        {
          G4int postProcessType = postProcess->GetProcessType();
          if(postProcessType != 1   /* transportation */ &&
             postProcessType != 10 /* parallel world */ )
            {
              fpBDSPointsContainer->push_back(new BDSTrajectoryPoint(aStep,
                                                                     storageOptions.storeLocal,
                                                                     storageOptions.storeLinks,
                                                                     storageOptions.storeIon));
            }
        }
    }
  else
    {
      fpBDSPointsContainer->push_back(new BDSTrajectoryPoint(aStep,
                                                             storageOptions.storeLocal,
                                                             storageOptions.storeLinks,
                                                             storageOptions.storeIon));
    }
}

void BDSTrajectory::MergeTrajectory(G4VTrajectory* secondTrajectory)
{
  if(!secondTrajectory)
    {return;}
  
  BDSTrajectory* second = (BDSTrajectory*)secondTrajectory;
  G4int ent = second->GetPointEntries();
  // initial point of the second trajectory should not be merged
  for (G4int i = 1; i < ent; ++i)
    {fpBDSPointsContainer->push_back((*(second->fpBDSPointsContainer))[i]);}
  delete (*second->fpBDSPointsContainer)[0];
  second->fpBDSPointsContainer->clear();
}

BDSTrajectoryPoint* BDSTrajectory::FirstInteraction()const
{
  // loop over trajectory to find non transportation step
  for (G4int i = 0; i < GetPointEntries(); ++i)
    {
      BDSTrajectoryPoint* point = static_cast<BDSTrajectoryPoint*>(GetPoint(i));
      if (point->IsScatteringPoint())
        {return point;}
    }
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "no interaction" << G4endl;
#endif
  return static_cast<BDSTrajectoryPoint*>(GetPoint(0));
}

BDSTrajectoryPoint* BDSTrajectory::LastInteraction()const
{
  // loop over trajectory backwards to find non transportation step
  for (G4int i = GetPointEntries()-1; i >= 0; --i)
    {
      BDSTrajectoryPoint* point = static_cast<BDSTrajectoryPoint*>(GetPoint(i));
      if (point->IsScatteringPoint())
        {return point;}
  }
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "no interaction" << G4endl;
#endif
  return static_cast<BDSTrajectoryPoint*>(GetPoint(GetPointEntries()-1));
}

std::ostream& operator<< (std::ostream& out, BDSTrajectory const& t)
{
  for (G4int i = 0; i < t.GetPointEntries(); i++)
    {out << *(static_cast<BDSTrajectoryPoint*>(t.GetPoint(i))) << G4endl;}
  return out;
}

