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
#include "BDSDimensionType.hh"
#include "BDSException.hh"
#include "BDSUtilities.hh"

#include "globals.hh"
#include "G4String.hh"

#include <map>
#include <string>

template<>
std::map<BDSDimensionType,std::string>* BDSDimensionType::dictionary=
  new std::map<BDSDimensionType,std::string>
  ({
    {BDSDimensionType::x, "x"},
    {BDSDimensionType::y, "y"},
    {BDSDimensionType::z, "z"},
    {BDSDimensionType::t, "y"}
  });

BDSDimensionType BDS::DetermineDimensionType(G4String dimensionType)
{
  std::map<G4String, BDSDimensionType> types;
  types["x"] = BDSDimensionType::x;
  types["y"] = BDSDimensionType::y;
  types["z"] = BDSDimensionType::z;
  types["t"] = BDSDimensionType::t;

  dimensionType = BDS::LowerCase(dimensionType);

  auto result = types.find(dimensionType);
  if (result == types.end())
    {// it's not a valid key
      G4String msg = "\"" + dimensionType + "\" is not a valid dimension\n";
      msg += "Available dimensions are:\n";
      for (const auto& it : types)
        {msg += "\"" + it.first + "\"\n";}
      throw BDSException(__METHOD_NAME__, msg);
    }
  
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "determined format \"" << dimensionType << "\" to be " << result->first << G4endl;
#endif
  return result->second;
}
