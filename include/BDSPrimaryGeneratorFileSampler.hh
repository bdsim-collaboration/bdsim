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
#ifndef BDSPRIMARYGENERATORFILESAMPLER_H
#define BDSPRIMARYGENERATORFILESAMPLER_H

#include "BDSPrimaryGeneratorFile.hh"

#include "G4RotationMatrix.hh"
#include "G4String.hh"
#include "G4ThreeVector.hh"
#include "G4Types.hh"

#include <vector>

class BDSBunchEventGenerator;
class BDSOutputLoaderSampler;
class G4Event;
class G4PrimaryParticle;
class G4PrimaryVertex;
class G4VSolid;

/**
 * @brief Loader to read a specific sampler from a BDSIM ROOT output file.
 * 
 * @author Laurie Nevay
 */

class BDSPrimaryGeneratorFileSampler: public BDSPrimaryGeneratorFile
{
public:
  /// Do not require default constructor.
  BDSPrimaryGeneratorFileSampler() = delete;
  /// Constructor takes full distrType string including semicolon and
  /// eventgeneratorfile prefix. The filename is assumed to be correctly
  /// prefixed if a relative path already. The bunch definition is used
  /// for the reference coordinates and offset of the beam point.
  BDSPrimaryGeneratorFileSampler(const G4String& distrType,
                                 const G4String& fileNameIn,
                                 BDSBunchEventGenerator* bunchIn,
                                 G4bool loopFileIn,
                                 G4bool removeUnstableWithoutDecayIn = true,
                                 G4bool warnAboutSkippedParticlesIn  = true);
  virtual ~BDSPrimaryGeneratorFileSampler();

  /// Read the next non-empty sampler entry from the file.
  virtual void GeneratePrimaryVertex(G4Event* anEvent);

  /// Advance to the correct event number in the file for recreation.
  virtual void RecreateAdvanceToEvent(G4int eventOffset);

  struct DisplacedVertex
  {
    G4ThreeVector xyz;
    G4double T;
    G4PrimaryParticle* vertex;
  };

  /// Just advance to a different event index. Have a function to put the
  /// implementation in one place and be similar to BDSPrimaryGeneratorFileHEPMC.
  void SkipEvents(G4long nEventsToSkip);
  
protected:
  /// Read sampler hits and put into primary vertices if they pass filters.
  void ReadSingleEvent(G4long index, G4Event* anEvent);

  /// Conversion from HepMC::GenEvent to G4Event.
  //void HepMC2G4(const HepMC3::GenEvent* hepmcevt, G4Event* g4event);
  
  void ReadPrimaryParticlesFloat(G4long index);
  void ReadPrimaryParticlesDouble(G4long index);

private:
  BDSOutputLoaderSampler*   reader;
  G4String                  fileName;
  G4String                  samplerName;
  G4bool                    removeUnstableWithoutDecay;
  G4bool                    warnAboutSkippedParticles;
  G4RotationMatrix          referenceBeamMomentumOffset;
  
  /// Used for transiently loading information.
  std::vector<DisplacedVertex> vertices;
  
  std::vector<G4PrimaryVertex*> currentVertices;
};

#endif
