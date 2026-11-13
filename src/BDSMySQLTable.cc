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
#include "BDSMySQLTable.hh"
#include "BDSMySQLVariable.hh"

#include "G4String.hh"
#include "G4Version.hh"

#include <cstdlib>
#include <string>
#include <vector>

BDSMySQLTable::BDSMySQLTable (G4String aTableName)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " - creating new table named: " << aTableName << G4endl;
#endif
  SetValidVarTypes();
  itsTableName = aTableName;
  itsNVariables = 0;
}

void BDSMySQLTable::AddVariable(G4String aName, G4String aType)
{
  if(aName.empty())
    {return;}
    
  if(!isValidVarType(aType))
    {return;}
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " - adding variable: " << aName << " " << aType << " to table " << GetName() << G4endl;
#endif
  itsVar.push_back(new BDSMySQLVariable(aName,aType));
  itsNVariables++;
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " - nVariables = " << GetNVariables() << G4endl;
#endif
}

void BDSMySQLTable::Print()
{
  G4cout << GetName() << G4endl;
  for(int i=0; i<itsNVariables; i++)
    {GetVariable(i)->Print();}
}

BDSMySQLVariable* BDSMySQLTable::GetVariable(G4String aVarName)
{
  for (G4int i=0; i<(G4int)itsVar.size(); i++)
    {
#if G4VERSION_NUMBER > 1099
      if (G4StrUtil::icompare(itsVar[i]->GetName(), aVarName) == 0)
#else
      G4String::caseCompare cmpmode = G4String::ignoreCase;
      if( (itsVar[i])->GetName().compareTo(aVarName,cmpmode)==0)
#endif
	{return itsVar[i];}
    }
  return nullptr; //if does not exist return null
}

BDSMySQLVariable* BDSMySQLTable::GetVariable(G4int aVarN)
{
  return itsVar[aVarN];
}

void BDSMySQLTable::SetValidVarTypes(){
  _validVarTypes.push_back((G4String)"DOUBLE");
  _validVarTypes.push_back((G4String)"INTEGER");
  _validVarTypes.push_back((G4String)"STRING");

}

G4bool BDSMySQLTable::isValidVarType(G4String val){
  G4bool result = false;
  for(std::vector<G4String>::const_iterator iter = _validVarTypes.begin(); iter != _validVarTypes.end(); ++iter){
    if((*iter) == val) result = true;
  }
  return result;
}

BDSMySQLTable::~BDSMySQLTable()
{
}


void BDSMySQLVariable::Print(){
  G4cout << __METHOD_NAME__ << G4endl;
  std::string varType=GetVarType();
  G4cout << GetName() << " " << GetVarType() << " " << G4endl;
  for(int i=0; i<GetNVariables(); i++){
    if(varType=="DOUBLE") G4cout << GetDblValue(i) << G4endl;
    if(varType=="INTEGER")G4cout << GetIntValue(i) << G4endl;
    if(varType=="STRING")G4cout << GetStrValue(i) << G4endl;
  }
}

