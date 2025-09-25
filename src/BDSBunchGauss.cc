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
#include "BDSBunchGauss.hh"

#include "parser/beam.h"

#include "Randomize.hh"
#include "CLHEP/RandomObjects/RandMultiGauss.h"

#include <cmath>

BDSBunchGauss::BDSBunchGauss():
  BDSBunchGaussBase("gauss")
{;}

void BDSBunchGauss::SetOptions(const BDSParticleDefinition* beamParticle,
                               const GMAD::Beam& beam,
                               const BDSBunchType& distrType,
                               G4Transform3D beamlineTransformIn,
                               const G4double beamlineSIn)
{
  BDSBunchGaussBase::SetOptions(beamParticle, beam, distrType, beamlineTransformIn, beamlineSIn);

  sigmaGM[0][0] = std::pow(beam.sigmaX, 2);
  sigmaGM[1][1] = std::pow(beam.sigmaXp,2);
  sigmaGM[2][2] = std::pow(beam.sigmaY, 2);
  sigmaGM[3][3] = std::pow(beam.sigmaYp,2);
  sigmaGM[4][4] = std::pow(sigmaT, 2); // these are made slightly finite in BDSBunchGaussBase
  sigmaGM[5][5] = std::pow(sigmaE, 2); // if 0 to ensure +ve definiteness

  delete gaussMultiGen;
  gaussMultiGen = CreateMultiGauss(*CLHEP::HepRandom::getTheEngine(),meansGM,sigmaGM);
}
