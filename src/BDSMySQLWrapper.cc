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
#include "BDSMySQLWrapper.hh"
#include "BDSMySQLTable.hh"
#include "BDSMySQLVariable.hh"
#include "BDSUtilities.hh"

#include "globals.hh"
#include "G4String.hh"
#include "G4Version.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include <cstdlib>
#include <iostream>
#include <regex>
#include <string>
#include <vector>


BDSMySQLWrapper::BDSMySQLWrapper(const G4String& SQLFileName):
  ifs(SQLFileName.c_str()),
  ComponentN(0),
  tableN(-1)
{
  _startOfFile=true;
  BeginTokens();

  if(ifs)
    {
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "loading SQL Filename: " << SQLFileName << G4endl;
#endif
    }
  else
    {throw BDSException(__METHOD_NAME__, "Unable to load SQL file: " + SQLFileName);}
}

BDSMySQLWrapper::~BDSMySQLWrapper()
{;}

std::vector<BDSMySQLTable*> BDSMySQLWrapper::ConstructTable()
{
  ComponentN=0;
  tableN=-1;
  while(NextToken())
    {ParseComponent();}
  return table;
}

void BDSMySQLWrapper::TokenizeLine()
{
  // empty old tokens
  _tokens.clear();

  // Separate with comma and space, separate and keep with ; ( ) and \n
  // Done in 2 steps, once for dropped delimiters and then for the kept delimiters
  // Delimiters are spaces, tabs (changed from earlier versions) and/or commas, but no newlines
  std::regex drop_delimiters("[ \t,]+");

  std::string &s = _currentLine;
  // Specifying -1 as the fourth argument makes the function skip over any patterns matching
  // the regex_obj, causing the iterator to iterate through the tokens—which consist of text
  // between each occurrence of the pattern.
  std::sregex_token_iterator tok_iter(s.begin(), s.end(), drop_delimiters, -1);
  
  // default constructor = end-of-sequence:
  std::sregex_token_iterator rend;
  for (; tok_iter!=rend; ++tok_iter) {
    std::string subtoken = *tok_iter;
    //    G4cout << __METHOD_NAME__ << " - subtoken: = <" << subtoken << ">" << G4endl;

    // keep ; ( ) 
    std::regex keep_delimiters("[;\\(\\)]");
    // delimiters (0) and remaining (-1)
    std::sregex_token_iterator tok_iter2(subtoken.begin(), subtoken.end(), keep_delimiters, {-1, 0});

    for (; tok_iter2!=rend; ++tok_iter2) { 
    
      std::string token = *tok_iter2;
      
      // only put non-empty tokens, but put empty strings ""
      if (token.empty())
	{continue;}
      RemoveQuotesFromLine(token);
      RemoveWhitespace(token);
      _tokens.push_back(token);
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "- token: = <" << token << ">" << G4endl;
#endif
    }
  }
  BeginTokens();
}

bool BDSMySQLWrapper::NextToken()
{
  ++_tokens_iter;
  if(_startOfFile)
    {--_tokens_iter;}
  if(_startOfFile || EndTokens()){
    if(ifs.good()){
      ReadLine();
      TokenizeLine();
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "- Token() = " << Token() << G4endl;
#endif
    }else{
      return false;
    }
  } else {
#ifdef BDSDEBUG
    G4cout << __METHOD_NAME__ << "- Token() = " << Token() << G4endl;
#endif
  }
  return true;
}

G4int BDSMySQLWrapper::ParseComponent()
{
  // not using these commands yet...
  if(Token()==CMD_DROP) { return 0;}
  else if(Token()==CMD_DATABASE) { return 0;}
  else if(Token()==CMD_IF) { return 0;}
  else if(Token()==CMD_EXISTS) { return 0;}
  else if(Token()==CMD_USE) { return 0;}
  //The following commands are used...
  else if(Token()==CMD_CREATE) {Create(); return 0;}
  else if(Token()==CMD_INSERT) {Insert(); ComponentN++; return 0;}
  return 0;
}

void BDSMySQLWrapper::CreateDatabase()
{;}

void BDSMySQLWrapper::CreateTable()
{
  G4String varname;
  G4String vartype;

  _NEXTINPUT
    CurrentTableName=Token();
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " reading CurrentTableName: " << CurrentTableName << G4endl;
#endif
  ProceedToEndOfLine();
  table.push_back(new BDSMySQLTable(CurrentTableName));
  tableN++;
#if G4VERSION_NUMBER > 1099
  while(!G4StrUtil::contains(varname,";") && (!G4StrUtil::contains(vartype,";")))
#else
  while((!varname.contains(";")) && (!vartype.contains(";")))
#endif
  {
    //Get next variable, skipping blanks.
    do{
      _NEXTINPUT
	} while((EmptyToken()) || (EndOfLine()));
    varname=Token();
#ifdef BDSDEBUG
    G4cout << __METHOD_NAME__ << " reading varname: " << varname << G4endl;
#endif
    if (BDS::StrContains(varname, ";")) {return;}
    _NEXTINPUT
      vartype=Token();
#ifdef BDSDEBUG
    G4cout << __METHOD_NAME__ << " reading vartype: " << vartype << G4endl;
#endif
    if (BDS::StrContains(vartype,";")) return; //Semicolon indicates end of table.
    if(BDS::StrContains(vartype, "DOUBLE")) vartype="DOUBLE";
    else if(BDS::StrContains(vartype, "VARCHAR")) vartype="STRING";
    else if(BDS::StrContains(vartype, "INTEGER")) vartype="INTEGER";
    table[tableN]->AddVariable(varname,vartype);
    ProceedToEndOfLine();
  }
}

void BDSMySQLWrapper::Create()
{
  _NEXT
#ifdef BDSDEBUG
    G4cout << __METHOD_NAME__ << " reading input: " << Token() << G4endl;
#endif
  if(Token()==CMD_DATABASE) {CreateDatabase();}
  else if(Token()==CMD_TABLE) {CreateTable();}
}

void BDSMySQLWrapper::Insert(){
  _NEXT
#ifdef BDSDEBUG
    G4cout << __METHOD_NAME__ << " reading input: " << Token() << G4endl;
#endif
  if(Token()==CMD_INTO){InsertInto();}
}

void BDSMySQLWrapper::InsertInto() {
  _NEXT
    InsertTableName=Token();
  _NEXT
    if(Token()==CMD_VALUES){Values();}
}
 
void BDSMySQLWrapper::Values() {
  for(G4int j=0; j<(G4int)table.size(); j++){
    if(table[j]->GetName()==InsertTableName){
      for(G4int k=0; k<table[j]->GetNVariables(); k++){
	_NEXTINPUT
	  //Skip first bracket...
	while (Token()=="(") {
	  _NEXTINPUT
	}
	if (Token()==")") {
	  //	  std::stringstream excptnSstrm;
	  //	  excptnSstrm<<__METHOD_NAME__<< " - expected " << table[j]->GetNVariables() << " values for insertion into table " << InsertTableName << "\n";
	  //	  G4Exception(excptnSstrm.str().c_str(), "-1", FatalException, "");
	  //	  return;
	}
#ifdef BDSDEBUG
	G4cout << __METHOD_NAME__ << " inserting value " << Token() << G4endl;
#endif
	if(table[j]->GetVariable(k)->GetVarType()=="DOUBLE")
	  table[j]->GetVariable(k)->AddValue(std::atoi(Token().c_str())*CLHEP::mm);
	else if(table[j]->GetVariable(k)->GetVarType()=="STRING")
	  table[j]->GetVariable(k)->AddValue(Token());
	else if(table[j]->GetVariable(k)->GetVarType()=="INTEGER")
	  table[j]->GetVariable(k)->AddValue(std::atoi(Token().c_str()));
      }
    }
  }
}

void BDSMySQLWrapper::BeginTokens(){
  _tokens_iter=_tokens.begin();
}

bool BDSMySQLWrapper::NextInputToken(){
  if(ifs.good()){
    bool status;
    do{
      status=NextToken();
    } while (EndOfLine());
    return status;
  } else {
    return false;
  }
}

bool BDSMySQLWrapper::EndTokens()
{
  return(_tokens_iter==_tokens.end());
}

bool BDSMySQLWrapper::EndOfLine()
{
  return((*_tokens_iter)=="\n");
}

bool BDSMySQLWrapper::EmptyToken()
{
  return((*_tokens_iter)=="");
}

std::string BDSMySQLWrapper::Token()
{
  return *_tokens_iter;
}

void BDSMySQLWrapper::ProceedToEndOfLine()
{
  while(!EndOfLine()){
    _NEXT
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << " " << Token() << G4endl;
#endif
  }
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " finished." << G4endl;
#endif
}

void BDSMySQLWrapper::ReadLine(){
  _currentLine.clear();
  _startOfFile=false;
  //returns non zero if unsuccessful
  char buffer[512];
  ifs.getline(buffer,512);
  _currentLine=buffer;
  RemoveCommentsFromLine(_currentLine);
  //Put back the new line character
  _currentLine += ' ';
  _currentLine += '\n';
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " - _currentLine = " << _currentLine << G4endl;
#endif
}

void BDSMySQLWrapper::RemoveCommentsFromLine(std::string& value){
  //Strip all characters after and including '#'
  unsigned pos = value.find('#');
  std::string uncommented = value.substr(0,pos);
  value=uncommented;
}

void BDSMySQLWrapper::RemoveQuotesFromLine(std::string& value){
  //Strip all "
  value.erase (std::remove(value.begin(), value.end(), '\"'), value.end());
}


void BDSMySQLWrapper::RemoveWhitespace(G4String& val){
  std::string s = val;
  RemoveWhitespace(s);
  val=(G4String)s;
}

void BDSMySQLWrapper::RemoveWhitespace(std::string& val){
  val.erase( std::remove_if( val.begin(), val.end(), ::isblank ), val.end() );
}

/*
void Log(const G4String& tag, int depth, ostream& os)
{
  static const char* tab = "----|";
  while(depth--) os<<tab;
  os<<' '<<tag<<endl;
}
*/




 
