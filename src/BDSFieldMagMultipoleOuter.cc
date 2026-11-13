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
#include "BDSFieldMagDipole.hh"
#include "BDSFieldMagMultipoleOuter.hh"
#include "BDSUtilities.hh"

#include "globals.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4TwoVector.hh"

#include "CLHEP/Units/PhysicalConstants.h"

#include <cmath>
#include <vector>

BDSFieldMagMultipoleOuter::BDSFieldMagMultipoleOuter(G4int              orderIn,
						     G4double           poleTipRadiusIn,
						     const BDSFieldMag* innerFieldIn,
						     G4bool             kPositive,
                                                     G4double           brho,
                                                     G4double           arbitraryScaling):
  order(orderIn),
  phiOffset(0),
  spatialLimit(1),
  normalisation(1), // we have to get field first to calculate the normalisation which uses it, so start with 1
  positiveField(kPositive),
  poleNOffset(0),
  poleTipRadius(poleTipRadiusIn),
  maxField(1e100),
  initialisationPhase(true)
{
  // index of which current to start at when summing - will in effect start us at + or - current
  // and therefore control the sign of the field.
  poleNOffset = brho > 0 ? 1 : 0;
  
  // we're assuming order > 0
  G4int nPoles = 2*order;
  
  // In the special case of a dipole, it can be inconsistent which parameter upstream
  // gives the field direction. e.g. we can have the field vector (0,-1,0) and +ve magnitude
  // in the "field" parameter and also "angle"=0. The parameterisation is also different from
  // sbends to kickers and kickers have a variety of their own. Therefore, we simply follow
  // the field direction, whatever it is. We get the nominal dipole field direction, then
  // build our currents in a loop starting from that angular offset in the x,y plane. Note,
  // phi = 0 is for the +x axis, and we want to start at the 12 o'clock position so a pi/2
  // offset is introduced. This is agnostic of upstream parameterisation.
  // This only applies for a dipole field.
  const auto innerDipoleField = dynamic_cast<const BDSFieldMagDipole*>(innerFieldIn);
  if (innerDipoleField)
    {
      poleNOffset = 0;        // for consistency no matter brho - already in inner field direction
      positiveField = false;  // don't fiddle the sign of the field at the end
      G4ThreeVector innerB = innerDipoleField->FieldValue();
      G4double phiXY = innerB.phi();
      phiOffset = phiXY - CLHEP::halfpi;
    }
  
  // prepare vector of infinite wire current sources
  G4TwoVector firstCurrent(poleTipRadius, 0);
  for (G4int i = 0; i < nPoles; ++i)
    {
      G4TwoVector c = firstCurrent; // copy it
      c.rotate(phiOffset + (G4double)i*CLHEP::twopi / nPoles); // rotate copy
      currents.push_back(c);
    }

  // work out a radial extent close to a current source where we artificially saturate
  // do this based on the distance between two current sources
  G4TwoVector pointA(poleTipRadius, 0);
  G4TwoVector pointB = pointA;
  pointB.rotate(CLHEP::twopi / nPoles);
  G4double interPoleDistance = (pointB - pointA).mag();
  // arbitrary -> let's say 5% of the distance between poles for saturation
  spatialLimit = 0.05*interPoleDistance;

  // query inner field at pole tip radius
  // choose a point to query carefully though
  // beside a current source (and with intialisationPhase=true) the function is unbound
  // so we choose a point halfway in between the first two current sources
  G4double angleC1 = currents[0].phi();
  G4double angleC2 = currents[1].phi();
  G4double angleAvg = (angleC1 + angleC2) / 2.0;
  G4TwoVector queryPoint;
  queryPoint.setPolar(poleTipRadius, angleAvg);
  // technically we should also rotate by phiOffset here, but that's only used for a dipole
  // and in that case it's always uniform, making no difference here, so skip it.
  G4ThreeVector poleTipPoint(queryPoint.x(), queryPoint.y(), 0);

  G4ThreeVector fieldAtPoleTip = innerFieldIn->GetField(poleTipPoint,/*t=*/0);
  G4double fieldAtPoleTipMag = fieldAtPoleTip.mag();
  
  finiteStrength = !BDS::IsFinite(fieldAtPoleTipMag) || innerFieldIn->FiniteStrength();

  // include arbitrary scaling here so it can obey an arbitrary scaling
  maxField = fieldAtPoleTipMag * arbitraryScaling;

  // we query this field object but with the normalisation initialised to 1 so it won't
  // affect the result despite using the same code
  // the inner field is by default OFF just now so won't affect this
  G4ThreeVector rawOuterlFieldAtPoleTip = GetField(poleTipPoint);
  G4double rawOuterlFieldAtPoleTipMag = rawOuterlFieldAtPoleTip.mag();
  
  // normalisation
  normalisation = fieldAtPoleTipMag / rawOuterlFieldAtPoleTipMag;
  normalisation *= arbitraryScaling;
  if (!std::isfinite(normalisation))
    {
      normalisation = 0;
      finiteStrength = false;
    }
  initialisationPhase = false;
}

G4ThreeVector BDSFieldMagMultipoleOuter::GetField(const G4ThreeVector& position,
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
      G4TwoVector val = std::pow(-1, pole+poleNOffset)*cToPosPerp.unit() * reciprocal;
      if (std::isfinite(val.x()) && std::isfinite(val.y())) // tolerate bad values
	{result += val;}
      pole++;
    }

  // get sign right to match convention
  if (positiveField)
    {result *= -1;}

  // normalisation
  result *= normalisation;

  // limit to pole tip maximum
  if (!initialisationPhase)
    {
      if ((result.mag() > maxField) || closeToPole)
	{result = result.unit() * maxField;}
    }
  
  return G4ThreeVector(result.x(), result.y(), 0);
}
