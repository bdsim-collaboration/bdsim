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
#include "BDSEventGeneratorFileType.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSUtilities.hh"

#include "globals.hh"
#include "G4String.hh"

#include <map>
#include <string>

// dictionary for BDSEventGeneratorFileType
template<>
std::map<BDSEventGeneratorFileType, std::string>* BDSEventGeneratorFileType::dictionary =
  new std::map<BDSEventGeneratorFileType, std::string> ({
      {BDSEventGeneratorFileType::hepmc2,       "hepmc2"},
      {BDSEventGeneratorFileType::hepmc3,       "hepmc3"},
      {BDSEventGeneratorFileType::hpe,          "hpe"},
      {BDSEventGeneratorFileType::root,         "root"},
      {BDSEventGeneratorFileType::treeroot,     "treeroot"},
      {BDSEventGeneratorFileType::lhef,         "lhef"},
});

BDSEventGeneratorFileType BDS::DetermineEventGeneratorFileType(G4String distrType)
{
  std::map<G4String, BDSEventGeneratorFileType> types;

  types["hepmc2"]       = BDSEventGeneratorFileType::hepmc2;
  types["hepmc3"]       = BDSEventGeneratorFileType::hepmc3;
  types["hpe"]          = BDSEventGeneratorFileType::hpe;
  types["root"]         = BDSEventGeneratorFileType::root;
  types["treeroot"]     = BDSEventGeneratorFileType::treeroot;
  types["lhef"]         = BDSEventGeneratorFileType::lhef;

  distrType = BDS::LowerCase(distrType);

  auto result = types.find(distrType);
  if (result == types.end())
    {// it's not a valid key
      G4String msg = "\"" + distrType + "\" is not a valid event generator file format\n";
      msg += "Available formats are:\n";
      for (const auto& it : types)
	{msg += "\"" + it.first + "\"\n";}
      throw BDSException(__METHOD_NAME__, msg);
    }
  
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "determined event generator file format to be " << result->second  << G4endl;
#endif
  return result->second;
}
