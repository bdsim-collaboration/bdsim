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
#include "BDSHitThinThing.hh"
#include "BDSTrajectoryPoint.hh"
#include "BDSTrajectoryPointHit.hh"
#include "BDSTrajectoryPrimary.hh"

#include "G4Allocator.hh"

#include <algorithm>
#include <map>
#include <vector>

G4Allocator<BDSHitThinThing> BDSAllocatorThinThing;

BDSHitThinThing::BDSHitThinThing(G4int pdgIDIn,
				 G4int trackIDIn,
				 G4int parentIDIn,
				 G4int turnsTakenIn,
				 BDSTrajectoryPoint* hitIn):
  pdgID(pdgIDIn),
  trackID(trackIDIn),
  parentID(parentIDIn),
  turnsTaken(turnsTakenIn),
  hit(hitIn)
{;}

BDSHitThinThing::~BDSHitThinThing()
{
  delete hit;
}

std::vector<const BDSTrajectoryPoint*> BDSHitThinThing::TrajectoryPointsFromHC(BDSHitsCollectionThinThing* hits)
{
  std::vector<const BDSTrajectoryPoint*> result;
  if (!hits)
    {return result;}
  else
    {
      for (G4int i = 0; i < (G4int)hits->entries(); i++)
	{result.push_back((*hits)[i]->hit);}
      return result;
    }
}

BDSTrajectoryPointHit* BDSHitThinThing::GetNewTrajectoryPointHit() const
{
  return new BDSTrajectoryPointHit(pdgID,trackID, parentID, hit);
}

std::vector<const BDSTrajectoryPointHit*>
BDSHitThinThing::ResolvePossibleEarlierThinHits(const std::vector<const BDSTrajectoryPrimary*>& primaryTrajectoryHits,
                                                const BDSHitsCollectionThinThing* thinThingHits)
{
  auto basicResult = [&]() // lambda to avoid code duplication
  {
    std::vector<const BDSTrajectoryPointHit*> result;
    for (auto p : primaryTrajectoryHits)
      {
	auto fh = p->FirstHit();
	if (fh) // may not have hit anything, ie nullptr
	  {result.push_back(new BDSTrajectoryPointHit(p, fh));}
      }
    return result;
  };
  
  // thinThingHits is optional
  // the vector might be empty
  if (!thinThingHits)
    {return primaryTrajectoryHits.empty() ? std::vector<const BDSTrajectoryPointHit*>() : basicResult();}
  else if (thinThingHits->entries() == 0)
    {return basicResult();}
  else
    {
      // note there could be more than one thin hit per primary particle (e.g. multiple wire scanners)
      // build map of hits from the two sources per primary track ID
      std::map<G4int, std::vector<const BDSTrajectoryPointHit*>> hitsPerPrimary;
      for (G4int i = 0; i < (G4int)thinThingHits->entries(); i++)
	{
	  auto hit = (*thinThingHits)[i];
	  hitsPerPrimary[hit->trackID].push_back(hit->GetNewTrajectoryPointHit());
	}
      for (auto p : primaryTrajectoryHits)
	{hitsPerPrimary[p->GetTrackID()].push_back(new BDSTrajectoryPointHit(p, p->LastPoint()));}
      
      // use a lambda function to compare contents of pointers and not pointers themselves
      auto TrajPointComp = [](const BDSTrajectoryPointHit* a, const BDSTrajectoryPointHit* b)
			   {
			     if (!a || !b)
			       {return false;}
			     else
			       {return *a < *b;}
			   };
      
      // reduce multiple hits to the earliest one in the beam line for the 'first' hit
      std::vector<const BDSTrajectoryPointHit*> result;
      for (auto& primaryPoints : hitsPerPrimary)
	{
	  auto& points = primaryPoints.second;
	  std::sort(points.begin(), points.end(), TrajPointComp);
	  result.push_back(points[0]);
	}
      return result;
    }
}
