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
#ifndef BDSEVENTINFO_H
#define BDSEVENTINFO_H

#include "BDSOutputROOTEventInfo.hh"

#include "globals.hh" // geant4 types / globals
#include "G4VUserEventInformation.hh"

#include <ctime>

/**
 * @brief Interface to store event information use G4 hooks.
 * 
 * This class inherits G4VUserEventInformation so it can be used
 * to pass information around as part of a G4Event (required by G4).
 * This is basically a wrapper for BDSOutputROOTEventInfo. Can't inherit
 * BDSOutputROOTEventInfo as the Print overload for geant4 clashes with
 * one from TObject.  Keep an instance and use that.
 * 
 * @author Laurie Nevay
 */

class BDSEventInfo: public G4VUserEventInformation
{
public:
  BDSEventInfo();
  virtual ~BDSEventInfo();

  virtual void Print() const;

  /// @{ Setters
  inline void SetStartTime(const time_t& startTimeIn)   {info->startTime = startTimeIn;}
  inline void SetStopTime(const time_t& stopTimeIn)     {info->stopTime  = stopTimeIn;}
  inline void SetDurationWall(G4float durationWallIn)   {info->durationWall = (float)durationWallIn;}
  inline void SetDurationCPU(G4float  durationCPUIn)    {info->durationCPU  = (float)durationCPUIn;}
  inline void SetSeedStateAtStart(const G4String& seedStateAtStartIn) {info->seedStateAtStart = (std::string)seedStateAtStartIn;}
  inline void SetIndex(G4int indexIn)                   {info->index     = (int)indexIn;}
  inline void SetAborted(G4bool abortedIn)              {info->aborted   = (bool)abortedIn;}
  inline void SetPrimaryHitMachine(G4bool hitIn)        {info->primaryHitMachine = (bool)hitIn;}
  inline void SetMemoryUsage(G4double memoryUsageMbIn)  {info->memoryUsageMb = (double)memoryUsageMbIn;}
  inline void SetPrimaryAbsorbedInCollimator(G4bool absorbed) {info->primaryAbsorbedInCollimator = absorbed;}
  inline void SetNTracks(long long int nTracks)         {info->nTracks = nTracks;}
  inline void SetBunchIndex(int bunchIndexIn)           {info->bunchIndex = bunchIndexIn;}
  /// @}

  /// Accessor.
  inline const BDSOutputROOTEventInfo* GetInfo() const {return info;}

  virtual void Flush() {info->Flush();}

protected:
  /// Instance of info.
  BDSOutputROOTEventInfo* info;
};

#endif
