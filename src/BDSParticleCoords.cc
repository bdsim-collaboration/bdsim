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
#include "BDSParticleCoords.hh"

#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"

#include "CLHEP/Geometry/Point3D.h"

#include <ostream>

BDSParticleCoords::BDSParticleCoords():
  x(0),
  y(0),
  z(0),
  xp(0),
  yp(0),
  zp(0),
  T(0)
{;}

BDSParticleCoords::BDSParticleCoords(G4double xIn,
				     G4double yIn,
				     G4double zIn,
				     G4double xpIn,
				     G4double ypIn,
				     G4double zpIn,
				     G4double tIn):
  x(xIn),
  y(yIn),
  z(zIn),
  xp(xpIn),
  yp(ypIn),
  zp(zpIn),
  T(tIn)
{;}

BDSParticleCoords::BDSParticleCoords(G4ThreeVector pos,
				     G4ThreeVector mom,
				     G4double      tIn):
  x(pos.x()),
  y(pos.y()),
  z(pos.z()),
  xp(mom.x()),
  yp(mom.y()),
  zp(mom.z()),
  T(tIn)
{;}

BDSParticleCoords BDSParticleCoords::ApplyTransform(const G4Transform3D& transform) const
{
  // if no transform, return copy of self
  if (transform == G4Transform3D::Identity)
    {return BDSParticleCoords(*this);}
  
  G4ThreeVector originalPos = G4ThreeVector(x,y,z);
  G4ThreeVector newPos      = transform * (HepGeom::Point3D<G4double>)originalPos;
  G4ThreeVector originalMom = G4ThreeVector(xp,yp,zp);
  G4ThreeVector newMom      = transform * (HepGeom::Vector3D<G4double>)originalMom;
  return BDSParticleCoords(newPos.x(), newPos.y(), newPos.z(),
			   newMom.x(), newMom.y(), newMom.z(),
			   T);
}

BDSParticleCoords BDSParticleCoords::ApplyOffset(const G4ThreeVector& offset) const
{
  return BDSParticleCoords(x + offset.x(),
			   y + offset.y(),
			   z + offset.z(),
			   xp, yp, zp, T);
}

void BDSParticleCoords::AddOffset(const G4ThreeVector& offset,
                                  G4double offsetT)
{
  x += offset.x();
  y += offset.y();
  z += offset.z();
  T += offsetT;
}

std::ostream& operator<< (std::ostream& out, BDSParticleCoords const& p)
{
  p.Print(out);
  return out;
}

void BDSParticleCoords::Print(std::ostream& out) const
{
  out <<   "Position: (" << x  << ", " << y  << ", " << z  << ")";
  out << ", Momentum: (" << xp << ", " << yp << ", " << zp << ")";
  out << ", t: " << T << G4endl;
}
