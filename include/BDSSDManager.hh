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
#ifndef BDSSDMANAGER_H
#define BDSSDMANAGER_H

#include "BDSSDSamplerLink.hh"
#include "BDSSDType.hh"

#include "G4String.hh"
#include "G4Types.hh"
#include "G4Version.hh"

#include <map>
#include <set>
#include <vector>

class BDSSDApertureImpacts;
class BDSSDCollimator;
class BDSSDEnergyDeposition;
class BDSSDEnergyDepositionGlobal;
class BDSLinkRegistry;
class BDSMultiSensitiveDetectorOrdered;
class BDSSDFilterPDGIDSet;
class BDSSDSampler;
class BDSSDSamplerCylinder;
class BDSSDSamplerSphere;
class BDSSDTerminator;
class BDSSDThinThing;
class BDSSDVolumeExit;

class G4VSDFilter;

#if G4VERSION_NUMBER < 1030
// In this case we use only the energy counter SD and return it
// as a base class pointer. Include header so casting works.
#include "BDSSDEnergyDeposition.hh"
#include "BDSSDEnergyDepositionGlobal.hh"
#endif

/**
 * @brief A singleton class that holds all required sensitive
 * detector class instances.  
 * 
 * Each sensitive detector class
 * need only be instantiated once and attached to the relevant
 * volume. This instantiates all necessary SDs and holds them.
 * 
 * @author Laurie Nevay
 */

class BDSSDManager
{
public:
  static BDSSDManager* Instance(); /// Singleton accessor.
  
  BDSSDManager(const BDSSDManager&) = delete;
  BDSSDManager& operator=(const BDSSDManager&) = delete;
  ~BDSSDManager();

  /// Access a sensitive detector by the class enum. Default is a nullptr and also
  /// in the case of Geant < 4.10.3 for world complete as requires multiple sensitive
  /// detector. It's safe to set a nullptr to the SD of a logical volume. If applyOptions
  /// is used, the correct SD will only be returned if required according to the options
  /// in BDSGlobalConstants for storing hits. This way only hits are generated that are
  /// required (cpu and memory efficient).
  G4VSensitiveDetector* SensitiveDetector(const BDSSDType sdType,
					  G4bool applyOptions = false) const;

  /// SD for samplers (plane type). See also SamplerPlaneWithFilter below.
  inline BDSSDSampler* SamplerPlane() const {return samplerPlane;}

  /// SD for samplers (cylinder type).
  inline BDSSDSamplerCylinder* SamplerCylinder() const {return samplerCylinder;}
  
  /// SD for samplers (sphere type).
  inline BDSSDSamplerSphere* SamplerSphere() const {return samplerSphere;}

  /// SD for link samplers.
  inline BDSSDSamplerLink* SamplerLink() const {return samplerLink;}

  /// SD for measuring turns around circular machine and terminating
  /// particles appropriately.
  inline BDSSDTerminator* Terminator() const {return terminator;}
  
  /// SD for general energy deposition.
  inline BDSSDEnergyDeposition* EnergyDeposition() const {return energyDeposition;}

  /// SD for general energy deposition but always include extra half of information.
  inline BDSSDEnergyDeposition* EnergyDepositionFull() const {return energyDepositionFull;}

  /// SD for energy deposition in vacuum volumes.
  inline BDSSDEnergyDeposition* EnergyDepositionVacuum() const {return energyDepositionVacuum;}

  /// SD for tunnel energy counter.
  inline BDSSDEnergyDeposition* EnergyDepositionTunnel() const {return energyDepositionTunnel;}

  /// SD for energy deposition in the world volume.
  inline BDSSDEnergyDepositionGlobal* EnergyDepositionWorld() const {return energyDepositionWorld;}

  /// SD for energy deposition in things that were already placed in the externally provided world.
  inline BDSSDEnergyDepositionGlobal* EnergyDepositionWorldContents() const {return energyDepositionWorldContents;}

  /// SD for world exit hits.
  inline BDSSDVolumeExit* WorldExit() const {return worldExit;}

  /// SD for aperture impact hits.
  inline BDSSDApertureImpacts* ApertureImpacts() const {return apertureImpacts;}

#if G4VERSION_NUMBER > 1029
  /// SD for multiple SDs for world - energy loss and exit.
  inline G4VSensitiveDetector* WorldComplete()        const {return worldCompleteSD;}
  inline G4VSensitiveDetector* ApertureComplete()     const {return apertureCompleteSD;}
#else
  /// SD for world energy loss as in Geant earlier than 4.10.3 we can only have
  /// one SD for each logical volume.
  inline G4VSensitiveDetector* WorldComplete()        const {return energyDepositionWorld;}
  inline G4VSensitiveDetector* ApertureComplete()     const {return energyDeposition;}
#endif

  /// SD for collimator impact locations.
  inline BDSSDCollimator* Collimator() const {return collimatorSD;}

  /// SD for collimator impacts + energy deposition at the same time in order.
  inline BDSMultiSensitiveDetectorOrdered* CollimatorComplete() const {return collimatorCompleteSD;}

  /// SD for generating primary hits for thin elements where discrete processes
  /// may not work regularly.
  inline BDSSDThinThing* ThinThing() const {return thinThingSD;}

  /// SD for wire scanner wires that is a composite of thin things + energy deposition full.
  inline BDSMultiSensitiveDetectorOrdered* WireComplete() const {return wireCompleteSD;}

  /// Make a record of a primitive scorer name. If it has a '/' in it, we take the last
  /// bit of the name as the just primitive scorer name. We store both versions in member vectors.
  void RegisterPrimitiveScorerName(const G4String& nameIn, G4double unit = 1.0);

  /// Loop over a vector and apply above single name function.
  void RegisterPrimitiveScorerNames(const std::vector<G4String>& namesIn,
				    const std::vector<G4double>* units = nullptr);
  
  /// Access a vector the full primitive scorer names as registered.
  inline const std::vector<G4String>& PrimitiveScorerNamesComplete() const {return primitiveScorerNamesComplete;}

  /// Access a vector of the just primitive scorer part of the names.
  inline const std::vector<G4String>& PrimitiveScorerNames() const {return primitiveScorerNames;}

  /// Access the map of units for primitive scorers.
  inline const std::map<G4String, G4double>& PrimitiveScorerUnits() const {return primitiveScorerNameToUnit;}

  /// If samplerLink member exists, set the registry to look up links for that SD.
  void SetLinkRegistry(BDSLinkRegistry* registry);
  inline void SetLinkMinimumEK(G4double minimumEKIn) {samplerLink->SetMinimumEK(minimumEKIn);}
  inline void SetLinkProtonsAndIonsOnly(G4bool protonsAndIonsOnlyIn) {samplerLink->SetProtonsAndIonsOnly(protonsAndIonsOnlyIn);}
  
  /// Construct extra instances of the sampler SD but with a filter of a set of PDG codes. Associate
  /// it with an integer ID we can use to match up to a set. The IDs are given by the parser and we
  /// follow those in BDSIM consistently.
  void ConstructSamplerSDsForParticleSets(const std::map<int, std::set<int>>& samplerFilterIDtoPDGSetIn);

  /// Access a vector of names of extra samplers so we can identify the hits collections.
  inline const std::vector<G4String>& ExtraSamplerWithFilterNamesComplete() const {return extraSamplerWithFilterNamesComplete;}
  
  /// Access a vector of names of extra samplers so we can identify the hits collections.
  inline const std::vector<G4String>& ExtraSamplerCylinderWithFilterNamesComplete() const {return extraSamplerCylinderWithFilterNamesComplete;}
  
  /// Access a vector of names of extra samplers so we can identify the hits collections.
  inline const std::vector<G4String>& ExtraSamplerSphereWithFilterNamesComplete() const {return extraSamplerSphereWithFilterNamesComplete;}
  
  /// Access the relevant SD for a given particle filter set ID. It will return nullptr if the ID is invalid.
  BDSSDSampler* SamplerPlaneWithFilter(G4int ID) const;
  
  /// Access the relevant SD for a given particle filter set ID. It will return nullptr if the ID is invalid.
  BDSSDSamplerCylinder* SamplerCylinderWithFilter(G4int ID) const;
  
  /// Access the relevant SD for a given particle filter set ID. It will return nullptr if the ID is invalid.
  BDSSDSamplerSphere* SamplerSphereWithFilter(G4int ID) const;
  
private:
  /// Private default constructor for singleton.
  BDSSDManager();
 
  static BDSSDManager* instance;

  /// @{ SD instance.
  BDSSDSampler*                samplerPlane;
  BDSSDSamplerCylinder*        samplerCylinder;
  BDSSDSamplerSphere*          samplerSphere;
  BDSSDSamplerLink*            samplerLink;
  BDSSDTerminator*             terminator;
  BDSSDEnergyDeposition*       energyDeposition;
  BDSSDEnergyDeposition*       energyDepositionFull;
  BDSSDEnergyDeposition*       energyDepositionVacuum;
  BDSSDEnergyDeposition*       energyDepositionTunnel;
  BDSSDEnergyDepositionGlobal* energyDepositionWorld;
  BDSSDEnergyDepositionGlobal* energyDepositionWorldContents;
  BDSSDVolumeExit*             worldExit;
  BDSSDApertureImpacts*        apertureImpacts;
#if G4VERSION_NUMBER > 1029
  G4VSensitiveDetector* worldCompleteSD;
  G4VSensitiveDetector* apertureCompleteSD;
#endif
  /// @}
  BDSSDCollimator*                  collimatorSD;
  BDSMultiSensitiveDetectorOrdered* collimatorCompleteSD;
  BDSSDThinThing*                   thinThingSD;
  BDSMultiSensitiveDetectorOrdered* wireCompleteSD;

  /// Map of all filters used. This class owns a single instance of each.
  std::map<G4String, G4VSDFilter*> filters;

  /// Vector of complete (including mesh name) primitive scorer names.
  std::vector<G4String> primitiveScorerNamesComplete;

  /// Just the primitive scorer part of the name.
  std::vector<G4String> primitiveScorerNames;

  /// Map of primitive scorer names to units.
  std::map<G4String, G4double> primitiveScorerNameToUnit;
  
  std::map<G4int, BDSSDSampler*> extraSamplersWithFilters;
  std::map<G4int, BDSSDSamplerCylinder*> extraSamplerCylindersWithFilters;
  std::map<G4int, BDSSDSamplerSphere*> extraSamplerSpheresWithFilters;
  std::map<G4int, BDSSDFilterPDGIDSet*> extraSamplerFilters;
  std::vector<G4String> extraSamplerWithFilterNamesComplete;
  std::vector<G4String> extraSamplerCylinderWithFilterNamesComplete;
  std::vector<G4String> extraSamplerSphereWithFilterNamesComplete;

  /// @{ Cache of global constant option.
  G4bool   storeCollimatorHitsAll;
  G4bool   storeCollimatorHitsIons;
  G4double collimatorHitsMinimumKE;
  G4bool   generateApertureImpacts;
  G4bool   storeApertureImpactsAll;
  G4bool   storeApertureImpactsIons;
  G4double apertureImpactsMinimumKE;
  G4bool   generateELossHits;
  G4bool   generateELossVacuumHits;
  G4bool   generateELossTunnelHits;
  G4bool   generateELossWorldContents;
  G4bool   storeELossWorld;
  G4bool   storeELossExtras;
  G4bool   generateCollimatorHits;
  /// @}
};

#endif
