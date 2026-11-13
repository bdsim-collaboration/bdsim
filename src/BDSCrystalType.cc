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
#include "BDSCrystalType.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSUtilities.hh"

#include "globals.hh"
#include "G4String.hh"

#include <map>
#include <string>

// dictionary for BDSCrystalType
template<>
std::map<BDSCrystalType, std::string>* BDSCrystalType::dictionary =
  new std::map<BDSCrystalType, std::string> ({
  {BDSCrystalType::box,      "box"},
  {BDSCrystalType::cylinder, "cylinder"},
  {BDSCrystalType::torus,    "torus"}
});

BDSCrystalType BDS::DetermineCrystalType(G4String crystalType)
{
  std::map<G4String, BDSCrystalType> types;
  types["box"]      = BDSCrystalType::box;
  types["cylinder"] = BDSCrystalType::cylinder;
  types["torus"]    = BDSCrystalType::torus;

  crystalType = BDS::LowerCase(crystalType);

  auto result = types.find(crystalType);
  if (result == types.end())
    {// it's not a valid key
      G4String msg = "\"" + crystalType + "\" is not a valid crystal shape\n";
      msg += "Available crystal shapes are:\n";
      for (const auto& it : types)
	{msg += "\"" + it.first + "\"\n";}
      throw BDSException(__METHOD_NAME__, msg);
    }
  
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " determined crystal shape to be " << result->second << G4endl;
#endif
  return result->second;
}
