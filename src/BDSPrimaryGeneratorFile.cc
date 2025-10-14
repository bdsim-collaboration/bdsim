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
#include "BDSBunch.hh"
#include "BDSBunchEventGenerator.hh"
#include "BDSBunchType.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSGlobalConstants.hh"
#include "BDSPrimaryGeneratorFile.hh"
#include "BDSPrimaryGeneratorFileSampler.hh"
#include "BDSRunAction.hh"
#include "BDSUtilities.hh"

#ifdef USE_HEPMC3
#include "BDSPrimaryGeneratorFileHEPMC.hh"
#endif

#include "geomdefs.hh"
#include "G4LogicalVolume.hh"
#include "G4Navigator.hh"
#include "G4String.hh"
#include "G4ThreeVector.hh"
#include "G4TransportationManager.hh"
#include "G4Types.hh"
#include "G4VPhysicalVolume.hh"

#include "parser/beam.h"

#include <string>

BDSPrimaryGeneratorFile::BDSPrimaryGeneratorFile(G4bool loopFileIn,
                                                 BDSBunchEventGenerator* bunchIn):
  loopFile(loopFileIn),
  bunch(bunchIn),
  recreate(false),
  endOfFileReached(false),
  vertexGeneratedSuccessfully(false),
  currentFileEventIndex(0),
  nEventsInFile(0),
  nEventsReadThatPassedFilters(0),
  nEventsSkipped(0),
  worldSolid(nullptr)
{;}

BDSPrimaryGeneratorFile::~BDSPrimaryGeneratorFile()
{;}

void BDSPrimaryGeneratorFile::ThrowExceptionIfRecreateOffsetTooHigh(G4long eventOffset) const
{
  G4int nLoops = bunch->DistrFileLoopNTimes();
  if (eventOffset > nEventsInFile*nLoops)
    {
      G4String msg = "eventOffset (" + std::to_string(eventOffset) + ") is greater than the number of valid data lines in this file.";
      throw BDSException(__METHOD_NAME__, msg);
    }
}

G4bool BDSPrimaryGeneratorFile::GeneratePrimaryVertexSafe(G4Event* event)
{
  vertexGeneratedSuccessfully = false;
  GeneratePrimaryVertex(event);//derived class must update vertexGeneratedSuccessfully
  if (!vertexGeneratedSuccessfully && !endOfFileReached)
    {bunch->IncrementNEventsInFileSkipped();}
  return vertexGeneratedSuccessfully;
}

G4long BDSPrimaryGeneratorFile::NEventsLeftInFile() const
{
  return nEventsInFile - currentFileEventIndex;
}

G4bool BDSPrimaryGeneratorFile::OKToLoopFile() const
{
  return nEventsReadThatPassedFilters > 0 || recreate;
}

BDSPrimaryGeneratorFile* BDSPrimaryGeneratorFile::ConstructGenerator(const GMAD::Beam& beam,
                                                                     BDSBunch* bunchIn,
                                                                     G4bool recreate,
                                                                     G4int eventOffset,
                                                                     G4bool batchMode)
{
  BDSPrimaryGeneratorFile* generatorFromFile = nullptr;
  
  BDSBunchType egf = BDSBunchType::eventgeneratorfile;
  G4bool useEventGeneratorFile = BDS::StrContains(G4String(beam.distrType), egf.ToString());
  BDSBunchType samp = BDSBunchType::bdsimsampler;
  G4bool useSamplerLoader = BDS::StrContains(beam.distrType, samp.ToString());
  
  if (useEventGeneratorFile || useSamplerLoader)
    {
      if (beam.distrFile.empty())
        {throw BDSException(__METHOD_NAME__, "no distrFile specified for event generator beam distribution.");}
      G4String filename = BDS::GetFullPath(beam.distrFile, false, beam.distrFileFromExecOptions);
      BDSBunchEventGenerator* beg = dynamic_cast<BDSBunchEventGenerator*>(bunchIn);
      if (!beg)
        {throw BDSException(__METHOD_NAME__, "must be used with a BDSBunchEventGenerator instance");}
      G4bool shouldLoopFile = beam.distrFileLoop || !batchMode || beam.distrFileLoopNTimes > 1;
      if (useEventGeneratorFile)
        {
#ifdef USE_HEPMC3
          generatorFromFile = new BDSPrimaryGeneratorFileHEPMC(beam.distrType,
                                                               filename,
                                                               beg,
                                                               shouldLoopFile,
                                                               beam.removeUnstableWithoutDecay,
                                                               beam.eventGeneratorWarnSkippedParticles);
#else
          throw BDSException(__METHOD_NAME__, "event generator file being used but BDSIM not compiled with HEPMC3");
#endif
        }
      else // must be useSamplerLoader is true
        {
          generatorFromFile = new BDSPrimaryGeneratorFileSampler(beam.distrType,
                                                                 filename,
                                                                 beg,
                                                                 shouldLoopFile,
                                                                 beam.removeUnstableWithoutDecay,
                                                                 beam.eventGeneratorWarnSkippedParticles);
          
        }
      
      // common bits
      if (recreate)
        {
          generatorFromFile->RecreateAdvanceToEvent(eventOffset);
          generatorFromFile->SetRecreationOn();
        }
      if (beam.distrFileMatchLength)
        {
          G4int nEventsPerLoop = (G4int)generatorFromFile->NEventsLeftInFile();
          G4int distrFileLoopNTimes = (G4int)beam.distrFileLoopNTimes;
          BDSGlobalConstants::Instance()->SetNumberToGenerate(nEventsPerLoop*distrFileLoopNTimes);
          G4cout << __METHOD_NAME__ << "distrFileMatchLength is true -> simulating " << nEventsPerLoop << " events";
          if (distrFileLoopNTimes > 1)
            {G4cout << " " << distrFileLoopNTimes << " times";}
          G4cout << G4endl;
        }
    }
  
  return generatorFromFile; // could be nullptr
}

G4bool BDSPrimaryGeneratorFile::VertexInsideWorld(const G4ThreeVector& pos) const
{
  if (!worldSolid)
    {// cache the world solid
      G4Navigator* navigator = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
      G4VPhysicalVolume* world = navigator->GetWorldVolume();
      worldSolid = world->GetLogicalVolume()->GetSolid();
    }
  EInside qinside = worldSolid->Inside(pos);
  return qinside == EInside::kInside;
}
