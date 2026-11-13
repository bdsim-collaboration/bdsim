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
#ifndef COMPRESULTEVENTTREE_H
#define COMPRESULTEVENTTREE_H

#include "Result.hh"
#include "ResultEvent.hh"

#include <sstream>
#include <string>

/**
 * @brief Result of comparison of all entries in an Event tree in BDSIM output.
 *
 * @author Laurie Nevay
 */

class ResultEventTree: public Result
{ 
public:
  int t1NEntries;
  int t2NEntries;
  std::vector<int> offendingEvents;
  std::vector<ResultEvent> eventResults;

  virtual std::string print() const
  {
    std::stringstream ss; 
    ss << Result::print();
    ss << "Event Tree (1/2) entries (" << t1NEntries << "/" << t2NEntries << ")\n";
    ss << "Offending event numbers: ";
    for (const auto& eventNo : offendingEvents)
      {ss << eventNo << " ";}
    for (const auto& eventResult : eventResults)
      {
	if (!eventResult.passed)
	  {ss << eventResult.print();}
      }
    ss << "\n";
    return ss.str();
  }

  virtual ~ResultEventTree(){}
};

#endif
