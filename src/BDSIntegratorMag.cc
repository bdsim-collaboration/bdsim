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
#include "BDSGlobalConstants.hh"
#include "BDSIntegratorMag.hh"
#include "BDSMagnetStrength.hh"
#include "BDSStep.hh"
#include "BDSUtilities.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ClassicalRK4.hh"

G4double BDSIntegratorMag::thinElementLength = -1; // mm
G4double BDSIntegratorMag::nominalMatrixRelativeMomCut = -1;
G4bool BDSIntegratorMag::currentTrackIsPrimary = false;

BDSIntegratorMag::BDSIntegratorMag(G4Mag_EqRhs* eqOfMIn,
				   G4int        nVariablesIn):
  G4MagIntegratorStepper(eqOfMIn, nVariablesIn),
  eqOfM(eqOfMIn),
  nVariables(nVariablesIn),
  zeroStrength(false),
  distChordPrivate(0)
{
  backupStepper = new G4ClassicalRK4(eqOfMIn, nVariablesIn);
  backupStepperMomLimit = BDSGlobalConstants::Instance()->BackupStepperMomLimit();

  if (thinElementLength < 0)
    {thinElementLength = BDSGlobalConstants::Instance()->ThinElementLength();}

  if (nominalMatrixRelativeMomCut < 0)
    {nominalMatrixRelativeMomCut = BDSGlobalConstants::Instance()->NominalMatrixRelativeMomCut();}
}
  
BDSIntegratorMag::~BDSIntegratorMag()
{
  delete backupStepper;
}

void BDSIntegratorMag::ConvertToGlobal(const G4ThreeVector& localPos,
				       const G4ThreeVector& localMom,
				       G4double             yOut[],
				       G4double             yErr[],
                                       const G4double       momScaling)
{
  BDSStep globalPosDir = ConvertToGlobalStep(localPos, localMom, false);
  G4ThreeVector globalPos = globalPosDir.PreStepPoint();
  G4ThreeVector globalMom = globalPosDir.PostStepPoint();	
  globalMom*=momScaling; // multiply the unit direction by magnitude to get momentum

  yOut[0] = globalPos.x();
  yOut[1] = globalPos.y();
  yOut[2] = globalPos.z();

  yOut[3] = globalMom.x();
  yOut[4] = globalMom.y();
  yOut[5] = globalMom.z();

  // errors
  const G4double standardError = 1e-8;
  G4ThreeVector momUnit = globalMom.unit();
  momUnit *= standardError;
  for (G4int i = 0; i < 3; i++)
    {
      yErr[i]     = momUnit[i];
      yErr[i + 3] = 1e-40;
    }
}

