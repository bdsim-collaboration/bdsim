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
#include "BDSFieldMagDipoleOuter.hh"
#include "BDSMagnetStrength.hh"

#include "globals.hh"
#include "G4ThreeVector.hh"

#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>
#include <vector>

G4double BDSFieldMagDipoleOuter::transitionLengthScale = 1*CLHEP::cm;

BDSFieldMagDipoleOuter::BDSFieldMagDipoleOuter(const BDSMagnetStrength* strength,
					       const G4double&          poleTipRadiusIn,
                                               G4double           arbitraryScaling):
  spatialLimit(0.05*poleTipRadiusIn),
  poleTipRadius(poleTipRadiusIn),
  normalisation(1),
  maxField(0),
  initialisationPhase(true)
{
  BDSFieldMagDipole* innerField = new BDSFieldMagDipole(strength); // encapsulates logic about field parameters
  // store copy of nominal field strength in vector form
  localField = innerField->GetField(G4ThreeVector(0,0,0)); // doesn't vary with position
  m = localField.unit(); // ensure unit vector

  G4ThreeVector normalisationPoint = m*poleTipRadius;
  G4ThreeVector innerFieldValue = innerField->GetField(normalisationPoint);
  G4ThreeVector outerFieldValue = GetField(normalisationPoint);

  // include arbitrary scaling here so it can obey an arbitrary scaling
  maxField = innerFieldValue.mag() * arbitraryScaling;

  normalisation = maxField / outerFieldValue.mag();
  normalisation *= arbitraryScaling;
  if (std::isnan(normalisation))
    {
      normalisation = 0; // possible for 0 strength -> b inner = 0 / b outer = 0;
      finiteStrength = false;
    }
  initialisationPhase = false;
  delete innerField;
}

G4ThreeVector BDSFieldMagDipoleOuter::GetField(const G4ThreeVector& position,
					       const G4double       /*t*/) const
{
  if (!finiteStrength)
    {return G4ThreeVector();} // 0,0,0
  G4double rmag = position.mag();

  if (rmag < spatialLimit) // too close to current source
    {return localField;}

  // calculate the field according to a magnetic dipole m at position r.
  G4ThreeVector b = 3*position*(m.dot(position))/std::pow(rmag,5) - m/std::pow(rmag,3);

  // normalise with tanh (sigmoid-type function) between -3 and 3 in x and 0,1 in y.
  // scaled spatially in r across the transition length scale
  // tanh(x) goes from -1 to 1 -> change to 0,1 via +1 and /2
  G4double weight = (std::tanh(3*(rmag - std::abs(0.5*poleTipRadius))/(transitionLengthScale)) + 1) / 2.0;
  if (std::abs(weight) > 0.99 || std::abs(weight) < 0.01)
    {weight = std::round(weight);} // tanh is asymptotic - snap to 1.0 when beyond 0.99
  b = weight*b + (1-weight)*localField; // weighted sum of two components

  b *= normalisation;

  if (!initialisationPhase)
    {
      if (b.mag() > maxField)
        {b = b.unit() * maxField;}
    }

  return b;
}
