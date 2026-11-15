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
#ifndef BDSBEAMPIPEFACTORYGENERAL_H
#define BDSBEAMPIPEFACTORYGENERAL_H

#include "BDSBeamPipeFactoryBase.hh"
#include "BDSBeamPipe.hh"

class BDSBeamPipeInfo;

/**
 * @brief General factory for beam pipes that follow a set pattern.
 * 
 * @author Laurie Nevay
 */

class BDSBeamPipeFactoryGeneral: public BDSBeamPipeFactoryBase
{
public:
  BDSBeamPipeFactoryGeneral();
  virtual ~BDSBeamPipeFactoryGeneral(){;}

  BDSBeamPipe* CreateBeamPipe(const G4String&   name,
                              G4double          length,
                              const BDSBeamPipeInfo* bpi) override;
};
  
#endif
