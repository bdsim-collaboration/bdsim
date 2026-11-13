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
#ifndef COMPRESULTEVENT_H
#define COMPRESULTEVENT_H

#include "Result.hh"
#include "ResultSampler.hh"

#include <sstream>
#include <string>

/**
 * @brief Result of comparison of single entry of an Event tree in BDSIM output.
 *
 * @author Laurie Nevay
 */

class ResultEvent: public Result
{ 
public:
  std::vector<std::string> offendingBranches;
  std::vector<ResultSampler> samplerResults;

  virtual std::string print() const
  {
    std::stringstream ss; 
    ss << Result::print();
    ss << "Offending branches: ";
    for (const auto& branchName : offendingBranches)
      {ss << branchName << " ";}
    ss << "\nOffending samplers: ";
    for (const auto& samplerResult : samplerResults)
      {
	if (!samplerResult.passed)
	  {ss << samplerResult.print();}
      }
    ss << "\n";
    return ss.str();
  }

  virtual ~ResultEvent() {}
};

#endif
