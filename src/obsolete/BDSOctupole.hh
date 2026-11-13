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
#ifndef BDSOCTUPOLE_H
#define BDSOCTUPOLE_H

#include "BDSMagnet.hh"

#include "globals.hh" // geant4 types / globals
#include "G4Material.hh"

class  BDSBeamPipeInfo;
struct BDSMagnetOuterInfo;

class BDSOctupole: public BDSMagnet
{
public:
  BDSOctupole(G4String            name,
	      G4double            length,
	      G4double            bTriplePrime,
	      BDSBeamPipeInfo*    beamPipeInfo,
	      BDSMagnetOuterInfo* magnetOuterInfo);
  ~BDSOctupole(){;};

protected:
  virtual void Build();
};

#endif
