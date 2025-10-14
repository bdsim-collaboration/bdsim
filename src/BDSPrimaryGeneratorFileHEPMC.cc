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
#ifdef USE_HEPMC3

#include "BDSBunchEventGenerator.hh"
#include "BDSDebug.hh"
#include "BDSEventGeneratorFileType.hh"
#include "BDSException.hh"
#include "BDSPrimaryGeneratorFileHEPMC.hh"
#include "BDSParticleCoords.hh"
#include "BDSParticleCoordsFull.hh"
#include "BDSParticleCoordsFullGlobal.hh"
#include "BDSPhysicalConstants.hh"
#include "BDSPrimaryVertexInformation.hh"
#include "BDSPrimaryVertexInformationV.hh"
#include "BDSUtilities.hh"
#include "BDSWarning.hh"

#include "G4Event.hh"
#include "G4LorentzVector.hh"
#include "G4ParticleDefinition.hh"
#include "G4PrimaryParticle.hh"
#include "G4PrimaryVertex.hh"
#include "G4String.hh"
#include "G4Types.hh"

#include "HepMC3/Attribute.h"
#include "HepMC3/GenParticle.h"
#include "HepMC3/GenVertex.h"
#include "HepMC3/Reader.h"
#include "HepMC3/ReaderAscii.h"
#include "HepMC3/ReaderAsciiHepMC2.h"
#include "HepMC3/ReaderHEPEVT.h"
#include "HepMC3/ReaderLHEF.h"
#ifdef USE_HEPMC3_ROOTIO
#include "HepMC3/ReaderRoot.h"
#include "HepMC3/ReaderRootTree.h"
#endif

#include "CLHEP/Units/SystemOfUnits.h"

#include "globals.hh"

#include <cmath>
#include <utility>
#include <vector>


BDSPrimaryGeneratorFileHEPMC::BDSPrimaryGeneratorFileHEPMC(const G4String& distrType,
                                                           const G4String& fileNameIn,
                                                           BDSBunchEventGenerator* bunchIn,
                                                           G4bool loopFileIn,
                                                           G4bool removeUnstableWithoutDecayIn,
                                                           G4bool warnAboutSkippedParticlesIn):
  BDSPrimaryGeneratorFile(loopFileIn, bunchIn),
  hepmcEvent(nullptr),
  reader(nullptr),
  fileName(fileNameIn),
  removeUnstableWithoutDecay(removeUnstableWithoutDecayIn),
  warnAboutSkippedParticles(warnAboutSkippedParticlesIn)
{
  std::pair<G4String, G4String> ba = BDS::SplitOnColon(distrType); // before:after
  fileType = BDS::DetermineEventGeneratorFileType(ba.second);
  G4cout << __METHOD_NAME__ << "event generator file format to be " << fileType.ToString() << G4endl;
  referenceBeamMomentumOffset = bunch->ReferenceBeamMomentumOffset();
  nEventsInFile = CountEventsInFile();
  bunch->SetNEventsInFile(nEventsInFile);
  OpenFile();
  if (!bunch)
    {throw BDSException(__METHOD_NAME__, "must be constructed with a valid BDSBunchEventGenerator instance");}
  SkipEvents(bunch->eventGeneratorNEventsSkip);
}

BDSPrimaryGeneratorFileHEPMC::~BDSPrimaryGeneratorFileHEPMC()
{
  delete hepmcEvent;
  delete reader;
}

void BDSPrimaryGeneratorFileHEPMC::GeneratePrimaryVertex(G4Event* anEvent)
{
  if (!reader)
    {throw BDSException(__METHOD_NAME__, "no file reader available");}
  
  G4bool readEventOK = ReadSingleEvent();
  if (readEventOK)
    {HepMC2G4(hepmcEvent, anEvent);} // this will update vertexGeneratedSuccessfully if one is created
}

void BDSPrimaryGeneratorFileHEPMC::RecreateAdvanceToEvent(G4int eventOffset)
{
  G4cout << __METHOD_NAME__ << "advancing file to event: " << eventOffset << G4endl;
  ThrowExceptionIfRecreateOffsetTooHigh(eventOffset);
  SkipEvents(eventOffset);
}

void BDSPrimaryGeneratorFileHEPMC::OpenFile(G4bool usualPrintOut)
{
  currentFileEventIndex = 0;
  endOfFileReached = false;
  if (usualPrintOut)
    {G4cout << __METHOD_NAME__ << "Opening file: " << fileName << G4endl;}
  switch (fileType.underlying())
    {
    case BDSEventGeneratorFileType::hepmc2:
      {reader = new HepMC3::ReaderAsciiHepMC2(fileName); break;}
    case BDSEventGeneratorFileType::hepmc3:
      {reader = new HepMC3::ReaderAscii(fileName); break;}
    case BDSEventGeneratorFileType::hpe:
      {reader = new HepMC3::ReaderHEPEVT(fileName); break;}
#ifdef USE_HEPMC3_ROOTIO
    case BDSEventGeneratorFileType::root:
      {reader = new HepMC3::ReaderRoot(fileName); break;}
    case BDSEventGeneratorFileType::treeroot:
      {reader = new HepMC3::ReaderRootTree(fileName); break;}
#else
    case BDSEventGeneratorFileType::root:
    case BDSEventGeneratorFileType::treeroot:
      {
        throw BDSException(__METHOD_NAME__, "HEPMC3 installation not compiled with ROOT support so can't load root file.");
        break;
      }
#endif
    case BDSEventGeneratorFileType::lhef:
      {reader = new HepMC3::ReaderLHEF(fileName); break;}
    default:
      {break;}
    }
}

void BDSPrimaryGeneratorFileHEPMC::CloseFile()
{
  if (reader)
    {reader->close();}
  delete reader;
  reader = nullptr;
  currentFileEventIndex = 0;
  endOfFileReached = true;
}

G4long BDSPrimaryGeneratorFileHEPMC::CountEventsInFile()
{
  G4cout << __METHOD_NAME__ << "counting number of events" << G4endl;
  OpenFile(false);
  G4long nEvents = 0;
  while (!reader->failed())
    {
      auto tempEvent = new HepMC3::GenEvent();
      bool readEventOK = reader->read_event(*tempEvent);
      if (!readEventOK) // warn but continue
        {G4cout << __METHOD_NAME__ << "error in reading event index " << nEvents << G4endl;}
      // the reader will read ok beyond the last event (stupid), so we then have to check the file
      // status again here to know we've not really read an event!
      if (reader->failed())
        {continue;}
      nEvents++;
      delete tempEvent;
    }
  CloseFile();
  G4cout << __METHOD_NAME__ << nEvents << " events found in file" << G4endl;
  return nEvents;
}

G4bool BDSPrimaryGeneratorFileHEPMC::ReadSingleEvent()
{
  delete hepmcEvent;
  hepmcEvent = new HepMC3::GenEvent();

  // it will return false if there is a problem with the input stream or in reading one complete event
  // throw an exception as file inaccessible rather than finished
  bool readEventOK = reader->read_event(*hepmcEvent);
  if (!readEventOK)
    {
      delete hepmcEvent;
      hepmcEvent = nullptr;
      throw BDSException(__METHOD_NAME__, "problem with event generator file \"" + fileName + "\"");
    }
  
  if (reader->failed()) // code for end of the file
    {
      delete hepmcEvent;
      hepmcEvent = nullptr;
      CloseFile();

      if (loopFile)
        {
          if (OKToLoopFile())
            {
              G4cout << __METHOD_NAME__ << "Returning to beginning of file for next event." << G4endl;
              OpenFile();
            }
          else
            {BDS::Warning(__METHOD_NAME__, "file looping requested, but 0 events passed filters - ending.");}
        }
      return false;
    }
  else
    {
      currentFileEventIndex++;
      return true;
    }
}

void BDSPrimaryGeneratorFileHEPMC::SkipEvents(G4int nEventsToSkip)
{
  if (nEventsToSkip > 0)
    {G4cout << __METHOD_NAME__ << "skipping " << nEventsToSkip << " into file." << G4endl;}
  else
    {return ;}
  G4long distrFileLoopNTimes = bunch->DistrFileLoopNTimes();
  G4long nAvailable = nEventsInFile * distrFileLoopNTimes;
  if ((G4long)nEventsToSkip > nAvailable)
    {
      G4String msg = "number of events to skip (" + std::to_string(nEventsToSkip) + ") is greater than the number of events (";
      msg += std::to_string(nEventsInFile);
      if (distrFileLoopNTimes > 1)
        {msg += " x " + std::to_string(distrFileLoopNTimes) + " loops of file";}
      msg += ") in this file.";
      throw BDSException("BDSBunchUserFile::RecreateAdvanceToEvent>", msg);
    }
  G4long nToSkipSinglePass = nEventsToSkip % nEventsInFile;
  for (G4int i = 0; i < nToSkipSinglePass; i++)
    {ReadSingleEvent();}
}

void BDSPrimaryGeneratorFileHEPMC::HepMC2G4(const HepMC3::GenEvent* hepmcevt,
                                            G4Event* g4event)
{
  BDSParticleCoordsFull centralCoords = bunch->GetNextParticleLocal();
  // do the transform for the reference particle (if any transform) and use that
  BDSParticleCoordsFullGlobal centralCoordsGlobal = bunch->ApplyTransform(centralCoords);
  G4PrimaryVertex* g4vtx = new G4PrimaryVertex(centralCoordsGlobal.global.x,
                                               centralCoordsGlobal.global.y,
                                               centralCoordsGlobal.global.z,
                                               centralCoordsGlobal.global.T);
  
  double overallWeight = 1.0;
  if (!(hepmcevt->weights().empty()))
    {overallWeight = hepmcevt->weight();}
  std::vector<BDSPrimaryVertexInformation> vertexInfos;
  G4int nParticlesSkipped = 0;
  for (const auto& particlePtr : hepmcevt->particles())
    {
      const HepMC3::GenParticle* particle = particlePtr.get();
      if (!particle->children().empty())
        {continue;} // this particle is not at the end of the tree - ignore
      
      int pdgcode = particle->pdg_id();
      const HepMC3::FourVector& fv = particle->momentum();
      G4LorentzVector p(fv.px(), fv.py(), fv.pz(), fv.e());
      G4double px = p.x() * CLHEP::GeV;
      G4double py = p.y() * CLHEP::GeV;
      G4double pz = p.z() * CLHEP::GeV;
      G4ThreeVector originalUnitMomentum(px,py,pz);
      // as the particle is given by momentum (and not Etotal) then we don't need to check Etotal >= rest mass
      originalUnitMomentum = originalUnitMomentum.unit();
      G4double rp = std::hypot(originalUnitMomentum.x(), originalUnitMomentum.y());
      
      // apply any reference coordinate offsets. Copy the vector first.
      G4ThreeVector unitMomentum = originalUnitMomentum;
      unitMomentum.transform(referenceBeamMomentumOffset);
      // it's ok that this G4PrimaryParticle doesn't have the correct momentum direction as we only use it for
      // total energy and checking - it's direction is updated later based on unitMomentum with a beam line transform.
      G4PrimaryParticle* g4prim = new G4PrimaryParticle(pdgcode, px, py, pz);

      // if the particle definition isn't found from the pdgcode in the construction
      // of G4PrimaryParticle, it means the mass, charge, etc will be wrong - don't
      // stack this particle into the vertex.
      const G4ParticleDefinition* pd = g4prim->GetParticleDefinition();
      G4bool deleteIt = !pd;
      if (pd && removeUnstableWithoutDecay)
        {deleteIt = !(pd->GetPDGStable()) && !pd->GetDecayTable();}
      
      if (deleteIt)
        {
#ifdef BDSDEBUG
          G4cout << __METHOD_NAME__ << "skipping particle with PDG ID: " << pdgcode << G4endl;
#endif
          delete g4prim;
          nParticlesSkipped++;
          continue;
        }

      BDSParticleCoordsFull local(centralCoords.x,
                                  centralCoords.y,
                                  centralCoords.z,
                                  unitMomentum.x(),
                                  unitMomentum.y(),
                                  unitMomentum.z(),
                                  centralCoords.T,
                                  centralCoords.s,
                                  g4prim->GetTotalEnergy(),
                                  overallWeight);

      if (!bunch->AcceptParticle(local, rp, g4prim->GetKineticEnergy(), pdgcode))
        {
          delete g4prim;
          nParticlesSkipped++;
          continue;
        }
      
      BDSParticleCoordsFullGlobal fullCoords = bunch->ApplyTransform(local);
      // ensure it's in the world - not done in primary generator action for event generators
      if (!VertexInsideWorld(fullCoords.global.Position()))
        {
          delete g4prim;
          nParticlesSkipped++;
          continue;
        }  
      
      G4double brho     = 0;
      G4double charge   = g4prim->GetCharge();
      G4double momentum = g4prim->GetTotalMomentum();
      if (BDS::IsFinite(charge)) // else leave as 0
        {
          brho = momentum / CLHEP::GeV / BDS::cOverGeV / charge;
          brho *= CLHEP::tesla*CLHEP::m; // rigidity (in Geant4 units)
        }
      BDSPrimaryVertexInformation vertexInfo(fullCoords,
                                             g4prim->GetTotalMomentum(),
                                             g4prim->GetCharge(),
                                             brho,
                                             g4prim->GetMass(),
                                             pdgcode);
      vertexInfos.emplace_back(vertexInfo);

      // update momentum in case of a beam line transform
      g4prim->SetMomentumDirection(G4ThreeVector(fullCoords.global.xp,
                                                 fullCoords.global.yp,
                                                 fullCoords.global.zp));
      g4vtx->SetPrimary(g4prim);
    }

  if (nParticlesSkipped > 0 && warnAboutSkippedParticles)
    {G4cout << __METHOD_NAME__ << nParticlesSkipped << " particles skipped" << G4endl;}
  g4vtx->SetUserInformation(new BDSPrimaryVertexInformationV(vertexInfos));

  if (g4vtx->GetNumberOfParticle() == 0)
    {// no particles found that pass criteria... we can't simulate this event... abort and move on
      delete g4vtx;
      nEventsSkipped++;
      if (warnAboutSkippedParticles)
        {G4cout << __METHOD_NAME__ << "no particles found in event number: " << currentFileEventIndex << " in the file" << G4endl;}
      return;
    }
  else
    {
      g4event->AddPrimaryVertex(g4vtx);
      vertexGeneratedSuccessfully = true;
      nEventsReadThatPassedFilters++;
    }
}

#else
// insert empty function to avoid no symbols warning
void _SymbolToPreventWarningHEPMC3Reader(){;}
#endif
