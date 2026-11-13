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
#ifndef COMPRESULTHISTOGRAM2D_H
#define COMPRESULTHISTOGRAM2D_H

#include "ResultHistogram.hh"

#include <sstream>
#include <string>

/**
 * @brief Result of comparing 2 2D histograms.
 *
 * @author Stewart Boogert.
 */

class ResultHistogram2D: public ResultHistogram
{
public:
  int    h1NYBins;
  int    h2NYBins;
  double h1YMean;
  double h2YMean;
  double h1YRms;
  double h2YRms;

  virtual std::string print() const
  {
    std::stringstream ss; 
    ss << ResultHistogram::print();
    ss << h1NYBins << " " << h2NYBins << " " << h1YMean << " " << h2YMean << " ";
    return ss.str();
  }

  virtual ~ResultHistogram2D() {}
};

#endif
