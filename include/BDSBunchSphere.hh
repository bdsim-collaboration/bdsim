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
#ifndef BDSBUNCHSPHERE_H
#define BDSBUNCHSPHERE_H 

#include "BDSBunch.hh"

/**
 * @brief A bunch distribution that produces an uncorrelated uniform
 * random direction distribution over a sphere. The position, energy
 * and time are all reference.
 * 
 * @author Laurie Nevay
 */

class BDSBunchSphere: public BDSBunch
{ 
public: 
  BDSBunchSphere(); 
  virtual ~BDSBunchSphere(){;}
  
  /// @{ Assignment and copy constructor not implemented nor used
  BDSBunchSphere& operator=(const BDSBunchSphere&) = delete;
  BDSBunchSphere(BDSBunchSphere&) = delete;
  /// @}

  /// Generate a random unit vector for direction and use reference spatial
  /// coordinates. No energy spread.
  virtual BDSParticleCoordsFull GetNextParticleLocal();
};

#endif
