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
#include "BDSException.hh"
#include "BDSOutputROOTEventTrajectory.hh"
#include "BDSOutputROOTEventTrajectoryPoint.hh"
#include "BDSProcessMap.hh"
#include "DataLoader.hh"
#include "Event.hh"

#include "TROOT.h"
#include "TSystem.h"

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <vector>

int main(int argc, char** argv)
{
  if (argc < 2 || argc > 2)
    {std::cout << "Incorrect arguments\nusage: BDSTrajectoryTest <datafile>" << std::endl; return 1;}

  std::string dataFile = std::string(argv[1]);

  // to simply syntax
  typedef BDSOutputROOTEventTrajectoryPoint point;
  typedef std::vector<BDSOutputROOTEventTrajectoryPoint> pointV;
  
  try
    {
      DataLoader* dl = new DataLoader(dataFile);
      TTree* eventTree = dl->GetEventTree();
      Event* event     = dl->GetEvent();
      
      eventTree->GetEntry(0);
      
      // test the process map class by looping over all trajectories in the first event and all steps
      // don't do anything with the value - just check it returns ok
      auto processMap = BDSProcessMap::Instance();
      auto traj = event->Trajectory;
      for (int i = 0; i < traj->n; i++)
        {
          for (int j = 0; j < (int)traj->postProcessTypes[i].size(); j++)
	          {
              int ty = traj->postProcessTypes[i][j];
              int st = traj->postProcessSubTypes[i][j];
              (*processMap)(ty, st);
            }
        }
      
      // test each function in the trajectory class
      // trackID 1 is the first track, so the primary
      pointV trackInteractions = event->Trajectory->trackInteractions(1);
      
      point parentProcessPoint = event->Trajectory->parentProcessPoint(1);
      
      //int nTrajectories = event->Trajectory->n;
      std::vector<unsigned int> trackIDsSorted = event->Trajectory->trackID;
      std::sort(trackIDsSorted.begin(), trackIDsSorted.end());
      int highTrackID = (int)trackIDsSorted.back();
      
      point  primProcessPoint  = event->Trajectory->primaryProcessPoint(highTrackID);

      pointV processHistory    = event->Trajectory->processHistory(highTrackID);

      event->Trajectory->printTrajectoryInfoByTrackID(highTrackID);
      
      event->Trajectory->printTrajectoryInfo(/*storageIndex=*/0);
      
      bool parentIsPrimary = event->Trajectory->parentIsPrimary(highTrackID);
      std::string answer = parentIsPrimary ? "True" : "False";
      std::cout << "Parent of trackID " << highTrackID << " is a primary? : " << answer << std::endl;
      
      // now test them with an invalid trackID to ensure no bad exception thrown
      // trackID should be >= 1
      event->Trajectory->trackInteractions(0);
      event->Trajectory->primaryProcessPoint(0);
      event->Trajectory->processHistory(0);
      event->Trajectory->parentIsPrimary(0);

      // now try with the primary which has no parent. The primary should by construction
      // have trackID = 1
      event->Trajectory->trackInteractions(1);
      event->Trajectory->primaryProcessPoint(1);
      event->Trajectory->processHistory(1);
      event->Trajectory->parentIsPrimary(1);

      delete dl;
    }
  catch (const BDSException& e)
    {std::cout << e.what() << std::endl;}
  catch (const std::exception& e)
    {std::cout << e.what() << std::endl;}
  std::cout << "End of test" << std::endl;
  std::_Exit(0);
  // return 0;
}
