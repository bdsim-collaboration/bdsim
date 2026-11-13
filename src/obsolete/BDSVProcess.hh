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
#ifndef BDSVPROCESS_H
#define BDSVPROCESS_H

#include "G4VProcess.hh"
#include "G4ProcessType.hh"


class BDSVProcess: public G4VProcess
{
 public:
  BDSVProcess(const G4String& aName="NoName", G4ProcessType aType=fNotDefined);
  BDSVProcess(const BDSVProcess &right);
  virtual ~BDSVProcess();
  virtual G4double GetCrossSecFactor() = 0; //Want to make this polymorphically available.
};

#endif
