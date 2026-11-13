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
#include "BinGeneration.hh"
#include "RBDSException.hh"

#include <algorithm>
#include <cmath>
#include <limits>
#include <string>
#include <vector>

std::vector<double> RBDS::LogSpace(double start,
				   double stop,
				   int    nBins,
				   double base,
				   bool   includeLastPoint)
{
  if (start <= std::numeric_limits<double>::min_exponent10 + 1)
    {throw RBDSException("Lower limit of log binning (" + std::to_string(start) + ") is too low to represent as a double in C++");}
  if (stop >= std::numeric_limits<double>::max_exponent10 - 1)
    {throw RBDSException("Upper limit of log binning (" + std::to_string(stop) + ") is too high to represent as a double in C++");}
  
  double realStart = std::pow(base, start);
  double realBase  = std::pow(base, (stop-start)/nBins);

  std::vector<double> result;
  int n = includeLastPoint ? nBins+1 : nBins;
  result.reserve(n);
  std::generate_n(std::back_inserter(result), n, RBDS::Logspace<double>(realStart,realBase));
  return result;
}

std::vector<double> RBDS::LinSpace(double start,
				   double stop,
				   int    nBins,
				   bool   includeLastPoint)
{
  std::vector<double> result;
  int n = includeLastPoint ? nBins+1 : nBins;
  result.reserve(n);
  double step = (stop - start) / (double)nBins;
  int i = 0; // counter
  for (double value = start; i < n; value += step, i++)
    {result.push_back(value);}
  return result;
}
