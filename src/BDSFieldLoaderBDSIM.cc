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
#include "BDSArray1DCoords.hh"
#include "BDSArray2DCoords.hh"
#include "BDSArray3DCoords.hh"
#include "BDSArray4DCoords.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSFieldLoaderBDSIM.hh"
#include "BDSFieldValue.hh"
#include "BDSUtilities.hh"

#include "globals.hh"
#include "G4String.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include <algorithm>
#include <array>
#include <exception>
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
BDSFieldLoaderBDSIM<T>::BDSFieldLoaderBDSIM():
  nColumns(0),
  fv(BDSFieldValue()),
  result(nullptr),
  headerMustBePositiveKeys({"nx", "ny", "nz", "nt"}),
  indexOfFirstFieldValue(0)
{
  dimKeyMap = {
    {BDSDimensionType::x, {"nx", "xmin", "xmax"}},
    {BDSDimensionType::y, {"ny", "ymin", "ymax"}},
    {BDSDimensionType::z, {"nz", "zmin", "zmax"}},
    {BDSDimensionType::t, {"nt", "tmin", "tmax"}}
  };
  dimUnitsMap = {
    {BDSDimensionType::x, CLHEP::cm},
    {BDSDimensionType::y, CLHEP::cm},
    {BDSDimensionType::z, CLHEP::cm},
    {BDSDimensionType::t, CLHEP::s},
  };
}

template <class T>
BDSFieldLoaderBDSIM<T>::~BDSFieldLoaderBDSIM()
{;}

template <class T>
void BDSFieldLoaderBDSIM<T>::CleanUp()
{
  nColumns = 0;
  lineData.clear();
  fv = BDSFieldValue();
  std::vector<G4String> allKeys = {"nx", "ny", "nz", "nt",
                                   "xmin", "xmax", "ymin", "ymax",
                                   "zmin", "zmax", "tmin", "tmax"};
  for (auto& key : allKeys)
    {header[key] = 0;}
  for (auto& key : headerMustBePositiveKeys)
    {header[key] = 1;}
  result    = nullptr;
  loopOrder = "xyzt";
  indexOfFirstFieldValue = 0;
}

template <class T>
void BDSFieldLoaderBDSIM<T>::Terminate(const G4String& message)
{
  file.close();
  throw BDSException("BDSFieldLoaderBDSIM", message);
} 

template <class T>
BDSArray1DCoords* BDSFieldLoaderBDSIM<T>::Load1D(const G4String& fileName)
{
  Load(fileName,1);
  return static_cast<BDSArray1DCoords*>(result);
}

template <class T>
BDSArray2DCoords* BDSFieldLoaderBDSIM<T>::Load2D(const G4String& fileName)
{
  Load(fileName,2);
  return static_cast<BDSArray2DCoords*>(result);
}

template <class T>
BDSArray3DCoords* BDSFieldLoaderBDSIM<T>::Load3D(const G4String& fileName)
{
  Load(fileName,3);
  return static_cast<BDSArray3DCoords*>(result);
}

template <class T>
BDSArray4DCoords* BDSFieldLoaderBDSIM<T>::Load4D(const G4String& fileName)
{
  Load(fileName,4);
  return result;
}

template <class T>
void BDSFieldLoaderBDSIM<T>::Load(const G4String& fileName,
                                  const unsigned int nDim)
{
  G4String functionName = "BDSIM Field Format> ";
  CleanUp();
  
  file.open(fileName);
  long long unsigned int currentLineNumber = 0;
  // test if file is valid
#ifdef USE_GZSTREAM
  bool validFile = file.rdbuf()->is_open();
#else
  bool validFile = file.is_open();
#endif
  if (!validFile)
    {throw BDSException(__METHOD_NAME__, "Invalid file name or no such file named \"" + fileName + "\"");}
  else
    {G4cout << functionName << "Loading \"" << fileName << "\"" << G4endl;}

  // temporary variables
  unsigned long xIndex = 0;
  unsigned long yIndex = 0;
  unsigned long zIndex = 0;
  G4bool intoData     = false;
  std::string line;

  // Number of points in each dimension - not necessarily x,y,z,t.
  // For example, could be xz
  // These are always in xyzt order
  G4int n1 = 1, n2 = 1, n3 = 1, n4 = 1;
  
  // wrap in vectors for easy assignment
  G4String nominalOrder = "xyzt";
  
  float maximumFieldValue = 0;
  float minimumFieldValue = 0;
  // read top of the file
  while (!intoData)
    {
      std::getline(file, line);
      
      currentLineNumber++;
      // Skip a line if it's only whitespace
      if (std::all_of(line.begin(), line.end(), isspace))
        {continue;}

      if (currentLineNumber > 100) // !intoData is always true as we're inside this while loop
        {
          G4String msg = functionName+"did not encounter column specification line starting with '!' in the first 100 lines\n";
          msg += "This does not appear to be a BDSIM-format field map.";
          Terminate(msg);
        }
      
      // now several checks
      // 1) key > number
      // 2) key > word
      // 3) row starting '!' (column row ->then constructs data)

      // key definition
      // if (line.find(">") != std::string::npos)
      // NOTE the use of a regex expression here allows us to safely parse the first line
      // in a tar.gz file which normally has a load of bumf upfront for the tar file. Each
      // subsequent line comes out normally with the gzstream reader.
      std::smatch matchHeaderNumber;
      std::regex keyValue(R"((\w*)\s*>\s*([0-9eE.+-]+))");
      if (std::regex_search(line, matchHeaderNumber, keyValue))
        {// must be key definition
          if (matchHeaderNumber.size() < 2)
            {Terminate(functionName+"Invalid key definition in field format: " + line);}
          else
            {
              G4String key = G4String(matchHeaderNumber[1]);
              key = BDS::LowerCase(key);

              // check it's a valid key - header preloaded with valid keys
              if (header.find(key) == header.end())
                {Terminate(functionName+"Invalid key \"" + key +"\" in header");}
              
              G4double value = 0;
              try
                {value = std::stod(matchHeaderNumber[2]);}
              catch (const std::invalid_argument&)
                {Terminate(functionName+"Invalid argument " + std::string(matchHeaderNumber[2]));}
              catch (const std::out_of_range&)
                {Terminate(functionName+"Number out of range " + std::string(matchHeaderNumber[2]));}
              
              if (std::find(headerMustBePositiveKeys.begin(), headerMustBePositiveKeys.end(), key) != headerMustBePositiveKeys.end())
                {
                  if (value < 1)
                    {Terminate(functionName+"Number of points in dimension must be greater than 0 -> see \"" + key + "\"");}
                }
              
              header[key] = value;
              continue;
            }
        }

      std::smatch matchHeaderString;
      // matches "key > string" where string is 1-4 characters (not numbers)
      // can be padded between each part with whitespace \s*
      // not more than four characters (via \b for word boundary)
      std::regex keyWord(R"((\w+)\s*>\s*([a-zA-Z]{1,4})\b\s*)");
      if (std::regex_search(line, matchHeaderString, keyWord))
        {
          loopOrder = G4String(matchHeaderString[2]); // member variable
          
          G4bool test1 = loopOrder == "xyzt";
          G4bool test2 = loopOrder == "tzyx";
          if (! (test1 || test2) )
            {Terminate(functionName+"loopOrder> header variable is not one of (\"xyzt\" or \"tzyx\")");}
          continue;
        }
      
      // if starts with '!' - columns
      // check all required keys have been built up ok
      // set nColumns
      std::regex columnRow("^\\s*!"); // ignore any initial white space and look for '!'
      if (std::regex_search(line, columnRow))
        {
          // we only need to record the number of columns and which ones are
          // the x,y,z field component ones.
          std::regex afterExclamation("\\s*!\\s*(.+)");
          std::smatch match;
          std::regex_search(line, match, afterExclamation);
          std::string restOfLine = match[1];
          std::string columnName;
          std::istringstream restOfLineSS(restOfLine);
          std::vector<G4String> columnNames;
          while (restOfLineSS >> columnName)
            {
              nColumns++;
              if (columnName.find("Fx") != std::string::npos)
                {xIndex = nColumns; continue;}
              else if (columnName.find("Fy") != std::string::npos)
                {yIndex = nColumns; continue;}
              else if (columnName.find("Fz") != std::string::npos)
                {zIndex = nColumns; continue;}
              else
                {columnNames.emplace_back(columnName);}
            }
          lineData.resize(nColumns + 1); // +1 for default value
          indexOfFirstFieldValue = std::min({xIndex, yIndex, zIndex});
          
          for (const auto& key : headerMustBePositiveKeys)
            {
              if (header[key] < 1)
                {Terminate(functionName+"Number of points in dimension must be greater than 0 -> see \"" + key + "\"");}
            }
          
          if (nColumns < (nDim + 3)) // 3 for field components
            {Terminate(functionName+"Too few columns for " + std::to_string(nDim) + "D field loading");}
          
          // we have all the information now, so initialise the container
          switch (nDim)
            {
            case 1:
              {
                BDSDimensionType firstDim = BDS::DetermineDimensionType(columnNames[0]);
                auto keys = dimKeyMap[firstDim];
                double unit = dimUnitsMap[firstDim];
                n1 = G4int(header[keys.number]);
                result = new BDSArray1DCoords(n1,
                                              header[keys.min] * unit,
                                              header[keys.max] * unit,
                                              firstDim);
                break;
              }
            case 2:
              {
                BDSDimensionType firstDim  = BDS::DetermineDimensionType(columnNames[0]);
                BDSDimensionType secondDim = BDS::DetermineDimensionType(columnNames[1]);
                auto fKeys = dimKeyMap[firstDim];
                double fUnit = dimUnitsMap[firstDim];
                auto sKeys = dimKeyMap[secondDim];
                double sUnit = dimUnitsMap[secondDim];
                n1 = G4int(header[fKeys.number]);
                n2 = G4int(header[sKeys.number]);
                result = new BDSArray2DCoords(n1, n2,
                                              header[fKeys.min] * fUnit,
                                              header[fKeys.max] * fUnit,
                                              header[sKeys.min] * sUnit,
                                              header[sKeys.max] * sUnit,
                                              firstDim,
                                              secondDim);
                break;
              }
            case 3:
              {
                BDSDimensionType firstDim  = BDS::DetermineDimensionType(columnNames[0]);
                BDSDimensionType secondDim = BDS::DetermineDimensionType(columnNames[1]);
                BDSDimensionType thirdDim  = BDS::DetermineDimensionType(columnNames[2]);
                auto fKeys = dimKeyMap[firstDim];
                double fUnit = dimUnitsMap[firstDim];
                auto sKeys = dimKeyMap[secondDim];
                double sUnit = dimUnitsMap[secondDim];
                auto tKeys = dimKeyMap[thirdDim];
                double tUnit = dimUnitsMap[thirdDim];
                n1 = G4int(header[fKeys.number]);
                n2 = G4int(header[sKeys.number]);
                n3 = G4int(header[tKeys.number]);
                result = new BDSArray3DCoords(n1, n2, n3,
                                              header[fKeys.min] * fUnit,
                                              header[fKeys.max] * fUnit,
                                              header[sKeys.min] * sUnit,
                                              header[sKeys.max] * sUnit,
                                              header[tKeys.min] * tUnit,
                                              header[tKeys.max] * tUnit,
                                              firstDim,
                                              secondDim,
                                              thirdDim);
                break;
              }
            case 4:
              {
                n1 = G4int(header["nx"]);
                n2 = G4int(header["ny"]);
                n3 = G4int(header["nz"]);
                n4 = G4int(header["nt"]);
                result = new BDSArray4DCoords(n1, n2, n3, n4,
                                              header["xmin"] * CLHEP::cm, header["xmax"] * CLHEP::cm,
                                              header["ymin"] * CLHEP::cm, header["ymax"] * CLHEP::cm,
                                              header["zmin"] * CLHEP::cm, header["zmax"] * CLHEP::cm,
                                              header["tmin"] * CLHEP::s,  header["tmax"] * CLHEP::s);
                break;
              }
            default:
              {break;}
            }
          intoData = true;
          continue;
        }
    }
  
  // now only read data - two loops, one for each way of looping
  if (loopOrder == "tzyx")
    {
      for (G4int i = 0; i < n1; i++)
        {
          for (G4int j = 0; j < n2; j++)
            {
              for (G4int k = 0; k < n3; k++)
                {
                  for (G4int l = 0; l < n4; l++)
                    {
                      if (!std::getline(file, line))
                        {Terminate(functionName + "unexpected end to file on line number " + std::to_string(currentLineNumber));}
                      currentLineNumber++;
                      if (std::all_of(line.begin(), line.end(), isspace))
                        {continue;}
                      ProcessData(line, xIndex, yIndex, zIndex, currentLineNumber); // changes member fv
                      (*result)(i, j, k, l) = fv;
                      float mag = fv.mag();
                      maximumFieldValue = std::max(maximumFieldValue, mag);
                      minimumFieldValue = std::min(minimumFieldValue, mag);
                    }
                }
            }
        }
    }
  else
    {// xyzt
      for (G4int l = 0; l < n4; l++)
        {
          for (G4int k = 0; k < n3; k++)
            {
              for (G4int j = 0; j < n2; j++)
                {
                  for (G4int i = 0; i < n1; i++)
                    {
                      if (!std::getline(file, line))
                        {Terminate(functionName + "unexpected end to file on line number " + std::to_string(currentLineNumber));}
                      currentLineNumber++;
                      if (std::all_of(line.begin(), line.end(), isspace))
                        {continue;}
                      ProcessData(line, xIndex, yIndex, zIndex, currentLineNumber); // changes member fv
                      (*result)(i, j, k, l) = fv;
                      float mag = fv.mag();
                      maximumFieldValue = std::max(maximumFieldValue, mag);
                      minimumFieldValue = std::min(minimumFieldValue, mag);
                    }
                }
            }
        }
    }
  
  file.close();
  G4cout << functionName << "Loaded " << currentLineNumber << " lines from file" << G4endl;
  G4cout << functionName << "(Min | Max) field magnitudes in loaded file (before scaling): (" << minimumFieldValue << " | " << maximumFieldValue << ")" << G4endl;
}

template <class T>
void BDSFieldLoaderBDSIM<T>::ProcessData(const std::string& line,
                                         const unsigned long xIndex,
                                         const unsigned long yIndex,
                                         const unsigned long zIndex,
                                         unsigned long long currentLineNumber)
{
  std::istringstream liness(line);
  G4float value = 0;
  std::fill(lineData.begin(), lineData.end(), 0); // reset data - technically unnecessary
  
  // read all columns - indices shifted +1 for default value offset
  for (unsigned long i = 1; i < nColumns+1; ++i)
    {
      liness >> value;
      if (i < indexOfFirstFieldValue)// coordinates before this index
        {value *= CLHEP::cm;}
      if (!std::isfinite(value))
        {throw BDSException("BDSFieldLoaderBDSIM>", "a not-finite value was found on line " + std::to_string(currentLineNumber));}
      lineData[i] = value;
    }
  
  // Construct field value
  fv = BDSFieldValue(lineData[xIndex],
                     lineData[yIndex],
                     lineData[zIndex]);
}


template class BDSFieldLoaderBDSIM<std::ifstream>;

#ifdef USE_GZSTREAM
template class BDSFieldLoaderBDSIM<igzstream>;
#endif
