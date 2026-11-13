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
#ifndef BDSEVENTACTION_H
#define BDSEVENTACTION_H

#include "BDSHitEnergyDeposition.hh"
#include "BDSHitSampler.hh"
#include "BDSTrajectoryFilter.hh"

#include "globals.hh" // geant4 types / globals
#include "G4UserEventAction.hh"

#include <bitset>
#include <ctime>
#include <map>
#include <string>
#include <vector>

class BDSEventInfo;
class BDSOutput;
class BDSTrajectoriesToStore;
class BDSTrajectory;
class BDSTrajectoryPrimary;
class G4Event;
class G4PrimaryVertex;

/**
 * @brief Process information at the event level.
 */

class BDSEventAction: public G4UserEventAction
{
public:
  explicit BDSEventAction(BDSOutput* outputIn);
  virtual ~BDSEventAction();
  
  virtual void BeginOfEventAction(const G4Event*);
  virtual void EndOfEventAction(const G4Event*);

  void StoreSeedState(G4String seedState) {seedStateAtStart = seedState;}
  G4int CurrentEventIndex() const {return currentEventIndex;}

  /// Flag that the primary was absorbed in a collimator - can be done externally to this class.
  void SetPrimaryAbsorbedInCollimator(G4bool stoppedIn) {primaryAbsorbedInCollimator = stoppedIn;}

  /// Update the vector of sampler IDs to match for trajectories.
  void SetSamplerIDsForTrajectories(const std::vector<G4int>& samplerIDsIn) {trajectorySamplerID = samplerIDsIn;}

  /// Interface for tracking action to increment the number of  tracks in each event.
  void IncrementNTracks() {nTracks++;}
  
  /// Append this trajectory to vector of primaries we keep to avoid sifting at the end of event.
  void RegisterPrimaryTrajectory(const BDSTrajectoryPrimary* trajectoryIn);

  /// For updating the print modulo after construciton in the case this information
  /// might come from the primary generator action later on after the even action
  /// has already been constructed.
  inline void SetPrintModulo(G4int printModuloIn) {printModulo = printModuloIn;}

protected:
  /// Sift through all trajectories (if any) and mark for storage.
  BDSTrajectoriesToStore* IdentifyTrajectoriesForStorage(const G4Event* evt,
							 G4bool verboseThisEvent,
							 BDSHitsCollectionEnergyDeposition* eCounterHits,
							 BDSHitsCollectionEnergyDeposition* eCounterFullHits,
							 const std::vector<BDSHitsCollectionSampler*>& allSamplerHits,
							 G4int nChar = 50) const;

  /// Recursively (using this function) mark each parent trajectory as true - to be stored,
  /// and also flag the bitset for 'connect' as true.
  void ConnectTrajectory(std::map<BDSTrajectory*, bool>& interestingTraj,
                         BDSTrajectory* trajectoryToConnect,
                         std::map<BDSTrajectory*, std::bitset<BDS::NTrajectoryFilters> >& trajectoryFilters) const;
  
private:
  BDSOutput* output;         ///< Cache of output instance. Not owned by this class.
  G4bool verboseEventBDSIM;
  G4int  verboseEventStart;
  G4int  verboseEventStop;
  G4bool storeTrajectory;    ///< Cache of whether to store trajectories or not.
  G4bool storeTrajectoryAll; ///< Store all trajectories irrespective of filters.
  G4bool storeTrajectorySecondary;
  G4int  printModulo;

  G4int samplerCollID_plane;      ///< Collection ID for plane sampler hits.
  G4int samplerCollID_cylin;      ///< Collection ID for cylindrical sampler hits.
  G4int samplerCollID_sphere;     ///< Collection ID for spherical sampler hits.
  G4int eCounterID;               ///< Collection ID for general energy deposition hits.
  G4int eCounterFullID;           ///< Collection ID for general energy deposition full hits.
  G4int eCounterVacuumID;         ///< Collection ID for the vacuum energy deposition hits.
  G4int eCounterTunnelID;         ///< Collection ID for the tunnel energy deposition hits.
  G4int eCounterWorldID;          ///< Collection ID for the world energy deposition hits.
  G4int eCounterWorldContentsID;  ///< Collection ID for the world energy deposition hits.
  G4int worldExitCollID;          ///< Collection ID for the world exit hits.
  G4int collimatorCollID;         ///< Collection ID for the collimator hits.
  G4int apertureCollID;           ///< Collection ID for the aperture hits.
  G4int thinThingCollID;          ///< Collection ID for the thin thing hits.
  std::map<G4String, G4int> scorerCollectionIDs; ///< Collection IDs for all scorers.
  std::map<G4String, G4int> extraSamplerCollectionIDs; ///< Collection IDs for extra samplers.
  std::map<G4String, G4int> extraSamplerCylinderCollectionIDs; ///< Collection IDs for extra samplers.
  std::map<G4String, G4int> extraSamplerSphereCollectionIDs; ///< Collection IDs for extra samplers.

  time_t startTime; ///< Time at the start of the event.
  time_t stopTime;  ///< Time at the end of the event.

  G4double starts; ///< Precise start time in seconds.
  G4double stops;  ///< Precise stop time in seconds.

  // Note that individual calls to std::clock have no meaning, only
  // the differences between them, and therefore this should not be
  // written to the output.
  std::clock_t cpuStartTime; ///< CPU time at the start of the event.

  G4bool primaryAbsorbedInCollimator; ///< Whether primary stopped in a collimator.

  /// @{ Cache of variable from global constants.
  G4bool   trajectoryFilterLogicAND;
  G4double trajectoryEnergyThreshold;
  G4double trajectoryCutZ;
  G4double trajectoryCutR;
  G4bool   trajConnect;
  G4String trajParticleNameToStore;
  G4String trajParticleIDToStore;
  std::vector<int> trajParticleIDIntToStore;
  G4int            trajDepth;
  std::vector<int> trajectorySamplerID;
  std::vector<std::pair<double,double>> trajSRangeToStore;
  std::bitset<BDS::NTrajectoryFilters>  trajFiltersSet;
  /// @}

  std::string seedStateAtStart; ///< Seed state at start of the event.
  G4int currentEventIndex;

  /// A copy of the pointer to event info instance that is registered to the event. Geant4
  /// deletes this as necessary.
  BDSEventInfo* eventInfo;

  long long int nTracks; ///< Accumulated number of tracks for the event.
  
  /// Cache of primary trajectories as constructed. Do this as a map because
  /// the primary trajectory may be update and appended (merged) at some point
  /// leading to more than one temporary object per final one trajectory. Therefore
  /// we can't end up with degenerate ones here.
  std::map<G4int, const BDSTrajectoryPrimary*> primaryTrajectoriesCache;// Cache of primary trajectories as constructed
};

#endif

