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
#include "BDSBunchGaussMatrix.hh"
#include "BDSDebug.hh"
#include "BDSUtilities.hh"

#include "parser/beam.h"

#include "Randomize.hh"
#include "CLHEP/RandomObjects/RandMultiGauss.h"

BDSBunchGaussMatrix::BDSBunchGaussMatrix():
  BDSBunchGaussBase("gaussmatrix")
{;}

void BDSBunchGaussMatrix::SetOptions(const BDSParticleDefinition* beamParticle,
                                     const GMAD::Beam& beam,
                                     const BDSBunchType& distrType,
                                     G4Transform3D beamlineTransformIn,
                                     const G4double beamlineSIn)
{
  BDSBunchGaussBase::SetOptions(beamParticle, beam, distrType, beamlineTransformIn, beamlineSIn);

  sigmaGM[0][0] = beam.sigma11;
  sigmaGM[0][1] = beam.sigma12;
  sigmaGM[0][2] = beam.sigma13;
  sigmaGM[0][3] = beam.sigma14;
  sigmaGM[0][4] = beam.sigma15;
  sigmaGM[0][5] = beam.sigma16;
  sigmaGM[1][1] = beam.sigma22;
  sigmaGM[1][2] = beam.sigma23;
  sigmaGM[1][3] = beam.sigma24;
  sigmaGM[1][4] = beam.sigma25;
  sigmaGM[1][5] = beam.sigma26;
  sigmaGM[2][2] = beam.sigma33;
  sigmaGM[2][3] = beam.sigma34;
  sigmaGM[2][4] = beam.sigma35;
  sigmaGM[2][5] = beam.sigma36;
  sigmaGM[3][3] = beam.sigma44;
  sigmaGM[3][4] = beam.sigma45;
  sigmaGM[3][5] = beam.sigma46;
  sigmaGM[4][4] = beam.sigma55;
  sigmaGM[4][5] = beam.sigma56;
  sigmaGM[5][5] = beam.sigma66;
  if (BDS::IsFinite(beam.sigma55, 1e-50))
    {finiteSigmaT = true;}
  if (BDS::IsFinite(beam.sigma66))
    {finiteSigmaE = true;}

  delete gaussMultiGen;
  gaussMultiGen = CreateMultiGauss(*CLHEP::HepRandom::getTheEngine(), meansGM, sigmaGM);
}
