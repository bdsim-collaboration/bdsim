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
#include "BDSException.hh"
#include "BDSOutputType.hh"
#include "BDSUtilities.hh"

#include "globals.hh"
#include "G4String.hh"

#include <map>
#include <string>

template<>
std::map<BDSOutputType,std::string>* BDSOutputType::dictionary=
  new std::map<BDSOutputType,std::string> ({
      {BDSOutputType::none,"none"},
      {BDSOutputType::rootevent,"rootevent"}
    });

BDSOutputType BDS::DetermineOutputType(G4String outputType)
{
  std::map<G4String, BDSOutputType> types;
  types["none"]      = BDSOutputType::none;
  types["rootevent"] = BDSOutputType::rootevent;

  outputType = BDS::LowerCase(outputType);

  auto result = types.find(outputType);
  if (result == types.end())
    {// it's not a valid key
      G4String msg = "\"" + outputType + "\" is not a valid output format\n";
      msg += "Available output formats are:\n";
      for (const auto& it : types)
	{msg += "\"" + it.first + "\"\n";}
      throw BDSException(__METHOD_NAME__, msg);
    }
  
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "determined format \"" << outputType << "\" to be " << result->first << G4endl;
#endif
  return result->second;
}
