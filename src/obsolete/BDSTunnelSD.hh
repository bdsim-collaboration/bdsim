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
#ifndef BDSTUNNELSD_H
#define BDSTUNNELSD_H

#include "G4VSensitiveDetector.hh"
#include "BDSTunnelHit.hh"
#include "G4Navigator.hh"
#include "G4GFlashSpot.hh"
#include "G4VGFlashSensitiveDetector.hh"

class G4VProcess;
class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;

/**
 * @brief Sensitive detector for tunnel geometry.
 *
 * @author Laurie Nevay
 */

class BDSTunnelSD: public G4VSensitiveDetector, public G4VGFlashSensitiveDetector
{

public:
  BDSTunnelSD(G4String name);
  ~BDSTunnelSD();

  virtual void   Initialize (G4HCofThisEvent* HCE);
  virtual G4bool ProcessHits(G4Step*aStep, G4TouchableHistory* ROhist);
  virtual G4bool ProcessHits(G4GFlashSpot* aSpot, G4TouchableHistory* ROhist);

private:
  /// assignment and copy constructor not implemented nor used
  BDSTunnelSD& operator=(const BDSTunnelSD&);
  BDSTunnelSD(BDSTunnelSD&);

  G4bool   verbose;
  BDSTunnelHitsCollection* tunnelHitsCollection;

  G4int    HCID;
  G4double energy;
  G4double X,Y,Z,S; // global coordinates
  G4double x,y,z;   // local coordinates
  G4double r,theta;
};



#endif

