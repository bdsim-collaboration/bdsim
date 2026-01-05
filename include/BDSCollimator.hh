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
#ifndef BDSCOLLIMATOR_H
#define BDSCOLLIMATOR_H

#include "BDSAcceleratorComponent.hh"

#include "globals.hh" // geant4 types / globals

class G4Colour;
class G4Material;
class G4UserLimits;
class G4VSolid;

/**
 * @brief Base class for collimators with common construction.
 * 
 * @author Laurie Nevay
 */

class BDSCollimator: public BDSAcceleratorComponent
{
public:
  BDSCollimator() = delete;
  BDSCollimator(const G4String& name,
                G4double        length,
                G4double        horizontalWidth,
                const G4String& type,
                G4Material* collimatorMaterial,
                G4Material* vacuumMaterial,
                G4double    xApertureIn    = 0,
                G4double    yApertureIn    = 0,
                G4double    xApertureOutIn = 0,
                G4double    yApertureOutIn = 0,
                G4Colour*   colourIn       = nullptr,
                G4bool      circularOuterIn = false);
  /// @{ Assignment and copy constructor not implemented nor used
  BDSCollimator& operator=(const BDSCollimator&) = delete;
  BDSCollimator(BDSCollimator&) = delete;
  /// @}
  virtual ~BDSCollimator();

  /// @{ Accessor.
  virtual G4String Material() const;
  virtual G4double XApertureIn()  const {return xAperture;}
  virtual G4double XApertureOut() const {return xApertureOut;}
  virtual G4double YApertureIn()  const {return yAperture;}
  virtual G4double YApertureOut() const {return yApertureOut;}
  /// @}

  virtual void SetMinimumKineticEnergy(G4double minimimumKineticEnergyIn) {minKineticEnergy = minimimumKineticEnergyIn;}

protected:
  /// Check and update parameters before construction. Called at the start of Build() as
  /// we can't call a virtual function in a constructor.
  virtual void CheckParameters();
  
  virtual void Build();

  virtual void BuildContainerLogicalVolume();
  
  /// Pure virtual function to be provided by derived classes.
  /// Must produce vacuumSolid and innerSolid - the inner is used
  /// to subtract from the mass and the vacuum is placed inside it all
  virtual void BuildInnerCollimator() = 0;

  /// Return either default user limits or custom ones based on optional minimumKineticEnergy.
  G4UserLimits* CollimatorUserLimits();

  ///@{ Geometrical objects:
  G4VSolid* collimatorSolid;
  G4VSolid* innerSolid;
  G4VSolid* vacuumSolid;
  ///@}
  G4double    horizontalWidth;    ///< Horizontal width.
  G4Material* collimatorMaterial; ///< Material.
  G4Material* vacuumMaterial;     ///< Vacuum material.
  G4double    xAperture;          ///< Aperture at entrance in x dimension.
  G4double    yAperture;          ///< Aperture at entrance in y dimension.
  G4double    xApertureOut;       ///< Aperture at exit in x dimension.
  G4double    yApertureOut;       ///< Aperture at exit in y dimension.
  G4bool      tapered;            ///< Flag for tapered collimator.
  G4Colour*   colour;             ///< Colour of collimator.
  G4double    minKineticEnergy;   ///< Optional minimum kinetic energy for collimator materials.
  G4bool      circularOuter;      ///< Aperture type of the collimator
};

#endif
