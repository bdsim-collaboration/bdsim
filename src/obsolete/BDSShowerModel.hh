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
#ifndef BDSSHOWERMODEL_H
#define BDSSHOWERMODEL_H

#include "GFlashShowerModel.hh"

#include  <vector>

class G4FastTrack;
class G4Region;

/**
 * @brief Revised implementation of GFlash with different fraction of shower considered.
 *
 * Not currently maintained.
 */

class BDSShowerModel: public GFlashShowerModel
{
public:
  BDSShowerModel(G4String, G4Region*);
  BDSShowerModel(G4String);
  virtual ~BDSShowerModel(){;}
private:
  G4bool CheckContainment(const G4FastTrack& fastTrack);
};
#endif

