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
#ifndef BDSCCDPIXELSD_H
#define BDSCCDPIXELSD_H

#include "G4VSensitiveDetector.hh"
#include "BDSCCDPixelHit.hh"

class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;

class BDSCCDPixelSD: public G4VSensitiveDetector
{
  
public:
  explicit BDSCCDPixelSD(G4String name);
  virtual ~BDSCCDPixelSD();
  
  virtual void Initialize(G4HCofThisEvent*HCE);
  virtual G4bool ProcessHits(G4Step*aStep,G4TouchableHistory*ROhist);
  virtual void EndOfEvent(G4HCofThisEvent*HCE);
  
private:
  BDSCCDPixelHitsCollection *CCDPixelCollection;
  //  G4bool                    StoreHit;
  //  G4String itsType;
  G4String _collectionName;
};
#endif

