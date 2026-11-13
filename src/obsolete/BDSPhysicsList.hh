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
#ifndef BDSPHYSICSLIST_H
#define BDSPHYSICSLIST_H

#include "G4VModularPhysicsList.hh"
#include "globals.hh"

#include "G4Cerenkov.hh"
#include "G4Scintillation.hh"
#include "G4OpAbsorption.hh"
#include "G4OpRayleigh.hh"
#include "G4OpMieHG.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4CoulombScattering.hh"

#include <vector>

class BDSPhysicsList: public G4VUserPhysicsList
{
public:
  BDSPhysicsList(G4String physicsListNameIn);
  virtual ~BDSPhysicsList();

public:

  void AddParameterisation();
  
  virtual void ConstructParticle();
  virtual void ConstructProcess();

  virtual void SetCuts();

  // construct particular physics list processes

  void ConstructEM();
  void ConstructEMSingleScatter();
  void ConstructEMMisc();
  void ConstructMultipleScattering();
  void ConstructCoulombScattering();

  void ConstructMuon();

  void ConstructDecay();

  void ConstructOptical();

  void ConstructMerlin();

  void ConstructEM_Low_Energy();

  void ConstructLaserWire();

  void ConstructHadronic();

  void ConstructSR();
  
private:
  /// Private default constructor to force use of provided one.
  BDSPhysicsList();
  
  G4String physicsListName;
  
  G4bool verbose;

  /// A vector of all the physics lists names
  std::vector<G4String> physicsListNames;

  G4Cerenkov*          theCerenkovProcess;
  G4Scintillation*     theScintillationProcess;
  G4OpAbsorption*      theAbsorptionProcess;
  G4OpRayleigh*        theRayleighScatteringProcess;
  G4OpMieHG*           theMieHGScatteringProcess;
  G4OpBoundaryProcess* theBoundaryProcess;

  /// physics lists
  // standard physics list
  G4VPhysicsConstructor* theReferenceHadronicPhysList;
  G4VPhysicsConstructor* theReferenceEmPhysList;
  // BDSIM physics lists
  G4VPhysicsConstructor* theBDSIMPhysList;
  // hadronic physics lists
  G4VPhysicsConstructor* theHadPhysList1;
  G4VPhysicsConstructor* theHadPhysList2;
};

#endif



