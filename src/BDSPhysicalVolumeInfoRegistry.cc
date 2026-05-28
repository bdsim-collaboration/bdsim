/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2024.

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
#include "BDSDebug.hh"
#include "BDSPhysicalVolumeInfo.hh"
#include "BDSPhysicalVolumeInfoRegistry.hh"

#include "globals.hh" // geant4 globals / types
#include "G4VPhysicalVolume.hh"

#include <map>
#include <set>
#include <iomanip>

BDSPhysicalVolumeInfoRegistry* BDSPhysicalVolumeInfoRegistry::instance = nullptr;

BDSPhysicalVolumeInfoRegistry* BDSPhysicalVolumeInfoRegistry::Instance()
{
  if (!instance)
    {instance = new BDSPhysicalVolumeInfoRegistry();}
  return instance;
}

BDSPhysicalVolumeInfoRegistry::BDSPhysicalVolumeInfoRegistry():
  subElementIDCounter(0)
{
  readOutSearch = readOutRegister.begin();
  backupSearch  = backupRegister.begin();
}

BDSPhysicalVolumeInfoRegistry::~BDSPhysicalVolumeInfoRegistry()
{
  for (auto& info : pvInfosForDeletion)
    {delete info;}
  instance = nullptr;
}

void BDSPhysicalVolumeInfoRegistry::RegisterInfo(G4VPhysicalVolume*     physicalVolume,
						 BDSPhysicalVolumeInfo* info,
						 G4bool                 isReadOutVolume,
						 G4bool                 isTunnel)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "registering volume:   " << physicalVolume->GetName() << G4endl;
  G4cout << __METHOD_NAME__ << "with info:            " << *info                     << G4endl;
  G4cout << __METHOD_NAME__ << "is a read out volume: " << isReadOutVolume           << G4endl;
#endif
  if (IsRegistered(physicalVolume))
    {//uh oh - we've found it somewhere - abort
      G4cerr << __METHOD_NAME__ << physicalVolume->GetName() << " is already registered" << G4endl;
      return;
    }
  
  pvInfosForDeletion.insert(info);

  // if it's a tunnel one, register and return
  if (isTunnel)
    {
      tunnelRegister[physicalVolume] = info;
      return;
    }
  // doesn't already exist so register it
  if (isReadOutVolume)
    {readOutRegister[physicalVolume] = info;}
  else
    {backupRegister[physicalVolume] = info;}
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "component registered" << G4endl;
#endif
}

void BDSPhysicalVolumeInfoRegistry::RegisterInfo(const std::set<G4VPhysicalVolume*>& physicalVolumes,
                                                 BDSPhysicalVolumeInfo* info,
                                                 G4bool                 isReadOutVolume,
                                                 G4bool                 isTunnel)
{
  for (auto& pv : physicalVolumes)
    {RegisterInfo(pv, info, isReadOutVolume, isTunnel);}
}

BDSPhysicalVolumeInfo* BDSPhysicalVolumeInfoRegistry::GetInfo(G4VPhysicalVolume* physicalVolume,
							      G4bool             isTunnel)
{
  if (!physicalVolume)
    {return nullptr;}
  if (excludedVolumes.find(physicalVolume) != excludedVolumes.end())
    {// it was found in excluded volumes
      return nullptr;
    }
  if (isTunnel)
    {
      tunnelSearch = tunnelRegister.find(physicalVolume);
      if (tunnelSearch == tunnelRegister.end())
	{return nullptr;}
      else
	{return tunnelSearch->second;}
    }
  else if (IsRegisteredToReadOutRegister(physicalVolume))
    {return readOutSearch->second;}
  else if (IsRegisteredToBackupRegister(physicalVolume))
    {return backupSearch->second;}
  else
    {//uh oh - not found!
#ifdef BDSDEBUG
      G4cerr << __METHOD_NAME__ << "physical volume not found" << G4endl;
      G4cerr << __METHOD_NAME__ << "pv name is: " << physicalVolume->GetName() << G4endl;
#endif
      return nullptr;
    }
}

void BDSPhysicalVolumeInfoRegistry::RegisterExcludedPV(G4VPhysicalVolume* physicalVolume)
{
  excludedVolumes.insert(physicalVolume);
}

void BDSPhysicalVolumeInfoRegistry::RegisterPVsForOutput(const BDSBeamlineElement* element,
                                                         const std::set<G4VPhysicalVolume*>& physicalVolumes)
{
  pvsForAGivenElement[element] = physicalVolumes;
}

G4bool BDSPhysicalVolumeInfoRegistry::IsRegistered(G4VPhysicalVolume* physicalVolume)
{
  return (IsRegisteredToReadOutRegister(physicalVolume) || IsRegisteredToBackupRegister(physicalVolume));
}
  
G4bool BDSPhysicalVolumeInfoRegistry::IsRegisteredToReadOutRegister(G4VPhysicalVolume* physicalVolume)
{
  readOutSearch = readOutRegister.find(physicalVolume);
  if (readOutSearch == readOutRegister.end())
    {return false;}
  else
    {return true;}
}

G4bool BDSPhysicalVolumeInfoRegistry::IsRegisteredToBackupRegister(G4VPhysicalVolume* physicalVolume)
{
  backupSearch = backupRegister.find(physicalVolume);
  if (backupSearch == backupRegister.end())
    {return false;}
  else
    {return true;}
}

 G4bool BDSPhysicalVolumeInfoRegistry::IsRegisteredToTunnelRegister(G4VPhysicalVolume* physicalVolume)
 {
   tunnelSearch = tunnelRegister.find(physicalVolume);
   if (tunnelSearch == tunnelRegister.end())
     {return false;}
   else
     {return true;}
 }

std::ostream& operator<< (std::ostream& out, BDSPhysicalVolumeInfoRegistry const &r)
{
  out << "Physical Volume Registry:" << G4endl;
  out << "Read out volume register:" << G4endl;
  for (BDSPVInfoIteratorConst itRO = r.readOutRegister.begin(); itRO != r.readOutRegister.end(); ++itRO)
    {out << itRO->first->GetName() << "\t" << *(itRO->second) << G4endl;}
  out << G4endl << "General volume register:" << G4endl;
  for (BDSPVInfoIteratorConst itBU = r.backupRegister.begin(); itBU != r.backupRegister.end(); ++ itBU)
    {out << itBU->first->GetName() << "\t" << *(itBU->second) << G4endl;}
  return out;
}

const std::set<G4VPhysicalVolume*>* BDSPhysicalVolumeInfoRegistry::PVsForBeamlineElement(BDSBeamlineElement* element) const
{
  auto search = pvsForAGivenElement.find(element);
  return search != pvsForAGivenElement.end() ? &search->second : nullptr;
}

void BDSPhysicalVolumeInfoRegistry::RegisterSubElementID(G4VPhysicalVolume* pv,
                                                         const G4String&    name)
{
  if (pvToSubElementID.find(pv) != pvToSubElementID.end())
  {return;}
  pvToSubElementID[pv] = subElementIDCounter;
  subElementIDToName[subElementIDCounter] = name;
  subElementIDCounter++;
}

G4int BDSPhysicalVolumeInfoRegistry::GetSubElementID(G4VPhysicalVolume* pv) const
{
  auto it = pvToSubElementID.find(pv);
  if (it == pvToSubElementID.end())
  {return -1;}
  return it->second;
}

G4String BDSPhysicalVolumeInfoRegistry::GetSubElementName(G4int id) const
{
  auto it = subElementIDToName.find(id);
  if (it == subElementIDToName.end())
  {return "";}
  return it->second;
}

void BDSPhysicalVolumeInfoRegistry::PrintSubElementMap() const
{
  G4cout << "\n--- Sub Element ID Map ---" << G4endl;
  G4cout << std::setw(10) << std::left << "ID"
         << std::setw(40) << std::left << "Name"
         << std::setw(20) << "Address"
         << G4endl;
  G4cout << std::string(70, '-') << G4endl;
  for (const auto& entry : pvToSubElementID)
  {
    G4cout << std::setw(10) << std::left << entry.second
           << std::setw(40) << std::left << entry.first->GetName()
           << std::setw(20) << entry.first
           << G4endl;
  }
  G4cout << std::string(70, '-') << G4endl;
}