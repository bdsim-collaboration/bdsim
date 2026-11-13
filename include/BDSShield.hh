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
#ifndef BDSSHIELD_H
#define BDSSHIELD_H

#include "BDSAcceleratorComponent.hh"

#include "globals.hh" // geant4 types / globals

class BDSBeamPipeInfo;

class G4Colour;
class G4Material;

/**
 * @brief A square mask of solid material with a square aperture.
 *
 * Optionally may have a piece of beam pipe inside it.
 *
 * @author Laurie Nevay
 */

class BDSShield: public BDSAcceleratorComponent
{
public:
  BDSShield(const G4String&  name,
            G4double         length,
            G4double         horizontalWidthIn,
            G4double         xSize,
            G4double         ySize,
            G4Material*      materialIn,
            G4Colour*        colourIn,
            BDSBeamPipeInfo* beamPipeInfoIn = nullptr);
  virtual ~BDSShield();

protected:
  /// Build a container volume for everything.
  virtual void BuildContainerLogicalVolume();

  /// Do common BDSAcceleratorComponent::Build then build and place the shield,
  /// then the beampipe if it exists.
  virtual void Build();

private:
  /// @{ Default constructor, assignment and copy constructor not used.
  BDSShield() = delete;
  BDSShield& operator=(const BDSShield&) = delete;
  BDSShield(BDSShield&) = delete;
  /// @}
  
  /// Build the outer shield geoemtry.
  void BuildShield();

  /// Build a beam pipe in the hole if required.
  void BuildBeamPipe();
  
  G4double    horizontalWidth; ///< Outer size of shield.
  G4double    xSize;           ///< Inner horizontal half width of shield.
  G4double    ySize;           ///< Inner vertical half width of shield.
  G4Material* material;        ///< Shield material.
  G4Colour*   colour;          ///< Colour of shielding block.
};

#endif
