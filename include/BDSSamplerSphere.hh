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
#ifndef BDSSAMPLERSPHERE_H
#define BDSSAMPLERSPHERE_H

#include "BDSSampler.hh"

#include "G4String.hh"
#include "G4Types.hh"

#include "CLHEP/Units/SystemOfUnits.h"

/** 
 * @brief Spherical sampler around an object.
 * 
 * Creates a hollow very thin (1um thick) sphere around an object
 * without end caps that acts as a sampler.
 *
 * @author Laurie Nevay
 */

class BDSSamplerSphere: public BDSSampler
{
public:
  BDSSamplerSphere() = delete;
  BDSSamplerSphere(const G4String& name,
                   G4double        radiusIn,
                   G4double        startAnglePhi   = 0,
                   G4double        sweepAnglePhi   = CLHEP::twopi,
                   G4double        startAngleTheta = 0,
                   G4double        sweepAngleTheta = CLHEP::pi,
                   G4int           filterSetIDIn   = -1);
  
  /// @{ Assignment and copy constructor not implemented nor used
  BDSSamplerSphere& operator=(const BDSSamplerSphere&) = delete;
  BDSSamplerSphere(BDSSamplerSphere&) = delete;
  /// @}

  virtual ~BDSSamplerSphere(){;}

protected:
  virtual void SetSensitivity();
};

#endif
