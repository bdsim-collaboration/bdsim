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
#ifndef BDSPARTICLECOORDS_H
#define BDSPARTICLECOORDS_H 

#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"
#include "G4Types.hh"

#include <ostream>

/**
 * @brief A set of particle coordinates.
 * 
 * @author Laurie Nevay
 */

class BDSParticleCoords
{
public:
  BDSParticleCoords();
  BDSParticleCoords(G4double xIn,
		    G4double yIn,
		    G4double zIn,
		    G4double xpIn,
		    G4double ypIn,
		    G4double zpIn,
		    G4double tIn);
  BDSParticleCoords(G4ThreeVector pos,
		    G4ThreeVector mom,
		    G4double      tIn);

  virtual ~BDSParticleCoords(){;}

  /// Apply a transform to the coordinates and return a copy of them transformed.
  BDSParticleCoords ApplyTransform(const G4Transform3D& transform) const;

  /// Apply an offset to the spatial coordinates only and return a copy.
  BDSParticleCoords ApplyOffset(const G4ThreeVector& offset) const;
  
  /// Apply an offset to the spatial and T coordinates only - assignment.
  void AddOffset(const G4ThreeVector& offset, G4double offsetT = 0);
  
  /// Output stream
  friend std::ostream& operator<< (std::ostream& out, BDSParticleCoords const& p);
  
  /// Actual print out method so it can be called from a derived class.
  virtual void Print(std::ostream& out) const;
  
  inline G4ThreeVector Position() const {return G4ThreeVector(x,y,z);}
  inline G4ThreeVector Momentum() const {return G4ThreeVector(xp,yp,zp);}
  
  G4double x;
  G4double y;
  G4double z;
  G4double xp;
  G4double yp;
  G4double zp;
  G4double T;
};

#endif
