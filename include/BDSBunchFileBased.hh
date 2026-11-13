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
#ifndef BDSBUNCHFILEBASED_H
#define BDSBUNCHFILEBASED_H 

#include "BDSBunch.hh"

#include "G4String.hh"

/**
 * @brief An intermediate layer for any bunch classes that are file based.
 *
 * This class is purely to hold a record of variables that will be passed
 * to the end of run action. Even for 'distributions' like bdsimsampler and
 * hepmc that don't really use the BDSBunch classes fully, they can publish
 * information in the BDSBunchFileBased instance they get.
 * 
 * @author Laurie Nevay
 */

class BDSBunchFileBased: public BDSBunch
{
public: 
  explicit BDSBunchFileBased(const G4String& distributionName);
  virtual ~BDSBunchFileBased(); 
  /// @{ Assignment and copy constructor not implemented nor used
  BDSBunchFileBased& operator=(const BDSBunchFileBased&) = delete;
  BDSBunchFileBased(BDSBunchFileBased&) = delete;
  /// @}

  /// Customise the beginning of run action to allow file looping always
  /// when in interactive mode, i.e. not batch mode.
  virtual void BeginOfRunAction(G4int numberOfEvents,
                                G4bool batchMode);

  /// @{ Accessor.
  unsigned long long int NOriginalEvents() const {return nOriginalEvents;}
  unsigned long long int NEventsInFile() const {return nEventsInFile;}
  unsigned long long int NEventsInFileSkipped() const {return nEventsInFileSkipped;}
  G4int DistrFileLoopNTimes() const {return distrFileLoopNTimes;}
  /// @}

  void SetNEventsInFile(unsigned long long int nEventsInFileIn) {nEventsInFile = nEventsInFileIn;}
  void SetNOriginalEvents(unsigned long long int nOriginalEventsIn) {nOriginalEvents = nOriginalEventsIn;}

  void IncrementNEventsInFileSkipped() {nEventsInFileSkipped += 1;}
  void IncrementNEventsInFileSkipped(unsigned long long int plus) {nEventsInFileSkipped += plus;}

  /// Pull out the relevant options and then pass through to BDSBunch::SetOptions().
  virtual void SetOptions(const BDSParticleDefinition* beamParticle,
                          const GMAD::Beam& beam,
                          const BDSBunchType& distrType,
                          G4Transform3D beamlineTransformIn = G4Transform3D::Identity,
                          const G4double beamlineS = 0);
  
protected:
  unsigned long long int nOriginalEvents; ///< nOriginalEvents from upstream file if skimmed - need to pass through.
  unsigned long long int nEventsInFile;   ///< The number of entries in the file loaded.
  unsigned long long int nEventsInFileSkipped; ///< Number that are skipped as we go through the file due to filters.
  G4bool distrFileLoop;
  G4int distrFileLoopNTimes;
};

#endif
