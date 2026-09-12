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
#include "BDSBeamPipeType.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSUtilities.hh"

#include "globals.hh"
#include "G4String.hh"

#include <map>
#include <string>

// dictionary for BDSBeamPipeType
template<>
std::map<BDSBeamPipeType, std::string>* BDSBeamPipeType::dictionary =
  new std::map<BDSBeamPipeType, std::string> ({
   {BDSBeamPipeType::circular,       "circular"},
   {BDSBeamPipeType::elliptical,     "elliptical"},
   {BDSBeamPipeType::lhc,            "lhc"},
   {BDSBeamPipeType::lhcdetailed,    "lhcdetailed"},
   {BDSBeamPipeType::rectangular,    "rectangular"},
   {BDSBeamPipeType::rectellipse,    "rectellipse"},
   {BDSBeamPipeType::racetrack,      "racetrack"},
   {BDSBeamPipeType::octagonal,      "octagonal"},
   {BDSBeamPipeType::circularvacuum, "circularvacuum"},
   {BDSBeamPipeType::clicpcl,        "clicpcl"},
   {BDSBeamPipeType::rhombus,        "rhombus"}
});	

BDSBeamPipeType BDS::DetermineBeamPipeType(G4String apertureType)
{
  std::map<G4String, BDSBeamPipeType> types;
  types["circular"]       = BDSBeamPipeType::circular;
  types["elliptical"]     = BDSBeamPipeType::elliptical;
  types["rectangular"]    = BDSBeamPipeType::rectangular;
  types["lhc"]            = BDSBeamPipeType::lhc;
  types["lhcscreen"]      = BDSBeamPipeType::lhc; // shortcut for madx compatability
  types["lhcdetailed"]    = BDSBeamPipeType::lhcdetailed;
  types["rectellipse"]    = BDSBeamPipeType::rectellipse;
  types["racetrack"]      = BDSBeamPipeType::racetrack;
  types["octagonal"]      = BDSBeamPipeType::octagonal;
  types["circularvacuum"] = BDSBeamPipeType::circularvacuum;
  types["clicpcl"]        = BDSBeamPipeType::clicpcl;
  types["pointsfile"]     = BDSBeamPipeType::pointsfile; // added only so the error print out will be correct
  types["rhombus"]        = BDSBeamPipeType::rhombus;

  apertureType = BDS::LowerCase(apertureType);

  // string will be pointsfile:somefile.dat, so we specially
  // detect this one as it won't match exactly
  if (BDS::StrContains(apertureType, "pointsfile"))
    {return BDSBeamPipeType::pointsfile;}

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
