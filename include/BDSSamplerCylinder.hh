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
#ifndef BDSSAMPLERCYLINDER_H
#define BDSSAMPLERCYLINDER_H

#include "BDSSampler.hh"

#include "G4String.hh"
#include "G4Types.hh"

#include "CLHEP/Units/SystemOfUnits.h"

/** 
 * @brief Cylindrical sampler around an object.
 * 
 * Creates a hollow very thin (1um thick) cylinder around an object
 * without end caps that acts as a sampler.
 *
 * @author Laurie Nevay
 */

class BDSSamplerCylinder: public BDSSampler
{
public:
  BDSSamplerCylinder() = delete;
  /// Straight cylinder without angled ends, i.e. a G4Tubs.
  BDSSamplerCylinder(const G4String& name,
                     G4double        radiusIn,
                     G4double        fullLength,
                     G4double        startAngle = 0,
                     G4double        sweepAngle = CLHEP::twopi,
                     G4int           filterSetIDIn = -1);
  
  /// Alternative constructor for angled faces - uses G4CutTubs.
  BDSSamplerCylinder(const G4String& name,
                     G4double        radiusIn,
                     G4double        fullLength,
                     const G4ThreeVector& inputFaceNormal,
                     const G4ThreeVector& outputFaceNormal,
                     G4double        startAngle = 0,
                     G4double        sweepAngle = CLHEP::twopi,
                     G4int           filterSetIDIn = -1);
  
  /// @{ Assignment and copy constructor not implemented nor used
  BDSSamplerCylinder& operator=(const BDSSamplerCylinder&) = delete;
  BDSSamplerCylinder(BDSSamplerCylinder&) = delete;
  /// @}

  virtual ~BDSSamplerCylinder(){;}

protected:
  virtual void SetSensitivity();
};

#endif
