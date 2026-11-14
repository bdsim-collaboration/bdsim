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
   {BDSApertureType::circle,      "circle"},
   {BDSApertureType::ellipse,     "ellipse"},
   {BDSApertureType::rectangle,   "rectangle"},
   {BDSApertureType::rectcircle,  "rectcircle"},
   {BDSApertureType::rectellipse, "rectellipse"},
   {BDSApertureType::racetrack,   "racetrack"},
   {BDSApertureType::octagon,     "octagon"},
   {BDSApertureType::clicpcl,     "clicpcl"},
   {BDSApertureType::rhombus,     "rhombus"},
   {BDSApertureType::points,      "points"},
   {BDSApertureType::regularpolygon, "regularpolygon"}
});	

BDSApertureType BDS::DetermineApertureType(G4String apertureType,
                                           G4bool tolerateBeamPipeType)
{
  std::map<G4String, BDSApertureType> types;
  types["circle"]       = BDSApertureType::circle;
  types["ellipse"]      = BDSApertureType::ellipse;
  types["rectangle"]    = BDSApertureType::rectangle;
  types["rectcircle"]   = BDSApertureType::rectcircle;
  types["rectellipse"]  = BDSApertureType::rectellipse;
  types["racetrack"]    = BDSApertureType::racetrack;
  types["octagon"]      = BDSApertureType::octagon;
  types["clicpcl"]      = BDSApertureType::clicpcl;
  types["rhombus"]      = BDSApertureType::rhombus;
  types["points"]       = BDSApertureType::points;
  types["regularpolygon"] = BDSApertureType::regularpolygon;

  // alternatives
  types["lhc"]          = BDSApertureType::rectcircle;

  if (tolerateBeamPipeType)
    {
      types["circular"]       = BDSApertureType::circle;
      types["elliptical"]     = BDSApertureType::ellipse;
      types["rectangular"]    = BDSApertureType::rectangle;
      types["lhc"]            = BDSApertureType::rectcircle;
      types["lhcscreen"]      = BDSApertureType::rectcircle; // shortcut for madx compatability
      types["lhcdetailed"]    = BDSApertureType::rectcircle;
      types["rectellipse"]    = BDSApertureType::rectellipse;
      types["racetrack"]      = BDSApertureType::racetrack;
      types["octagonal"]      = BDSApertureType::octagon;
      types["circularvacuum"] = BDSApertureType::circle;
      types["clicpcl"]        = BDSApertureType::clicpcl;
      types["rhombus"]        = BDSApertureType::rhombus;
    }
  
  apertureType = BDS::LowerCase(apertureType);

  if (BDS::StrContains(apertureType, "pointsfile"))
    {return BDSApertureType::points;}

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
