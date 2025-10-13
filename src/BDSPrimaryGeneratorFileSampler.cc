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
#include "BDSBunchEventGenerator.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSOutputLoaderSampler.hh"
#include "BDSPrimaryGeneratorFileSampler.hh"
#include "BDSParticleCoords.hh"
#include "BDSParticleCoordsFull.hh"
#include "BDSParticleCoordsFullGlobal.hh"
#include "BDSPhysicalConstants.hh"
#include "BDSPrimaryVertexInformation.hh"
#include "BDSPrimaryVertexInformationV.hh"
#include "BDSUtilities.hh"
#include "BDSWarning.hh"

#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4LorentzVector.hh"
#include "G4PrimaryParticle.hh"
#include "G4PrimaryVertex.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include "globals.hh"

#include <utility>

BDSPrimaryGeneratorFileSampler::BDSPrimaryGeneratorFileSampler(const G4String& distrType,
                                                               const G4String& fileNameIn,
                                                               BDSBunchEventGenerator* bunchIn,
                                                               G4bool loopFileIn,
                                                               G4bool removeUnstableWithoutDecayIn,
                                                               G4bool warnAboutSkippedParticlesIn):
  BDSPrimaryGeneratorFile(loopFileIn, bunchIn),
  reader(nullptr),
  fileName(fileNameIn),
  removeUnstableWithoutDecay(removeUnstableWithoutDecayIn),
  warnAboutSkippedParticles(warnAboutSkippedParticlesIn)
{
  std::pair<G4String, G4String> ba = BDS::SplitOnColon(distrType); // before:after
  samplerName = ba.second;
  G4cout << __METHOD_NAME__ << "reading sampler named: \"" << samplerName << "\"" << G4endl;
  referenceBeamMomentumOffset = bunch->ReferenceBeamMomentumOffset();
  reader = new BDSOutputLoaderSampler(fileName, samplerName);
  nEventsInFile = reader->NEventsInFile();
  bunch->SetNEventsInFile(nEventsInFile);
  bunch->SetNOriginalEvents(reader->NOriginalEvents());
  G4cout << __METHOD_NAME__ << nEventsInFile << " events found in file" << G4endl;
  if (!bunch)
    {throw BDSException(__METHOD_NAME__, "must be constructed with a valid BDSBunchEventGenerator instance");}
  SkipEvents(bunch->eventGeneratorNEventsSkip);
}

BDSPrimaryGeneratorFileSampler::~BDSPrimaryGeneratorFileSampler()
{
  delete reader;
}

void BDSPrimaryGeneratorFileSampler::GeneratePrimaryVertex(G4Event* anEvent)
{
  if (!reader)
    {throw BDSException(__METHOD_NAME__, "no file reader available");}

  currentVertices.clear();

  // currentFileEventIndex is zero counting by nEventsInFile will be 1 greater
  if (currentFileEventIndex >= nEventsInFile)
	  {
	    endOfFileReached = true;
	    G4cout << __METHOD_NAME__ << "End of file reached. ";
	  
	    if (loopFile)
	      {
          if (OKToLoopFile())
            {
              G4cout << "Returning to beginning of file for next event." << G4endl;
              currentFileEventIndex = 0;
              endOfFileReached = false;
            }
          else
            {
              BDS::Warning(__METHOD_NAME__, "file looping requested, but 0 events passed filters - ending.");
              return;
            }
	      }
	    else
	      {
	        G4cout << G4endl; // to flush output
	        return;
	      }
	  }
      
  ReadSingleEvent(currentFileEventIndex, anEvent);
}

void BDSPrimaryGeneratorFileSampler::RecreateAdvanceToEvent(G4int eventOffset)
{
  G4cout << "BDSROOTSamplerLoader::RecreateAdvanceToEvent> Advancing file to event: " << eventOffset << G4endl;
  ThrowExceptionIfRecreateOffsetTooHigh(eventOffset);
  SkipEvents(eventOffset);
}

void BDSPrimaryGeneratorFileSampler::ReadPrimaryParticlesFloat(G4long index)
{
  vertices.clear();
  const auto sampler = reader->SamplerDataFloat(index);
  
  int n = sampler->n;
  for (int i = 0; i < n; i++)
    {
      G4int pdgID = (G4int)sampler->partID[i];
      G4double xp = (G4double)sampler->xp[i];
      G4double yp = (G4double)sampler->yp[i];
      G4double zp = (G4double)sampler->zp[i];
      G4double p  = (G4double)sampler->p[i];
      G4ThreeVector momentum = G4ThreeVector(xp,yp,zp) * p * CLHEP::GeV;
      G4double x = (G4double)sampler->x[i] * CLHEP::m;
      G4double y = (G4double)sampler->y[i] * CLHEP::m;
      G4double T = (G4double)sampler->T[i] * CLHEP::s;
      G4ThreeVector localPosition(x,y,0);
      G4double weight = (G4double)sampler->weight[i];
      auto g4prim = new G4PrimaryParticle(pdgID, momentum.x(), momentum.y(), momentum.z());
      g4prim->SetWeight(weight);
      vertices.emplace_back(DisplacedVertex{localPosition, T, g4prim});
    }
}

void BDSPrimaryGeneratorFileSampler::ReadPrimaryParticlesDouble(G4long index)
{
  vertices.clear();
  const auto sampler = reader->SamplerDataDouble(index);
  
  int n = sampler->n;
  for (int i = 0; i < n; i++)
    {
      G4int pdgID = (G4int)sampler->partID[i];
      G4double xp = (G4double)sampler->xp[i];
      G4double yp = (G4double)sampler->yp[i];
      G4double zp = (G4double)sampler->zp[i];
      G4double p  = (G4double)sampler->p[i];
      G4ThreeVector momentum = G4ThreeVector(xp,yp,zp) * p;
      G4double x = (G4double)sampler->x[i] * CLHEP::m;
      G4double y = (G4double)sampler->y[i] * CLHEP::m;
      G4double T = (G4double)sampler->T[i] * CLHEP::s;
      G4ThreeVector localPosition(x,y,0);
      G4double weight = (G4double)sampler->weight[i];
      auto g4prim = new G4PrimaryParticle(pdgID, momentum.x(), momentum.y(), momentum.z());
      g4prim->SetWeight(weight);
      vertices.emplace_back(DisplacedVertex{localPosition, T, g4prim});
    }
}

void BDSPrimaryGeneratorFileSampler::ReadSingleEvent(G4long index, G4Event* anEvent)
{
  if (reader->DoublePrecision())
    {ReadPrimaryParticlesDouble(index);}
  else
    {ReadPrimaryParticlesFloat(index);}
  
  G4int nParticlesSkipped = 0;
  for (const auto& xyzVertex : vertices)
    {
      const auto vertex = xyzVertex.vertex;
      // if the particle definition isn't found from the pdgcode in the construction
      // of G4PrimaryParticle, it means the mass, charge, etc. will be wrong - don't
      // stack this particle into the vertex.
      // technically shouldn't happen as bdsim produced this output... but safety first
      const G4ParticleDefinition* pd = vertex->GetParticleDefinition();
      G4bool deleteIt = !pd;
      if (pd && removeUnstableWithoutDecay)
        {deleteIt = !(pd->GetPDGStable()) && !pd->GetDecayTable();}
      
      if (deleteIt)
        {
          nParticlesSkipped++;
          continue;
        }
  
      G4ThreeVector unitMomentum = vertex->GetMomentumDirection();
      unitMomentum.transform(referenceBeamMomentumOffset);
      G4double rp = unitMomentum.perp();
  
      BDSParticleCoordsFull centralCoords = bunch->GetNextParticleLocal();
      centralCoords.AddOffset(xyzVertex.xyz, xyzVertex.T); // add on the local offset from the sampler
      
      BDSParticleCoordsFull local(centralCoords.x,
                                  centralCoords.y,
                                  centralCoords.z,
                                  unitMomentum.x(),
                                  unitMomentum.y(),
                                  unitMomentum.z(),
                                  centralCoords.T,
                                  centralCoords.s,
                                  vertex->GetTotalEnergy(),
                                  vertex->GetWeight());

      if (!bunch->AcceptParticle(local, rp, vertex->GetKineticEnergy(), vertex->GetPDGcode()))
        {
          nParticlesSkipped++;
          continue;
        }
  
      BDSParticleCoordsFullGlobal fullCoordsGlobal = bunch->ApplyTransform(local);
  
      auto g4vtx = new G4PrimaryVertex(fullCoordsGlobal.global.x,
                                       fullCoordsGlobal.global.y,
                                       fullCoordsGlobal.global.z,
                                       fullCoordsGlobal.global.T);
      
      // ensure it's in the world - not done in primary generator action for event generators
      if (!VertexInsideWorld(fullCoordsGlobal.global.Position()))
        {
          delete g4vtx;
          nParticlesSkipped++;
          continue;
        }
      
      G4double brho     = 0;
      G4double charge   = vertex->GetCharge();
      G4double momentum = vertex->GetTotalMomentum();
      if (BDS::IsFinite(charge)) // else leave as 0
        {
          brho = momentum / CLHEP::GeV / BDS::cOverGeV / charge;
          brho *= CLHEP::tesla*CLHEP::m; // rigidity (in Geant4 units)
        }
      auto vertexInfo = new BDSPrimaryVertexInformation(fullCoordsGlobal,
                                                        vertex->GetTotalMomentum(),
                                                        vertex->GetCharge(),
                                                        brho,
                                                        vertex->GetMass(),
                                                        vertex->GetPDGcode());

      // update momentum in case of a beam line transform
      auto prim = new G4PrimaryParticle(*vertex);
      prim->SetMomentumDirection(G4ThreeVector(fullCoordsGlobal.global.xp,
                                               fullCoordsGlobal.global.yp,
                                               fullCoordsGlobal.global.zp));
      g4vtx->SetPrimary(prim);
      g4vtx->SetUserInformation(vertexInfo);
      currentVertices.push_back(g4vtx);
    }
  
  if (nParticlesSkipped > 0 && warnAboutSkippedParticles)
    {G4cout << __METHOD_NAME__ << nParticlesSkipped << " particles skipped" << G4endl;}

  if (currentVertices.empty())
    {// no particles found that pass criteria... we can't simulate this event... abort and move on
      nEventsSkipped++;
      if (warnAboutSkippedParticles)
        {G4cout << __METHOD_NAME__ << "no particles found in event number: " << currentFileEventIndex << " in the file" << G4endl;}
    }
  else
    {
      vertexGeneratedSuccessfully = true;
      nEventsReadThatPassedFilters++;
    }
  
  // clear initially loaded ones
  for (auto& v : vertices)
    {delete v.vertex;}
  vertices.clear();

  currentFileEventIndex++;

  for (auto* v : currentVertices)
    {anEvent->AddPrimaryVertex(v);}
  currentVertices.clear();
}

void BDSPrimaryGeneratorFileSampler::SkipEvents(G4long nEventsToSkip)
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
  currentFileEventIndex = nToSkipSinglePass;
}
