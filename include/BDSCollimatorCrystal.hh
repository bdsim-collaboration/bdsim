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
#ifndef BDSCOLLIMATORCRYSTAL_H
#define BDSCOLLIMATORCRYSTAL_H

#include "globals.hh"

#include "BDSAcceleratorComponent.hh"

class BDSBeamPipeInfo;
class BDSCrystal;
class BDSCrystalInfo;

/**
 * @brief A piece of vacuum beam pipe with a crystal for channelling.
 * 
 * @author Laurie Nevay
 */

class BDSCollimatorCrystal: public BDSAcceleratorComponent
{
public:
  BDSCollimatorCrystal(const G4String&  name,
		       G4double         length,
		       BDSBeamPipeInfo* beamPipeInfoIn,
		       BDSCrystalInfo*  crystalInfoLeftIn,
		       BDSCrystalInfo*  crystalInfoRightIn = nullptr,
		       G4double         halfGapLeftIn      = 0,
		       G4double         halfGapRightIn     = 0,
		       G4double         angleYAxisLeftIn   = 0,
		       G4double         angleYAxisRightIn  = 0);
  virtual ~BDSCollimatorCrystal();

  /// Override base class version and return crystal material.
  virtual G4String Material() const;

protected:
  /// Construct geometry.
  virtual void Build();

private:
  /// No default constructor.
  BDSCollimatorCrystal() = delete;
  
  /// Void function to fulfill BDSAcceleratorComponent requirements.
  virtual void BuildContainerLogicalVolume(){;}

  /// If the crystal will cause an overlap in the vacuum volume due to
  /// its rotation, print warning and exit.
  void LongitudinalOverlap(const BDSExtent& extCrystal,
			   G4double         crystalAngle,
			   const G4String& side) const;

  /// Register logical volumes in crystals and collimator sets in accelerator model.
  void RegisterCrystalLVs(const BDSCrystal* crystal) const;

  /// Calculate appropriate offset in x so the inside edge will align
  /// properly with the xsize opening of the collimator.
  G4double TransverseOffsetToEdge(const BDSCrystal* crystal,
				  G4double          placementAngle,
				  G4bool            left) const;

  BDSCrystalInfo*    crystalInfoLeft;   ///< Model associated with left crystal.
  BDSCrystalInfo*    crystalInfoRight;  ///< Model associated with right crystal.
  const G4double     halfGapLeft;
  const G4double     halfGapRight;
  const G4double     angleYAxisLeft;
  const G4double     angleYAxisRight;
  BDSCrystal* crystalLeft;
  BDSCrystal* crystalRight;
};

#endif
