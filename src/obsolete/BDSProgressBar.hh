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
#ifndef BDSPROGRESSBAR_H
#define BDSPROGRESSBAR_H

#include <iostream>
#include <iomanip>
#include <string>

/**
 * @brief Display a progress bar in ascii.
 *
 * @author Lawrence Deacon
 */

class BDSProgressBar
{
public:
  explicit BDSProgressBar(double maxCountIn);

  void Reset();
  inline double Count()    const {return count;}
  inline double MaxCount() const {return maxCount;}

  inline void Increment(double val)
  {
    count += val;
    if(count > nextTicMarkCount)
      {Display();}
  }

private:
  /// Private default constructor to force use of supplied one.
  BDSProgressBar() = delete;
  
  double count, maxCount, nextTicMarkCount;
  double ticMark;
  double nTicMarksInBar;
  double countsPerTicMark;

  inline void Display()
  {
    ticMark = 0;
    double ticMarksNeeded = nTicMarksInBar * count/maxCount;
    nextTicMarkCount = (ticMarksNeeded + 1) * countsPerTicMark;
    std::cout << "]\n\033[F\033[J" << "0%" << "[";
    
    do {std::cout << '-' ;}
    while ( ++ticMark < ticMarksNeeded );
    if(ticMark < nTicMarksInBar)
      {
	std::cout << '>';      
	while (++ticMark < nTicMarksInBar)
	  {std::cout << ' ';} 
      }
    std::cout << "]100%" << std::flush;
  }
};

#endif
