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
#ifndef BDSPLANCKSCATTERBUILDER_H
#define BDSPLANCKSCATTERBUILDER_H

#include "globals.hh"
#include "G4ios.hh"

#include "BDSPlanckScatter.hh"
#include "G4VProcess.hh"

class BDSPlanckScatterBuilder 
{
public: 
  BDSPlanckScatterBuilder();
  virtual ~BDSPlanckScatterBuilder();
  
public: 
  void Build();
  
protected:
  BDSPlanckScatter thePlanckScatterProcess;
};
// 2003 by G.A Blair (after J.P. Wellisch)
#endif





