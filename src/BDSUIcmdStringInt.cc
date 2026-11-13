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
#include "BDSUIcmdStringInt.hh"

#include "G4UImessenger.hh"
#include "G4UIparameter.hh"

BDSUIcmdStringInt::BDSUIcmdStringInt(const char* theCommandPath,
				     G4UImessenger* theMessenger):
  G4UIcommand(theCommandPath, theMessenger)
{
  G4UIparameter* strParam = new G4UIparameter('s');
  SetParameter(strParam);
  G4UIparameter* intParam = new G4UIparameter('i');
  SetParameter(intParam);
  intParam->SetOmittable(true);
  SetParameterName("element_name", "instance_number", false);
}

void BDSUIcmdStringInt::SetParameterName(const char* theName0,
                                         const char* theName1,
                                         G4bool omittable,
                                         G4bool currentAsDefault)
{
  G4UIparameter* theParam0 = GetParameter(0);
  theParam0->SetParameterName(theName0);
  theParam0->SetOmittable(omittable);
  theParam0->SetCurrentAsDefault(currentAsDefault);
  G4UIparameter* theParam1 = GetParameter(1);
  theParam1->SetParameterName(theName1);
}

void BDSUIcmdStringInt::SetCandidates(const char* candidateList)
{
  G4UIparameter* theParam = GetParameter(0);
  G4String canList        = candidateList;
  theParam->SetParameterCandidates(canList);
}

void BDSUIcmdStringInt::SetDefaultValue(const char* defVal)
{
  G4UIparameter* theParam = GetParameter(0);
  theParam->SetDefaultValue(defVal);
}
