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
#ifndef BDSTILTOFFSET_H
#define BDSTILTOFFSET_H

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"

#include <ostream>

/**
 * @brief A holder for any placement offsets and rotations for a BDSAcceleratorComponent.
 * 
 * Note, no rotation variables are provided other than along the beam
 * axis (z) as this would cause overlapping volumes between a component
 * and the next one in the line.  This leads to much more complicated 
 * geometry or padding space (perhaps acceptable for very low angles), 
 * but for now is left to be implemented in future.
 * 
 * @author Laurie Nevay
 */

class BDSTiltOffset
{
public:
  BDSTiltOffset();

  BDSTiltOffset(G4double xOffset,
		G4double yOffset,
		G4double tiltIn);

  ///@{ Accessor
  inline G4double GetXOffset() const {return dx;}
  inline G4double GetYOffset() const {return dy;}
  inline G4double GetTilt()    const {return tilt;}
  ///@}

  /// More advance accessor for offset - only in x,y.
  G4ThreeVector GetOffset() const {return G4ThreeVector(dx, dy, 0);}

  /// Get a transform to represent this tilt offset.
  G4Transform3D Transform3D() const;
  
  ///@{ Inspector.
  G4bool HasFiniteOffset() const;
  G4bool HasFiniteTilt()   const;
  ///@}

  /// Output stream.
  friend std::ostream& operator<< (std::ostream &out, BDSTiltOffset const &to);
  
private:
  /// Horizontal displacement (mm) - note right handed coordinate system
  G4double dx;
  /// Vertical displacement (mm)
  G4double dy;
  /// Tilt angle (rad) - rotation angle about Z axis
  G4double tilt;
};

#endif
