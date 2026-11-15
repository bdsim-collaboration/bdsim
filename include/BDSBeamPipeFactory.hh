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
#ifndef BDSBEAMPIPEFACTORY_H
#define BDSBEAMPIPEFACTORY_H

#include "BDSBeamPipeType.hh"

#include "G4String.hh"
#include "G4Types.hh"

class G4Material;

class BDSBeamPipe;
class BDSBeamPipeInfo;
class BDSBeamPipeFactoryBase;

/**
 * @brief The main interface for creating beam pipes.
 * 
 * @author Laurie Nevay
 */

class BDSBeamPipeFactory
{
public:
  BDSBeamPipeFactory();
  ~BDSBeamPipeFactory();
  
  BDSBeamPipe* CreateBeamPipe(const G4String& name,
                              G4double fullLength,
                              BDSBeamPipeInfo* bpi);

private:
  /// Return the appropriate factory singleton pointer given a type.
  BDSBeamPipeFactoryBase* GetAppropriateFactory(BDSBeamPipeType beamPipeTypeIn);

  /// @{ Factory instance.
  BDSBeamPipeFactoryBase* general;
  BDSBeamPipeFactoryBase* lhcdetailed;
  /// @}
};

#endif
