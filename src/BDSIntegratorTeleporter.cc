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
#include "BDSGlobalConstants.hh"
#include "BDSIntegratorTeleporter.hh"
#include "BDSPTCOneTurnMap.hh"
#include "BDSStep.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"

BDSIntegratorTeleporter::BDSIntegratorTeleporter(G4Mag_EqRhs*      eqOfMIn,
						 G4Transform3D     transformIn,
						 G4double          teleporterLengthIn,
						 BDSPTCOneTurnMap* oneTurnMapIn):
  BDSIntegratorMag(eqOfMIn,6),
  transform(transformIn),
  dPos(transform.getTranslation()),
  teleporterLength(teleporterLengthIn),
  oneTurnMap(oneTurnMapIn)
{
  threeDMethod = BDSGlobalConstants::Instance()->TeleporterFullTransform();
}

BDSIntegratorTeleporter::~BDSIntegratorTeleporter()
{
  delete oneTurnMap;
}

void BDSIntegratorTeleporter::Stepper(const G4double yIn[],
				      const G4double /*dxdy*/[],
				      const G4double h,
				      G4double       yOut[],
				      G4double       yErr[])
{
  for(G4int i = 0; i < nVariables; i++)
    {yErr[i] = 0;}
  SetDistChord(0);

  G4int turnsTaken = BDSGlobalConstants::Instance()->TurnsTaken();
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "turnsTaken: " << turnsTaken << G4endl;
#endif

  G4double lengthFraction = h / teleporterLength;

  // has to have completed at least 1 turn and be going forwards
  // must test for this to avoid backwards going particles getting stuck
  // also don't apply if for whatever reason the step length is less than half
  // the teleporter length -> this ensures only applied once
  if (turnsTaken > 0 && yIn[5] > 0 && lengthFraction > 0.51 && lengthFraction < 1.1)
    {
      G4ThreeVector globalPos = G4ThreeVector(yIn[0], yIn[1], yIn[2]);
      G4ThreeVector globalMom = G4ThreeVector(yIn[3], yIn[4], yIn[5]);

      G4ThreeVector globalPosAfter;
      G4ThreeVector globalMomAfter;
      G4bool shouldApplyOTMToPrimary = false;

      if (oneTurnMap)
	{shouldApplyOTMToPrimary = oneTurnMap->ShouldApplyToPrimary(globalMom.mag(), turnsTaken);}

      if (oneTurnMap && currentTrackIsPrimary && shouldApplyOTMToPrimary)
	{
#ifdef BDSDEBUG
	  G4cout << __METHOD_NAME__ << "applying 1 turn map" << G4endl;
#endif

	  // pass by reference, returning BDSIM coordinates:
	  G4double x, px, y, py, pz;
	  oneTurnMap->GetThisTurn(x, px, y, py, pz, turnsTaken);

	  // Get this for the sake of local.z, and also setting some
	  // internal state necessary for using ConvertToGlobalStep.
	  BDSStep localPosMom = ConvertToLocal(globalPos, globalMom, h, false, thinElementLength);
	  G4ThreeVector localPosition = localPosMom.PreStepPoint();
	  G4ThreeVector outLocalMomentum = G4ThreeVector(px, py, pz);
	  G4ThreeVector outLocalPosition = G4ThreeVector(x, y, localPosition.z());
	  BDSStep globalPosDir = ConvertToGlobalStep(outLocalPosition, outLocalMomentum, false);

	  // Set the output positions and momenta, including the threeDMethod from below...
	  globalPosAfter = globalPosDir.PreStepPoint() + dPos;
	  globalMomAfter = globalPosDir.PostStepPoint().transform(transform.getRotation());
#ifdef BDSDEBUG
	  G4cout << __METHOD_NAME__ << "applied the map." << G4endl;
	  G4cout << "Applying teleporter offset: " << G4endl;
#endif
	}
      else if (threeDMethod)
	{ // new method - full tranfsorm3D - works in 3d
	  // with beam line offset / rotation
#ifdef BDSDEBUG
	  G4cout << __METHOD_NAME__ << "teleporter 3d method" << G4endl;
#endif
	  globalPosAfter = globalPos + dPos;
	  globalMomAfter = globalMom.transform(transform.getRotation());
	}
      else
	{// old method - only works with beam line pointing in global (0,0,1)
	  // global to local
	  BDSStep   localPosMom = ConvertToLocal(globalPos, globalMom, h, false, thinElementLength);
	  G4ThreeVector localPosition = localPosMom.PreStepPoint();
	  G4ThreeVector localMomentum = localPosMom.PostStepPoint();
	  G4ThreeVector localPositionAfter;
	  G4ThreeVector localMomentumAfter;
	  G4ThreeVector posDelta = transform.getTranslation();
	  localPositionAfter[0] = localPosition.x() - posDelta.x();
	  localPositionAfter[1] = localPosition.y() - posDelta.y();
	  localPositionAfter[2] = localPosition.z() + h;

	  BDSStep globalPosDir = ConvertToGlobalStep(localPositionAfter, localMomentum, false);
	  globalPosAfter = globalPosDir.PreStepPoint();
	  globalMomAfter = globalPosDir.PostStepPoint();
	}

      // Whichever method gets used above, if the OTM is valid and the
      // particle is a primary but not applicable according to
      // ShouldApplyToPrimary, we should update the cached coordinates within
      // the OTM regardless.
      if (oneTurnMap && currentTrackIsPrimary && !shouldApplyOTMToPrimary)
	{
#ifdef BDSDEBUG
	  G4cout << "Updating coordinates in place of map application..." << G4endl;
#endif
	  BDSStep localPosMom = ConvertToLocal(globalPosAfter, globalMomAfter, h,
					       false, thinElementLength);
	  G4ThreeVector localPosition = localPosMom.PreStepPoint();
	  G4ThreeVector localMomentum = localPosMom.PostStepPoint();
	  oneTurnMap->UpdateCoordinates(localPosition, localMomentum,
					turnsTaken);
	}

      // Update the particle coordinates for whichever of the methods
      // above was used.
      for (G4int i = 0; i < 3; i++)
	{
	  yOut[i]   = globalPosAfter[i];
	  yOut[i+3] = globalMomAfter[i];
	}
    }
  else
    {
      // move particle to other side without affecting anything
      yOut[0] = yIn[0];
      yOut[1] = yIn[1];
      if (yIn[5] > 0) // going forwards
	{yOut[2] = yIn[2] + h;}
      else // going backwards
	{yOut[2] = yIn[2] - h;}
      yOut[3] = yIn[3];
      yOut[4] = yIn[4];
      yOut[5] = yIn[5];
    }

#ifdef BDSDEBUG
  G4ThreeVector inA = G4ThreeVector(yIn[0], yIn[1], yIn[2]);
  G4ThreeVector inB = G4ThreeVector(yIn[3], yIn[4], yIn[5]);
  G4ThreeVector outA = G4ThreeVector(yOut[0], yOut[1], yOut[2]);
  G4ThreeVector outB = G4ThreeVector(yOut[3], yOut[4], yOut[5]);
  BDSStep localPosMomIn = ConvertToLocal(inA, inB, h, false, thinElementLength);
  BDSStep localPosMomOut = ConvertToLocal(outA, outB, h, false, thinElementLength);
  G4ThreeVector localPosIn = localPosMomIn.PreStepPoint();
  G4ThreeVector localMomIn = localPosMomIn.PostStepPoint();
  G4ThreeVector localPosOut = localPosMomOut.PreStepPoint();
  G4ThreeVector localMomOut = localPosMomOut.PostStepPoint();
  std::ios_base::fmtflags ff = G4cout.flags(); // save cout flags
  G4cout.precision(10);
  G4cout << __METHOD_NAME__ << G4endl;
  G4cout << "h (step length) (metres) " << h / CLHEP::m << G4endl;
  G4cout << "Global Input (x, y, z)     " << inA / CLHEP::m << G4endl;
  G4cout << "Global Input (px, py, pz)  " << inB / CLHEP::m << G4endl;
  G4cout << "Global Output (x, y, z)    " << outA / CLHEP::m << G4endl;
  G4cout << "Global Output (px, py, pz) " << outB / CLHEP::m << G4endl;
  G4cout << "Local Input (x, y, z)      " << localPosIn / CLHEP::m << G4endl;
  G4cout << "Local Input (px, py, pz)   " << localMomIn / CLHEP::m << G4endl;
  G4cout << "Local Output (x, y, z)     " << localPosOut / CLHEP::m << G4endl;
  G4cout << "Local Output (px, py, pz)  " << localMomOut / CLHEP::m << G4endl;
  G4cout.flags(ff); // reset cout flags
#endif
}
