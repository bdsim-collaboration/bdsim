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
#include "SelectionLoader.hh"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <regex>
#include <stdexcept>
#include <string>
#include <vector>

std::string RBDS::LoadSelection(const std::string& selectionFile)
{
  std::ifstream f(selectionFile.c_str());
  if(!f)
    {throw std::invalid_argument("LoadSelection> could not open file " + selectionFile);}

  int lineCounter = 0;
  std::string line;

  // unique patterns to match
  // match a line starting with #
  std::regex comment("^\\#.*");

  std::string selection;  
  while(std::getline(f, line))
    {
      lineCounter++;
      if (std::all_of(line.begin(), line.end(), isspace))
	{continue;} // skip empty lines
      else if (std::regex_search(line, comment))
	{continue;} // skip lines starting with '#'
      else
	
      {
        if (!selection.empty())
	  {throw std::invalid_argument("Multiple selections (non-empty lines) found in file - only one should be specified.");}
	  std::vector<std::string> results;
	  std::regex wspace("\\s+"); // any whitespace
	  // -1 here makes it point to the suffix, ie the word rather than the wspace
	  std::sregex_token_iterator iter(line.begin(), line.end(), wspace, -1);
	  std::sregex_token_iterator end;
	  for (; iter != end; ++iter)
	    {
	      std::string res = (*iter).str();
	      if (res.empty())
		{continue;}
	      results.push_back(res);
	    }
	  if (results.size() != 1)
	    {
	      std::cerr << "Error on line " << lineCounter << "\"" << line << "\"" << std::endl;
	      throw std::invalid_argument("More than one word on line -> no white space allowed");
	    }
	  else
	    {selection = results[0];}
	}
    }
  
  f.close();

  return selection;
}
