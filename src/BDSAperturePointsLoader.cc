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
#include "BDSAperturePointsLoader.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSUtilities.hh"

#include "G4String.hh"
#include "G4TwoVector.hh"
#include "G4Types.hh"
#include "G4UnitsTable.hh"
#include "G4Version.hh"

#include <algorithm>
#include <array>
#include <exception>
#include <fstream>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

#ifdef USE_GZSTREAM
#include "src-external/gzstream/gzstream.h"
#endif

BDSAperturePointsCache* BDSAperturePointsCache::instance = nullptr;

template <class T>
BDSAperturePointsLoader<T>::BDSAperturePointsLoader()
{;}

template <class T>
BDSAperturePointsLoader<T>::~BDSAperturePointsLoader()
{;}

template <class T>
std::vector<G4TwoVector>* BDSAperturePointsLoader<T>::Load(const G4String& fileName,
							   G4double unit) const
{
  G4String functionName = "BDSAperturePointsLoader::Load> ";
  T file;
  
  if (fileName.empty())
    {throw BDSException(functionName, "empty file name given");}

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
  auto result = new std::vector<G4TwoVector>();
  std::array<G4double, 2> values({0,0});
  G4int lineNo = 1;
  std::regex commentLine("^\\s*[\\#!]");
  while (std::getline(file, line))
    {
      // skip a line if it's only whitespace
      if (std::all_of(line.begin(), line.end(), isspace))
        {
          lineNo += 1;
          continue;
        }
      // or a comment
      if (std::regex_search(line, commentLine))
        {
          lineNo += 1;
          continue;
        }

      std::vector<G4String> wordsInLine = BDS::SplitOnWhiteSpace(line);

      if ((G4int)wordsInLine.size() != 2)
	{throw BDSException(functionName, "invalid number of coordinates on line " + std::to_string(lineNo));}

      for (G4int i = 0; i < (G4int)values.size(); i++)
	{
	  G4double coord = 0;
	  try
	    {coord = std::stod(wordsInLine[i]);}
	  catch (std::exception& e)
	    {throw BDSException(functionName, "Error on line> " + std::to_string(lineNo) + "> cannot convert to number");}
	  (values[i]) = coord * unit;
	}
      
      result->emplace_back(G4TwoVector(values[0], values[1]));

      lineNo += 1;
    }

  file.close();

  G4cout << functionName << "loaded " << result->size() << " points" << G4endl;
  
  if (result->size() < 3)
    {throw BDSException(functionName, "require at least 3 points to make an enclosed volume");}

  return result;
}

std::vector<G4TwoVector>* BDS::LoadAperturePoints(const G4String& fileName,
                                                  const G4String& unit)
{
  G4String functionName = "BDS::LoadAperturePoints>"; // namespaced functions don't print so well
  
  G4double unitAsNumber = 1.0;
  if (!unit.empty())
    {
#if G4VERSION_NUMBER > 1029
      if (!G4UnitDefinition::IsUnitDefined(unit))
        {throw BDSException(functionName, "no such unit \"" + unit + "\"");}
      else
        {unitAsNumber = G4UnitDefinition::GetValueOf(unit);}
#else
      G4String unitLower = BDS::LowerCase(unit);
      if (unitLower == "mm")
        {unitAsNumber = 1.0;}
      else if (unitLower == "cm")
        {unitAsNumber = 10;}
      else if (unitLower == "m")
        {unitAsNumber = 1000;}
      else
        {throw BDSException(functionName, "Unknown unit \"" + unit + "\" (for this version of Geant4/BDSIM) - use one of (mm, cm, m)");}
#endif
    }
    
  auto cachedResult = BDSAperturePointsCache::Instance()->FindCachedFile(fileName+unit); // can return nullptr
  if (cachedResult)
    {return cachedResult;}
  
  G4String fullFilePath = BDS::GetFullPath(fileName);
  if (!BDS::FileExists(fullFilePath))
    {throw BDSException(functionName, "no such file: \"" + fullFilePath + "\"");}

  std::vector<G4TwoVector>* result;
  if (fullFilePath.rfind("gz") != std::string::npos)
    {
#ifdef USE_GZSTREAM
      BDSAperturePointsLoader<igzstream> loader;
      result = loader.Load(fullFilePath, unitAsNumber);
#else
      throw BDSException(functionName, "Compressed file loading - but BDSIM not compiled with ZLIB.");
#endif
    }
  else
    {
      BDSAperturePointsLoader<std::ifstream> loader;
      result = loader.Load(fullFilePath, unitAsNumber);
    }
  
  BDSAperturePointsCache::Instance()->CacheFile(fileName+unit, result);
  
  return result;
}


BDSAperturePointsCache* BDSAperturePointsCache::Instance()
{
  if (!instance)
    {instance = new BDSAperturePointsCache();}
  return instance;
}

BDSAperturePointsCache::~BDSAperturePointsCache()
{
  ClearCachedFiles();
}

void BDSAperturePointsCache::ClearCachedFiles()
{
  for (auto& v : cachedFiles)
    {delete v.second;}
  cachedFiles.clear();
}

std::vector<G4TwoVector>* BDSAperturePointsCache::FindCachedFile(const G4String& fileNameAndUnits) const
{
  std::vector<G4TwoVector>* result = nullptr;
  auto search = cachedFiles.find(fileNameAndUnits);
  if (search != cachedFiles.end())
    {return search->second;}
  else
    {return result;}
}

void BDSAperturePointsCache::CacheFile(const G4String& fileNameAndUnits,
                                       std::vector<G4TwoVector>* contents)
{
  auto search = cachedFiles.find(fileNameAndUnits);
  if (search != cachedFiles.end())
    {
      if (contents != search->second)
        {throw BDSException(__METHOD_NAME__, "overwriting cache of aperture points with different contents for file name: "+fileNameAndUnits);}
      return;
    }
  else
    {cachedFiles[fileNameAndUnits] = contents;}
}


template class BDSAperturePointsLoader<std::ifstream>;

#ifdef USE_GZSTREAM
template class BDSAperturePointsLoader<igzstream>;
#endif
