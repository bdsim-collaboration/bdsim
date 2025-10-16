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
#include "BDSAcceleratorModel.hh"
#include "BDSBeamline.hh"
#include "BDSBeamlineElement.hh"
#include "BDSBLMRegistry.hh"
#include "BDSDebug.hh"
#include "BDSEventInfo.hh"
#include "BDSException.hh"
#include "BDSGlobalConstants.hh"
#include "BDSHistBinMapper.hh"
#include "BDSHitApertureImpact.hh"
#include "BDSHitCollimator.hh"
#include "BDSHitEnergyDeposition.hh"
#include "BDSHitEnergyDepositionGlobal.hh"
#include "BDSHitSampler.hh"
#include "BDSHitSamplerCylinder.hh"
#include "BDSHitSamplerSphere.hh"
#include "BDSHitSamplerLink.hh"
#include "BDSOutput.hh"
#include "BDSOutputROOTEventAperture.hh"
#include "BDSOutputROOTEventBeam.hh"
#include "BDSOutputROOTEventCollimator.hh"
#include "BDSOutputROOTEventCavityInfo.hh"
#include "BDSOutputROOTEventCollimatorInfo.hh"
#include "BDSOutputROOTEventCoords.hh"
#include "BDSOutputROOTEventLossWorld.hh"
#include "BDSOutputROOTEventHeader.hh"
#include "BDSOutputROOTEventHistograms.hh"
#include "BDSOutputROOTEventInfo.hh"
#include "BDSOutputROOTEventLoss.hh"
#include "BDSOutputROOTEventModel.hh"
#include "BDSOutputROOTEventOptions.hh"
#include "BDSOutputROOTEventRunInfo.hh"
#include "BDSOutputROOTEventSampler.hh"
#include "BDSOutputROOTEventSamplerC.hh"
#include "BDSOutputROOTEventSamplerS.hh"
#include "BDSOutputROOTEventTrajectory.hh"
#include "BDSOutputROOTParticleData.hh"
#include "BDSParticleDefinition.hh"
#include "BDSPrimaryVertexInformation.hh"
#include "BDSPrimaryVertexInformationV.hh"
#include "BDSScorerHistogramDef.hh"
#include "BDSSDManager.hh"
#include "BDSStackingAction.hh"
#include "BDSTrajectoriesToStore.hh"
#include "BDSTrajectoryPoint.hh"
#include "BDSTrajectoryPointHit.hh"
#include "BDSUtilities.hh"

#include "globals.hh"
#include "G4ParticleDefinition.hh"
#include "G4PrimaryParticle.hh"
#include "G4PrimaryVertex.hh"
#include "G4THitsMap.hh"
#include "G4Version.hh"

#include "TH1D.h"
#include "TH3D.h"

#include "parser/beamBase.h"
#include "parser/optionsBase.h"

#include <algorithm>
#include <cmath>
#include <map>
#include <ostream>
#include <set>
#include <utility>
#include <vector>

#include "CLHEP/Units/SystemOfUnits.h"

const std::set<G4String> BDSOutput::protectedNames = {
  "Event", "Histos", "Info", "Primary", "PrimaryGlobal",
  "Eloss", "ElossVacuum", "ElossTunnel", "ElossWorld", "ElossWorldExit",
  "ElossWorldContents",
  "PrimaryFirstHit", "PrimaryLastHit", "Trajectory", "ApertureImpacts"
};

BDSOutput::BDSOutput(const G4String& baseFileNameIn,
                     const G4String& fileExtensionIn,
                     G4int           fileNumberOffset):
  BDSOutputStructures(BDSGlobalConstants::Instance()),
  baseFileName(baseFileNameIn),
  fileExtension(fileExtensionIn),
  outputFileNumber(fileNumberOffset),
  sMinHistograms(0),
  sMaxHistograms(0),
  nbins(0),
  energyDeposited(0),
  energyDepositedVacuum(0),
  energyDepositedWorld(0),
  energyDepositedWorldContents(0),
  energyDepositedTunnel(0),
  energyImpactingAperture(0),
  energyImpactingApertureKinetic(0),
  energyWorldExit(0),
  energyWorldExitKinetic(0),
  nCollimatorsInteracted(0)
{
  const BDSGlobalConstants* g = BDSGlobalConstants::Instance();
  numberEventPerFile = g->NumberOfEventsPerNtuple();
  useScoringMap      = g->UseScoringMap();

  sMinHistograms             = g->BeamlineS();
  storeApertureImpacts       = g->StoreApertureImpacts();
  storeApertureImpactsHistograms = g->StoreApertureImpactsHistograms();
  storeCavityInfo            = g->StoreCavityInfo();
  storeCollimatorInfo        = g->StoreCollimatorInfo();
  storeCollimatorHitsLinks   = g->StoreCollimatorHitsLinks();
  storeCollimatorHitsIons    = g->StoreCollimatorHitsIons();
  // store primary hits if ion hits or links hits are turned on
  storeCollimatorHits        = g->StoreCollimatorHits() || storeCollimatorHitsLinks || storeCollimatorHitsIons || g->StoreCollimatorHitsAll();

  createCollimatorOutputStructures = storeCollimatorInfo || storeCollimatorHits;

  storeELoss                 = g->StoreELoss();
  // store histograms if storing general energy deposition as negligible in size
  storeELossHistograms       = g->StoreELossHistograms() || storeELoss;
  storeELossTunnel           = g->StoreELossTunnel();
  storeELossTunnelHistograms = g->StoreELossTunnelHistograms() || storeELossTunnel;
  storeELossVacuum           = g->StoreELossVacuum();
  storeELossVacuumHistograms = g->StoreELossVacuumHistograms() || storeELossVacuum;
  storeELossWorld            = g->StoreELossWorld();
  storeELossWorldContents    = g->StoreELossWorldContents() || g->UseImportanceSampling();
  storeParticleData          = g->StoreParticleData();
  storeModel                 = g->StoreModel();
  storePrimaries             = g->StorePrimaries();
  storePrimaryHistograms     = g->StorePrimaryHistograms();
  storeSamplerPolarCoords    = g->StoreSamplerPolarCoords();
  storeSamplerCharge         = g->StoreSamplerCharge();
  storeSamplerKineticEnergy  = g->StoreSamplerKineticEnergy();
  storeSamplerMass           = g->StoreSamplerMass();
  storeSamplerRigidity       = g->StoreSamplerRigidity();
  storeSamplerIon            = g->StoreSamplerIon();
  storeTrajectory            = g->StoreTrajectory();
  storeTrajectoryStepPoints  = g->StoreTrajectoryStepPoints();
  storeTrajectoryStepPointLast = g->StoreTrajectoryStepPointLast();
  storeTrajectoryOptions     = g->StoreTrajectoryOptions();
  storePerEventHistos        = g->StorePerEventHistos();
  
  // easy option for everything - overwrite bools we've just set individually
  if (g->StoreSamplerAll())
    {
      storeSamplerPolarCoords   = true;
      storeSamplerCharge        = true;
      storeSamplerKineticEnergy = true;
      storeSamplerMass          = true;
      storeSamplerRigidity      = true;
      storeSamplerIon           = true;
    }
}

void BDSOutput::InitialiseGeometryDependent()
{
  if (createCollimatorOutputStructures)
    {
      PrepareCollimatorInformation(); // prepare names, offsets and indices
      InitialiseCollimators(); // allocate local objects
    }
  if (storeCavityInfo)
    {PrepareCavityInformation();} // prepare names, offsets and indices
  CreateHistograms();
  InitialiseSamplers();
  InitialiseMaterialMap();
}

void BDSOutput::FillHeader()
{
  headerOutput->Flush();
  headerOutput->Fill(); // updates time stamp
  WriteHeader();
  // we purposively don't call ClearStructuresHeader() as we may yet update and overwrite the header info
}

void BDSOutput::FillParticleData(G4bool writeIons)
{
  // always prepare particle data and link to other classes, but optionally fill it
  particleDataOutput->Flush();
  particleDataOutput->Fill(writeIons);

#ifdef __ROOTDOUBLE__
  BDSOutputROOTEventSampler<double>::particleTable = particleDataOutput;
#else
  BDSOutputROOTEventSampler<float>::particleTable = particleDataOutput;
#endif
  BDSOutputROOTEventCollimator::particleTable = particleDataOutput;
  BDSOutputROOTEventAperture::particleTable   = particleDataOutput;
  
  if (storeParticleData)
    {WriteParticleData();}
}

void BDSOutput::FillBeam(const GMAD::BeamBase* beam)
{
  *beamOutput = BDSOutputROOTEventBeam(beam);
  WriteBeam();
  ClearStructuresBeam();
}

void BDSOutput::FillOptions(const GMAD::OptionsBase* options)
{
  *optionsOutput = BDSOutputROOTEventOptions(options);
  WriteOptions();
  ClearStructuresOptions();
}

void BDSOutput::FillModel()
{
  if (storeModel)
    {
      const auto& smpm = BDSAcceleratorModel::Instance()->ScorerMeshPlacementsMap();
      modelOutput->Fill(collimatorIndices,
                        collimatorIndicesByName,
                        collimatorInfo,
                        collimatorNames,
            cavityIndices,
            cavityIndicesByName,
            cavityInfo,
            cavityNames,
                        &smpm,
                        &materialIDToNameUnique,
                        storeTrajectory);
      WriteModel();
      ClearStructuresModel();
    }
}

void BDSOutput::FillPrimary(const G4PrimaryVertex* vertex,
                            const G4int            turnsTaken)
{
  const G4VUserPrimaryVertexInformation* vertexInfo = vertex->GetUserInformation();
  if (const auto vertexInfoBDS = dynamic_cast<const BDSPrimaryVertexInformation*>(vertexInfo))
    {
      primary->Fill(vertexInfoBDS->primaryVertex.local,
                    vertexInfoBDS->momentum,
                    vertexInfoBDS->charge,
                    vertexInfoBDS->pdgID,
                    turnsTaken,
                    vertexInfoBDS->primaryVertex.beamlineIndex,
                    vertexInfoBDS->nElectrons,
                    vertexInfoBDS->mass,
                    vertexInfoBDS->rigidity);
      primaryGlobal->Fill(vertexInfoBDS->primaryVertex.global);
    }
  else if (const auto vertexInfoBDSV = dynamic_cast<const BDSPrimaryVertexInformationV*>(vertexInfo))
    {// vector version - multiple primaries at primary vertex
      primary->Fill(vertexInfoBDSV, turnsTaken);
      primaryGlobal->Fill(vertexInfoBDSV);
    }
  auto nextLinkedVertex = vertex->GetNext();
  if (nextLinkedVertex)
    {FillPrimary(nextLinkedVertex, turnsTaken);}
}

void BDSOutput::FillEventPrimaryOnly(const BDSParticleCoordsFullGlobal& coords,
                                     const BDSParticleDefinition*       particle)
{
  G4bool isIon = particle->IsAnIon();
  G4int  ionA  = 0;
  G4int  ionZ  = 0;
  if (isIon)
    {// fill primary ion info correctly when we have no particle table available
      ionA = particle->IonDefinition()->A();
      ionZ = particle->IonDefinition()->Z();
    }
  primary->Fill(coords.local,
      particle->Momentum(),
      particle->Charge(),
      particle->PDGID(),
      0, 0,
      particle->NElectrons(),
      particle->Mass(),
      particle->BRho(),
      true,
      &isIon, &ionA, &ionZ);
  primaryGlobal->Fill(coords.global);
  WriteFileEventLevel();
  ClearStructuresEventLevel();
}

void BDSOutput::FillEvent(const BDSEventInfo*                            info,
                          const G4PrimaryVertex*                         vertex,
                          const std::vector<BDSHitsCollectionSampler*>&  samplerHitsPlane,
                          const std::vector<BDSHitsCollectionSamplerCylinder*>&  samplerHitsCylinder,
                          const std::vector<BDSHitsCollectionSamplerSphere*>&  samplerHitsSphere,
                          const BDSHitsCollectionSamplerLink*            samplerHitsLink,
                          const BDSHitsCollectionEnergyDeposition*       energyLoss,
                          const BDSHitsCollectionEnergyDeposition*       energyLossFull,
                          const BDSHitsCollectionEnergyDeposition*       energyLossVacuum,
                          const BDSHitsCollectionEnergyDeposition*       energyLossTunnel,
                          const BDSHitsCollectionEnergyDepositionGlobal* energyLossWorld,
                          const BDSHitsCollectionEnergyDepositionGlobal* energyLossWorldContents,
                          const BDSHitsCollectionEnergyDepositionGlobal* worldExitHits,
                          const std::vector<const BDSTrajectoryPointHit*>& primaryHits,
                          const std::vector<const BDSTrajectoryPointHit*>& primaryLosses,
                          const BDSTrajectoriesToStore*                  trajectories,
                          const BDSHitsCollectionCollimator*             collimatorHits,
                          const BDSHitsCollectionApertureImpacts*        apertureImpactHits,
                          const std::map<G4String, G4THitsMap<G4double>*>& scorerHits,
                          const G4int                                    turnsTaken)
{
  // Clear integrals in this class -> here instead of BDSOutputStructures as
  // looped over here -> do only once as expensive as lots of hits
  energyDeposited              = 0;
  energyDepositedVacuum        = 0;
  energyDepositedWorld         = 0;
  energyDepositedWorldContents = 0;
  energyDepositedTunnel        = 0;
  energyImpactingAperture      = 0;
  energyImpactingApertureKinetic = 0;
  energyWorldExit              = 0;
  energyWorldExitKinetic       = 0;
  nCollimatorsInteracted       = 0;
  
  if (vertex && storePrimaries)
    {FillPrimary(vertex, turnsTaken);}
  FillSamplerHitsVector(samplerHitsPlane);
  FillSamplerCylinderHitsVector(samplerHitsCylinder);
  FillSamplerSphereHitsVector(samplerHitsSphere);
  if (samplerHitsLink)
    {FillSamplerHitsLink(samplerHitsLink);}
  if (energyLoss)
    {FillEnergyLoss(energyLoss,        BDSOutput::LossType::energy);}
  if (energyLossFull)
    {FillEnergyLoss(energyLossFull,    BDSOutput::LossType::energy);}
  if (energyLossVacuum)
    {FillEnergyLoss(energyLossVacuum,  BDSOutput::LossType::vacuum);}
  if (energyLossTunnel)
    {FillEnergyLoss(energyLossTunnel,  BDSOutput::LossType::tunnel);}
  if (energyLossWorld)
    {FillEnergyLoss(energyLossWorld,   BDSOutput::LossType::world);}
  if (worldExitHits)
    {FillEnergyLoss(worldExitHits,     BDSOutput::LossType::worldexit);}
  if (energyLossWorldContents)
    {FillEnergyLoss(energyLossWorldContents, BDSOutput::LossType::worldcontents);}
  FillPrimaryHit(primaryHits);
  FillPrimaryLoss(primaryLosses);
  if (trajectories)
    {FillTrajectories(trajectories);}
  if (collimatorHits)
    {FillCollimatorHits(collimatorHits, primaryLosses);}
  if (apertureImpacts)
    {FillApertureImpacts(apertureImpactHits);}
  FillScorerHits(scorerHits); // map always exists

  // we do this after energy loss and collimator hits as the energy loss
  // is integrated for putting in event info and the number of collimators
  // interacted with counted
  if (info)
    {FillEventInfo(info);}
  
  WriteFileEventLevel();
  ClearStructuresEventLevel();
}

void BDSOutput::CloseAndOpenNewFile()
{
  ClearStructuresHeader();
  CloseFile();
  NewFile();
  InitialiseGeometryDependent();
}

void BDSOutput::FillRun(const BDSEventInfo* info,
                        unsigned long long int nOriginalEventsIn,
                        unsigned long long int nEventsRequestedIn,
                        unsigned long long int nEventsInOriginalDistrFileIn,
                        unsigned long long int nEventsDistrFileSkippedIn,
                        unsigned int distrFileLoopNTimesIn)
{
  FillRunInfoAndUpdateHeader(info, nOriginalEventsIn, nEventsRequestedIn, nEventsInOriginalDistrFileIn, nEventsDistrFileSkippedIn, distrFileLoopNTimesIn);
  WriteFileRunLevel();
  WriteHeaderEndOfFile();
  ClearStructuresRunLevel();
}

G4bool BDSOutput::InvalidSamplerName(const G4String& samplerName)
{
  return protectedNames.find(samplerName) != protectedNames.end();
}

void BDSOutput::PrintProtectedNames(std::ostream& out)
{
  out << "Protected names for output " << G4endl;
  for (const auto& key : protectedNames)
    {out << "\"" << key << "\"" << G4endl;}
}
 
G4String BDSOutput::GetNextFileName()
{
  outputFileNumber++;
  const BDSGlobalConstants* globalConstants = BDSGlobalConstants::Instance();
  
  // Base root file name 
  G4String newFileName = baseFileName;

  // if more than one file add number (starting at 0)
  // of numberEventPerFile is specified and the number already generated exceeds that
  if (numberEventPerFile > 0 && globalConstants->NGenerate() > numberEventPerFile)
    {newFileName += "_" + std::to_string(outputFileNumber);} // note underscore
  
  // policy: overwrite if output filename specifically set, otherwise increase number
  // always check in interactive mode
  if (!globalConstants->OutputFileNameSet() || !globalConstants->Batch())
    {// check if file exists
      G4String original = newFileName; // could have nper file number suffix too
      G4int nTimeAppended = 1;
      while (BDS::FileExists(newFileName + fileExtension)) // always test with extension
        {// if exists increment suffix integer
          newFileName = original + "-" + std::to_string(nTimeAppended);
          nTimeAppended +=1;
        }
    }

  // add extension now we've got the base part fixed
  newFileName += fileExtension;
  
  G4cout << __METHOD_NAME__ << "Setting up new file: " << newFileName << G4endl;

  return newFileName;
}

void BDSOutput::CalculateHistogramParameters()
{
  // rounding up so last bin definitely covers smax
  // (max - min) / bin width -> min = 0 here.
  const G4double binWidth = BDSGlobalConstants::Instance()->ELossHistoBinWidth();
  const BDSBeamline* flatBeamline = BDSAcceleratorModel::Instance()->BeamlineMain();
  if (flatBeamline)
    {// don't access a nullptr
      if (!flatBeamline->empty())
        {
          G4double sMax = flatBeamline->GetLastItem()->GetSPositionEnd();
          nbins = (int) std::ceil((sMax - sMinHistograms)/ binWidth); // round up to integer # of bins
        }
    }
  else
    {nbins = 1;} // can happen for generate primaries only

  if (nbins == 0)
    {nbins = 1;}
  
  sMaxHistograms = sMinHistograms + nbins * binWidth;
}

void BDSOutput::CreateHistograms()
{
  // construct output histograms
  // calculate histogram dimensions
  CalculateHistogramParameters();
  const G4double smin = sMinHistograms / CLHEP::m;
  const G4double smax = sMaxHistograms / CLHEP::m;
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "histogram parameters calculated to be: " << G4endl;
  G4cout << "s minimum: " << smin     << " m" << G4endl;
  G4cout << "s maximum: " << smax     << " m" << G4endl;
  G4cout << "# of bins: " << nbins    << G4endl;
#endif
  // create the histograms
  if (storePrimaryHistograms)
    {
      histIndices1D["Phits"] = Create1DHistogram("PhitsHisto","Primary Hits",nbins,smin,smax);
      histIndices1D["Ploss"] = Create1DHistogram("PlossHisto","Primary Loss",nbins,smin,smax);
    }
  if (storeELossHistograms)
    {histIndices1D["Eloss"] = Create1DHistogram("ElossHisto","Energy Loss",nbins,smin,smax);}
  // prepare bin edges for a by-element histogram
  std::vector<G4double> binedges;
  const BDSBeamline* flatBeamline = BDSAcceleratorModel::Instance()->BeamlineMain();
  if (flatBeamline) // can be nullptr in case of generate primaries only
    {binedges = flatBeamline->GetEdgeSPositions();}
  else
    {binedges = {0,1};}
  // create per element ("pe") bin width histograms
  if (storePrimaryHistograms)
    {
      histIndices1D["PhitsPE"] = Create1DHistogram("PhitsPEHisto",
                                                   "Primary Hits per Element",
                                                   binedges);
      histIndices1D["PlossPE"] = Create1DHistogram("PlossPEHisto",
                                                   "Primary Loss per Element",
                                                   binedges);
    }
  if (storeELossHistograms)
    {
      histIndices1D["ElossPE"] = Create1DHistogram("ElossPEHisto",
                                                   "Energy Loss per Element" ,
                                                   binedges);
    }
  if (storeELossVacuumHistograms)
    {
      histIndices1D["ElossVacuum"] = Create1DHistogram("ElossVacuumHisto",
                                                       "Energy Loss in Vacuum",
                                                       nbins,smin,smax);
      histIndices1D["ElossVacuumPE"] = Create1DHistogram("ElossVaccumPEHisto",
                                                         "Energy Loss in Vacuum per Element" ,
                                                         binedges);
    }

  if (storeApertureImpactsHistograms)
    {
      histIndices1D["PFirstAI"] = Create1DHistogram("PFirstAIHisto",
                                                    "Primary aperture impacts",
                                                    nbins, smin, smax);
    }

  // only create tunnel histograms if we build the tunnel
  const BDSBeamline* tunnelBeamline = BDSAcceleratorModel::Instance()->TunnelBeamline();
  if (!tunnelBeamline)
    {
      storeELossTunnel = false;
      storeELossTunnelHistograms = false;
    }
  if (storeELossTunnelHistograms && tunnelBeamline)
    {
      binedges = tunnelBeamline->GetEdgeSPositions();
      histIndices1D["ElossTunnel"] = Create1DHistogram("ElossTunnelHisto",
                                                       "Energy Loss in Tunnel",
                                                       nbins, smin,smax);
      histIndices1D["ElossTunnelPE"] = Create1DHistogram("ElossTunnelPEHisto",
                                                         "Energy Loss in Tunnel per Element",
                                                         binedges);
    }

  if (storeCollimatorInfo && nCollimators > 0)
    {
      std::vector<G4String> collHistNames = {"CollPhitsPE",
                                             "CollPlossPE",
                                             "CollElossPE",
                                             "CollPInteractedPE"};
      std::vector<G4String> collHistDesciptions = {"Primary Hits per Coll",
                                                   "Primary Loss per Coll",
                                                   "Energy Loss per Collimator",
                                                   "Primary Interacted per Collimator"};
      for (G4int i = 0; i < (G4int)collHistNames.size(); i++)
        {
          histIndices1D[collHistNames[i]] = Create1DHistogram(collHistNames[i],
                                                              collHistDesciptions[i],
                                                              nCollimators, 0, nCollimators);
        }
    }

  // one unique 'scorer' - single 3d histogram 3d
  if (useScoringMap && storeELossHistograms)
    {
      const BDSGlobalConstants* g = BDSGlobalConstants::Instance();
      if (!BDS::IsFinite(g->XMax() - g->XMin()))
        {throw BDSException(__METHOD_NAME__, "0 width in general 3D scoring histogram in x dimension - check options, xmin and xmax");}
      if (!BDS::IsFinite(g->YMax() - g->YMin()))
        {throw BDSException(__METHOD_NAME__, "0 width in general 3D scoring histogram in y dimension - check options, ymin and ymax");}
      if (!BDS::IsFinite(g->ZMax() - g->ZMin()))
        {throw BDSException(__METHOD_NAME__, "0 width in general 3D scoring histogram in z dimension - check options, zmin and zmax");}
      if (g->NBinsX() <= 0)
        {throw BDSException(__METHOD_NAME__, "invalid number of bins in x dimension of 3D scoring histogram - check option, nbinsx");}
      if (g->NBinsY() <= 0)
        {throw BDSException(__METHOD_NAME__, "invalid number of bins in y dimension of 3D scoring histogram - check option, nbinsx");}
      if (g->NBinsZ() <= 0)
        {throw BDSException(__METHOD_NAME__, "invalid number of bins in z dimension of 3D scoring histogram - check option, nbinsx");}

      G4int scInd = Create3DHistogram("ScoringMap", "Energy Deposition",
                                                 g->NBinsX(), g->XMin()/CLHEP::m, g->XMax()/CLHEP::m,
                                                 g->NBinsY(), g->YMin()/CLHEP::m, g->YMax()/CLHEP::m,
                                                 g->NBinsZ(), g->ZMin()/CLHEP::m, g->ZMax()/CLHEP::m);
      histIndices3D["ScoringMap"] = scInd;
    }

  // scoring maps
  const std::map<G4String, BDSScorerHistogramDef> scorerHistogramDefs = BDSAcceleratorModel::Instance()->ScorerHistogramDefinitionsMap();
  if (!scorerHistogramDefs.empty())
    {
      for (const auto& nameDef : scorerHistogramDefs)
        {
          const auto def = nameDef.second;
          // use safe output name without any slashes in the name
          G4int histID = -1;

          if (def.nBinsE <=1)
            {
              if (def.geometryType == "box")
                {
                  histID = Create3DHistogram(def.outputName, def.outputName,
                                             def.nBinsX, def.xLow/CLHEP::m, def.xHigh/CLHEP::m,
                                             def.nBinsY, def.yLow/CLHEP::m, def.yHigh/CLHEP::m,
                                             def.nBinsZ, def.zLow/CLHEP::m, def.zHigh/CLHEP::m);
                }
              else if (def.geometryType == "cylindrical")
                {
                  histID = Create3DHistogram(def.outputName, def.outputName,
                                             def.nBinsZ, def.zLow/CLHEP::m, def.zHigh/CLHEP::m,
                                             def.nBinsPhi, 0, CLHEP::twopi,
                                             def.nBinsR, def.rLow/CLHEP::m, def.rHigh/CLHEP::m);
                }
              
              histIndices3D[def.uniqueName] = histID;
              histIndexToUnits3D[histID] = def.primitiveScorerUnitValue;
              // avoid using [] operator for map as we have no default constructor for BDSHistBinMapper3D
            }
          else
            {
              if (def.geometryType == "box")
                {
                  histID = Create4DHistogram(def.outputName+"-"+def.eScale,def.outputName,def.eScale,def.eBinsEdges,
                                             def.nBinsX, def.xLow/CLHEP::m, def.xHigh/CLHEP::m,
                                             def.nBinsY, def.yLow/CLHEP::m, def.yHigh/CLHEP::m,
                                             def.nBinsZ, def.zLow/CLHEP::m, def.zHigh/CLHEP::m,
                                             def.nBinsE, def.eLow/CLHEP::GeV, def.eHigh/CLHEP::GeV);
                }
              else if (def.geometryType == "cylindrical")
                {
                  histID = Create4DHistogram(def.outputName+"-"+def.eScale, def.outputName, def.eScale,def.eBinsEdges,
                                             def.nBinsZ, def.zLow/CLHEP::m, def.zHigh/CLHEP::m,
                                             def.nBinsPhi, 0, CLHEP::twopi,
                                             def.nBinsR, def.rLow/CLHEP::m, def.rHigh/CLHEP::m,
                                             def.nBinsE, def.eLow/CLHEP::GeV, def.eHigh/CLHEP::GeV);
                }
              
              histIndices4D[def.uniqueName] = histID;
              histIndexToUnits4D[histID] = def.primitiveScorerUnitValue;
            }
          scorerCoordinateMaps.insert(std::make_pair(def.uniqueName, def.coordinateMapper));
        }
    }
  
  G4int nBLMs = BDSBLMRegistry::Instance()->NBLMs();
  if (nBLMs > 0)
    {     
      // the same primitive scorers for BLMs may be used in multiple SDs (each representing
      // a unique combination of primitive scorers. However, we want 1 histogram per primitive
      // scorer for all BLMs. We want to fill the same scoring quantity into the one histogram
      // even from different collections ("SD/PS"). Determine 'set' of histogram names, which is
      // set of primitive scorers use for BLMs.
      // note there may be scorers from general 3d meshes and not just blms
      std::vector<G4String> psnamesc = BDSSDManager::Instance()->PrimitiveScorerNamesComplete();
      std::vector<G4String> psnames  = BDSSDManager::Instance()->PrimitiveScorerNames();
      std::map<G4String, G4double> scorerUnits = BDSSDManager::Instance()->PrimitiveScorerUnits();
      std::set<G4String> blmHistoNames;
      std::map<G4String, G4String> psFullNameToPS;
      for (const auto& scorerNameComplete : psnamesc)
        {
          if (BDS::StrContains(scorerNameComplete, "blm_"))
            {
              for (const auto& scorerName : psnames)
                {
                  if (BDS::StrContains(scorerNameComplete, "/"+scorerName)) // only match end of full name with '/'
                    {
                      blmHistoNames.insert(scorerName);
                      psFullNameToPS[scorerNameComplete] = scorerName;
                    }
                }
            }
        }
      
      // make BLM histograms and map the full collection name to that histogram ID for easy filling
      // at the end of event. Note, multiple collections may feed into the same histogram.
      for (const auto &hn : blmHistoNames)
        {
          G4String blmHistName = "BLM_" + hn;
          G4int hind = Create1DHistogram(blmHistName, blmHistName, nBLMs, 0, nBLMs);
          histIndices1D[blmHistName] = hind;
          histIndexToUnits1D[hind]   = scorerUnits[hn];
          for (const auto& kv : psFullNameToPS)
            {
              if (hn == kv.second)
                {blmCollectionNameToHistogramID[kv.first] = hind;}
            }
        }
    }
}

void BDSOutput::FillEventInfo(const BDSEventInfo* info)
{
  if (info)
    {*evtInfo = *(info->GetInfo());}
  evtInfo->energyDeposited              = energyDeposited;
  evtInfo->energyDepositedVacuum        = energyDepositedVacuum;
  evtInfo->energyDepositedWorld         = energyDepositedWorld;
  evtInfo->energyDepositedWorldContents = energyDepositedWorldContents;
  evtInfo->energyDepositedTunnel        = energyDepositedTunnel;
  evtInfo->energyWorldExit              = energyWorldExit;
  evtInfo->energyWorldExitKinetic       = energyWorldExitKinetic;
  evtInfo->energyImpactingAperture      = energyImpactingAperture;
  evtInfo->energyImpactingApertureKinetic = energyImpactingApertureKinetic;
  G4double ek = BDSStackingAction::energyKilled / CLHEP::GeV;
  evtInfo->energyKilled = ek;
  evtInfo->energyTotal =  energyDeposited
    + energyDepositedVacuum
    + energyDepositedWorld
    + energyDepositedWorldContents
    + energyDepositedTunnel
    + energyWorldExit
    + ek;

  evtInfo->nCollimatorsInteracted = nCollimatorsInteracted;
}

void BDSOutput::FillSamplerHitsVector(const std::vector<BDSHitsCollectionSampler*>& hits)
{
  for (const auto& hc : hits)
    {
      if (!hc)
        {continue;} // could be nullptr
      if (!(hc->entries() > 0))
        {continue;}
      for (int i = 0; i < (int) hc->entries(); i++)
        {
          const BDSHitSampler* hit = (*hc)[i];
          G4int samplerID = hit->samplerID;
          G4int samplerVectorIndex = samplerIDToIndexPlane[samplerID];
          samplerTrees[samplerVectorIndex]->Fill(hit, storeSamplerMass, storeSamplerCharge,
                                                 storeSamplerPolarCoords, storeSamplerIon,
                                                 storeSamplerRigidity, storeSamplerKineticEnergy);
        }
    }
  // extra information - do only once at the end
  if (storeSamplerIon)
    {
      for (auto& sampler : samplerTrees)
        {sampler->FillIon();}
    }
}

void BDSOutput::FillSamplerCylinderHitsVector(const std::vector<BDSHitsCollectionSamplerCylinder*>& hits)
{
  for (const auto& hc : hits)
    {
      if (!hc)
        {continue;} // could be nullptr
      if (!(hc->entries() > 0))
        {continue;}
      for (int i = 0; i < (int) hc->entries(); i++)
        {
          const BDSHitSamplerCylinder* hit = (*hc)[i];
          G4int samplerID = hit->samplerID;
          G4int samplerVectorIndex = samplerIDToIndexCylinder[samplerID];
          samplerCTrees[samplerVectorIndex]->Fill(hit, storeSamplerMass, storeSamplerCharge,
                                                  storeSamplerIon, storeSamplerRigidity,
                                                  storeSamplerKineticEnergy);
        }
    }
  // extra information - do only once at the end
  if (storeSamplerIon)
    {
      for (auto& sampler : samplerCTrees)
        {sampler->FillIon();}
    }
}

void BDSOutput::FillSamplerSphereHitsVector(const std::vector<BDSHitsCollectionSamplerSphere*>& hits)
{
  for (const auto& hc : hits)
    {
      if (!hc)
        {continue;} // could be nullptr
      if (!(hc->entries() > 0))
        {continue;}
      for (int i = 0; i < (int) hc->entries(); i++)
        {
          const BDSHitSamplerSphere* hit = (*hc)[i];
          G4int samplerID = hit->samplerID;
          G4int samplerVectorIndex = samplerIDToIndexSphere[samplerID];
          samplerSTrees[samplerVectorIndex]->Fill(hit, storeSamplerMass, storeSamplerCharge,
                                                  storeSamplerIon, storeSamplerRigidity,
                                                  storeSamplerKineticEnergy);
        }
    }
  // extra information - do only once at the end
  if (storeSamplerIon)
    {
      for (auto& sampler : samplerSTrees)
        {sampler->FillIon();}
    }
}


void BDSOutput::FillSamplerHits(const BDSHitsCollectionSampler* hits)
{
  if (!(hits->entries() > 0))
    {return;}
  for (int i = 0; i < (int)hits->entries(); i++)
    {
      const BDSHitSampler* hit = (*hits)[i];
      G4int samplerID = hit->samplerID;
      G4int samplerVectorIndex = samplerIDToIndexPlane[samplerID];
      samplerTrees[samplerVectorIndex]->Fill(hit, storeSamplerMass, storeSamplerCharge,
                                             storeSamplerPolarCoords, storeSamplerIon,
                                             storeSamplerRigidity, storeSamplerKineticEnergy);
    }

  // extra information
  if (storeSamplerIon)
    {
      for (auto& sampler : samplerTrees)
        {sampler->FillIon();}
    }
}

void BDSOutput::FillSamplerHitsLink(const BDSHitsCollectionSamplerLink* hits)
{
  G4int nHits = (G4int)hits->entries();
  if (nHits == 0) // integer so ok to compare
    {return;}
  for (G4int i = 0; i < nHits; i++)
    {
      const BDSHitSamplerLink* hit = (*hits)[i];
      G4int samplerID = hit->samplerID;
      samplerID += 1; // offset index by one due to primary branch.
      samplerTrees[samplerID]->Fill(hit, storeSamplerMass, storeSamplerCharge, storeSamplerPolarCoords, storeSamplerIon, storeSamplerRigidity, storeSamplerKineticEnergy);
    }
  // extra information - do only once at the end
  G4bool firstSampler = true;
  for (auto& sampler : samplerTrees)
    {
      if (firstSampler) // skip primaries (1st sampler) as it always has extras filled in
        {firstSampler = false; continue;}
      if (storeSamplerIon)
        {sampler->FillIon();}
    }
}

void BDSOutput::FillEnergyLoss(const BDSHitsCollectionEnergyDepositionGlobal* hits,
                               const LossType lossType)
{
  switch (lossType)
    {
    case BDSOutput::LossType::world:
    case BDSOutput::LossType::worldexit:
    case BDSOutput::LossType::worldcontents:
      {break;}
    default:
      {return; break;} // don't fill for other types of hits
    }

  G4int nHits = (G4int)hits->entries();
  if (nHits == 0) // integer so ok to compare
    {return;}
  switch (lossType)
    {
    case BDSOutput::LossType::world:
      {
        for (G4int i=0; i < nHits; i++)
          {
            BDSHitEnergyDepositionGlobal* hit = (*hits)[i];
            energyDepositedWorld += hit->TotalEnergyWeighted()/CLHEP::GeV;
            if (storeELossWorld)
              {eLossWorld->Fill(hit);}
          }
        break;
      }
    case BDSOutput::LossType::worldexit:
      {
        for (G4int i = 0; i < nHits; i++)
          {
            BDSHitEnergyDepositionGlobal* hit = (*hits)[i];
            energyWorldExit += hit->TotalEnergyWeighted()/CLHEP::GeV;
            energyWorldExitKinetic += hit->KineticEnergyWeighted()/CLHEP::GeV;
            if (storeELossWorld)
              {eLossWorldExit->Fill(hit);}
          }
        break;
      }
    case BDSOutput::LossType::worldcontents:
      {
        for (G4int i = 0; i < nHits; i++)
          {
            BDSHitEnergyDepositionGlobal* hit = (*hits)[i];
            energyDepositedWorldContents += hit->TotalEnergyWeighted()/CLHEP::GeV;
            if (storeELossWorldContents)
              {eLossWorldContents->Fill(hit);}
          }
        break;
      }
    default:
      {break;} // only to prevent compiler warning
    }
}

void BDSOutput::FillEnergyLoss(const BDSHitsCollectionEnergyDeposition* hits,
                               const LossType lossType)
{
  G4int nHits = (G4int)hits->entries();
  if (nHits == 0)
    {return;}
  switch (lossType)
    {
    case BDSOutput::LossType::energy:
      {
        G4int indELoss = histIndices1D["Eloss"];
        G4int indELossPE = histIndices1D["ElossPE"];
        for (G4int i = 0; i < nHits; i++)
          {
            BDSHitEnergyDeposition* hit = (*hits)[i];
            G4double sHit = hit->GetSHit() / CLHEP::m;
            G4double eW = hit->GetEnergyWeighted() / CLHEP::GeV;
            energyDeposited += eW;
            if (storeELoss)
              {eLoss->Fill(hit);}
            if (storeELossHistograms)
              {
                runHistos->Fill1DHistogram(indELoss, sHit, eW);
                evtHistos->Fill1DHistogram(indELoss, sHit, eW);
                runHistos->Fill1DHistogram(indELossPE, sHit, eW);
                evtHistos->Fill1DHistogram(indELossPE, sHit, eW);
              }
          }
        break;
      }
    case BDSOutput::LossType::vacuum:
      {
        G4int indELossVacuum = storeELossVacuumHistograms ? histIndices1D["ElossVacuum"] : -1;
        G4int indELossVacuumPE = storeELossVacuumHistograms ? histIndices1D["ElossVacuumPE"] : -1;
        for (G4int i = 0; i < nHits; i++)
          {
            BDSHitEnergyDeposition* hit = (*hits)[i];
            G4double sHit = hit->GetSHit() / CLHEP::m;
            G4double eW = hit->GetEnergyWeighted() / CLHEP::GeV;
            energyDepositedVacuum += eW;
            if (storeELossVacuum)
              {eLossVacuum->Fill(hit);}
            if (storeELossVacuumHistograms)
              {
                evtHistos->Fill1DHistogram(indELossVacuum, sHit, eW);
                runHistos->Fill1DHistogram(indELossVacuumPE, sHit, eW);
              }
          }
        break;
      }
    case BDSOutput::LossType::tunnel:
      {
        G4int indELossTunnel = storeELossTunnelHistograms ? histIndices1D["ElossTunnel"] : -1;
        G4int indELossTunnelPE = storeELossTunnelHistograms ? histIndices1D["ElossTunnelPE"] : -1;
        for (G4int i = 0; i < nHits; i++)
          {
            BDSHitEnergyDeposition *hit = (*hits)[i];
            G4double sHit = hit->GetSHit() / CLHEP::m;
            G4double eW = hit->GetEnergyWeighted() / CLHEP::GeV;
            energyDepositedTunnel += eW;
            if (storeELossTunnel)
              {eLossTunnel->Fill(hit);}
            if (storeELossTunnelHistograms)
              {
                runHistos->Fill1DHistogram(indELossTunnel, sHit, eW);
                evtHistos->Fill1DHistogram(indELossTunnel, sHit, eW);
                runHistos->Fill1DHistogram(indELossTunnelPE, sHit, eW);
                evtHistos->Fill1DHistogram(indELossTunnelPE, sHit, eW);
              }
          }
      }
    default:
      {break;}
    }

  if (useScoringMap)
    {
      G4int indScoringMap = histIndices3D["ScoringMap"];
      for (G4int i = 0; i < nHits; i++)
        {
          BDSHitEnergyDeposition *hit = (*hits)[i];
          G4double sHit = hit->GetSHit() / CLHEP::m;
          G4double eW = hit->GetEnergyWeighted() / CLHEP::GeV;
          G4double x = hit->Getx() / CLHEP::m;
          G4double y = hit->Gety() / CLHEP::m;
          evtHistos->Fill3DHistogram(indScoringMap, x, y, sHit, eW);
          runHistos->Fill3DHistogram(indScoringMap, x, y, sHit, eW);
        }
    }

  if (storeCollimatorInfo &&
      nCollimators > 0 &&
      (lossType == BDSOutput::LossType::energy) &&
      storeELossHistograms)
    {CopyFromHistToHist1D("ElossPE", "CollElossPE", collimatorIndices);}
}

void BDSOutput::FillPrimaryHit(const std::vector<const BDSTrajectoryPointHit*>& primaryHits)
{
  for (auto phit : primaryHits)
    {
      if (!phit)
        {continue;}
      pFirstHit->Fill(phit);
      const G4double preStepSPosition = phit->point->GetPreS() / CLHEP::m;
      if (storePrimaryHistograms)
        {
          runHistos->Fill1DHistogram(histIndices1D["Phits"], preStepSPosition);
          evtHistos->Fill1DHistogram(histIndices1D["Phits"], preStepSPosition);
          runHistos->Fill1DHistogram(histIndices1D["PhitsPE"], preStepSPosition);
          evtHistos->Fill1DHistogram(histIndices1D["PhitsPE"], preStepSPosition);
          
          if (storeCollimatorInfo && nCollimators > 0)
            {CopyFromHistToHist1D("PhitsPE", "CollPhitsPE", collimatorIndices);}
        }
    }
}

void BDSOutput::FillPrimaryLoss(const std::vector<const BDSTrajectoryPointHit*>& primaryLosses)
{
  for (auto ploss : primaryLosses)
    {
      if (!ploss)
        {continue;}
      pLastHit->Fill(ploss);
      const G4double postStepSPosition = ploss->point->GetPostS() / CLHEP::m;
      if (storePrimaryHistograms)
        {
          runHistos->Fill1DHistogram(histIndices1D["Ploss"], postStepSPosition);
          evtHistos->Fill1DHistogram(histIndices1D["Ploss"], postStepSPosition);
          runHistos->Fill1DHistogram(histIndices1D["PlossPE"], postStepSPosition);
          evtHistos->Fill1DHistogram(histIndices1D["PlossPE"], postStepSPosition);
          
          if (storeCollimatorInfo && nCollimators > 0)
            {CopyFromHistToHist1D("PlossPE", "CollPlossPE", collimatorIndices);}
        }
    }
}

void BDSOutput::FillTrajectories(const BDSTrajectoriesToStore* trajectories)
{
  if (storeTrajectory)
    {traj->Fill(trajectories, storeTrajectoryStepPoints, storeTrajectoryStepPointLast, storeTrajectoryOptions, materialToID);}
}

void BDSOutput::FillCollimatorHits(const BDSHitsCollectionCollimator* hits,
                                   const std::vector<const BDSTrajectoryPointHit*>& primaryLossPoints)
{
  G4int nHits = (G4int)hits->entries();
  for (G4int i = 0; i < nHits; i++)
    {
      BDSHitCollimator* hit = (*hits)[i];
      G4int collimatorIndex = hit->collimatorIndex;      
      collimators[collimatorIndex]->Fill(hit,
                                         collimatorInfo[collimatorIndex],
                                         collimatorDifferences[collimatorIndex],
                                         storeCollimatorHits);  // this includes the || storeCollimatorHitsLinks || storeCollimatorHitsIons);
    }

  // identify whether the primary loss point was in a collimator
  // only do if there's a beam line, ie finished in a beam line, and there are collimators
  if (!primaryLossPoints.empty())
    {
      for (auto primaryLossPoint : primaryLossPoints)
        {
          if (primaryLossPoint->point->GetBeamLine() && nCollimators > 0)
            {
              G4int lossPointBLInd = primaryLossPoint->point->GetBeamLineIndex(); // always the mass world index
              auto result = std::find(collimatorIndices.begin(), collimatorIndices.end(), lossPointBLInd);
              if (result != collimatorIndices.end())
                {
                  G4int collIndex = (int) (result - collimatorIndices.begin());
                  collimators[collIndex]->SetPrimaryStopped(true);
                  collimators[collIndex]->primaryInteracted = true;
                  // it must've interacted if it stopped - could be that we kill
                  // secondaries and there's no energy deposition therefore not identified
                  // as primaryInteracted=true in BDSOutputROOTEventCollimator::Fill()
                }
            }
        }
    }
  
  // if required loop over collimators and get them to calculate and fill extra information
  if (storeCollimatorHitsLinks || storeCollimatorHitsIons)
    {
      for (auto collimator : collimators)
        {collimator->FillExtras(storeCollimatorHitsIons, storeCollimatorHitsLinks);}
    }

  // after all collimator hits have been filled, we summarise whether the primary
  // interacted in a histogram
  G4int histIndex = histIndices1D["CollPInteractedPE"];
  for (G4int i = 0; i < (G4int)collimators.size(); i++)
    {evtHistos->Fill1DHistogram(histIndex, i, (int)collimators[i]->primaryInteracted);}


  // loop over collimators and count the number that were interacted with in this event
  for (const auto collimator : collimators)
    {
      if (collimator->primaryInteracted)
        {nCollimatorsInteracted += 1;}
    }
}

void BDSOutput::FillApertureImpacts(const BDSHitsCollectionApertureImpacts* hits)
{
  if ((!storeApertureImpacts && !storeApertureImpactsHistograms) || !hits)
    {return;}

  G4int nPrimaryImpacts = 0;
  G4int nHits = (G4int)hits->entries();
  G4int histIndex = histIndices1D["PFirstAI"];
  for (G4int i = 0; i < nHits; i++)
    {
      const BDSHitApertureImpact* hit = (*hits)[i];
      G4double eW = (hit->totalEnergy / CLHEP::GeV) * hit->weight;
      energyImpactingAperture += eW;
      energyImpactingApertureKinetic += (hit->preStepKineticEnergy * hit->weight ) / CLHEP::GeV;
      if (hit->parentID == 0)
        {
          nPrimaryImpacts += 1;
          // only store one primary aperture hit in this histogram even if they were multiple
          if (storeApertureImpactsHistograms && nPrimaryImpacts == 1)
            {evtHistos->Fill1DHistogram(histIndex, hit->S / CLHEP::m);}
        }
      // hits are generated in order as the particle progresses
      // through the model, so the first one in the collection
      // for the primary is the first one in S.
      if (storeApertureImpacts)
        {apertureImpacts->Fill(hit, nPrimaryImpacts==1);}
    }
}

void BDSOutput::FillScorerHits(const std::map<G4String, G4THitsMap<G4double>*>& scorerHitsMap)
{
  for (const auto& nameHitsMap : scorerHitsMap)
    {
#if G4VERSION < 1039
      if (nameHitsMap.second->GetSize() == 0)
#else
      if (nameHitsMap.second->size() == 0)
#endif
#ifdef BDSDEBUG
        {G4cout << nameHitsMap.first << " empty" << G4endl; continue;}
#else
        {continue;}
#endif
      FillScorerHitsIndividual(nameHitsMap.first, nameHitsMap.second);
    }
}

void BDSOutput::FillScorerHitsIndividual(const G4String& histogramDefName,
                                         const G4THitsMap<G4double>* hitMap)
{
  if (BDS::StrContains(histogramDefName, "blm_"))
    {return FillScorerHitsIndividualBLM(histogramDefName, hitMap);}

  if (!(histIndices3D.find(histogramDefName) == histIndices3D.end()))
    {
      G4int histIndex = histIndices3D[histogramDefName];
      G4double unit   = BDS::MapGetWithDefault(histIndexToUnits3D, histIndex, 1.0);
      // avoid using [] operator for map as we have no default constructor for BDSHistBinMapper3D
      const BDSHistBinMapper& mapper = scorerCoordinateMaps.at(histogramDefName);
      TH3D* hist = evtHistos->Get3DHistogram(histIndex);
      G4int x,y,z,e;
#if G4VERSION < 1039
      for (const auto& hit : *hitMap->GetMap())
#else
      for (const auto& hit : *hitMap)
#endif
        {
          // convert from scorer global index to 3d i,j,k index of 3d scorer
          mapper.IJKLFromGlobal(hit.first, x,y,z,e);
          G4int rootGlobalIndex = (hist->GetBin(x + 1, y + 1, z + 1)); // convert to root system (add 1 to avoid underflow bin)
          evtHistos->Set3DHistogramBinContent(histIndex, rootGlobalIndex, *hit.second / unit);
        }
      runHistos->AccumulateHistogram3D(histIndex, evtHistos->Get3DHistogram(histIndex));
    }
  
  if (!(histIndices4D.find(histogramDefName) == histIndices4D.end()))
    {
      G4int histIndex = histIndices4D[histogramDefName];
      G4double unit   = BDS::MapGetWithDefault(histIndexToUnits4D, histIndex, 1.0);
      // avoid using [] operator for map as we have no default constructor for BDSHistBinMapper3D
      const BDSHistBinMapper& mapper = scorerCoordinateMaps.at(histogramDefName);
      G4int x,y,z,e;
#if G4VERSION < 1039
      for (const auto& hit : *hitMap->GetMap())
#else
      for (const auto& hit : *hitMap)
#endif
        {
          // convert from scorer global index to 4d i,j,k,e index of 4d scorer
          mapper.IJKLFromGlobal(hit.first, x,y,z,e);
          evtHistos->Set4DHistogramBinContent(histIndex, x, y, z, e - 1, *hit.second / unit); // - 1 to go back to the Boost Histogram indexing (-1 for the underflow bin)
        }
      runHistos->AccumulateHistogram4D(histIndex, evtHistos->Get4DHistogram(histIndex));
    }
}

void BDSOutput::FillScorerHitsIndividualBLM(const G4String& histogramDefName,
                                            const G4THitsMap<G4double>* hitMap)
{
  G4int histIndex = blmCollectionNameToHistogramID[histogramDefName];
#if G4VERSION < 1039
  for (const auto& hit : *hitMap->GetMap())
#else
  for (const auto& hit : *hitMap)
#endif
    {
#ifdef BDSDEBUG
      G4cout << "Filling hist " << histIndex << ", bin: " << hit.first+1 << " value: " << *hit.second << G4endl;
#endif
      G4double unit = BDS::MapGetWithDefault(histIndexToUnits1D, histIndex, 1.0);
      evtHistos->Fill1DHistogram(histIndex,hit.first, *hit.second / unit);
      runHistos->Fill1DHistogram(histIndex,hit.first, *hit.second / unit);
    }
}

void BDSOutput::FillRunInfoAndUpdateHeader(const BDSEventInfo* info,
                                           unsigned long long int nOriginalEventsIn,
                                           unsigned long long int nEventsRequestedIn,
                                           unsigned long long int nEventsInOriginalDistrFileIn,
                                           unsigned long long int nEventsDistrFileSkippedIn,
                                           unsigned int distrFileLoopNTimesIn)
{
  if (info)
    {*runInfo = BDSOutputROOTEventRunInfo(info->GetInfo());}
  // Note, check analysis/HeaderAnalysis.cc if the logic changes of only filling the 2nd
  // entry in the header tree with this information
  headerOutput->nOriginalEvents = nOriginalEventsIn;
  headerOutput->nEventsRequested = nEventsRequestedIn;
  headerOutput->nEventsInFile = nEventsInOriginalDistrFileIn;
  headerOutput->nEventsInFileSkipped = nEventsDistrFileSkippedIn;
  headerOutput->distrFileLoopNTimes = distrFileLoopNTimesIn;
}

void BDSOutput::CopyFromHistToHist1D(const G4String& sourceName,
                                     const G4String& destinationName,
                                     const std::vector<G4int>& indices)
{
  TH1D* sourceEvt      = evtHistos->Get1DHistogram(histIndices1D[sourceName]);
  TH1D* destinationEvt = evtHistos->Get1DHistogram(histIndices1D[destinationName]);
  // for the run ones we are overwriting but this is ok
  TH1D* sourceRun      = runHistos->Get1DHistogram(histIndices1D[sourceName]);
  TH1D* destinationRun = runHistos->Get1DHistogram(histIndices1D[destinationName]);
  G4int binIndex = 1; // starts at 1 for TH1; 0 is underflow
  for (const auto index : indices)
    {
      destinationEvt->SetBinContent(binIndex, sourceEvt->GetBinContent(index + 1));
      destinationEvt->SetBinError(binIndex,   sourceEvt->GetBinError(index + 1));
      destinationRun->SetBinContent(binIndex, sourceRun->GetBinContent(index + 1));
      destinationRun->SetBinError(binIndex,   sourceRun->GetBinError(index + 1));
      binIndex++;
    }
}
