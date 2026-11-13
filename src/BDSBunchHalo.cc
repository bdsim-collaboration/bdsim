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
#include "BDSBunchHalo.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSParticleCoordsFull.hh"

#include "parser/beam.h"

#include "globals.hh" // geant4 types / globals

#include "Randomize.hh"
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>

BDSBunchHalo::BDSBunchHalo():
  BDSBunch("halo"),
  alphaX(0.0), alphaY(0.0),
  betaX(0.0), betaY(0.0),
  emitX(0.0), emitY(0.0),
  gammaX(0.0), gammaY(0.0),
  sigmaX(0.0), sigmaY(0.0),
  sigmaXp(0.0), sigmaYp(0.0),
  haloNSigmaXInner(0.0), haloNSigmaXOuter(0.0),
  haloNSigmaYInner(0.0), haloNSigmaYOuter(0.0),
  haloXCutInner(0.0), 
  haloYCutInner(0.0),
  haloXCutOuter(0.0),
  haloYCutOuter(0.0),
  haloXpCutInner(0.0),
  haloYpCutInner(0.0),
  haloXpCutOuter(0.0),
  haloYpCutOuter(0.0),
  haloPSWeightParameter(0.0),
  weightFunction(""),  
  emitInnerX(0.0), emitInnerY(0.0),
  emitOuterX(0.0), emitOuterY(0.0),
  xMax(0.0), yMax(0.0),
  xpMax(0.0), ypMax(0.0)
{;}

BDSBunchHalo::~BDSBunchHalo() 
{;}

void  BDSBunchHalo::SetOptions(const BDSParticleDefinition* beamParticle,
                               const GMAD::Beam& beam,
                               const BDSBunchType& distrType,
                               G4Transform3D beamlineTransformIn,
                               const G4double beamlineSIn)
{
  BDSBunch::SetOptions(beamParticle, beam, distrType, beamlineTransformIn, beamlineSIn);
  alphaX                = G4double(beam.alfx);
  alphaY                = G4double(beam.alfy);
  betaX                 = G4double(beam.betx);
  betaY                 = G4double(beam.bety);
  gammaX                = (1.0+alphaX*alphaX)/betaX;
  gammaY                = (1.0+alphaY*alphaY)/betaY;
  haloNSigmaXInner      = G4double(beam.haloNSigmaXInner);
  haloNSigmaXOuter      = G4double(beam.haloNSigmaXOuter);
  haloNSigmaYInner      = G4double(beam.haloNSigmaYInner);
  haloNSigmaYOuter      = G4double(beam.haloNSigmaYOuter);
  haloXCutInner         = G4double(beam.haloXCutInner);
  haloYCutInner         = G4double(beam.haloYCutInner);  
  haloXCutOuter         = G4double(beam.haloXCutOuter);
  haloYCutOuter         = G4double(beam.haloYCutOuter);
  haloXpCutInner        = G4double(beam.haloXpCutInner);
  haloYpCutInner        = G4double(beam.haloYpCutInner);
  haloXpCutOuter        = G4double(beam.haloXpCutOuter);
  haloYpCutOuter        = G4double(beam.haloYpCutOuter);
  haloPSWeightParameter = G4double(beam.haloPSWeightParameter);
  weightFunction = G4String(beam.haloPSWeightFunction);

  G4double ex,ey; // dummy variables we don't need
  SetEmittances(beamParticle, beam, emitX, emitY, ex, ey);

  sigmaX                = std::sqrt(emitX * betaX);
  sigmaY                = std::sqrt(emitY * betaY);
  sigmaXp               = std::sqrt(gammaX * emitX);
  sigmaYp               = std::sqrt(gammaY * emitY);

  emitInnerX = std::pow(haloNSigmaXInner, 2) * emitX;
  emitInnerY = std::pow(haloNSigmaYInner, 2) * emitY;
  emitOuterX = std::pow(haloNSigmaXOuter, 2) * emitX;
  emitOuterY = std::pow(haloNSigmaYOuter, 2) * emitY;  

  xMax  = haloNSigmaXOuter * sigmaX;
  yMax  = haloNSigmaYOuter * sigmaY;
  xpMax = std::sqrt(std::pow(haloNSigmaXOuter, 2) * emitX * gammaX);
  ypMax = std::sqrt(std::pow(haloNSigmaYOuter, 2) * emitY * gammaY);
  
  CheckParameters(); 
}

BDSParticleCoordsFull BDSBunchHalo::GetNextParticleLocal()  
{
  // Central orbit 
  G4double x = X0;
  G4double y = Y0;
  G4double z = Z0;
  G4double xp = Xp0;
  G4double yp = Yp0;

  //  z += (T0 - envelopeT * (1.-2.*flatGen->shoot())) * CLHEP::c_light * CLHEP::s;
  z = 0;

  while (true)
  {
    G4double dx  = xMax  * (1 - 2 * G4RandFlat::shoot());
    G4double dy  = yMax  * (1 - 2 * G4RandFlat::shoot());
    G4double dxp = xpMax * (1 - 2 * G4RandFlat::shoot());
    G4double dyp = ypMax * (1 - 2 * G4RandFlat::shoot());
    
    // compute single particle emittance 
    double emitXSp = gammaX * std::pow(std::abs(dx), 2) + (2. * alphaX * dx * dxp) + betaX * std::pow(std::abs(dxp), 2);
    double emitYSp = gammaY * std::pow(std::abs(dy), 2) + (2. * alphaY * dy * dyp) + betaY * std::pow(std::abs(dyp), 2);

    // check if particle is within normal beam core, if so continue generation
    // also check if particle is within the desired cut.
    if ((std::abs(emitXSp) < emitInnerX || std::abs(emitYSp) < emitInnerY) ||
        (std::abs(emitXSp) > emitOuterX || std::abs(emitYSp) > emitOuterY)  ||
        (std::abs(dx)  < (haloXCutInner * sigmaX)) ||
        (std::abs(dy)  < (haloYCutInner * sigmaY)) ||
        (std::abs(dx)  > (haloXCutOuter * sigmaX)) ||
        (std::abs(dy)  > (haloYCutOuter * sigmaY)) ||
        (std::abs(dxp)  < (haloXpCutInner * sigmaXp)) ||
        (std::abs(dyp)  < (haloYpCutInner * sigmaYp)) ||
        (std::abs(dxp)  > (haloXpCutOuter * sigmaXp)) ||
        (std::abs(dyp)  > (haloYpCutOuter * sigmaYp)) )
      {
        continue;
      }
    else
      {
        // determine weight, initialise 1 so always passes
        double wx = 1.0;
        double wy = 1.0;
        if (weightFunction == "flat" || weightFunction.empty() || weightFunction == "one")
          {
            wx = 1.0;
            wy = 1.0;
          }
        else if (weightFunction == "oneoverr")
          {
            //abs because power of double - must be positive
            wx = std::pow(std::abs(emitInnerX / emitXSp), haloPSWeightParameter);
            wy = std::pow(std::abs(emitInnerY / emitYSp), haloPSWeightParameter);
          }
        else if (weightFunction == "oneoverrsqrd")
          {
            //abs because power of double - must be positive
            double eXsqrd = std::pow(std::abs(emitXSp), 2);
            double eYsqrd = std::pow(std::abs(emitYSp), 2);
            double eXInsq = std::pow(std::abs(emitInnerX), 2);
            double eYInsq = std::pow(std::abs(emitInnerY), 2);
            wx = std::pow(std::abs(eXInsq / eXsqrd), haloPSWeightParameter);
            wy = std::pow(std::abs(eYInsq / eYsqrd), haloPSWeightParameter);
          }
        else if (weightFunction == "exp")
          {
            wx = std::exp(-(emitXSp * haloPSWeightParameter) / (emitInnerX));
            wy = std::exp(-(emitYSp * haloPSWeightParameter) / (emitInnerY));
          }
        
#ifdef BDSDEBUG
        G4cout << __METHOD_NAME__ << emitXSp/emitX << " " << emitYSp/emitY << " " << wx << " " << wy << G4endl;
#endif
        // reject
        if (G4RandFlat::shoot() > wx && G4RandFlat::shoot() > wy)
          {continue;}
        
        // add to reference orbit 
        x += dx * CLHEP::m;
        y += dy * CLHEP::m;
        xp += dxp * CLHEP::rad;
        yp += dyp * CLHEP::rad;
        
        G4double zp = CalculateZp(xp, yp, Zp0);
        
#ifdef BDSDEBUG
        G4cout << __METHOD_NAME__ << "selected> " << dx << " " << dy << " " << dxp << " " << dyp << G4endl;
#endif
        // E0 and T0 from base class and already in G4 units
        BDSParticleCoordsFull result(x,y,z,xp,yp,zp,T0,S0+z,E0,/*weight=*/1.0);
        return result;
      }
  }
}

void BDSBunchHalo::CheckParameters()
{
  BDSBunch::CheckParameters();
  
  if (emitX <= 0)
    {throw BDSException(__METHOD_NAME__, "emitx must be finite!");}
  if (emitY <= 0)
    {throw BDSException(__METHOD_NAME__, "emity must be finite!");}
  
  if (betaX <= 0)
    {throw BDSException(__METHOD_NAME__, "betx must be finite!");}
  if (betaY <= 0)
    {throw BDSException(__METHOD_NAME__, "bety must be finite!");}

  std::vector<G4String> weightFunctions = {"", "one", "flat","oneoverr", "oneoverrsqrd", "exp"};
  auto search = std::find(weightFunctions.begin(), weightFunctions.end(), weightFunction);
  if (search == weightFunctions.end())
    {
      G4cerr << __METHOD_END__ << "invalid haloPSWeightFunction \"" << weightFunction << "\"" << G4endl;
      G4cout << "Available weight functions are:" << G4endl;
      for (const auto& w : weightFunctions)
        {G4cout << w << G4endl;}
      throw BDSException(__METHOD_END__, "");
    }
  
  if (haloNSigmaXInner <= 0)
    {throw BDSException(__METHOD_NAME__, "haloNSigmaXInner <= 0");}
  
  if (haloNSigmaYInner <= 0)
    {throw BDSException(__METHOD_NAME__, "haloNSigmaYInner <= 0");}
  
  if (haloNSigmaXInner > haloNSigmaXOuter)
    {throw BDSException(__METHOD_NAME__, "haloNSigmaXInner cannot be less than haloNSigmaXOuter");}
  
  if (haloNSigmaYInner > haloNSigmaYOuter)
    {throw BDSException(__METHOD_NAME__, "haloNSigmaYInner cannot be less than haloNSigmaYOuter");}

  if (haloXCutInner < 0)
    {throw BDSException(__METHOD_NAME__, "haloXCutInner < 0");}

  if (haloYCutInner < 0)
    {throw BDSException(__METHOD_NAME__, "haloYCutInner < 0");}

  if (haloXCutOuter <= haloXCutInner)
    {throw BDSException(__METHOD_NAME__, "haloXCutOuter must be greater than haloXCutInner!");}

  if (haloYCutOuter <= haloYCutInner)
    {throw BDSException(__METHOD_NAME__, "haloYCutOuter must be greater than haloYCutInner!");}

  if (haloXpCutInner < 0)
    {throw BDSException(__METHOD_NAME__, "haloXpCutInner < 0");}

  if (haloYpCutInner < 0)
    {throw BDSException(__METHOD_NAME__, "haloYpCutInner < 0");}

  if (haloXpCutOuter <= haloXpCutInner)
    {throw BDSException(__METHOD_NAME__, "haloXpCutOuter must be greater than haloXpCutInner!");}

  if (haloYpCutOuter <= haloYpCutInner)
    {throw BDSException(__METHOD_NAME__, "haloYpCutOuter must be greater than haloYpCutInner!");}
}
