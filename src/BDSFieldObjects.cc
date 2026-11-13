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
#include "BDSFieldManager.hh"
#include "BDSGlobalConstants.hh"
#include "BDSFieldInfo.hh"
#include "BDSFieldObjects.hh"

#include "G4ChordFinder.hh"
#include "G4ElectroMagneticField.hh"
#include "G4Field.hh"
#include "G4FieldManager.hh"
#include "G4LogicalVolume.hh"
#include "G4MagIntegratorDriver.hh" // for G4MagInt_Driver
#include "G4MagIntegratorStepper.hh"
#include "G4MagneticField.hh"
#include "G4Version.hh"

#include <vector>

BDSFieldObjects::BDSFieldObjects(const BDSFieldInfo*     infoIn,
				 G4Field*                fieldIn,
				 G4EquationOfMotion*     equationOfMotionIn,
				 G4MagIntegratorStepper* magIntegratorStepperIn,
				 G4ChordFinder*          chordFinderIn,
				 G4FieldManager*         fieldManagerIn):
  info(infoIn),
  field(fieldIn),
  equationOfMotion(equationOfMotionIn),
  magIntegratorStepper(magIntegratorStepperIn),
  chordFinder(chordFinderIn),
  fieldManager(fieldManagerIn),
  magIntDriver(nullptr)
{;}

BDSFieldObjects::BDSFieldObjects(const BDSFieldInfo*     infoIn,
				 G4ElectroMagneticField* fieldIn,
				 G4EquationOfMotion*     equationOfMotionIn,
				 G4MagIntegratorStepper* magIntegratorStepperIn):
  info(infoIn),
  field(fieldIn),
  equationOfMotion(equationOfMotionIn),
  magIntegratorStepper(magIntegratorStepperIn)
{
  G4double chordStepMinimum = info->ChordStepMinimum();
  if (chordStepMinimum <= 0)
    {chordStepMinimum = BDSGlobalConstants::Instance()->ChordStepMinimum();}
  
  magIntDriver = new G4MagInt_Driver(chordStepMinimum,
				     magIntegratorStepper,
				     magIntegratorStepper->GetNumberOfVariables());

  chordFinder  = new G4ChordFinder(magIntDriver);

  // We use our custom field manager that is a thin wrapper for the Geant4 one
  // that only identifies whether we have a primary track or not for BDSIntegratorMag
  fieldManager = new BDSFieldManager(field, chordFinder);
  if (info->IsThin())
    {SetFieldManagerOptionsThin();}
  else
    {SetFieldManagerOptions();}
}

#if G4VERSION_NUMBER > 1049
BDSFieldObjects::BDSFieldObjects(const BDSFieldInfo*     infoIn,
				 G4MagneticField*        fieldIn,
				 G4EquationOfMotion*     equationOfMotionIn,
				 G4MagIntegratorStepper* magIntegratorStepperIn):
  info(infoIn),
  field(fieldIn),
  equationOfMotion(equationOfMotionIn),
  magIntegratorStepper(magIntegratorStepperIn)
{
  G4double chordStepMinimum = info->ChordStepMinimum();
  if (chordStepMinimum <= 0)
    {chordStepMinimum = BDSGlobalConstants::Instance()->ChordStepMinimum();}
  
  magIntDriver = new G4MagInt_Driver(chordStepMinimum,
				     magIntegratorStepper,
				     magIntegratorStepper->GetNumberOfVariables());

  chordFinder  = new G4ChordFinder(magIntDriver);
  fieldManager = new G4FieldManager(field, chordFinder);

  if (info->IsThin())
    {SetFieldManagerOptionsThin();}
  else
    {SetFieldManagerOptions();}
}
#endif

BDSFieldObjects::~BDSFieldObjects()
{
  delete field;
  delete fieldManager;
  delete chordFinder;
  delete magIntegratorStepper;
  delete equationOfMotion;
  //delete magIntDriver; // not needed since deleted by chordFinder
}

void BDSFieldObjects::AttachToVolume(G4LogicalVolume* volume,
				     G4bool penetrateToDaughterVolumes) const
{
  volume->SetFieldManager(fieldManager, penetrateToDaughterVolumes);
  if (!info) // may not always exist
    {return;}

  // optionally attach user limits
  auto ul = info->UserLimits();
  if (ul)
    {AttachUserLimitsToVolume(volume, ul, penetrateToDaughterVolumes);}
}

void BDSFieldObjects::AttachToVolume(const std::vector<G4LogicalVolume*>& volumes,
				     G4bool penetrateToDaughterVolumes) const
{
  for (auto volume : volumes)
    {AttachToVolume(volume, penetrateToDaughterVolumes);}
}

void BDSFieldObjects::AttachUserLimitsToVolume(G4LogicalVolume* volume,
					       G4UserLimits*    userLimits,
					       G4bool           penetrateToDaughterVolumes) const
{
  volume->SetUserLimits(userLimits);
  if (penetrateToDaughterVolumes)
    {
      for (G4int i = 0; i < (G4int)volume->GetNoDaughters(); i++)
	{AttachUserLimitsToVolume(volume->GetDaughter(i)->GetLogicalVolume(), userLimits, penetrateToDaughterVolumes);}
    }
}

void BDSFieldObjects::SetFieldManagerOptions()
{
  BDSGlobalConstants* globals = BDSGlobalConstants::Instance();
  fieldManager->SetDeltaIntersection(globals->DeltaIntersection());
  fieldManager->SetMinimumEpsilonStep(globals->MinimumEpsilonStep());
  fieldManager->SetMaximumEpsilonStep(globals->MaximumEpsilonStep());
  fieldManager->SetDeltaOneStep(globals->DeltaOneStep());
}

void BDSFieldObjects::SetFieldManagerOptionsThin()
{
  BDSGlobalConstants* globals = BDSGlobalConstants::Instance();
  fieldManager->SetDeltaIntersection(globals->DeltaIntersection());
  fieldManager->SetMinimumEpsilonStep(globals->MinimumEpsilonStepThin());
  fieldManager->SetMaximumEpsilonStep(globals->MaximumEpsilonStepThin());
  fieldManager->SetDeltaOneStep(globals->DeltaOneStep());
}
