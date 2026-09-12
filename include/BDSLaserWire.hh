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
#ifndef BDSLASERWIRE_H
#define BDSLASERWIRE_H

#include "globals.hh"  // geant4 types / globals
#include "BDSAcceleratorComponent.hh"

/**
 * @brief A laser wire scanner.
 * 
 * This builds a box with the default aperture in BDSGlobalConstants
 * and with the desired length. The box is the sole piece of geometry
 * and contains the special "LaserVac" material the provides the 'target'
 * of photons from the laser that the beam will scatter from.
 */

class BDSLaserWire :public BDSAcceleratorComponent
{
public:
  BDSLaserWire(G4String aName,
	       G4double aLength,
	       G4double aWavelength,
	       G4ThreeVector aDirection);
  virtual ~BDSLaserWire();

  inline void SetLaserDirection(G4ThreeVector aDirection) {itsLaserDirection=aDirection;}
  inline G4ThreeVector GetLaserDirection() const {return itsLaserDirection;}

  inline void SetLaserWavelength(G4double aWavelength) {itsLaserWavelength=aWavelength;}
  inline G4double GetLaserWavelength() const {return itsLaserWavelength;}

private:
  /// Private default constructor to force the use of the supplied one.
  BDSLaserWire() = delete;

  /// @{ Assignment and copy constructor not implemented nor used
  BDSLaserWire& operator=(const BDSLaserWire&) = delete;
  BDSLaserWire(BDSLaserWire&);
  /// @}

  /// Construct geometry and assign with special "LaserVac" material.
  virtual void BuildContainerLogicalVolume();

  G4ThreeVector itsLaserDirection;
  G4double itsLaserWavelength;
};

#endif
