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
#ifndef BDSSURVEY_H
#define BDSSURVEY_H

#include "globals.hh" // geant4 types / globals

#include <fstream>
#include <vector>

class BDSBeamline;
class BDSBeamlineElement;
class BDSMagnetStrength;

/**
 * @brief A class of functions to output Geant4/Mokka/BDSIM parameters for the beamline.
 *
 * This writes all details about elements in a beamline including geometrical placement
 * information, fields, etc, much like MadX Tfs file.
 */

class BDSSurvey
{
public:
  explicit BDSSurvey(const G4String& filename);
  BDSSurvey() = delete;
  BDSSurvey(const BDSSurvey&) = delete;
  BDSSurvey& operator=(const BDSSurvey&) = delete;
  ~BDSSurvey();

  /// write line
  void Write(BDSBeamlineElement* beamlineElement);

  /// Write a whole beamline, summary of it and close the file.
  void Write(BDSBeamline* beamline);
  
private:
  BDSSurvey() = delete;
  BDSSurvey(const BDSSurvey&) = delete;
  BDSSurvey& operator=(const BDSSurvey&) = delete;

  /// Write header
  void WriteHeader();
  
  /// Output file stream
  std::ofstream survey;

  /// The default zero strength instance of BDSMagnetStrength that
  /// can be queried in place of one not being available - ie when
  /// the element isn't a magnet.
  BDSMagnetStrength const* nullStrength;

  /// Cache of all the possible magnet strength parameters.
  std::vector<G4String> magnetKeys;

  const int gp; ///< General precision - number of characters / item.
};

#endif
