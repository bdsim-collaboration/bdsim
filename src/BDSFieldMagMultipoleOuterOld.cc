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
#include "BDSFieldMagMultipoleOuterOld.hh"

#include "globals.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4TwoVector.hh"

#include "CLHEP/Units/PhysicalConstants.h"

#include <cmath>
#include <vector>

BDSFieldMagMultipoleOuterOld::BDSFieldMagMultipoleOuterOld(G4int              orderIn,
						     G4double           poleTipRadiusIn,
						     const BDSFieldMag* innerFieldIn,
						     G4bool             kPositive,
                                                           G4double           arbitraryScaling):
  order(orderIn),
  normalisation(1), // we have to get field first to calculate the normalisation which uses it, so start with 1
  positiveField(kPositive),
  poleTipRadius(poleTipRadiusIn)
{
  G4int nPoles = 2*order;

  // prepare vector of infinite wire current sources
  G4TwoVector firstCurrent(poleTipRadius, 0);
  for (G4int i = 0; i < nPoles; ++i)
    {
      G4TwoVector c = firstCurrent; // copy it
      c.rotate((G4double)i*CLHEP::twopi / nPoles); // rotate copy
      currents.push_back(c);
    }

  // query inner field - at point just outside pole tip
  G4ThreeVector poleTipPoint = G4ThreeVector(0, poleTipRadius, 0);
  G4double angleOffset = CLHEP::twopi/(G4double)nPoles;
  poleTipPoint.rotateZ(0.5*angleOffset); // rotate from 0,1 to the pole position (different for each magnet).
  G4ThreeVector fieldAtPoleTip = innerFieldIn->GetField(poleTipPoint,/*t=*/0);
  G4double fieldAtPoleTipMag = fieldAtPoleTip.mag();

  // we query this field object but with the normalisation initialised to 1 so it won't
  // affect the result despite using the same code
  // the inner field is by default OFF just now so won't affect this
  G4ThreeVector rawOuterlFieldAtPoleTip = GetField(poleTipPoint);
  G4double rawOuterlFieldAtPoleTipMag = rawOuterlFieldAtPoleTip.mag();
  
  // normalisation
  normalisation = arbitraryScaling * fieldAtPoleTipMag / rawOuterlFieldAtPoleTipMag;
  if (!std::isfinite(normalisation))
    {
      normalisation = 0;
      finiteStrength = false;
    }
}

G4ThreeVector BDSFieldMagMultipoleOuterOld::GetField(const G4ThreeVector& position,
						  const G4double       /*t*/) const
{
  G4TwoVector pos(position.x(), position.y());

  // temporary variables
  G4TwoVector result;
  G4TwoVector cToPos;
  G4double cToPosMag  = 0;
  G4double reciprocal = 0;
  G4TwoVector cToPosPerp;

  // loop over linear sum from all infinite wire sources
  G4int pole = 1; // counter
  const G4double spatialLimit = 6; // mm
  G4bool closeToPole = false;
  for (const auto& c : currents)
    {
      cToPos    = pos - c; // distance to this wire
      cToPosMag = cToPos.mag();
      if (cToPosMag < spatialLimit)
	{// we're close to a pole
	  // for the contribution from this pole, resample at spatial limit r
	  // from the current point - will give same direction
	  pos += cToPos.unit() * spatialLimit;
	  cToPos      = pos - c;
	  cToPosMag   = cToPos.mag();
	  closeToPole = true;
	}
      
      reciprocal = 1/cToPosMag;
      if (!std::isnormal(reciprocal))
	{reciprocal = 1.0;} // protect against bad values
      cToPosPerp = G4TwoVector(-cToPos.y(), cToPos.x());
      G4TwoVector val = std::pow(-1, pole+1)*cToPosPerp.unit() * reciprocal;
      if (std::isfinite(val.x()) && std::isfinite(val.y())) // tolerate bad values
	{result += val;}
      pole++;
    }

  // limit to pole tip maximum - 0.1 empirical factor to match
  if (closeToPole)
    {result = result.unit()*0.1;}

  // get sign right to match convention
  if (positiveField)
    {result *= -1;}

  // normalisation
  result *= normalisation;

  return G4ThreeVector(result.x(), result.y(), 0);
}
