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
#include "BDSCavityType.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSUtilities.hh"

#include "globals.hh"
#include "G4String.hh"

#include <map>
#include <string>

// dictionary for BDSCavityType
template<>
std::map<BDSCavityType, std::string>* BDSCavityType::dictionary =
  new std::map<BDSCavityType, std::string> ({
  {BDSCavityType::elliptical,  "elliptical"},
  {BDSCavityType::rectangular, "rectangular"},
  {BDSCavityType::pillbox,     "pillbox"}
});

BDSCavityType BDS::DetermineCavityType(G4String cavityType)
{
  std::map<G4String, BDSCavityType> types;
  types["elliptical"]  = BDSCavityType::elliptical;
  types["rectangular"] = BDSCavityType::rectangular;
  types["pillbox"]     = BDSCavityType::pillbox;

  cavityType = BDS::LowerCase(cavityType);

  auto result = types.find(cavityType);
  if (result == types.end())
    {// it's not a valid key
      G4String msg = "\"" + cavityType + "\" is not a valid cavity type\n";
      msg += "Available cavity types are:\n";
      for (const auto& it : types)
	{msg += "\"" + it.first + "\"\n";}
      throw BDSException(__METHOD_NAME__, msg);
    }
  
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " determined cavity type to be " << result->second << G4endl;
#endif
  return result->second;
}
