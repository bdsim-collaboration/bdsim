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
#ifndef BDSCCDPIXELHIT_H
#define BDSCCDPIXELHIT_H

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

class BDSCCDPixelHit :public G4VHit
{
public:
  BDSCCDPixelHit(G4int npixel,
		 G4double weight);

  ~BDSCCDPixelHit();
  
  inline void* operator new(size_t) ;
  inline void operator delete(void *aHit);
  
private:
  G4int _npixel;
  G4double _weight;
  
public:
  inline G4int npixel() const
  {return _npixel;}
  inline G4double weight() const
  {return _weight;}
};

typedef G4THitsCollection<BDSCCDPixelHit> BDSCCDPixelHitsCollection;
extern G4Allocator<BDSCCDPixelHit> BDSCCDPixelHitAllocator;

inline void* BDSCCDPixelHit::operator new(size_t)
{
  void* aHit;
  aHit=(void*) BDSCCDPixelHitAllocator.MallocSingle();
  return aHit;
}

inline void BDSCCDPixelHit::operator delete(void *aHit)
{
  BDSCCDPixelHitAllocator.FreeSingle((BDSCCDPixelHit*) aHit);
}

#endif

///
