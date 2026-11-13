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
#include "BDSScorerType.hh"
#include "BDSUtilities.hh"

#include "globals.hh"
#include "G4String.hh"

#include <map>
#include <string>

/// Dictionary for BDSScorerType.
template<>
std::map<BDSScorerType, std::string>* BDSScorerType::dictionary =
        new std::map<BDSScorerType, std::string> ({
	    {BDSScorerType::cellcharge,         "cellcharge"},
	    {BDSScorerType::cellcharge3d,       "cellcharge3d"},
	    {BDSScorerType::depositeddose,      "depositeddose"},
	    {BDSScorerType::depositeddose3d,    "depositeddose3d"},
	    {BDSScorerType::depositedenergy,    "depositedenergy"},
	    {BDSScorerType::depositedenergyblm, "depositedenergyblm"},
	    {BDSScorerType::depositedenergy,    "depositedenergy3d"},
	    {BDSScorerType::population,         "population"},
	    {BDSScorerType::population3d,       "population3d"},
	    {BDSScorerType::populationscaled,   "populationscaled"},
	    {BDSScorerType::cellflux,           "cellflux"},
	    {BDSScorerType::cellflux3d,         "cellflux3d"},
	    {BDSScorerType::cellfluxscaled,     "cellfluxscaled"},
	    {BDSScorerType::cellfluxscaled3d,   "cellfluxscaled3d"},
	    {BDSScorerType::cellfluxscaledperparticle,   "cellfluxscaledperparticle"},
	    {BDSScorerType::cellfluxscaledperparticle3d, "cellfluxscaledperparticle3d"},
	    {BDSScorerType::cellflux4d,"cellflux4d"}
        });

BDSScorerType BDS::DetermineScorerType(G4String scorerType)
{
  std::map<G4String, BDSScorerType> types;
  types["cellcharge"]         = BDSScorerType::cellcharge;
  types["cellcharge3d"]       = BDSScorerType::cellcharge3d;
  types["depositeddose"]      = BDSScorerType::depositeddose;
  types["depositeddose3d"]    = BDSScorerType::depositeddose3d;
  types["depositedenergy"]    = BDSScorerType::depositedenergy;
  types["depostiedenergyblm"] = BDSScorerType::depositedenergyblm;
  types["depositedenergy3d"]  = BDSScorerType::depositedenergy3d;
  types["population"]         = BDSScorerType::population;
  types["population3d"]       = BDSScorerType::population3d;
  types["populationscaled"]   = BDSScorerType::populationscaled;
  types["cellflux"]           = BDSScorerType::cellflux;
  types["cellflux3d"]         = BDSScorerType::cellflux3d;
  types["cellfluxscaled"]     = BDSScorerType::cellfluxscaled;
  types["cellfluxscaled3d"]   = BDSScorerType::cellfluxscaled3d;
  types["cellfluxscaledperparticle"]   = BDSScorerType::cellfluxscaledperparticle;
  types["cellfluxscaledperparticle3d"] = BDSScorerType::cellfluxscaledperparticle3d;
  types["cellflux4d"] = BDSScorerType::cellflux4d;

  scorerType = BDS::LowerCase(scorerType);
  
  auto result = types.find(scorerType);
  if (result == types.end())
    {// it's not a valid key
      G4String msg = "\"" + scorerType + "\" is not a valid scorer type\n";
      msg += "Available scorer types are:\n";
      for (const auto& it : types)
        {msg += "\"" + it.first + "\"\n";}
      throw BDSException(__METHOD_NAME__, msg);
    }
  
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " determined scorer type to be " << result->second << G4endl;
#endif
  return result->second;
}
