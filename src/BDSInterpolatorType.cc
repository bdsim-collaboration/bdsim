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
#include "BDSInterpolatorType.hh"
#include "BDSUtilities.hh"

#include "globals.hh"
#include "G4String.hh"

#include <map>
#include <string>
#include <utility>

// dictionary for BDSInterpolatorType for reflexivity
template<>
std::map<BDSInterpolatorType, std::string>* BDSInterpolatorType::dictionary =
  new std::map<BDSInterpolatorType, std::string> ({
      {BDSInterpolatorType::nearestauto,"nearestauto"},
      {BDSInterpolatorType::nearest1d,  "nearest1d"},
      {BDSInterpolatorType::nearest2d,  "nearest2d"},
      {BDSInterpolatorType::nearest3d,  "nearest3d"},
      {BDSInterpolatorType::nearest4d,  "nearest4d"},
      {BDSInterpolatorType::linearauto, "linearauto"},
      {BDSInterpolatorType::linear1d,   "linear1d"},
      {BDSInterpolatorType::linear2d,   "linear2d"},
      {BDSInterpolatorType::linear3d,   "linear3d"},
      {BDSInterpolatorType::linear4d,   "linear4d"},
      {BDSInterpolatorType::linearauto, "linearmagauto"},
      {BDSInterpolatorType::linear1d,   "linearmag1d"},
      {BDSInterpolatorType::linear2d,   "linearmag2d"},
      {BDSInterpolatorType::linear3d,   "linearmag3d"},
      {BDSInterpolatorType::linear4d,   "linearmag4d"},
      {BDSInterpolatorType::cubicauto,  "cubicauto"},
      {BDSInterpolatorType::cubic1d,    "cubic1d"},
      {BDSInterpolatorType::cubic2d,    "cubic2d"},
      {BDSInterpolatorType::cubic3d,    "cubic3d"},
      {BDSInterpolatorType::cubic4d,    "cubic4d"}
    });

BDSInterpolatorType BDS::DetermineInterpolatorType(G4String interpolatorType)
{
  std::map<G4String, BDSInterpolatorType> types;
  types["nearest"]     = BDSInterpolatorType::nearestauto;
  types["nearest1d"]   = BDSInterpolatorType::nearest1d;
  types["nearest2d"]   = BDSInterpolatorType::nearest2d;
  types["nearest3d"]   = BDSInterpolatorType::nearest3d;
  types["nearest4d"]   = BDSInterpolatorType::nearest4d;
  types["linear"]      = BDSInterpolatorType::linearauto;
  types["linear1d"]    = BDSInterpolatorType::linear1d;
  types["linear2d"]    = BDSInterpolatorType::linear2d;
  types["linear3d"]    = BDSInterpolatorType::linear3d;
  types["linear4d"]    = BDSInterpolatorType::linear4d;
  types["linearmag"]   = BDSInterpolatorType::linearmagauto;
  types["linearmag1d"] = BDSInterpolatorType::linearmag1d;
  types["linearmag2d"] = BDSInterpolatorType::linearmag2d;
  types["linearmag3d"] = BDSInterpolatorType::linearmag3d;
  types["linearmag4d"] = BDSInterpolatorType::linearmag4d;
  types["cubic"]       = BDSInterpolatorType::cubicauto;
  types["cubic1d"]     = BDSInterpolatorType::cubic1d;
  types["cubic2d"]     = BDSInterpolatorType::cubic2d;
  types["cubic3d"]     = BDSInterpolatorType::cubic3d;
  types["cubic4d"]     = BDSInterpolatorType::cubic4d;

  interpolatorType = BDS::LowerCase(interpolatorType);

  auto result = types.find(interpolatorType);
  if (result == types.end())
    {// it's not a valid key
      G4String msg = "\"" + interpolatorType + "\" is not a valid field type\n";
      msg += "Available interpolator types are:\n";
      for (const auto& it : types)
	{msg += "\"" + it.first + "\"\n";}
      throw BDSException(__METHOD_NAME__, msg);
    }

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "determined interpolator type to be " << result->second << G4endl;
#endif
  return result->second;
}

G4int BDS::NDimensionsOfInterpolatorType(const BDSInterpolatorType& it)
{
  G4int result = 0;
  switch (it.underlying())
    {
      case BDSInterpolatorType::nearest1d:
      case BDSInterpolatorType::linear1d:
      case BDSInterpolatorType::linearmag1d:
      case BDSInterpolatorType::cubic1d:
        {result = 1; break;}
      case BDSInterpolatorType::nearest2d:
      case BDSInterpolatorType::linear2d:
      case BDSInterpolatorType::linearmag2d:
      case BDSInterpolatorType::cubic2d:
        {result = 2; break;}
      case BDSInterpolatorType::nearest3d:
      case BDSInterpolatorType::linear3d:
      case BDSInterpolatorType::linearmag3d:
      case BDSInterpolatorType::cubic3d:
	{result = 3; break;}
      case BDSInterpolatorType::nearest4d:
      case BDSInterpolatorType::linear4d:	
      case BDSInterpolatorType::linearmag4d:
      case BDSInterpolatorType::cubic4d:
        {result = 4; break;}
      default:
        {result = 0; break;}
    }
  return result;
}

G4bool BDS::InterpolatorTypeIsAuto(BDSInterpolatorType typeIn)
{
  G4bool result = false;
  switch (typeIn.underlying())
    {
    case BDSInterpolatorType::nearestauto:
    case BDSInterpolatorType::linearauto:
    case BDSInterpolatorType::linearmagauto:
    case BDSInterpolatorType::cubicauto:
      {result = true; break;}
    default:
      {break;}
    }
  return result;
}

BDSInterpolatorType BDS::InterpolatorTypeSpecificFromAuto(G4int               nDimension,
							  BDSInterpolatorType autoType)
{
  std::map<std::pair<G4int, BDSInterpolatorType>, BDSInterpolatorType> mapping = {
    {std::make_pair(1, BDSInterpolatorType::nearestauto),    BDSInterpolatorType::nearest1d},
    {std::make_pair(2, BDSInterpolatorType::nearestauto),    BDSInterpolatorType::nearest2d},
    {std::make_pair(3, BDSInterpolatorType::nearestauto),    BDSInterpolatorType::nearest3d},
    {std::make_pair(4, BDSInterpolatorType::nearestauto),    BDSInterpolatorType::nearest4d},
    {std::make_pair(1, BDSInterpolatorType::linearauto),     BDSInterpolatorType::linear1d},
    {std::make_pair(2, BDSInterpolatorType::linearauto),     BDSInterpolatorType::linear2d},
    {std::make_pair(3, BDSInterpolatorType::linearauto),     BDSInterpolatorType::linear3d},
    {std::make_pair(4, BDSInterpolatorType::linearauto),     BDSInterpolatorType::linear4d},
    {std::make_pair(1, BDSInterpolatorType::linearmagauto),  BDSInterpolatorType::linearmag1d},
    {std::make_pair(2, BDSInterpolatorType::linearmagauto),  BDSInterpolatorType::linearmag2d},
    {std::make_pair(3, BDSInterpolatorType::linearmagauto),  BDSInterpolatorType::linearmag3d},
    {std::make_pair(4, BDSInterpolatorType::linearmagauto),  BDSInterpolatorType::linearmag4d},
    {std::make_pair(1, BDSInterpolatorType::cubicauto),      BDSInterpolatorType::cubic1d},
    {std::make_pair(2, BDSInterpolatorType::cubicauto),      BDSInterpolatorType::cubic2d},
    {std::make_pair(3, BDSInterpolatorType::cubicauto),      BDSInterpolatorType::cubic3d},
    {std::make_pair(4, BDSInterpolatorType::cubicauto),      BDSInterpolatorType::cubic4d},
  };
  auto key = std::make_pair(nDimension, autoType);
  auto search = mapping.find(key);
  if (search != mapping.end())
    {return search->second;}
  else
    {throw BDSException(__METHOD_NAME__, "invalid number of dimensions in auto-mapping to interpolators.");}
}
