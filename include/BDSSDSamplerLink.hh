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
#ifndef BDSSDSAMPLERLINK_H
#define BDSSDSAMPLERLINK_H

#include "BDSHitSamplerLink.hh"
#include "BDSSensitiveDetector.hh"

#include "G4Types.hh"

class BDSLinkRegistry;

class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;

/**
 * @brief The sensitive detector class that provides sensitivity to BDSSamplerLink instances.
 *
 * It creates BDSHitSamplerLink instances for each particle impact on a sampler this SD is
 * attached to.
 * 
 * @author Laurie Nevay
 */

class BDSSDSamplerLink: public BDSSensitiveDetector
{ 
public:
  /// Construct a sampler with name and type (plane/cylinder).
  explicit BDSSDSamplerLink(const G4String& name);
  virtual ~BDSSDSamplerLink();

  /// Overridden from G4VSensitiveDetector. Creates hits collection and registers it with
  /// the hits collection of this event (HCE).
  virtual void Initialize(G4HCofThisEvent* HCE);

  /// Overridden from G4VSensitiveDetector.  Creates hit instances and appends them to the
  /// hits collection.
  virtual G4bool ProcessHits(G4Step* aStep, G4TouchableHistory* readOutTH);

  /// Provide access to last hit.
  virtual G4VHit* last() const;

  /// Update cached pointer of link registry.
  inline void SetLinkRegistry(BDSLinkRegistry* registryIn) {registry = registryIn;}

  inline void SetMinimumEK(G4double minimumEKIn) {minimumEK = minimumEKIn;}
  inline void SetProtonsAndIonsOnly(G4bool protonsAndIonsOnlyIn) {protonsAndIonsOnly = protonsAndIonsOnlyIn;}

private:
  /// The hits collection for this sensitive detector class that's owned by each instance.
  BDSHitsCollectionSamplerLink* samplerLinkCollection;

  /// The name of the hits collection that's created and registered.
  G4String itsCollectionName;

  /// Hits collection ID - an integer look up for the hits collection
  /// provided by G4SDManager (a registry) that is given to the
  /// G4HCofThisEvent (Hits collection of the event).
  G4int itsHCID;
  
  /// Cached pointer to registry as accessed many times
  BDSLinkRegistry* registry;

  /// Minimum kinetic energy to generate a hit for.
  G4double minimumEK;

  /// Whether to return protons and ions only.
  G4bool protonsAndIonsOnly;
};

#endif

