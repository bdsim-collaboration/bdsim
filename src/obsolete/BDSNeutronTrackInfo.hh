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
// * regarding  this  software system or assume any liability for its *
// * use.                                                             *
// *                                                                  *
// * This  code  implementation is the  intellectual property  of the *
// * GEANT4 collaboration.                                            *
// * By copying,  distributing  or modifying the Program (or any work *
// * based  on  the Program)  you indicate  your  acceptance of  this *
// * statement, and all its terms.                                    *
// ********************************************************************
//
//
// $Id: BDSNeutronTrackInfo.hh,v 1.1 2005/01/22 17:05:31 agapov Exp $
// GEANT4 tag $Name:  $
//
//
//---------------------------------------------------------------
//
// G4VUserTrackInformation
//
// Class Description:
//
//  Abstract class which the user can derive his/her own concrete
// class for toring user's information associating with a G4Track
// class object.
//
//  It is user's responsibility that
//   1) Concrete class derived from this class MUST use G4Allocator
//     for memory management
//   2) Construct a concrete class object and set the pointer to
//     proper G4Track object
//
//  To set a pointer of a concrete class object to G4Track in
// G4UserTrackingAction concrete implementation, given the G4Track
// object is available only by "pointer to const", SetUserTrackInformation()
// method of G4TrackingManager is available.
//
//  The concrete class object is deleted by Geant4 kernel when
// associated G4Track object is deleted.


#ifndef BDSNeutronTrackInfo_H
#define BDSNeutronTrackInfo_H 1

#include "globals.hh" 
#include "G4VUserTrackInformation.hh"
#include "G4Allocator.hh"


class BDSNeutronTrackInfo:public G4VUserTrackInformation
{
public:
  //  BDSNeutronTrackInfo():G4VUserTrackInformation(){;};
  BDSNeutronTrackInfo(){;};
  ~BDSNeutronTrackInfo(){;};

  inline void *operator new(size_t);
  inline void operator delete(void* anInfo);


  inline G4bool GetIsLogged();
  inline void SetIsLogged(G4bool logger);


public:
  void Print() const;

private:
  G4bool IsLogged;
};

inline G4bool BDSNeutronTrackInfo::GetIsLogged()
{return IsLogged;}

inline void BDSNeutronTrackInfo::SetIsLogged(G4bool logger)
{IsLogged=logger;}

inline void BDSNeutronTrackInfo::Print() const
{G4cout<<" BDSNeutronTrackInfo: IsLogged="<<IsLogged<<G4endl;}


extern G4Allocator<BDSNeutronTrackInfo> theBDSNeutronTrackInfoAllocator;

inline void* BDSNeutronTrackInfo::operator new(size_t)
{
  void* anInfo;
  anInfo = (void*)theBDSNeutronTrackInfoAllocator.MallocSingle();
  return anInfo;
}

inline void BDSNeutronTrackInfo::operator delete(void* anInfo)
{
  theBDSNeutronTrackInfoAllocator.FreeSingle((BDSNeutronTrackInfo*)anInfo);
}


#endif

