/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2024.

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
#include "BDSModulatorType.hh"
#include "BDSUtilities.hh"

#include "G4String.hh"

#include <map>
#include <string>

// dictionary for BDSModulatorType for reflexivity
template<>
std::map<BDSModulatorType, std::string>* BDSModulatorType::dictionary =
  new std::map<BDSModulatorType, std::string> ({
                                                {BDSModulatorType::lineart,           "lineart"},
                                                {BDSModulatorType::none,              "none"},
                                                {BDSModulatorType::sint,              "sint"},
                                                {BDSModulatorType::singlobalt,        "singlobalt"},
                                                {BDSModulatorType::tophatt,           "tophatt"}
    });         

BDSModulatorType BDS::DetermineModulatorType(G4String mType)
{
  std::map<G4String, BDSModulatorType> types;
  types["lineart"]          = BDSModulatorType::lineart;
  types["none"]             = BDSModulatorType::none;
  types["sint"]             = BDSModulatorType::sint;
  types["singlobalt"]       = BDSModulatorType::singlobalt;
  types["tophatt"]          = BDSModulatorType::tophatt;
  
  mType = BDS::LowerCase(mType);

  auto result = types.find(mType);
  if (result == types.end())
    {// it's not a valid key
      G4String msg = "\"" + mType + "\" is not a valid modulator type\n";
      msg += "Available modulator types are:\n";
      for (const auto& it : types)
        {msg += "\"" + it.first + "\"\n";}
      throw BDSException(__METHOD_NAME__, msg);
    }

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "determined modulator type to be " << result->second << G4endl;
#endif
  return result->second;
}
