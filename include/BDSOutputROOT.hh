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
#ifndef BDSOUTPUTROOT_H
#define BDSOUTPUTROOT_H

#include "BDSOutput.hh"

#include "globals.hh"

#include "Rtypes.h"

class TFile;
class TTree;

/**
 * @brief ROOT Event output class.
 * 
 * @author Stewart Boogert
 */

class BDSOutputROOT: public BDSOutput 
{
public:
  /// No default constructor.
  BDSOutputROOT() = delete;
  
  /// Constructor with default file name (without extension or number suffix).
  /// Also, file number offset to start counting suffix from.
  BDSOutputROOT(const G4String& fileName,
		G4int           fileNumberOffset,
		G4int           compressionLevelIn = -1);
  virtual ~BDSOutputROOT();

  virtual void NewFile();    ///< Open a new file.
  virtual void CloseFile();  ///< Write contents and close file.
  
  /// Implementation for ROOT output. Only for link - not for regular use.
  virtual void UpdateSamplers();
private:
  /// Copy header and write to file.
  virtual void WriteHeader();
  
  /// Overwrite contents of header in the file.
  virtual void WriteHeaderEndOfFile();

  /// Copy geant4 data to file.
  virtual void WriteParticleData();
  
  /// Copy beam and write to file.
  virtual void WriteBeam();
  
  /// Copy options and write to file.
  virtual void WriteOptions();

  /// Copy model and write to file.
  virtual void WriteModel();
  
  /// Copy from local event structures to the actual file.  Ony event level
  /// structures are copied.
  virtual void WriteFileEventLevel();

  /// Copy from local run structures to the actual file.  Only run level
  /// structures are copied.
  virtual void WriteFileRunLevel();

  /// An implementation only in this class. We need a non-virtual function to
  /// call in the class destructor.
  void Close();
  
  G4int  compressionLevel;     ///< ROOT compression level for files.
  TFile* theRootOutputFile;    ///< Output file.
  TTree* theHeaderOutputTree;  ///< Header Tree.
  TTree* theParticleDataTree;  ///< Geant4 Data Tree.
  TTree* theBeamOutputTree;    ///< Beam Tree.
  TTree* theOptionsOutputTree; ///< Options tree.
  TTree* theModelOutputTree;   ///< Model tree.
  TTree* theEventOutputTree;   ///< Event tree.
  TTree* theRunOutputTree;     ///< Output histogram tree.
};

#endif
