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
#ifndef HEADERANALYSIS_H
#define HEADERANALYSIS_H

#include <string>
#include <vector>

class Header;
class TChain;

/**
 * @brief Analysis of the model tree.
 *
 * @author Laurie Nevay
 */

class HeaderAnalysis
{
public:
  /// Constructor intended for use to construct a model analysis object.
  HeaderAnalysis(const std::vector<std::string>& filenamesIn,
                 Header* headerIn,
                 TChain* chainIn);
  virtual ~HeaderAnalysis() noexcept;

  /// Add up nOriginalEvents from each file in the chain and also nEventsInFile
  /// and nEventsInFileSkipped so we have the total numbers. Use the second entry
  /// of the header in each file, if it exists. Will reset incoming variables to 0.
  unsigned long long int CountNOriginalEvents(unsigned long long int& nEventsInFileIn,
                                              unsigned long long int& nEventsInFileSkippedIn,
                                              unsigned long long int& nEventsRequestedIn,
                                              unsigned int& distrFileLoopNTimesIn);

protected:
  std::vector<std::string> filenames;
  Header* header;
  TChain* chain;
};


#endif
