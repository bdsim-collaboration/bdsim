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
#include "BDSException.hh"
#include "BDSFieldLoaderQueryPoints.hh"
#include "BDSFourVector.hh"
#include "BDSUtilities.hh"

#include "globals.hh"
#include "G4String.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

#ifdef USE_GZSTREAM
#include "src-external/gzstream/gzstream.h"
#endif

template <class T>
BDSFieldLoaderQueryPoints<T>::BDSFieldLoaderQueryPoints()
{;}

template <class T>
BDSFieldLoaderQueryPoints<T>::~BDSFieldLoaderQueryPoints()
{;}

template <class T>
std::vector<BDSFourVector<G4double>> BDSFieldLoaderQueryPoints<T>::Load(const G4String& fileName,
                                                                        std::vector<G4String>* columnNamesIn) const
{
  G4String functionName = "BDSFieldLoaderQueryPoints::Load> ";
  T file;

  file.open(fileName);
  
  // test if file is valid
#ifdef USE_GZSTREAM
  bool validFile = file.rdbuf()->is_open();
#else
  bool validFile = file.is_open();
#endif

  if (!validFile)
    {throw BDSException(functionName, "Cannot open file \"" + fileName + "\"");}
  else
    {G4cout << functionName << "loading \"" << fileName << "\"" << G4endl;}

  std::string line;
  std::vector<BDSFourVector<G4double>> result;

  G4double x = 0;
  G4double y = 0;
  G4double z = 0;
  G4double t = 0;
  std::map<G4String, G4double*> columnNameToVariableAddress = { {"x",&x},
                                                                {"y",&y},
                                                                {"z",&z},
                                                                {"t",&t} };
  std::map<G4String, G4double> columnNameToUnits = { {"x", CLHEP::m},
                                                     {"y", CLHEP::m},
                                                     {"z", CLHEP::m},
                                                     {"t", CLHEP::ns} };
  std::vector<G4double*> values;
  std::vector<G4double> units;
  std::vector<G4String>  columnNamesToPrint;
  G4int nCoordinates = 0;
  G4bool intoData = false;
  G4int lineNo = 1;
  while (std::getline(file, line))
    {
      x = 0;
      y = 0;
      z = 0;
      t = 0;
      
      // skip a line if it's only whitespace
      if (std::all_of(line.begin(), line.end(), isspace))
        {
          lineNo += 1;
          continue;
        }

      if (!intoData)
	{
	  std::regex columnRow("^\\s*[!\\#]"); // ignore any initial white space and look for '!'
	  if (std::regex_search(line, columnRow))
	    {
	      std::regex afterExclamation("\\s*!\\s*(.+)");
	      std::smatch match;
	      std::regex_search(line, match, afterExclamation);
	      std::string restOfLine = match[1];

	      if (restOfLine.empty())
		{
		  G4String msg = "Error on line> " + std::to_string(lineNo);
		  msg += ": column specification missing variables";
		  throw BDSException(functionName, msg);
		}

	      std::vector<G4String> columnNames = BDS::SplitOnWhiteSpace(restOfLine);
	      for (const auto& columnName : columnNames)
		{
		  G4String variableName = BDS::LowerCase(columnName);
		  auto search = columnNameToVariableAddress.find(variableName);
		  if (search == columnNameToVariableAddress.end())
		    {
		      G4String msg = "Error on line> " + std::to_string(lineNo) + "> variable name \"";
		      msg += variableName + "\" is not one of 'x', 'y', 'z', 't'";
		      throw BDSException(functionName, msg);
		    }
		  values.push_back(search->second);
		  units.push_back(columnNameToUnits[search->first]);
		  if (std::find(columnNamesToPrint.begin(), columnNamesToPrint.end(), search->first) != columnNamesToPrint.end())
		    {throw BDSException(functionName, "duplicate column name");}
		  columnNamesToPrint.push_back(search->first);
		}
	      nCoordinates = (G4int)values.size();
	      G4cout << functionName << "assuming " << nCoordinates << " coordinate";
	      if (nCoordinates > 1)
		{G4cout << "s";}
	      G4cout << " in each  line: (";
	      for (const auto& coordName : columnNamesToPrint)
		{G4cout << " " << coordName;}
	      G4cout << " )" << G4endl;
	      if (columnNamesIn)
		{*columnNamesIn = columnNamesToPrint;}
	    }
	  intoData = true;
	  lineNo += 1;
	  continue;
	}

      std::vector<G4String> wordsInLine = BDS::SplitOnWhiteSpace(line);

      if ((G4int)wordsInLine.size() != nCoordinates)
	{throw BDSException(functionName, "invalid number of coordinates on line " + std::to_string(lineNo));}

      for (G4int i = 0; i < (G4int)values.size(); i++)
	{
	  G4double coord = 0;
	  try
	    {coord = std::stod(wordsInLine[i]);}
	  catch (std::exception& e)
	    {throw BDSException(functionName, "Error on line> " + std::to_string(lineNo) + "> cannot convert to number");}
	  (*values[i]) = coord * units[i];
	}
      
      result.emplace_back(BDSFourVector<G4double>(x,y,z,t));

      lineNo += 1;
    }

  file.close();

  G4cout << functionName << "loaded " << result.size() << " points" << G4endl;

  return result;
}

std::vector<BDSFourVector<G4double>> BDS::LoadFieldQueryPoints(const G4String& fileName,
							       std::vector<G4String>* columnNamesIn)
{
  G4String functionName = "BDS::LoadFieldQueryPoints>"; // namespaced functions don't print so well
  
  G4String fullFilePath = BDS::GetFullPath(fileName);
  if (!BDS::FileExists(fullFilePath))
    {throw BDSException(functionName, "no such file: \"" + fullFilePath + "\"");}

  std::vector<BDSFourVector<G4double>> result;
  if (fullFilePath.rfind("gz") != std::string::npos)
    {
#ifdef USE_GZSTREAM
      BDSFieldLoaderQueryPoints<igzstream> loader;
      result = loader.Load(fullFilePath, columnNamesIn);
#else
      throw BDSException(functionName, "Compressed file loading - but BDSIM not compiled with ZLIB.");
#endif
    }
  else
    {
      BDSFieldLoaderQueryPoints<std::ifstream> loader;
      result = loader.Load(fullFilePath, columnNamesIn);
    }
  return result;
}

template class BDSFieldLoaderQueryPoints<std::ifstream>;

#ifdef USE_GZSTREAM
template class BDSFieldLoaderQueryPoints<igzstream>;
#endif
