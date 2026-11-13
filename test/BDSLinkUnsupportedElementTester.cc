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

#include "BDSException.hh"
#include "BDSIMLink.hh"
#include "BDSLinkBunch.hh"

#include <iostream>
#include <string>
#include <vector>

int main(int argc, char** argv)
{
  if (argc != 2)
    {
      std::cerr << "Usage: BDSLinkUnsupportedElementTester <gmad-file>" << std::endl;
      return 1;
    }

  BDSLinkBunch bunch;
  BDSIMLink link(&bunch);
  std::vector<std::string> arguments = {
    "bdsim",
    "--file=" + std::string(argv[1]),
    "--output=None",
    "--batch",
    "--seed=1234"
  };
  std::vector<char*> argumentPointers;
  for (auto& argument : arguments)
    {argumentPointers.push_back(&argument[0]);}

  try
    {
      link.Initialise((G4int)argumentPointers.size(), argumentPointers.data(), false);
    }
  catch (const BDSException& exception)
    {
      const std::string message = exception.what();
      if (message.find("BDSLink supports only straight element definitions") != std::string::npos)
        {return 0;}
      std::cerr << "Unexpected BDSException: " << message << std::endl;
      return 1;
    }

  std::cerr << "BDSLink accepted an element with unsupported curved geometry" << std::endl;
  return 1;
}
