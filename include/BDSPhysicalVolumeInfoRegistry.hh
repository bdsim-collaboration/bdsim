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
#ifndef BDSPHYSICALVOLUMEINFOREGISTRY_H
#define BDSPHYSICALVOLUMEINFOREGISTRY_H

#include "globals.hh" // geant4 globals / types

#include <iterator>
#include <map>
#include <set>

class G4VPhysicalVolume;
class BDSBeamlineElement;
class BDSPhysicalVolumeInfo;

typedef std::map<G4VPhysicalVolume*, BDSPhysicalVolumeInfo*>::iterator BDSPVInfoIterator;
typedef std::map<G4VPhysicalVolume*, BDSPhysicalVolumeInfo*>::const_iterator BDSPVInfoIteratorConst;

/**
 * @brief A registry of physical volume info instances that
 * can be searched. 
 * 
 * This actually contains two registries - one
 * for read out geometry and one for general geometry. The reason 
 * for this is that the physical volumes of the general geometry are
 * much more numerous and would increase the search time of whole registry
 * - important as it'll be used for every single energy deposition track.
 * However, if the read out geometry is used for an element, then we need only
 * register that as all hits will occur inside the read out geometry and that
 * can be used for info lookup. This means only one registration per beamline
 * element. It is not a formal requirement that each component have a read out
 * geometry (yet) so this look up should work without it.  Adding all the physical
 * volumes of a component will lead to polluting the main register with many more
 * volumes. This can be revisited and simplified if we force / require that every
 * element has a read out volume.
 * 
 * @author Laurie Nevay
 */

class BDSPhysicalVolumeInfoRegistry
{
public:
  /// Singleton accessor
  static BDSPhysicalVolumeInfoRegistry* Instance();

  /// Destructor resets instance to 0 so that the class can be reinstantiated
  /// if necessary
  ~BDSPhysicalVolumeInfoRegistry();

  /// Register an instance of logical volume info with the address of the logical
  /// volume, which is used as the key to access it. Optional isReadOutVolume flag
  /// means volume info will be stored in a separate register that will be searched
  /// first to minimise search time. If not found there, the main register will be
  /// searched. Optional flag isTunnel controls whether the info is stored to a
  /// separate registry just for tunnel segments (not so many so quicker look up,
  /// and / or doesn't pollute main registry -> slower).
  void RegisterInfo(G4VPhysicalVolume*     physicalVolume,
		    BDSPhysicalVolumeInfo* info,
		    G4bool                 isReadOutVolume = false,
		    G4bool                 isTunnel = false);
  
  /// Loop version of the same function above but for a set of pvs.
  void RegisterInfo(const std::set<G4VPhysicalVolume*>& physicalVolumes,
		    BDSPhysicalVolumeInfo* info,
		    G4bool                 isReadOutVolume = false,
		    G4bool                 isTunnel = false);

  /// Get the logical volume info for a particular logical volume (by address). Note,
  /// returns null pointer if none found. If isTunnel, gets only from tunnelRegistry.
  BDSPhysicalVolumeInfo* GetInfo(G4VPhysicalVolume* logicalVolume,
				 G4bool             isTunnel = false);

  /// Register a pointer to exclude from the search. If the registry is queried with
  /// one of these pointers, it immediately returns a nullptr without complaint. This
  /// registers the pointer to (hopefully small) member vector that is queried before
  /// the main search each time.
  void RegisterExcludedPV(G4VPhysicalVolume* physicalVolume);

  /// Register a set of PVs with respect to a beamline element for the purpose of
  /// providing this information in the output. Not for memory management.
  void RegisterPVsForOutput(const BDSBeamlineElement* element,
			    const std::set<G4VPhysicalVolume*>& physicalVolumes);

  /// output stream
  friend std::ostream& operator<< (std::ostream &out, BDSPhysicalVolumeInfoRegistry const &r);
  
  /// Access a set of volumes registered for the placement of a beamline element.
  const std::set<G4VPhysicalVolume*>* PVsForBeamlineElement(BDSBeamlineElement* element) const;

  void RegisterSubElementID(G4VPhysicalVolume* pv, const G4String& name);

  G4int GetSubElementID(G4VPhysicalVolume* pv) const;

  G4String GetSubElementName(G4int id) const;

  void PrintSubElementMap() const;

  const std::map<G4int, G4String>& GetSubElementIDToNameMap() const {return subElementIDToName;}


private:
  /// Default constructor is private as singleton
  G4int subElementIDCounter;

  BDSPhysicalVolumeInfoRegistry();

  /// Check whether a physical volume is registered at all
  G4bool IsRegistered(G4VPhysicalVolume* physicalVolume);

  /// Check whether a physical volume is registered to the read out registry
  G4bool IsRegisteredToReadOutRegister(G4VPhysicalVolume* physicalVolume);

  /// Check whether a physical volume is registered ot the general backup registry
  G4bool IsRegisteredToBackupRegister(G4VPhysicalVolume* physicalVolume);

  // Check whether a physical volume is registered ot the tunnel registry
  G4bool IsRegisteredToTunnelRegister(G4VPhysicalVolume* physicalVolume);

  /// @{ Search iterator
  BDSPVInfoIterator readOutSearch;
  BDSPVInfoIterator backupSearch;
  BDSPVInfoIterator tunnelSearch;
  /// @}
  
  /// The singleton instance
  static BDSPhysicalVolumeInfoRegistry* instance;

  /// Registry is a map - note 'register' is a protected keyword.
  std::map<G4VPhysicalVolume*, BDSPhysicalVolumeInfo*> readOutRegister;
  std::map<G4VPhysicalVolume*, BDSPhysicalVolumeInfo*> backupRegister;
  std::map<G4VPhysicalVolume*, BDSPhysicalVolumeInfo*> tunnelRegister;
  std::set<G4VPhysicalVolume*> excludedVolumes;
  
  std::set<BDSPhysicalVolumeInfo*> pvInfosForDeletion;

  std::map<G4VPhysicalVolume*, G4int>  pvToSubElementID;
  std::map<G4int, G4String>            subElementIDToName;
  /// This map is kept not for memory management, but for keeping a record of PVs
  /// for each beamline element placed. This information can be written to the output.
  std::map<const BDSBeamlineElement*, std::set<G4VPhysicalVolume*> > pvsForAGivenElement;
};






#endif
