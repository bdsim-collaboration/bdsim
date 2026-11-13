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
#ifndef BDSTRAJECTORY_H
#define BDSTRAJECTORY_H
#include "BDSTrajectoryOptions.hh"
#include "BDSTrajectoryPoint.hh"
#include "G4Trajectory.hh"

#include <ostream>
#include <vector>

class G4Step;
class G4Track;
class G4TrajectoryContainer;
class G4VTrajectoryPoint;

typedef std::vector<BDSTrajectoryPoint*>  BDSTrajectoryPointsContainer;

/**
 * @brief Trajectory information from track including last scatter etc.
 * 
 * BDSTrajectory stores BDSTrajectoryPoints
 *
 * @author S. Boogert
 */

class BDSTrajectory: public G4Trajectory
{
public:
  BDSTrajectory() = delete;
  BDSTrajectory(const G4Track* aTrack,
		G4bool         interactiveIn,
		const BDS::TrajectoryOptions& storageOptionsIn);
  /// copy constructor is not needed
  BDSTrajectory(BDSTrajectory &) = delete;

  virtual ~BDSTrajectory();

  inline void* operator new(size_t);
  inline void operator delete(void*);
  inline int operator == (const BDSTrajectory& right) const {return (this==&right);}

  /// Append a step point to this trajectory. This is required for the trajectory
  /// points to show up in the visualisation correctly.
  virtual void AppendStep(const G4Step* aStep);

  /// Append a step point. Use a pre-made BDSTrajectoryPoint to save creating
  /// it again, which involves coordinate transforms.
  void AppendStep(const BDSTrajectoryPoint* pointIn);

  /// Apply the options to store a trajectory extra delete them if not storing them.
  /// Used to compensate when copying a fuller primary trajectory point that will always
  /// have extra information, but may not be needed when appending to the primary trajectory.
  void CleanPoint(BDSTrajectoryPoint* point) const;

  /// Merge another trajectory into this one.
  virtual void MergeTrajectory(G4VTrajectory* secondTrajectory);

  /// Access a point - use this class's container.
  virtual G4VTrajectoryPoint* GetPoint(G4int i) const {return (*fpBDSPointsContainer)[i];}

  /// Get number of trajectory points in this trajectory.
  virtual int GetPointEntries() const {return (int)fpBDSPointsContainer->size();}

  /// Method to identify which one is a primary. Overridden in derived class.
  virtual G4bool IsPrimary() const {return false;}

  /// The index of the trajectory assigned in the output from the reduced set of
  /// indices. This is why it will not be the same as the track ID.
  inline void  SetTrajIndex(G4int trajIndexIn)                 {trajIndex = trajIndexIn;}
  inline G4int GetTrajIndex()                            const {return trajIndex;}

  /// Record the TrajIndex (i.e. output index) of the trajectory of the parent
  /// trajectory for this one.
  inline void  SetParentIndex(G4int parentIndexIn)             {parentIndex = parentIndexIn;}
  inline G4int GetParentIndex()                          const {return parentIndex;}

  /// The index of the step along the parent trajectory from which this one was created.
  inline void  SetParentStepIndex(G4int parentStepIndexIn)     {parentStepIndex = parentStepIndexIn;}
  inline G4int GetParentStepIndex()                      const {return parentStepIndex;}
  
  /// Depth in the tree. Will be filled later once all trajectories are created and sorted.
  inline G4int GetDepth() const {return depth;}
  inline void SetDepth(G4int depthIn) {depth = depthIn;}

  /// Record the parent trajectory.
  inline void  SetParent(BDSTrajectory* parentIn)              {parent = parentIn;}
  inline BDSTrajectory* GetParent()                      const {return parent;}
  inline G4int GetCreatorProcessType()                   const {return creatorProcessType;}
  inline G4int GetCreatorProcessSubType()                const {return creatorProcessSubType;}

  /// Output stream
  friend std::ostream& operator<< (std::ostream &out, BDSTrajectory const &t);

  /// Find the first point in a trajectory where the post step process isn't fTransportation
  /// AND the post step process isn't fGeneral in combination with the post step process subtype
  /// isn't step_limiter.
  BDSTrajectoryPoint* FirstInteraction() const;
  BDSTrajectoryPoint* LastInteraction()  const;

protected:
  G4int          creatorProcessType;
  G4int          creatorProcessSubType;
  G4double       weight;
  G4bool         interactive;
  G4bool         suppressTransportationAndNotInteractive;
  const BDS::TrajectoryOptions storageOptions;
  BDSTrajectory* parent;
  G4int          trajIndex;
  G4int          parentIndex;
  G4int          parentStepIndex;
  G4int          depth;

  /// Container of all points. This is really a vector so all memory is dynamically
  /// allocated and there's no need to make this dynamically allocated itself a la
  /// all Geant4 examples.
  BDSTrajectoryPointsContainer* fpBDSPointsContainer;
};

extern G4Allocator<BDSTrajectory> bdsTrajectoryAllocator;

inline void* BDSTrajectory::operator new(size_t)
{
  void* aTrajectory;
  aTrajectory = (void*)bdsTrajectoryAllocator.MallocSingle();
  return aTrajectory;
}

inline void BDSTrajectory::operator delete(void* aTrajectory)
{bdsTrajectoryAllocator.FreeSingle((BDSTrajectory*)aTrajectory);}


#endif
