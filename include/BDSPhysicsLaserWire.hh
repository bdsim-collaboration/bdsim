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
#ifndef BDSLASERWIREPHYSICS_H
#define BDSLASERWIREPHYSICS_H

#include "BDSSingleUse.hh"

#include "G4VPhysicsConstructor.hh"

/**
 * @brief Constructor for BDSLaserCompton process.
 * 
 * This class doesn't use the G4PhyiscsListHelper as it
 * doesn't know about our physics process.
 */

class BDSPhysicsLaserWire: public G4VPhysicsConstructor, public BDSSingleUse
{
public:
  BDSPhysicsLaserWire();
  virtual ~BDSPhysicsLaserWire();

  /// Construct electrons and positrons and the photon.
  virtual void ConstructParticle();

  /// Construct and attache BDSIM laserwire process.
  virtual void ConstructProcess();
};

#endif
