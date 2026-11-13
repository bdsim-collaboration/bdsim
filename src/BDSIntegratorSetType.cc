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
#include "BDSIntegratorSetType.hh"
#include "BDSUtilities.hh"

#include "globals.hh"
#include "G4String.hh"

#include <map>
#include <string>

template<>
std::map<BDSIntegratorSetType, std::string>* BDSIntegratorSetType::dictionary =
  new std::map<BDSIntegratorSetType, std::string> ({
      {BDSIntegratorSetType::geant4,      "geant4"},
      {BDSIntegratorSetType::bdsimone,    "bdsimone"},
      {BDSIntegratorSetType::bdsimtwo,    "bdsimtwo"},
      {BDSIntegratorSetType::bdsimmatrix, "bdsimmatrix"},
      {BDSIntegratorSetType::bdsimmatrixfringescaling, "bdsimmatrixfringescaling"}
#if G4VERSION_NUMBER > 1039
      ,
      {BDSIntegratorSetType::geant4dp, "geant4dp"}
#endif
    });

BDSIntegratorSetType BDS::DetermineIntegratorSetType(G4String integratorSet)
{
  std::map<G4String, BDSIntegratorSetType> types;
  types["bdsim"]       = BDSIntegratorSetType::bdsimtwo; // alias for bdsim two the default
  types["geant4"]      = BDSIntegratorSetType::geant4;
  types["bdsimone"]    = BDSIntegratorSetType::bdsimone;
  types["bdsimtwo"]    = BDSIntegratorSetType::bdsimtwo;
  types["bdsimmatrix"] = BDSIntegratorSetType::bdsimmatrix;
  types["bdsimmatrixfringescaling"] = BDSIntegratorSetType::bdsimmatrixfringescaling;
#if G4VERSION_NUMBER > 1039
  types["geant4dp"]    = BDSIntegratorSetType::geant4dp;
#endif

  integratorSet = BDS::LowerCase(integratorSet);

  auto result = types.find(integratorSet);
  if (result == types.end())
    {// it's not a valid key
      G4String msg = "\"" + integratorSet + "\" is not a valid integrator set\n";
      msg += "Available sets are:\n";
      for (const auto& it : types)
	{msg += "\"" + it.first + "\"\n";}
      throw BDSException(__METHOD_NAME__, msg);
    }

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "determined integrator set to be " << result->second << G4endl;
#endif
  return result->second;
}
