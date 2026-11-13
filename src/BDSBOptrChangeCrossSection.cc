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
// this class needs headers from Geant4 10.1 onwards
#include "G4Version.hh"

#if G4VERSION_NUMBER > 1009

#include "BDSBOptrChangeCrossSection.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSPhysicsUtilities.hh"

#include "globals.hh"
#include "G4BiasingProcessInterface.hh"
#include "G4BiasingProcessSharedData.hh"
#include "G4BOptnChangeCrossSection.hh"
#include "G4InteractionLawPhysical.hh"
#include "G4IonTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4Proton.hh"
#include "G4VProcess.hh"

#include <limits>
#include <regex>

BDSBOptrChangeCrossSection::BDSBOptrChangeCrossSection(const G4String& particleNameIn,
						       const G4String& name):
  G4VBiasingOperator(name),
  fSetup(true),
  particleName(particleNameIn),
  particleIsIon(false)
{
  fParticleToBias = G4ParticleTable::GetParticleTable()->FindParticle(particleName);

  if (!fParticleToBias)
    {throw BDSException(__METHOD_NAME__, "Particle \"" + particleName + "\" not found");}
  
  particleIsIon = BDS::IsIon(fParticleToBias);
}

BDSBOptrChangeCrossSection::~BDSBOptrChangeCrossSection()
{
  for (auto change : fChangeCrossSectionOperations)
    {delete change.second;}
}

void BDSBOptrChangeCrossSection::StartRun()
{
  // Setup stage:
  // Start by collecting processes under biasing, create needed biasing
  // operations and associate these operations to the processes:
  if (fSetup)
    {
      const G4ProcessManager*           processManager = fParticleToBias->GetProcessManager();
      const G4BiasingProcessSharedData* sharedData     = G4BiasingProcessInterface::GetSharedData(processManager);
      
      if (sharedData)
	{
	  // sharedData tested, as is can happen a user attaches an operator to a
	  // volume but without defined BiasingProcessInterface processes.
    for (const auto& wrapperProcess : sharedData->GetPhysicsBiasingProcessInterfaces())
      {
	      G4String operationName = "XSchange-"+wrapperProcess->GetWrappedProcess()->GetProcessName();
	      fChangeCrossSectionOperations[wrapperProcess] = new G4BOptnChangeCrossSection(operationName);
	      fXSScale[wrapperProcess]      = 1.0;
	      fPrimaryScale[wrapperProcess] = 0;
	    }
	}
      fSetup = false;
    }
}

void BDSBOptrChangeCrossSection::SetBias(const G4String& processName,
					 G4double bias,
					 G4int    iPrimary)
{
  const G4ProcessManager*           processManager = fParticleToBias->GetProcessManager();
  const G4BiasingProcessSharedData* sharedData     = G4BiasingProcessInterface::GetSharedData(processManager);

  G4bool allProcesses = false;
  if (processName == "all")
    {allProcesses = true;}
  
  G4bool processFound = false;
  for (const auto& wrapperProcess : sharedData->GetPhysicsBiasingProcessInterfaces())
    {
      G4String currentProcess = wrapperProcess->GetWrappedProcess()->GetProcessName();
      
      // check if the name is already wrapped for biasing of some kind or splitting
      std::regex braces("[\\w\\-\\+_$]*\\((\\w+)\\)");
      //std::regex braces("[\\w\\-\\_\\+]*\\((\\w+)\\)");
      std::smatch match;
      if (std::regex_search(currentProcess, match, braces))
        {currentProcess = match[1];} // overwrite the variable to match (in this scope)
      
      if (allProcesses || processName == currentProcess)
	{
	  fXSScale[wrapperProcess]      = bias;
	  fPrimaryScale[wrapperProcess] = iPrimary;
	  processFound                  = true; // the process was found at some point
	}
    }
  if (!processFound)
    {
      G4cout << "\nCouldn't find process by name. Available processes are:" << G4endl;
      for (const auto wrapperProcess : sharedData->GetPhysicsBiasingProcessInterfaces())
	{
	  G4String currentProcessName = wrapperProcess->GetWrappedProcess()->GetProcessName();
	  G4cout << "\"" << currentProcessName << "\"" << G4endl;
	}
      throw BDSException(__METHOD_NAME__, "Process \"" + processName +
			 "\" not found registered to particle \"" + particleName + "\"");
    }
}

G4VBiasingOperation* BDSBOptrChangeCrossSection::ProposeOccurenceBiasingOperation(const G4Track*                   track, 
										  const G4BiasingProcessInterface* callingProcess)
{
  // Check if current particle type is the one to bias:
  const G4ParticleDefinition* definition = track->GetDefinition();
  if (particleIsIon)
    {// we're looking for an ion and this generally isn't an ion
      if (!G4IonTable::IsIon(definition) || definition == G4Proton::Definition())
        {return nullptr;}
      // else it's generally an ion so continue - ie apply GenericIon to any ion
    }
  else if (definition != fParticleToBias)
    {return nullptr;}

  // select and setup the biasing operation for current callingProcess:
  // Check if the analog cross-section well defined : for example, the conversion
  // process for a gamma below e+e- creation threshold has an DBL_MAX interaction
  // length. Nothing is done in this case (ie, let analog process to deal with the case)

  G4double analogInteractionLength =  callingProcess->GetWrappedProcess()->GetCurrentInteractionLength();
  if (analogInteractionLength > std::numeric_limits<double>::max()/10.)
    {return nullptr;}

  // protect against negative interaction lengths
  // sometimes this appears as -1 - exactly -1
  if (analogInteractionLength < 0)
    {return nullptr;}

  // Analog cross-section is well-defined:
  G4double analogXS = 1./analogInteractionLength;
  
  // Choose a constant cross-section bias. But at this level, this factor can be made
  // direction dependent, like in the exponential transform MCNP case, or it
  // can be chosen differently, depending on the process, etc.
  G4double XStransformation = 1.0;
  
  // fetch the operation associated to this callingProcess:
  G4BOptnChangeCrossSection* operation = fChangeCrossSectionOperations[callingProcess];
  
  // get the operation that was proposed to the process in the previous step:
  G4VBiasingOperation* previousOperation = callingProcess->GetPreviousOccurenceBiasingOperation();
  
  // check for only scaling primary
  if ( fPrimaryScale[callingProcess] == 2 && track->GetParentID() != 0 )
    {return nullptr;}
  if ( fPrimaryScale[callingProcess] == 3 && track->GetParentID() == 0 )
    {return nullptr;}
  XStransformation = fXSScale[callingProcess];

  // STB Just return the operation before the multiple sampling check
  //operation->SetBiasedCrossSection( XStransformation * analogXS );
  //operation->Sample();

  // now setup the operation to be returned to the process: this
  // consists in setting the biased cross-section, and in asking
  // the operation to sample its exponential interaction law.
  // To do this, to first order, the two lines:
  //        operation->SetBiasedCrossSection( XStransformation * analogXS );
  //        operation->Sample();
  // are correct and sufficient.
  // But, to avoid having to shoot a random number each time, we sample
  // only on the first time the operation is proposed, or if the interaction
  // occured. If the interaction did not occur for the process in the previous,
  // we update the number of interaction length instead of resampling.

  if (!previousOperation)
    {
      operation->SetBiasedCrossSection( XStransformation * analogXS );
      operation->Sample();
    }
  else
    {
      if (previousOperation != operation)
	{// should not happen !
	  //G4cout << __METHOD_NAME__ << "Logic Problem" << G4endl;
	  return nullptr;
	}
      if (operation->GetInteractionOccured())
	{
	  operation->SetBiasedCrossSection( XStransformation * analogXS );
	  operation->Sample();
	}
    else
      {
	// update the 'interaction length' and underneath 'number of interaction lengths'
	// for past step  (this takes into accout the previous step cross-section value)
	operation->UpdateForStep(callingProcess->GetPreviousStepSize());
	// update the cross-section value:
	operation->SetBiasedCrossSection(XStransformation * analogXS);
	// forces recomputation of the 'interaction length' taking into account above
	// new cross-section value [tricky : to be improved]
	operation->UpdateForStep(0.0);
      }
    }
  return operation;  
}

void BDSBOptrChangeCrossSection::OperationApplied(const G4BiasingProcessInterface* callingProcess, 
						  G4BiasingAppliedCase,
						  G4VBiasingOperation* occurenceOperationApplied,
						  G4double,
						  G4VBiasingOperation*,    
						  const G4VParticleChange*)
{
  G4BOptnChangeCrossSection* operation = fChangeCrossSectionOperations[callingProcess];
  if (operation == occurenceOperationApplied)
    {operation->SetInteractionOccured();}
}

#endif
