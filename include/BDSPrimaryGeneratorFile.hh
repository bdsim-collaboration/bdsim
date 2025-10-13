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
#ifndef BDSPRIMARYGENERATORFILE_H
#define BDSPRIMARYGENERATORFILE_H

#include "G4ThreeVector.hh"
#include "G4Types.hh"
#include "G4VPrimaryGenerator.hh"

class BDSBunch;
class BDSBunchEventGenerator;
class G4Event;
class G4VSolid;

namespace GMAD
{
  class Beam;
}

/**
 * @brief Common interface for any primary generators that are file based.
 *
 * Any derived class must implement G4VPrimaryGeneratorAction::GeneratePrimaryVertex(event).
 * In this, it must update the protected members in this class of endOfFileReached and
 * vertexGeneratedSuccessfully.
 *
 * Matching the file length is done externally but by using the DistributionIsFinished() method
 * in this class. Whether looping is permitted is known about in this class.
 *
 * @author Laurie Nevay
 */

class BDSPrimaryGeneratorFile: public G4VPrimaryGenerator
{
public:
  BDSPrimaryGeneratorFile() = delete;
  BDSPrimaryGeneratorFile(G4bool loopFileIn,
                          BDSBunchEventGenerator* bunchIn);
  virtual ~BDSPrimaryGeneratorFile();
  
  /// Unbound function to construct the right generator from file. Can return nullptr
  /// if no generator is required according to the bunch distribution type.
  static BDSPrimaryGeneratorFile* ConstructGenerator(const GMAD::Beam& beam,
                                                     BDSBunch* bunchIn,
                                                     G4bool recreate,
                                                     G4int eventOffset,
                                                     G4bool batchMode);

  /// Return false if not able to generate a primary vertex.
  G4bool GeneratePrimaryVertexSafe(G4Event* event);

  /// Advance into the file as required.
  virtual void RecreateAdvanceToEvent(G4int eventOffset) = 0;
  
  /// Return the number of events in the file - not necessarily the number that
  /// match the filters but that are there in total.
  G4long NEventsInFile() const {return nEventsInFile;}
  
  /// Return number of available events (excluding any filters in derived classes) in the
  /// file. This is nominally nEventsInFile - currentFileEventIndex.
  G4long NEventsLeftInFile() const;

  /// Return whether at least 1 event has passed a filter in the file. If we have no
  /// events in the file that pass then we will loop infinitely to find one.
  G4bool OKToLoopFile() const;

  /// Mark internally that we are recreating and that therefore do not check if any
  /// particles have been successfully loaded before looping.
  void SetRecreationOn() {recreate = true;}
  
  /// Accessor.
  G4long NEventsReadThatPassedFilters() const {return nEventsReadThatPassedFilters;}

  /// Return the offset into the file if any.
  G4long NEventsSkipped() const {return nEventsSkipped;}
  
  /// Report whether the distribution is finished generating.
  G4bool DistributionIsFinished() const {return endOfFileReached;}
  
  /// Utility function to check eventOffset < nEventsInFile. This can only possibly happen
  /// If the recreation is done with the same filename but different contents or indeed somehow
  /// a different file.
  void ThrowExceptionIfRecreateOffsetTooHigh(G4long eventOffset) const;

protected:
  /// Utility function for derived classes to check a position is inside the world.
  G4bool VertexInsideWorld(const G4ThreeVector& pos) const;

  G4bool loopFile;
  BDSBunchEventGenerator* bunch;
  G4bool recreate;
  G4bool endOfFileReached;
  G4bool vertexGeneratedSuccessfully;
  G4long currentFileEventIndex;
  G4long nEventsInFile;
  G4long nEventsReadThatPassedFilters;
  G4long nEventsSkipped;
  mutable G4VSolid* worldSolid;
};

#endif
