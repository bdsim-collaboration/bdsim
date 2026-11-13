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
#ifndef BDSTUNNELHIT_H
#define BDSTUNNELHIT_H

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

class BDSTunnelHit;

typedef G4THitsCollection<BDSTunnelHit> BDSTunnelHitsCollection;
extern G4Allocator<BDSTunnelHit> BDSTunnelHitAllocator;

/**
 * @brief Information for a particle hit in a section of tunnel.
 * 
 * @author Laurie Nevay
 */

class BDSTunnelHit: public G4VHit
{
public:
  /// Construct a tunnel hit. Capital letters for global coordinates
  /// and small for local. [1] Note the s coordinate may not corresponds exactly
  /// to that of the beam line due to the tangential / chord difference between
  /// the segments and the beamline.
  BDSTunnelHit(G4int    nCopyIn           = 0,    // copy number of physical volume
	       G4double energyIn          = 0,    // energy of this 'hit'
	       G4double XIn               = 0,    // global x
	       G4double YIn               = 0,    // global y
	       G4double ZIn               = 0,    // global z
	       G4double SIn               = 0,    // S coordinate [1]
	       G4double xIn               = 0,    // local x position in this section of tunnel
	       G4double yIn               = 0,    // local y position in this section of tunnel
	       G4double zIn               = 0,    // local z position in this section of tunnel
	       G4double rIn               = 0,    // radius from centre of tunnel
	       G4double thetaIn           = 0,    // angle from x unit vector clockwise
	       G4int    partIDIn          = 0,    // PDG id - particle type
	       G4double weightIn          = 1,    // weight
	       G4bool   precisionRegionIn = false,// in the precision region
	       G4int    turnsTakenIn      = 1,    // turns taken if circular
	       G4int    eventNoIn         = 0);   // event number

  virtual ~BDSTunnelHit();
  
  inline void* operator new(size_t) ;
  inline void operator delete(void *aHit);

  inline G4double GetEnergy()          const;
  inline G4int    GetCopyNumber()      const;
  inline G4double GetX()               const;
  inline G4double GetY()               const;
  inline G4double GetZ()               const;
  inline G4double GetS()               const;
  inline G4double Getx()               const; 
  inline G4double Gety()               const;
  inline G4double Getz()               const;
  inline G4double Getr()               const;
  inline G4double Gettheta()           const;
  inline G4int    GetPartID()          const;
  inline G4double GetWeight()          const; 
  inline G4bool   GetPrecisionRegion() const;
  inline G4int    GetTurnsTaken()      const;
  inline G4int    GetEventNo()         const;
  
private:
  /// private default constructor (not implemented as defaults provide this)
  BDSTunnelHit();

  G4double energy;
  ///@{ global coordinates
  G4double X;
  G4double Y;
  G4double Z;
  G4double S;
  ///@}
  ///@{ local coordinates
  G4double x;
  G4double y;
  G4double z;
  ///@}
  G4double r;
  G4double theta;
  G4double weight;
  G4int    copyNumber;
  G4int    partID;
  G4bool   precisionRegion; //Whether or not the hit is in the precision region
  G4int    turnsTaken;
  G4int    eventNo;
};

inline G4double BDSTunnelHit::GetEnergy() const
{return energy;}

inline G4double BDSTunnelHit::GetX() const
{return X;}

inline G4double BDSTunnelHit::GetY() const
{return Y;}

inline G4double BDSTunnelHit::GetZ() const
{return Z;}

inline G4double BDSTunnelHit::GetS() const
{return S;}

inline G4double BDSTunnelHit::Getx() const
{return x;}

inline G4double BDSTunnelHit::Gety() const
{return y;}

inline G4double BDSTunnelHit::Getz() const
{return z;}

inline G4double BDSTunnelHit::Getr() const
{return r;}

inline G4double BDSTunnelHit::Gettheta() const
{return theta;}

inline G4double BDSTunnelHit::GetWeight() const
{return weight;}

inline G4int BDSTunnelHit::GetCopyNumber() const
{return copyNumber;}

inline G4int BDSTunnelHit::GetPartID() const
{return partID;}

inline G4bool BDSTunnelHit::GetPrecisionRegion() const
{return precisionRegion;}

inline G4int BDSTunnelHit::GetTurnsTaken() const
{return turnsTaken;}

inline G4int BDSTunnelHit::GetEventNo() const
{return eventNo;}

inline void* BDSTunnelHit::operator new(size_t)
{
  void* aHit;
  aHit=(void*) BDSTunnelHitAllocator.MallocSingle();
  return aHit;
}

inline void BDSTunnelHit::operator delete(void *aHit)
{BDSTunnelHitAllocator.FreeSingle((BDSTunnelHit*) aHit);}

#endif
