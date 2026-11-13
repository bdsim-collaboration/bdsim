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
#ifndef BINGENERATION_H
#define BINGENERATION_H

#include <vector>

namespace RBDS
{

  /* @brief Class to generate logarithmic points.
   *
   * Based on solution on StackOverFlow:
   * https://stackoverflow.com/a/21429452
   */
  template<typename T>
  class Logspace
  {
  public:
    Logspace(T first, T baseIn) : curValue(first), base(baseIn) {;}
    
    T operator()()
    {
      T retval = curValue;
      curValue *= base;
      return retval;
    }
    
  private:
    T curValue;
    T base;
  };

  /* @brief Method to generate evenly spaced points on a log scale.
   * 
   * Based on solution on StackOverFlow:
   * https://stackoverflow.com/a/21429452
   */
  std::vector<double> LogSpace(double start,
			       double stop,
			       int    nBins,
			       double base = 10,
			       bool   includeLastPoint = true);

  /// Linear range of values.
  std::vector<double> LinSpace(double start,
			       double stop,
			       int    nBins,
			       bool   includeLastPoint = true);
}
  
#endif
