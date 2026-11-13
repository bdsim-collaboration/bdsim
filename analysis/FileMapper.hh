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
#ifndef FILEMAPPER_H
#define FILEMAPPER_H

#include <string>
#include <vector>

class HistogramAccumulator;
class TDirectory;
class TFile;
class TH1;

namespace RBDS
{
  /// Get the format of the root file open (bdsim, rebdsim, combined). Returns
  /// true for success and false otherwise. Filetype string is written to the
  /// string passed by reference. Changes the branch address of header if it's
  /// already set. Optional dataVersion - if supplied it will be updated with
  /// the version of data in the file.
  bool GetFileType(TFile* file,
		   std::string& fileType,
		   int*   dataVersion = nullptr);

  /// Whether the file type is a BDSIM output one. Does not close file. May change
  /// the branch address for the header in the file. Optional dataVersion.
  bool IsBDSIMOutputFile(TFile* file,
			 int*   dataVersion = nullptr);

  /// Similar but opens file first.
  bool IsBDSIMOutputFile(const std::string& filePath,
			 int* dataVersion = nullptr);

  /// Whether the file type is a REBDSIM output one. Does not close file. May change
  /// the branch address for the header in the file.
  bool IsREBDSIMOutputFile(TFile* file);

  /// Similar but opens file first and closes afterward.
  bool IsREBDSIMOutputFile(const std::string& filePath);

  /// Whether the file type is a REBDSIM or REBDSIMCOMBINE one.  Does not close file.
  /// May change the branch address for the header in the file.
  bool IsREBDSIMOrCombineOutputFile(TFile* file);

  /// Similar but opens the file first and closes afterwards.
  bool IsREBDSIMOrCombineOutputFile(const std::string& filePath);

  /// Determine the number of dimensions of a histogram by dynamically casting.
  int DetermineDimensionality(TH1* h);

  /// Common print out method.
  void WarningMissingHistogram(const std::string& histName,
			       const std::string& fileName);

  /// Basic structure for accumulating histogram from rebdsim output files.
  struct HistogramPath
  {
    std::string path; ///< Without histogram name.
    std::string name; ///< Name of histogram.
    HistogramAccumulator* accumulator;
    TDirectory* outputDir;
    bool BDSBH4Dtype;
  };

  /// Types of merging.
  enum class MergeType {none, meanmerge, sum};

  /// Determine merge type from parent directory name.
  MergeType DetermineMergeType(const std::string& parentDir);
}

/**
 * @brief Class to map a rebdsim file structure and create duplicate in output.
 *
 * @author Laurie Nevay
 */

class HistogramMap
{
public:
  HistogramMap() = delete; ///< No need for default constructor.
  HistogramMap(TFile* file,
	       TFile* output,
	       bool   debugIn = false);
  ~HistogramMap(){;}

  /// Recursively inspect a directory and create similar directories in the output
  /// file. Also make histogram paths.
  void MapDirectory(TDirectory* dir,
		    TFile*      output,
		    const std::string& parentDir);

  /// Access full vector of histograms.
  inline const std::vector<RBDS::HistogramPath>& Histograms() const {return histograms;}
  
private:
  bool debug;                                   ///< Debug flag.
  std::vector<RBDS::HistogramPath> histograms;  ///< Storage of all objects.
};

#endif
