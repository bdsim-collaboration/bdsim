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
#ifndef BDSOUTPUTLOADER_H
#define BDSOUTPUTLOADER_H

#include "globals.hh" // geant4 types / globals

namespace GMAD {
  class Beam;
  class BeamBase;
  class Options;
  class OptionsBase;
}

class BDSOutputROOTEventBeam;
class BDSOutputROOTEventInfo;
class BDSOutputROOTEventOptions;
class TFile;
class TTree;

/**
 * @brief Loader of ROOT Event output for recreating events.
 *
 * @author Laurie Nevay
 */

class BDSOutputLoader
{
public:
  explicit BDSOutputLoader(const G4String& filePath);
  virtual ~BDSOutputLoader();

  inline G4int DataVersion() const {return dataVersion;}
  inline unsigned long long int NOriginalEvents() const {return nOriginalEvents;}
  GMAD::OptionsBase OptionsBaseClass();
  GMAD::Options     Options();

  GMAD::BeamBase BeamBaseClass();
  GMAD::Beam     Beam();

  /// Access the seed state for a given event index in the file (0 counting).
  G4String SeedState(G4int eventNumber = 0);
  
protected:
  TFile* file;
  G4int  dataVersion;
  unsigned long long int nOriginalEvents; ///< Cache of variable from header.
  G4bool badFilePath;
  G4bool rootEventFile;

  BDSOutputROOTEventBeam*    localBeam;
  BDSOutputROOTEventOptions* localOptions;
  BDSOutputROOTEventInfo*    localEventSummary;

  TTree* beamTree;
  TTree* optionsTree;
  TTree* eventTree;

private:
  BDSOutputLoader() = delete;
  BDSOutputLoader(const BDSOutputLoader&) = delete;
  BDSOutputLoader& operator=(const BDSOutputLoader&) = delete;
};

#endif
