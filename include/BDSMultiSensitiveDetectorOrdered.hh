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
#ifndef BDSMULTISENSITIVEDETECTORORDERED_H
#define BDSMULTISENSITIVEDETECTORORDERED_H

#include "BDSSensitiveDetector.hh"

#include "G4String.hh"
#include "G4Types.hh"
#include "G4VSensitiveDetector.hh"

#include <vector>

/**
 * @brief Modified G4MultiSensitiveDetector that retains order and passes hits in sequence.
 *
 * Generated G4VHit* objects are passed in order to each successive SD that should
 * inherit BDSSensitiveDetector. If no hit is stored for that SD, a nullptr is inserted
 * into the vector. That way, the vector will always have the same length for a given SD.
 *
 * @author Laurie Nevay
 */

class BDSMultiSensitiveDetectorOrdered: public G4VSensitiveDetector
{
public:
  BDSMultiSensitiveDetectorOrdered(const G4String& name);
  
  virtual ~BDSMultiSensitiveDetectorOrdered();

  BDSMultiSensitiveDetectorOrdered(const BDSMultiSensitiveDetectorOrdered &rhs);
  BDSMultiSensitiveDetectorOrdered& operator=(const BDSMultiSensitiveDetectorOrdered& rhs);

  virtual void Initialize(G4HCofThisEvent* /*HCE*/) {;}
  virtual void EndOfEvent(G4HCofThisEvent* /*HEC*/) {;}
  virtual void clear();
  virtual void DrawAll();
  virtual void PrintAll();
  virtual G4VSensitiveDetector* Clone() const;

  using sds_t = std::vector<BDSSensitiveDetector*>;
  using sdsConstIter = sds_t::const_iterator;
  BDSSensitiveDetector* GetSD(const int i) const {return sensitiveDetectors[i];}
  sds_t::size_type GetSize() const {return sensitiveDetectors.size();}
  sdsConstIter GetBegin() const {return sensitiveDetectors.begin();}
  sdsConstIter GetEnd() const {return sensitiveDetectors.end();}
  void ClearSDs() {sensitiveDetectors.clear();}
  void AddSD(BDSSensitiveDetector* sd) {sensitiveDetectors.push_back(sd);}
    
protected:
  virtual G4bool ProcessHits(G4Step* step,
			     G4TouchableHistory* rOHist);

  /// The following method does not have a meaning for this concrete class
  virtual G4int GetCollectionID(G4int /*i*/) final {return -1;}

private:
  sds_t sensitiveDetectors;  
};

#endif
