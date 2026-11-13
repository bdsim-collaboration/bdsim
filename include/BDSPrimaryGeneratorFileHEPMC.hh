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
#ifdef USE_HEPMC3

#ifndef BDSPRIMARYGENERATORHEPMC_H
#define BDSPRIMARYGENERATORHEPMC_H

#include "BDSEventGeneratorFileType.hh"
#include "BDSPrimaryGeneratorFile.hh"

#include "globals.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4VPrimaryGenerator.hh"

class BDSBunchEventGenerator;
class G4Event;

namespace HepMC3
{
  class GenEvent;
  class Reader;
}

/**
 * @brief Loader to use any HepMC3 compatible file.
 * 
 * Interface to use HepMC3 library to load a variety of files.
 * 
 * This is largely based on Geant4's example
 * examples/extended/eventgenerator/HepMC/HepMCEx01, however it
 * has been rewritten to use HepMC3 library rather than HepMC2.
 * Additionally, the interface has been written based on HepMC3
 * example "convert_example" to use multiple reader classes.
 * 
 * @author Helena Lefebvre, Laurie Nevay
 */

class BDSPrimaryGeneratorFileHEPMC: public BDSPrimaryGeneratorFile
{
public:
  /// Do not require default constructor.
  BDSPrimaryGeneratorFileHEPMC() = delete;
  /// Constructor takes full distrType string including semicolon and
  /// eventgeneratorfile prefix. The filename is assumed to be correctly
  /// prefixed if a relative path already. The bunch definition is used
  /// for the reference coordinates and offset of the beam point.
  BDSPrimaryGeneratorFileHEPMC(const G4String& distrType,
                               const G4String& fileNameIn,
                               BDSBunchEventGenerator* bunchIn,
                               G4bool loopFileIn,
                               G4bool removeUnstableWithoutDecayIn = true,
                               G4bool warnAboutSkippedParticlesIn  = true);
  virtual ~BDSPrimaryGeneratorFileHEPMC();

  /// Accessor.
  inline HepMC3::GenEvent* GetHepMCGenEvent() const {return hepmcEvent;}

  /// This will attempt once to generate an event by reading a single event
  /// and then converting it to a Geant4 one. The second step includes applying
  /// various filters for accepted particles and it is possible the event can be
  /// read ok but no particles pass the filters. This function can therefore be
  /// used repeatedly if needs be to generate a suitable event.
  virtual void GeneratePrimaryVertex(G4Event* anEvent);

  /// Advance to the correct event number in the file for recreation.
  virtual void RecreateAdvanceToEvent(G4int eventOffset);

protected:
  /// Construct the member "reader" and open the file for reading.
  void OpenFile(G4bool usualPrintOut = true);

  /// Close and delete reader. Have to delete as HepMC3 readers have no iteration
  /// or ability to loop back to the beginning.
  void CloseFile();
  
  /// Open the file, read all the events and count them. Do not apply any filters.
  /// Therefore, this returns the maximum number of raw events.
  G4long CountEventsInFile();

  /// Clear the hepmcEvent object, reallocate and read a single event and fill that member.
  /// Returns true if event read successfully, else false.
  G4bool ReadSingleEvent();
  
  /// Read events but do nothing with them. Will throw an exception if the number is greater
  /// than the number of events in the file.
  void SkipEvents(G4int nEventsToSkip);

  /// Conversion from HepMC::GenEvent to G4Event.
  void HepMC2G4(const HepMC3::GenEvent* hepmcevt, G4Event* g4event);
  
  // Note that the life of HepMC event object must be handled by users.
  // In the default implementation, a current HepMC event will be
  // deleted at GeneratePrimaryVertex() in the next event.
  HepMC3::GenEvent* hepmcEvent; // (care for single event case only)

private:
  HepMC3::Reader*           reader;
  G4String                  fileName;
  G4bool                    removeUnstableWithoutDecay;
  G4bool                    warnAboutSkippedParticles;
  BDSEventGeneratorFileType fileType;
  G4RotationMatrix          referenceBeamMomentumOffset;
};

#endif
#endif
