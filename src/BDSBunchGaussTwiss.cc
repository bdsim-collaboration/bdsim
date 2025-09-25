/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2024.

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
#include "BDSBunchGaussTwiss.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"

#include "parser/beam.h"

#include "globals.hh"

#include "Randomize.hh"
#include "CLHEP/RandomObjects/RandMultiGauss.h"

#include <cmath>
#include <vector>

BDSBunchGaussTwiss::BDSBunchGaussTwiss():
  BDSBunchGaussBase("gausstwiss"),
  betaX(0.0), betaY(0.0),
  alphaX(0.0), alphaY(0.0),
  emitX(0.0), emitY(0.0),
  gammaX(0.0), gammaY(0.0),
  dispX(0.0), dispY(0.0),
  dispXP(0.0), dispYP(0.0)
{;}

void BDSBunchGaussTwiss::SetOptions(const BDSParticleDefinition* beamParticle,
                               const GMAD::Beam& beam,
                               const BDSBunchType& distrType,
                               G4Transform3D beamlineTransformIn,
                               const G4double beamlineSIn)
{
  // Fill means and class BDSBunch::SetOptions
  BDSBunchGaussBase::SetOptions(beamParticle, beam, distrType, beamlineTransformIn, beamlineSIn);

  betaX  = beam.betx;
  betaY  = beam.bety;
  alphaX = beam.alfx;
  alphaY = beam.alfy;

  G4double ex,ey; // dummy variables we don't need
  SetEmittances(beamParticle, beam, emitX, emitY, ex, ey);

  dispX  = beam.dispx;
  dispY  = beam.dispy;
  dispXP = beam.dispxp;
  dispYP = beam.dispyp;
  gammaX = (1.0+alphaX*alphaX)/betaX;
  gammaY = (1.0+alphaY*alphaY)/betaY;

  // Fill sigmas
  //2x2 block in horizontal
  sigmaGM[0][0] =  emitX*betaX + std::pow(dispX*sigmaP,2); 
  sigmaGM[0][1] = -emitX*alphaX + dispX*dispXP*std::pow(sigmaP,2);
  sigmaGM[1][0] = -emitX*alphaX + dispX*dispXP*std::pow(sigmaP,2);
  sigmaGM[1][1] =  emitX*gammaX + std::pow(dispXP*sigmaP,2);

  //2x2 block in vertical
  sigmaGM[2][2] =  emitY*betaY + std::pow(dispY*sigmaP,2);
  sigmaGM[2][3] = -emitY*alphaY + dispY*dispYP*std::pow(sigmaP,2);
  sigmaGM[3][2] = -emitY*alphaY + dispY*dispYP*std::pow(sigmaP,2);;
  sigmaGM[3][3] =  emitY*gammaY + std::pow(dispYP*sigmaP,2);

  //2 2x2 blocks for horizontal-vertical coupling
  sigmaGM[2][0] =  dispX*dispY*std::pow(sigmaP,2);
  sigmaGM[0][2] =  dispX*dispY*std::pow(sigmaP,2);
  sigmaGM[3][0] =  dispX*dispYP*std::pow(sigmaP,2);
  sigmaGM[0][3] =  dispX*dispYP*std::pow(sigmaP,2);
  sigmaGM[2][1] =  dispXP*dispY*std::pow(sigmaP,2);
  sigmaGM[1][2] =  dispXP*dispY*std::pow(sigmaP,2);
  sigmaGM[3][1] =  dispXP*dispYP*std::pow(sigmaP,2);
  sigmaGM[1][3] =  dispXP*dispYP*std::pow(sigmaP,2);

  //2x2 block in longitudinal
  sigmaGM[4][4] =  std::pow(sigmaT,2);
  sigmaGM[5][5] =  std::pow(sigmaE,2);

  //4 2x2 blocks for longitudinal-transverse coupling
  sigmaGM[0][5] = dispX*std::pow(sigmaP,2);
  sigmaGM[5][0] = dispX*std::pow(sigmaP,2);
  sigmaGM[1][5] = dispXP*std::pow(sigmaP,2);
  sigmaGM[5][1] = dispXP*std::pow(sigmaP,2);
  sigmaGM[2][5] = dispY*std::pow(sigmaP,2);
  sigmaGM[5][2] = dispY*std::pow(sigmaP,2);
  sigmaGM[3][5] = dispYP*std::pow(sigmaP,2);
  sigmaGM[5][3] = dispYP*std::pow(sigmaP,2);

  // here we force check parameters early (called in factory) so it's before
  // checking the matrix for +ve definite-ness in CreateMultiGauss to give a
  // more meaningful error. Also keeps bunch classes overall simpler.
  CheckParameters();
  
  delete gaussMultiGen;
  gaussMultiGen = CreateMultiGauss(*CLHEP::HepRandom::getTheEngine(),meansGM,sigmaGM);
}

void BDSBunchGaussTwiss::CheckParameters()
{
  BDSBunchGaussBase::CheckParameters();
  if (emitX <= 0)
    {throw BDSException(__METHOD_NAME__, "emitx must be finite!");}
  if (emitY <= 0)
    {throw BDSException(__METHOD_NAME__, "emity must be finite!");}
  if (betaX <= 0)
    {throw BDSException(__METHOD_NAME__, "betx must be finite!");}
  if (betaY <= 0)
    {throw BDSException(__METHOD_NAME__, "bety must be finite!");}
}
