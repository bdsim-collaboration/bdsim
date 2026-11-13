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
#ifndef BDSCOLLIMATORELLIPTICAL_H
#define BDSCOLLIMATORELLIPTICAL_H

#include "BDSCollimator.hh"

#include "globals.hh" // geant4 globals / types

class G4Colour;
class G4Material;

/**
 * @brief A class for an elliptical collimator.
 */

class BDSCollimatorElliptical: public BDSCollimator
{
public:
  BDSCollimatorElliptical(const G4String& name,
			  G4double    length,
			  G4double    horizontalWidth,
			  G4Material* collimatorMaterial,
			  G4Material* vacuumMaterial,
			  G4double    xAperture    = 0,
			  G4double    yAperture    = 0,
			  G4double    xOutAperture = 0,
			  G4double    yOutAperture = 0,
			  G4Colour*   colour         = nullptr,
			  G4bool      circularOuter = false);
  virtual ~BDSCollimatorElliptical(){;};

  virtual void BuildInnerCollimator();

protected:
  /// Check and update parameters before construction. Called at the start of Build() as
  /// we can't call a virtual function in a constructor.
  virtual void CheckParameters();

private:
  /// Private default constructor to force the use of the supplied one.
  BDSCollimatorElliptical();

  /// @{ Assignment and copy constructor not implemented nor used
  BDSCollimatorElliptical& operator=(const BDSCollimatorElliptical&) = delete;
  BDSCollimatorElliptical(BDSCollimatorElliptical&) = delete;
  ///@}
};

#endif
