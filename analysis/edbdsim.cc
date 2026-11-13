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
#include "TRint.h"
#include "TRootApplication.h"

#include "Config.hh"
#include "EventDisplay.hh"

#include <exception>
#include <iostream>

int main(int /*argc*/, char *argv[])
{
  int rintArgc;
  char** rintArgv = nullptr;
  TRint* a = new TRint("App", &rintArgc, rintArgv);

  try
    {
      Config::Instance("analysisConfig.txt");
      EventDisplay::Instance(argv[2], argv[1]);
      a->Run(kTRUE);
    }
  catch (const std::exception& e)
    {std::cerr << e.what() << std::endl; return 1;}
  return 0;
}
