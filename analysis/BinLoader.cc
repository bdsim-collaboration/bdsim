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
#include "BinLoader.hh"
#include "RBDSException.hh"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

std::vector<double>* RBDS::LoadBins(const std::string& fileName)
{
  std::ifstream file;
  file.open(fileName);
  bool validFile = file.is_open();

  if (!validFile)
    {throw RBDSException("Cannot open file \"" + fileName + "\"");}
  else
    {std::cout << "LoadBins> loading \"" << fileName << "\"" << std::endl;}

  std::string line;
  std::vector<double>* result = new std::vector<double>();
  int lineNum = 1;
  while (std::getline(file, line))
    { // read a line only if it's not a blank one
      std::istringstream liness(line);

      // skip a line if it's only whitespace
      if (std::all_of(line.begin(), line.end(), isspace))
        {continue;}

      double binEdge;
      if (!(liness >> binEdge))
        {throw RBDSException("invalid bin edge on line " + std::to_string(lineNum) + "\n\"" + line + "\"\nCannot convert to a double.");}
      result->push_back(binEdge);
  
      if (!liness.eof())
	{
	  std::string remainder;
	  liness >> remainder;
	  std::string message = "Error: extra text \"" + remainder + "\" on line " + std::to_string(lineNum) + " of bin edges file \"" + fileName + "\"";
	  throw RBDSException(message);
	}
      
      lineNum += 1;
    }
  
  if (result->size() < 2)
    {throw RBDSException("insufficient number of bins in file - must be at least 2");}
  
  file.close();
  return result;
}
