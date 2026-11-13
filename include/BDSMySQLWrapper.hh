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
#ifndef BDSMYSQLWRAPPER_H
#define BDSMYSQLWRAPPER_H

#include "globals.hh"

#include <fstream>
#include <vector>
#include <string>

class BDSMySQLTable;

#define  _NEXT if(!NextToken()) return;
#define  _NEXTINPUT if(!NextInputToken()) return;
#define CMD_CREATE   "CREATE"
#define CMD_TABLE    "TABLE"
#define CMD_INSERT   "INSERT"
#define CMD_INTO     "INTO"
#define CMD_VALUES   "VALUES"
#define CMD_DROP     "DROP"
#define CMD_DATABASE "DATABASE"
#define CMD_USE      "USE"
#define CMD_IF       "IF"
#define CMD_EXISTS   "EXISTS"

/**
 * @brief Wrapper for mysql file handling.
 * 
 * @author John Carter
 */

class BDSMySQLWrapper 
{
public:
  explicit BDSMySQLWrapper (const G4String& SQLFileName);
  ~BDSMySQLWrapper();
  
  std::vector<BDSMySQLTable*> ConstructTable();

  G4int GetComponentN() {return ComponentN;}
 
  std::vector<BDSMySQLTable*> table;
  
private:
  BDSMySQLWrapper() = delete;
  // Data Members for Class Attributes

  std::ifstream ifs;
  
  G4String CurrentTableName;
  G4String InsertTableName;
  G4int ComponentN;

  G4int tableN;

  void ReadLine();
  void RemoveCommentsFromLine(std::string& value);
  void RemoveQuotesFromLine(std::string& value);
  void RemoveWhitespace(std::string&);
  void RemoveWhitespace(G4String&);
  void TokenizeLine();
  std::vector<std::string> _tokens;
  std::vector<std::string>::iterator _tokens_iter;

  void BeginTokens();
  bool NextToken();
  bool NextInputToken();
  bool EndTokens();
  std::string Token();
  bool EndOfLine();
  bool EmptyToken();
  void ProceedToEndOfLine();
  G4int ParseComponent();
  //Methods for carrying out the various instructions:
  void Create();
  void CreateDatabase();
  void CreateTable();
  void Insert();
  void InsertInto();
  void Values();

  std::string _currentLine;
  bool _startOfFile;
};

#endif
