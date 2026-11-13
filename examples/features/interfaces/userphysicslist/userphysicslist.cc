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
#include "BDSIMClass.hh" // bdsim interface

#include "SimplePhysics.hh"

#include <iostream>

int main(int argc, char** argv)
{
  // construct an instance of bdsim
  BDSIM* bds = new BDSIM();

  auto myphysics = new SimplePhysics();
  bds->RegisterUserPhysicsList(myphysics);

  // construct geometry and physics
  bds->Initialise(argc, argv);
  if (!bds->Initialised()) // check if there was a problem.
    {std::cout << "Intialisation failed" << std::endl; return 1;}
  
  bds->BeamOn(); // run the simulation
  delete bds;    // clean up
  return 0;      // exit nicely
}
