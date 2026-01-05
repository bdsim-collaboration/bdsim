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
#ifndef BDSUNDULATOR_H
#define BDSUNDULATOR_H

#include "BDSAcceleratorComponent.hh"

#include "globals.hh"

class G4Material;
class BDSBeamPipeInfo;
class BDSFieldInfo;

/**
 * @brief Undulator.
 *
 * Requires a valid BDSBeamPipeInfo* instance.
 *
 * @author Will Shields
 */

class BDSUndulator: public BDSAcceleratorComponent
{
public:
  BDSUndulator() = delete;
  BDSUndulator(const G4String&  nameIn,
               G4double         lengthIn,
               G4double         periodIn,
               G4double         magnetHeightIn,
               G4double         magnetWidthIn,
               G4double         undulatorGapIn,
               BDSBeamPipeInfo* beamPipeInfoIn,
               BDSFieldInfo*    vacuumFieldInfoIn,
               BDSFieldInfo*    outerFieldInfoIn,
               G4Material*      materialIn);
  /// @{ Assignment and copy constructor not implemented nor used
  BDSUndulator& operator=(const BDSUndulator&) = delete;
  BDSUndulator(BDSUndulator&) = delete;
  /// @}
  virtual ~BDSUndulator();

protected:
  virtual void Build();

  virtual void BuildContainerLogicalVolume();

  BDSFieldInfo* vacuumFieldInfo;
  BDSFieldInfo* outerFieldInfo;
  const G4double undulatorPeriod;
  const G4double horizontalWidth; ///< Element width (and height)
  G4double undulatorMagnetHeight; ///< Full magnet box height
  G4double undulatorGap;          ///< Full undulator gap
  G4int    numMagnets;            ///< Total number of magnets (1 undulator period is 2 magnets)
  G4Material* material;              ///< Undulator magnet material
};

#endif
