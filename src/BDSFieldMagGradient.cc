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
#include "BDSFieldMag.hh"
#include "BDSFieldMagGradient.hh"
#include "BDSFieldMagSkew.hh"
#include "BDSInterpolatorType.hh"
#include "BDSMagnetStrength.hh"

#include "globals.hh"
#include "G4ThreeVector.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include <string>
#include <vector>

BDSFieldMagGradient::BDSFieldMagGradient()
{;}

BDSFieldMagGradient::~BDSFieldMagGradient()
{;}

G4double BDSFieldMagGradient::GetBy(BDSFieldMag* field, G4double x, G4double y) const
{
  G4ThreeVector position(x, y, 0);
  G4ThreeVector fieldAtXY = field->GetField(position);
  G4double by = fieldAtXY[1]/CLHEP::tesla;
  return by;
}

BDSMagnetStrength* BDSFieldMagGradient::CalculateMultipoles(BDSFieldMag* BField,
							    G4int        order,
							    G4double     Brho)
{
  BDSMagnetStrength* outputstrengths = new BDSMagnetStrength();
  G4double h =0.5; //distance apart in CLHEP distance units (mm) to place query points.
  
  G4double brhoinv = 1./(Brho / CLHEP::tesla / CLHEP::m);
  G4int centreIndex = 0;
  std::vector<G4double> d = PrepareValues(BField, 5, 0, h, centreIndex);
  G4int centreIndexSkew = 0;
  std::vector<std::vector<G4double>> dskew = PrepareSkewValues(BField,5,0,h,centreIndexSkew);
  // o+1 as we start from k1 upwards - ie, 0th order isn't calculated
  for (G4int o = 0; o < order; ++o)
    {
      (*outputstrengths)["k" + std::to_string(o+1)] = Derivative(d, o+1, centreIndex, h) * brhoinv;
      (*outputstrengths)["k" + std::to_string(o+1) + "s"] = Derivative(dskew[o], o+1, centreIndex, h) * brhoinv;
      
#ifdef BDSDEBUG
      G4cout << "k" << o+1 << " = " << (*outputstrengths)["k" + std::to_string(o+1)] << G4endl;
      G4cout << "k" << o+1 << "s"<< " = " << (*outputstrengths)["k" + std::to_string(o+1) + "s"]<< G4endl;
#endif
    }
  return outputstrengths;
}

std::vector<G4double> BDSFieldMagGradient::PrepareValues(BDSFieldMag* field,
							 G4int        order,
							 G4double     centreX,
							 G4double     h,
							 G4int&       centreIndex) const
{

  G4int maxN = 2*order + 1;
  centreIndex = maxN; // write out maxN to centre index
  std::vector<G4double> data(2*maxN+1); // must initialise vector as not using push_back
  
  for (G4int i = -maxN; i <= maxN; i++)
    {data[maxN + i] = GetBy(field, centreX+(G4double)i*h);}
  return data;
}

std::vector<std::vector<G4double>> BDSFieldMagGradient::PrepareSkewValues(BDSFieldMag* field,
                                                                          G4int        order,
                                                                          G4double     centreX,
                                                                          G4double     h,
                                                                          G4int&       centreIndex) const
{
  G4double rotation[5] = {CLHEP::pi/4, CLHEP::pi/6, CLHEP::pi/8, CLHEP::pi/10, CLHEP::pi/12};
  std::vector<std::vector<G4double>> skewValues(order);
  for (G4int j=0; j<order; j++)
    {
      BDSFieldMagSkew* skewField = new BDSFieldMagSkew(field, rotation[j]);
      skewValues[j] = PrepareValues(skewField, order, centreX, h, centreIndex);
      delete skewField;
    }
  return skewValues;
}

G4double BDSFieldMagGradient::Derivative(const std::vector<G4double>& data,
					 const G4int                  order,
					 const G4int                  startIndex,
					 const G4double               h) const
{
  if (order == 0)
    {return data.at(startIndex);}
  G4int subOrder = order-1;
  G4double result = -Derivative(data, subOrder,startIndex+2,h)
    + 8*Derivative(data, subOrder,startIndex+1, h)
    - 8*Derivative(data, subOrder,startIndex-1, h)
    + Derivative(data, subOrder,startIndex-2, h);
  result /= 12*(h/CLHEP::m);
  return result;
}
