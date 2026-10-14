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
#ifndef BDSCOLLIMATORJAW_H
#define BDSCOLLIMATORJAW_H

#include "BDSCollimator.hh"

#include "globals.hh" // geant4 types / globals
#include "G4Material.hh"
#include "G4RotationMatrix.hh"

class G4Colour;
class G4LogicalVolume;
class G4VSolid;

/**
 * @brief Collimator with only two jaw and no top bit.
 *
 * @author Will Shields
 */

class BDSCollimatorJaw: public BDSCollimator
{
public:
  BDSCollimatorJaw(const G4String& nameIn,
                   G4double    lengthIn,
                   G4double    horizontalWidthIn,
                   G4double    xHalfGapIn,
                   G4double    yHalfHeightIn,
                   G4double    xsizeLeftIn,
                   G4double    xsizeRightIn,
                   G4double    leftJawTiltIn,
                   G4double    rightJawTiltIn,
                   G4bool      buildLeftJawIn,
                   G4bool      buildRightJawIn,
                   G4Material* collimatorMaterialIn,
                   G4Material* vacuumMaterialIn,
                   G4Colour*   colourIn = nullptr,
                   const G4String& objectType = "jcol");
  virtual ~BDSCollimatorJaw();

  inline G4double GetJawTiltLeft() const {return jawTiltLeft;}
  inline G4double GetJawTiltRight() const {return jawTiltRight;}

protected:
  /// Solid and placement for one (optionally tapered) jaw sub-block, as
  /// returned by BuildTaperedJawBox().
  struct TaperedBox
  {
    G4VSolid*         solid;
    G4ThreeVector     position;
    G4RotationMatrix* rotation; ///< nullptr for an untapered (plain box) solid.
  };

  /// Build one jaw sub-block (the bulk jaw, or - in a derived class - the tip),
  /// spanning local depth [depthInner, depthOuter] (both >= 0, depthOuter >
  /// depthInner) measured from the aperture edge of this jaw. 'sign' is +1 for
  /// the left jaw (depth increases along +x) or -1 for the right jaw (depth
  /// increases along -x). With a taper, the jaw is taperFlatLength long at its
  /// edge (depth 0) and grows by 2*depth/tan(tipTaperAngle) up to taperDepth (if
  /// set, else over the whole jaw), keeping a constant length beyond: the same
  /// profile for both jaws whatever their gaps. The block is then a
  /// G4ExtrudedSolid of that profile. If tipTaperAngle is zero, the returned
  /// solid is a plain G4Box of length fullChordLength, identical to the
  /// original (untapered) construction.
  TaperedBox BuildTaperedJawBox(const G4String& solidName,
                                G4double xHalfGapThisJaw,
                                G4int    sign,
                                G4double depthInner,
                                G4double depthOuter,
                                G4double halfHeight,
                                G4double fullChordLength) const;

  /// Length of the component for a jaw tapered at taperAngle whose face at
  /// the beam is flatLength long: flatLength + 2*depth/tan(taperAngle) for the
  /// deepest jaw built (or taperDepth, if set and smaller), with jaw gaps and
  /// depths worked out as in the constructor and Calculations(). Returns
  /// flatLength if there is no taper (or the angle is invalid, which
  /// CheckParameters() then rejects).
  static G4double TaperedLength(G4double flatLength,
                                G4double horizontalWidth,
                                G4double xHalfGap,
                                G4double xSizeLeft,
                                G4double xSizeRight,
                                G4bool   buildLeftJaw,
                                G4bool   buildRightJaw,
                                G4double taperAngle,
                                G4double taperDepth);

  void Calculations(); ///< Calculate offsets and sizes.

  /// Check and update parameters before construction. Called at the start of Build() as
  /// we can't call a virtual function in a constructor.
  virtual void CheckParameters() override;
  
  /// Override function in BDSCollimator for totally different construction.
  virtual void Build() override;

  /// Override function in BDSCollimator for different size based container.
  virtual void BuildContainerLogicalVolume() override;

  /// To fulfill inheritance but unused.
  virtual void BuildInnerCollimator() final {;}

  G4double  xSizeLeft;       ///< Offset of jaw 1
  G4double  xSizeRight;      ///< Offset of jaw 2
  G4double  xHalfGap;        ///< Half gap separation between jaws.
  G4double  jawTiltLeft;     ///< Tilt of jaw 1 (angle in x-z plane)
  G4double  jawTiltRight;    ///< Tilt of jaw 2 (angle in x-z plane)
  G4double  tipTaperAngle;   ///< Taper angle (rad) of the front and back jaw faces; 0 (default) = flat jaw, as before.
  G4double  taperFlatLength; ///< With a taper, length of the jaw face at the beam (the element length).
  G4double  taperDepth;      ///< Depth from the jaw edge where the taper ends; 0 = whole jaw.
  G4double  yHalfHeight;     ///< Half height of each jaw.
  G4bool    buildLeftJaw;    ///< Build left jaw or not.
  G4bool    buildRightJaw;   ///< Build right jaw or not.
  G4bool    buildAperture;   ///< Build aperture or not.

  G4double leftJawHalfGap;
  G4double rightJawHalfGap;
  G4double leftJawWidth;
  G4double rightJawWidth;
  G4double vacuumWidth;
  G4ThreeVector leftJawPos;
  G4ThreeVector rightJawPos;
  G4ThreeVector vacuumOffset;
  G4LogicalVolume* collimatorLV; ///< In case of no aperture, cache this volume for derived classes to place inside.

private:
  /// Private default constructor to force the use of the supplied one.
  BDSCollimatorJaw() = delete;

  /// @{ Assignment and copy constructor not implemented nor used
  BDSCollimatorJaw& operator=(const BDSCollimatorJaw&) = delete;
  BDSCollimatorJaw(BDSCollimatorJaw&) = delete;
  /// @}
};

#endif
