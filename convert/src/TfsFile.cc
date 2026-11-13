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

#include "TfsFile.hh"

#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <regex>
#include <string>
#include <sstream>
#include <vector>


PTC::TfsFile::TfsFile(const std::string& fileNameIn):
  fileName(fileNameIn),
  currentSegmentNumber(0),
  observationCounter(1),
  nObservationsThisSegment(0)
{;}

void PTC::TfsFile::Load()
{
  std::ifstream f(fileName.c_str());
  if (!f)
    {throw std::string("Could not open file");}

  // count lines in file by looping over it
  int n = 0;
  std::string s;
  while(getline(f, s))
    {n++;}

  // reset file to beginning
  f.clear();
  f.seekg(0, std::ios::beg);

  // calculate print out rate
  int perLinePrintOut = std::ceil(0.02*n); // 2% print out

  // temporary variables and regexes
  std::string line;
  std::regex headerrx("^\\@.*");
  std::regex columnTypesrx("\\$.*");
  std::regex columnsrx("\\*.*");

  bool intoData = false;
  int i = 0;
  while (std::getline(f, line))
    {
      if (i % perLinePrintOut == 0)
	{
	  std::cout << "\r Reading line # " << std::setw(6) << i;
	  std::cout.flush();
	}
      i++;
      if (std::all_of(line.begin(), line.end(), isspace))
        {continue;} // skip empty lines
      else if (intoData) // next option (for efficiency) into data only two choices
	{
	  // this will start first time through in data as observationCounter always
	  // initialise to 1, which is > 0=nObservationsThisSegment so far
	  if (observationCounter > nObservationsThisSegment)
	    {ParseSegment(line);} // we must be on to a new segment - parse its definition line
	  else
	    {
	      ParseData(line);
	      observationCounter++;
	    }
	}
      else
	{
	  if (std::regex_search(line, headerrx))
	    {ParseHeaderLine(line);}
	  else if (std::regex_search(line, columnTypesrx))
	    {
	      intoData = true;
	      continue; // skip column type line
	    }
	  else if (std::regex_search(line, columnsrx))
	    {ParseColumns(line);}
	}
    }
  std::cout << std::endl;
  f.close();
}

void PTC::TfsFile::ParseHeaderLine(const std::string& line)
{
  std::vector<std::string> words = BreakOnWhiteSpace(line);
  header[words[1]] = words.back();
}

void PTC::TfsFile::ParseColumns(const std::string& line)
{
  std::vector<std::string> words = BreakOnWhiteSpace(line);
  columnNames.resize(words.size()-1);
  std::copy(words.begin()+1, words.end(), columnNames.begin());
}  

void PTC::TfsFile::ParseSegment(const std::string& line)
{
  std::vector<std::string> words = BreakOnWhiteSpace(line);
  currentSegmentNumber = std::stoi(words[1]);
  nObservationsThisSegment = std::stoi(words[3]);
  observationCounter = 1;

  PTC::segment nextSegment;
  nextSegment.observations.reserve(nObservationsThisSegment);
  nextSegment.name = words.back();
  segments.push_back(nextSegment);
}

void PTC::TfsFile::ParseData(const std::string& line)
{
  std::vector<std::string> words = BreakOnWhiteSpace(line);

  std::vector<double> numbers;
  double num = 0;
  std::stringstream ss(line);
  while (ss >> num)
  {numbers.push_back(num);}
  //for (auto const& word : words)
  //  {numbers.push_back(std::stod(word));}

  PTC::observation obs = {static_cast<int>(numbers[0]), // observation index - particle number
			  numbers[2], // x
			  numbers[3], // px
			  numbers[4], // y
			  numbers[5], // py
			  numbers[6], // T
			  numbers[7], // pt
			  numbers[8], // S
			  numbers[9]};// E
  segments.back().observations.emplace_back(obs);
}

std::vector<std::string> PTC::TfsFile::BreakOnWhiteSpace(const std::string& line) const
{
  std::vector<std::string> results;
  std::regex wspace("\\s+"); // any whitepsace
  // -1 here makes it point to the suffix, ie the word rather than the wspace
  std::sregex_token_iterator iter(line.begin(), line.end(), wspace, -1);
  std::sregex_token_iterator endit;
  for (; iter != endit; ++iter)
    {
      std::string res = (*iter).str();
      results.push_back(res);
    }
  return results;
}

std::string PTC::TfsFile::HeaderValue(const std::string& key) const
{
  auto result = header.find(key);
  if (result != header.end())
    {return result->second;}
  else
    {throw std::string("No such segment name");}
}

PTC::segment PTC::TfsFile::Segment(const int& index) const
{
  if (index > (int)segments.size() || index < 0)
    {throw std::string("Index > number of segments");}
  return segments.at(index);
}
