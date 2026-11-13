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
#ifndef BDSLWCALORIMETERHIT_H
#define BDSLWCALORIMETERHIT_H

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

class BDSLWCalorimeterHit :public G4VHit
{
public:
  BDSLWCalorimeterHit(G4int nCopy, G4double Energy=0, G4int nEvent=0);

  virtual ~BDSLWCalorimeterHit();

  inline void* operator new(size_t) ;
  inline void operator delete(void *aHit);
   
private:
  G4double itsEnergy;
  G4int itsCopyNumber;
  G4int itsEventNo;

public:

  inline G4double GetEnergy()
    {return itsEnergy;}
  inline G4int GetCopyNumber()
    {return itsCopyNumber;}
  inline void SetEnergy(G4double Energy)
    {itsEnergy=Energy;}   
  inline void AddEnergy(G4double Energy)
    {itsEnergy+=Energy;}
  inline G4int GetEventNo() const
    {return itsEventNo;}
};


typedef G4THitsCollection<BDSLWCalorimeterHit> BDSLWCalorimeterHitsCollection;
extern G4Allocator<BDSLWCalorimeterHit> BDSLWCalorimeterHitAllocator;

inline void* BDSLWCalorimeterHit::operator new(size_t)
{
  void* aHit;
  aHit=(void*) BDSLWCalorimeterHitAllocator.MallocSingle();
  return aHit;
}

inline void BDSLWCalorimeterHit::operator delete(void *aHit)
{
 BDSLWCalorimeterHitAllocator.FreeSingle((BDSLWCalorimeterHit*) aHit);
}

#endif

///
