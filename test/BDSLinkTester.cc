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
/**
 * @file bdsim.cc
 *
 * \mainpage
 * BDSIM © 2001-@CURRENT_YEAR@
 *
 * version @BDSIM_VERSION@
 */
#include "BDSBunchSixTrackLink.hh"
#include "BDSIMLink.hh"
#include "BDSException.hh"

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

int main(int argc, char** argv)
{
  BDSIMLink* bds = nullptr;
  BDSBunchSixTrackLink* stp = nullptr;
  try
    {
      stp = new BDSBunchSixTrackLink();
      bds = new BDSIMLink(stp);

      //std::vector<std::string> arguments = {"--file=../examples/features/link/link_collimators.gmad",
      //				    "--vis_debug"/*, "--batch"*/};
      /*std::vector<char*> argvv;
	for (const auto& arg : arguments)
	{argvv.push_back((char*)arg.data());}
	argv.push_back(std::string(*(argv->[0])));
      */
      bds->Initialise(argc, argv, true, 100);
      if (!bds->Initialised())
	{
	  if (bds->InitialisationResult() == 1) // if 2 it's ok
	    {std::cout << "Intialisation failed" << std::endl; return 1;}
	}
      else
	{
	  bds->BeamOn();
	}
      auto hits = bds->SamplerHits();
      if (hits)
	{G4cout << hits->entries() << G4endl;}
      delete bds;
      delete stp;
    }
  catch (const BDSException& exception)
    {
      std::cerr << std::endl << exception.what() << std::endl;
      delete bds;
      delete stp;
      exit(1);
    }
  catch (const std::exception& exception)
    {
      std::cerr << std::endl << exception.what() << std::endl;
      delete bds;
      delete stp;
      exit(1);
    }

  return 0;
}
