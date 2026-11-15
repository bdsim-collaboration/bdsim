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
#ifndef BDSBEAMPIPEINFO_H
#define BDSBEAMPIPEINFO_H

#include "BDSBeamPipeType.hh"

#include "G4Types.hh"
#include "G4ThreeVector.hh"

class BDSAperture;
class BDSExtent;
class G4Material;

/**
 * @brief Holder class for all information required to describe a beam pipe model. 
 * 
 * This reduces the number of argument
 * to all magnet constructors plus aggregates common tasks
 * in the component factory.
 * 
 * @author Laurie Nevay
 */

class BDSBeamPipeInfo
{
public:
  /// No default constructor to ensure one of supplied constructors is used.
  BDSBeamPipeInfo() = delete;
  /// Constructor to assign all members at once
  BDSBeamPipeInfo(BDSBeamPipeType beamPipeTypeIn,
                  BDSAperture*    apertureIn,
                  G4Material*     vacuumMaterialIn,
                  G4double        beamPipeThicknessIn,
                  G4Material*     beamPipeMaterialIn,
                  G4bool          vacuumOnly         = false,
                  BDSAperture*    apertureOutIn      = nullptr,
                  G4ThreeVector*  inputFaceNormalIn  = nullptr,
                  G4ThreeVector*  outputFaceNormalIn = nullptr);

  BDSBeamPipeInfo(const BDSBeamPipeInfo& other);
  BDSBeamPipeInfo& operator=(const BDSBeamPipeInfo& other) = delete;

  ~BDSBeamPipeInfo();
  
  /// This gives the maximal extent of both the entrance and exit faces.
  BDSExtent Extent() const;
  
  /// Return an indicative extent of the beam pipe - typically the maximum of x or y extent.
  G4double IndicativeRadius() const;

  /// Return an indicative inner extent for the beam pipe vacuum.
  G4double MaximumInscribedCircleRadius() const;

  //  Public member for direct access
  /// @{ Necessary members.
  BDSBeamPipeType beamPipeType;
  BDSAperture*    aperture;
  G4Material*     vacuumMaterial;
  G4double        beamPipeThickness;
  G4Material*     beamPipeMaterial;
  G4bool          vacuumOnly;
  /// @}

  /// @{ Optional members.
  BDSAperture*    apertureOut;
  G4ThreeVector*  inputFaceNormal;
  G4ThreeVector*  outputFaceNormal;
  ///@}
};

#endif
