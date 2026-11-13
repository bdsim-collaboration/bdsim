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
#ifndef BDSBOPTRCHANGECROSSSECTION_H
#define BDSBOPTRCHANGECROSSSECTION_H

#include "globals.hh" // geant4 types / globals
#include "G4VBiasingOperator.hh"

class G4BOptnChangeCrossSection;
class G4ParticleDefinition;

#include <map>

/** 
 * @brief Operator that changes cross section of a process for a particle.
 *
 * Based on the GB01 example from Geant4.
 * 
 * @author Stewart Boogert
 */

class BDSBOptrChangeCrossSection: public G4VBiasingOperator
{
public:
  BDSBOptrChangeCrossSection(const G4String& particleToBias,
			     const G4String& name = "ChangeXS");
  virtual ~BDSBOptrChangeCrossSection();
  
  virtual void StartRun();
  void SetBias(const G4String& processName,
	       G4double dBias,
	       G4int    iPrimary);

private: 
  virtual G4VBiasingOperation* ProposeOccurenceBiasingOperation(const G4Track* track,
								const G4BiasingProcessInterface* callingProcess);

  virtual G4VBiasingOperation* ProposeFinalStateBiasingOperation(const G4Track*, const G4BiasingProcessInterface*) 
  {return 0;}

  virtual G4VBiasingOperation* ProposeNonPhysicsBiasingOperation(const G4Track*, const G4BiasingProcessInterface*)
  {return 0;}

  using G4VBiasingOperator::OperationApplied;

  virtual void OperationApplied(const G4BiasingProcessInterface* callingProcess,
				G4BiasingAppliedCase             biasingCase,
				G4VBiasingOperation*             occurenceOperationApplied,
				G4double                         weightForOccurenceInteraction,
				G4VBiasingOperation*             finalStateOperationApplied, 
				const G4VParticleChange*         particleChangeProduced );
  std::map<const G4BiasingProcessInterface*, G4BOptnChangeCrossSection*> fChangeCrossSectionOperations;
  std::map<const G4BiasingProcessInterface*, G4double>                   fXSScale;
  std::map<const G4BiasingProcessInterface*, G4int>                      fPrimaryScale;
  G4bool                                                                 fSetup;
  G4String                                                               particleName;
  const G4ParticleDefinition*                                            fParticleToBias;
  G4bool particleIsIon;
};

#endif
