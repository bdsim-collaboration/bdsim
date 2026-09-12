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
#include "BDSApertureType.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSUtilities.hh"

#include "globals.hh"
#include "G4String.hh"

#include <map>
#include <string>

// dictionary for BDSApertureType
template<>
std::map<BDSApertureType, std::string>* BDSApertureType::dictionary =
  new std::map<BDSApertureType, std::string> ({
   {BDSApertureType::circular,       "circular"},
   {BDSApertureType::elliptical,     "elliptical"},
   {BDSApertureType::lhc,            "lhc"},
   {BDSApertureType::lhcdetailed,    "lhcdetailed"},
   {BDSApertureType::rectangular,    "rectangular"},
   {BDSApertureType::rectellipse,    "rectellipse"},
   {BDSApertureType::racetrack,      "racetrack"},
   {BDSApertureType::octagonal,      "octagonal"},
   {BDSApertureType::circularvacuum, "circularvacuum"},
   {BDSApertureType::clicpcl,        "clicpcl"}
});	

BDSApertureType BDS::DetermineApertureType(G4String apertureType)
{
  std::map<G4String, BDSApertureType> types;
  types["circular"]       = BDSApertureType::circular;
  types["elliptical"]     = BDSApertureType::elliptical;
  types["rectangular"]    = BDSApertureType::rectangular;
  types["lhc"]            = BDSApertureType::lhc;
  types["lhcscreen"]      = BDSApertureType::lhc; // shortcut for madx compatability
  types["lhcdetailed"]    = BDSApertureType::lhcdetailed;
  types["rectellipse"]    = BDSApertureType::rectellipse;
  types["racetrack"]      = BDSApertureType::racetrack;
  types["octagonal"]      = BDSApertureType::octagonal;
  types["circularvacuum"] = BDSApertureType::circularvacuum;
  types["clicpcl"]        = BDSApertureType::clicpcl;

  apertureType = BDS::LowerCase(apertureType);

  auto result = types.find(apertureType);
  if (result == types.end())
    {// it's not a valid key
      G4String msg = "\"" + apertureType + "\" is not a valid apertureType\n";
      msg += "Available geometry types are:\n";
      for (const auto& it : types)
	{msg += "\"" + it.first + "\"\n";}
      throw BDSException(__METHOD_NAME__, msg);
    }
  
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "determined aperture type to be " << result->second  << G4endl;
#endif
  return result->second;
}
