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
#include "SimplePhysics.hh"

#include "G4DecayPhysics.hh"
#include "G4EmStandardPhysics_option2.hh"
#include "G4StepLimiterPhysics.hh"

SimplePhysics::SimplePhysics()
{
  RegisterPhysics(new G4DecayPhysics());
  RegisterPhysics(new G4EmStandardPhysics_option2());

  // recommended to include step limiter physics to step limits
  // for tracking in BDSIM work as well as kinetic energy cuts.
  // BDSIM's tracking integrators work better with limits on as
  // Geant4 default steps of 10km can be problematic in strong
  // fields
  RegisterPhysics(new G4StepLimiterPhysics());
}

SimplePhysics::~SimplePhysics()
{;}
