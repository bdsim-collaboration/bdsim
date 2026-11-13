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
#ifndef BDSOUTPUTROOTEVENTHEADER_H
#define BDSOUTPUTROOTEVENTHEADER_H

#include "Rtypes.h"
#include "TObject.h"

#include <string>
#include <vector>

/** 
 * @brief Information about the software and the file.
 * 
 * @author Laurie Nevay
 */

class BDSOutputROOTEventHeader: public TObject
{
public:
  BDSOutputROOTEventHeader();
  BDSOutputROOTEventHeader(const BDSOutputROOTEventHeader&) = default;
  virtual ~BDSOutputROOTEventHeader();
  virtual void Flush(){FlushLocal();}

  /// We have a non-virtual version as it's bad to call this from a constructor.
  void FlushLocal();
  
  std::string bdsimVersion;
  std::string geant4Version;
  std::string rootVersion;
  std::string clhepVersion;
  std::string timeStamp;
  std::string fileType;
  int         dataVersion;                     ///< Our data format version.
  bool        doublePrecisionOutput;           ///< Whether using double precision output - assumed float if not
  std::vector<std::string> analysedFiles;      ///< List of which files were analysed in case of a rebdsim output file.
  std::vector<std::string> combinedFiles;      ///< List of which files were combined in case of a rebdsimCombine output file.
  int                      nTrajectoryFilters; ///< Length of bitset used for trajectory filtering - compile time info.
  std::vector<std::string> trajectoryFilters;  ///< Names of filters.
  bool skimmedFile;                            ///< Whether this is a skimmed output file
  unsigned long long int nOriginalEvents;      ///< Number of original events if skimmed.
  unsigned long long int nEventsRequested;     ///< Number of events requested to be simulated from the file.
  unsigned long long int nEventsInFile;        ///< Number of events in the input distribution file irrespective of filters.
  unsigned long long int nEventsInFileSkipped; ///< Number of events from distribution file that were skipped due to filters.
  unsigned int           distrFileLoopNTimes;  ///< Number of times a distribution file was replayed.
  
  /// Update the file type.
  void SetFileType(const std::string& fileTypeIn) {fileType = fileTypeIn;}
  
  /// #ifndef __ROOTBUILD__
  /// Nominally, we don't expose the fill methods to the analysis root dictionaries
  /// but as this doesn't use geant4 and is required when creating analysis output
  /// file, we break that convention.
  void Fill(const std::vector<std::string>& analysedFilesIn = std::vector<std::string>(),
            const std::vector<std::string>& combinedFilesIn = std::vector<std::string>());

#ifndef __ROOTBUILD__
  /// Fill with information from Geant4 side of things.
  void FillGeant4Side();
#endif

  ClassDef(BDSOutputROOTEventHeader,5);
};

#endif
