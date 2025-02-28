/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2024.

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
#ifndef RBDSDATA_H
#define RBDSDATA_H

#include "Rtypes.h" // for classdef

#include <map>
#include <string>

class TDirectory;
class TFile;
class TTree;

/**
 * @brief Utility functions for data files.
 *
 * @author Laurie Nevay
 */

namespace RBDS
{  
  /// Create a new TFile and add a header to it. The header object is filled and written.
  TFile* CreateEmptyRebdsimFile(const std::string& fileName,
                                unsigned long long int nOriginalEventsIn = 1);

  /// Create a new TFile and add a header to it. The header object if filled and written.
  TFile* CreateEmptyBdskimFile(const std::string& originalFileName,
                               const std::string& newOutputFileName);

  TFile* CreateEmptyBdskimFile(TFile* originalFile,
                               const std::string& newOutputFileName);

  /// Add the treeName minus the last character (expected to be a '.') to the
  /// output file then inside that directory, add PerEntryHistograms, SimpleHistograms,
  /// and MergedHistograms.
  std::map<std::string, TDirectory*> CreateDirectories(TFile* outputFile,
						       std::string treeName);
}

/**
 * @brief A dummy class as ROOT won't seem to import namespaced functions in CLING for python.
 * 
 * @author Laurie Nevay
 */

class DataDummyClass
{
public:
  DataDummyClass();
  virtual ~DataDummyClass();

  TFile* CreateEmptyRebdsimFile(const std::string& fileName,
                                unsigned long long int nOriginalEventsIn = 1);

  TFile* CreateEmptyBdskimFile(const std::string& originalFileName,
                               const std::string& newOutputFileName);

  std::map<std::string, TDirectory*> CreateDirectories(TFile* outputFile,
                                                       std::string treeName);
  ClassDef(DataDummyClass,1);
};

#endif
