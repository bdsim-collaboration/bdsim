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
#ifndef BDSPARTICLECOORDSSPHERICAL_H
#define BDSPARTICLECOORDSSPHERICAL_H 

#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"
#include "G4Types.hh"

#include <ostream>

/**
 * @brief A set of spherical particle coordinates.
 * 
 * @author Laurie Nevay
 */

class BDSParticleCoordsSpherical
{
public:
  BDSParticleCoordsSpherical();
  BDSParticleCoordsSpherical(G4double rIn,
			     G4double thetaIn,
			     G4double phiIn,
			     G4double rpIn,
			     G4double thetapIn,
			     G4double phipIn,
			     G4double tIn);
  virtual ~BDSParticleCoordsSpherical(){;}
  
  /// Output stream
  friend std::ostream& operator<< (std::ostream& out, BDSParticleCoordsSpherical const& p);
  
  /// Actual print out method so it can be called from a derived class.
  virtual void Print(std::ostream& out) const;
  
  G4double r;
  G4double theta;
  G4double phi;
  G4double rp;
  G4double thetap;
  G4double phip;
  G4double T;
};

#endif
