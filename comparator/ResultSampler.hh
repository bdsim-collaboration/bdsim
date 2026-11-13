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
#ifndef COMPRESULTSAMPLER_H
#define COMPRESULTSAMPLER_H

#include "Result.hh"

#include <sstream>
#include <string>

/**
 * @brief Result of comparison of a sampler branch.
 * 
 * @author Laurie Nevay
 */

class ResultSampler: public Result
{ 
public:
  ResultSampler(std::string nameIn):
    Result(nameIn, "Sampler")
  {;}
  
  virtual ~ResultSampler() {}

  std::vector<std::string> offendingLeaves;

  virtual std::string print() const
  {
    std::stringstream ss; 
    ss << Result::print();
    ss << "Offending leaves: ";
    for (const auto& leafName : offendingLeaves)
      {ss << "\"" << leafName << "\" ";}
    ss << "\n";
    return ss.str();
  }
};

#endif
