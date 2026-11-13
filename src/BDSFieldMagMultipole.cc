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
#include "BDSDebug.hh"
#include "BDSFieldMagMultipole.hh"
#include "BDSMagnetStrength.hh"
#include "BDSUtilities.hh"

#include "globals.hh"
#include "G4ThreeVector.hh"

#include <cmath>
#include <vector>

BDSFieldMagMultipole::BDSFieldMagMultipole(BDSMagnetStrength const* strength,
					   G4double          const  brho,
					   G4int             const  orderIn):
  order(orderIn),
  maximumNonZeroOrder(0),
  normalComponents(strength->NormalComponents()),
  skewComponents(strength->SkewComponents())
{
  // multiple by brho to get field coefficients and work out maximum finite
  // order to loop up to
  // power is i+2 as 0th element is quadrupole coefficient which has 1/m^2
  for (G4int i = 0; i < (G4int)normalComponents.size(); i++)
    {
      normalComponents[i] *= (brho / std::pow(CLHEP::m, i+2));
      G4double kn      = normalComponents[i];
      G4bool nonZeroKN = BDS::IsFiniteStrength(kn);
      finiteStrength   = nonZeroKN || finiteStrength;
      if (nonZeroKN)
	{maximumNonZeroOrder = std::max(maximumNonZeroOrder, i);}
    }
  for (G4int i = 0; i < (G4int)skewComponents.size(); i++)
    {
      skewComponents[i] *= (brho / std::pow(CLHEP::m, i+2));
      G4double ks = skewComponents[i];
      G4bool nonZeroKS = BDS::IsFiniteStrength(ks);
      finiteStrength = nonZeroKS || finiteStrength;
      if (nonZeroKS)
	{maximumNonZeroOrder = std::max(maximumNonZeroOrder, i);}
    }
  maximumNonZeroOrder += 1; // 0 to 1 counting
  
  // safety check - ensure we're not going to a higher order than the strength
  // class supports.
  if (std::abs(order) > (G4int)normalComponents.size())
    {order = (G4int)normalComponents.size();}
}

G4ThreeVector BDSFieldMagMultipole::GetField(const G4ThreeVector &position,
					     const G4double       /*t*/) const
{
  // Translate cartesian to polar coordinates
  G4double r   = std::hypot(position.x(),position.y());
  G4double phi = 0;
  if (BDS::IsFiniteStrength(std::abs(r)))
    {phi = atan2(position.y(),position.x());}

  // compute B field in cylindrical coordinates first then convert to cartesian
  // Bx = Br * cos(phi) - Bphi * sin(phi)
  // By = Br * sin(phi) + Bphi * cos(phi)
  // where, if n=1 is for dipole:
  // Br  (n) (normal) = +Bn/(n-1)! * r^(n-1) * sin(n*phi)
  // Bphi(n) (normal) = +Bn/(n-1)! * r^(n-1) * cos(n*phi)
  // Br  (n) (skewed) = +Bn/(n-1)! * r^(n-1) * cos(n*phi)
  // Bphi(n) (skewed) = -Bn/(n-1)! * r^(n-1) * sin(n*phi)
  // cumulative variables for calculating the field

  G4double br = 0;
  G4double bphi = 0;
  // I want to use the strange convention of dipole coeff. with opposite sign -
  // then it is the same sign as angle.
  G4double ffact = -1;
  for (G4int i = 0; i < maximumNonZeroOrder; i++)
    {
      // Here we add to so order represents kn properly
      G4double o = (G4double)i+2; // the current order
      br   += normalComponents[i] * std::pow(r, o - 1) * std::sin(o * phi) / ffact; //normal
      br   -= skewComponents[i]   * std::pow(r, o - 1) * std::cos(o * phi) / ffact; //skewed

      bphi += normalComponents[i] * std::pow(r, o - 1) * std::cos(o * phi) / ffact; //normal
      bphi += skewComponents[i]   * std::pow(r, o - 1) * std::sin(o * phi) / ffact; //skewed

      // Ignore dipole component for now!
      //if(o==1) // for the angle convention
	  //{br *= -1; bphi *=-1; }

      ffact *= o;
    }

  G4ThreeVector cartesianField;
  cartesianField[0] = (br * std::cos(phi) - bphi * std::sin(phi)); // B_x
  cartesianField[1] = (br * std::sin(phi) + bphi * std::cos(phi)); // B_y
  cartesianField[2] = 0;                                           // B_z

  return cartesianField;
}



