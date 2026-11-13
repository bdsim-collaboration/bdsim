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
#ifndef BDSMYSQLTABLE_H
#define BDSMYSQLTABLE_H

#include "globals.hh"
#include <vector>

class BDSMySQLVariable;

/**
 * @brief Representation of a mysql table.
 *
 * @author John Carter
 */

class BDSMySQLTable 
{
public:
  explicit BDSMySQLTable (G4String aTableName);
  ~BDSMySQLTable();

  void AddVariable(G4String aVarName, G4String aType);

  inline G4String GetName()       const {return itsTableName;}
  inline G4int    GetNVariables() const {return itsNVariables;}

  BDSMySQLVariable* GetVariable(G4String aVarName);
  BDSMySQLVariable* GetVariable(G4int aVarN);

  void Print();
  
private:
  G4int itsNVariables;
  G4String itsTableName;
  std::vector<BDSMySQLVariable*> itsVar;

  //A list of valid variable types (STRING, DOUBLE, INTEGER...)
  void SetValidVarTypes();
  std::vector<G4String> _validVarTypes;
  //To check is a var type is valid...
  G4bool isValidVarType(G4String);
};

#endif
