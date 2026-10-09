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
#ifndef BDSCOLLIMATORJAWTIP_H
#define BDSCOLLIMATORJAWTIP_H

#include "BDSCollimatorJaw.hh"

#include "G4String.hh"
#include "G4Types.hh"

class G4Colour;
class G4Material;
class G4VSolid;

/**
 * @brief Collimator with jaws having a bulk material and a tip of a different material.
 *
 * @autor Giacomo Broggi
 */

class BDSCollimatorJawTip: public BDSCollimatorJaw
{
public:
  BDSCollimatorJawTip() = delete;
  /// With a taper (tipTaperAngleIn > 0), lengthIn is the length of the flat
  /// jaw face at the beam; the component is then longer, see TaperedLength().
  /// tipTaperDepthIn is the depth from the jaw edge where the taper ends
  /// (beyond it the jaw keeps a constant length); 0 tapers the whole jaw.
  BDSCollimatorJawTip(const G4String& nameIn,
                      G4double    lengthIn,
                      G4double    horizontalWidthIn,
                      G4double    xHalfGapIn,
                      G4double    yHalfHeightIn,
                      G4double    xsizeLeftIn,
                      G4double    xsizeRightIn,
                      G4double    leftJawTiltIn,
                      G4double    rightJawTiltIn,
                      G4double    tipThicknessIn,
                      G4double    tipTaperAngleIn,
                      G4double    tipTaperDepthIn,
                      G4bool      buildLeftJawIn,
                      G4bool      buildRightJawIn,
                      G4Material* collimatorMaterialIn,
                      G4Material* collimatorTipMaterialIn,
                      G4Material* vacuumMaterialIn,
                      G4Colour*   colourIn = nullptr,
                      G4Colour*   tipColourIn = nullptr);
  virtual ~BDSCollimatorJawTip();

  /// @{ Assignment and copy constructor not implemented nor used
  BDSCollimatorJawTip& operator=(const BDSCollimatorJawTip&) = delete;
  BDSCollimatorJawTip(BDSCollimatorJawTip&) = delete;
  /// @}

protected:
  /// Adjust the calculated values to include the space for the tips that are separate placements.
  void UpdateCalculations();

  /// Check and update parameters before construction. Called at the start of Build() as
  /// we can't call a virtual function in a constructor.
  void CheckParametersForTips();

  /// Override function in BDSCollimator for totally different construction.
  virtual void Build() override;

  virtual void BuildTips();

  G4Colour*   tipColour;
  G4double    tipThickness;
  G4Material* collimatorTipMaterial;
  G4ThreeVector leftJawTipPos;
  G4ThreeVector rightJawTipPos;
};

#endif
