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
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSGlobalConstants.hh"
#include "BDSMultiSensitiveDetectorOrdered.hh"
#include "BDSSensitiveDetector.hh" // for inheritance
#include "BDSSDApertureImpacts.hh"
#include "BDSSDCollimator.hh"
#include "BDSSDEnergyDeposition.hh"
#include "BDSSDEnergyDepositionGlobal.hh"
#include "BDSSDFilterIon.hh"
#include "BDSSDFilterOr.hh"
#include "BDSSDFilterPDGIDSet.hh"
#include "BDSSDFilterPrimary.hh"
#include "BDSSDManager.hh"
#include "BDSSDSampler.hh"
#include "BDSSDSamplerCylinder.hh"
#include "BDSSDSamplerSphere.hh"
#include "BDSSDSamplerLink.hh"
#include "BDSSDThinThing.hh"
#include "BDSSDType.hh"
#include "BDSSDTerminator.hh"
#include "BDSSDVolumeExit.hh"

#include "G4SDKineticEnergyFilter.hh"
#include "G4SDManager.hh"
#include "G4Version.hh"

#if G4VERSION_NUMBER > 1029
#include "G4MultiSensitiveDetector.hh"
#endif

#include <map>
#include <string>
#include <vector>

class BDSLinkRegistry;

BDSSDManager* BDSSDManager::instance = nullptr;

BDSSDManager* BDSSDManager::Instance()
{
  if (!instance)
    {instance = new BDSSDManager();}
  return instance;
}

BDSSDManager::~BDSSDManager()
{
  // no need to delete SDs as they are all registered in G4SDManager
  instance = nullptr;

  for (auto& kv : filters)
    {delete kv.second;}
  for (auto& kv : extraSamplerFilters)
    {delete kv.second;}
}

BDSSDManager::BDSSDManager()
{
  BDSGlobalConstants* g    = BDSGlobalConstants::Instance();
  storeCollimatorHitsAll   = g->StoreCollimatorHitsAll();
  storeCollimatorHitsIons  = g->StoreCollimatorHitsIons();
  collimatorHitsMinimumKE  = g->CollimatorHitsMinimumKE();
  generateApertureImpacts  = g->StoreApertureImpacts() || g->StoreApertureImpactsHistograms();
  storeApertureImpactsAll  = g->StoreApertureImpactsAll();
  storeApertureImpactsIons = g->StoreApertureImpactsIons();
  apertureImpactsMinimumKE = g->ApertureImpactsMinimumKE();
  generateELossHits        = g->StoreELoss() || g->StoreELossHistograms();
  generateELossVacuumHits  = g->StoreELossVacuum() || g->StoreELossVacuumHistograms();
  generateELossTunnelHits  = g->StoreELossTunnel() || g->StoreELossTunnelHistograms();

  G4bool killedParticleMassAddedToEloss = g->KilledParticlesMassAddedToEloss();

  generateELossWorldContents = g->UseImportanceSampling() || g->StoreELossWorldContents() || g->StoreELossWorldContentsIntegral();
  
  storeELossWorld         = g->StoreELossWorld() || g->StoreELossWorldIntegral();
  storeELossExtras        = g->StoreELossTurn()
    || g->StoreELossLinks()
    || g->StoreELossLocal()
    || g->StoreELossGlobal()
    || g->StoreELossTime()
    || g->StoreELossStepLength()
    || g->StoreELossPreStepKineticEnergy()
    || g->StoreELossModelID()
    || g->StoreTrajectory() // if we store trajectories, we need the edep track id
    || g->StoreELossPhysicsProcesses();
  generateCollimatorHits = storeCollimatorHitsAll
                           || storeCollimatorHitsIons
                           || g->StoreCollimatorInfo()
                           || g->StoreCollimatorHits()
                           || g->StoreCollimatorHitsLinks();
  
  filters["primary"] = new BDSSDFilterPrimary("primary");
  filters["ion"]     = new BDSSDFilterIon("ion");
  BDSSDFilterOr* primaryOrIon = new BDSSDFilterOr("primary_or_ion");
  primaryOrIon->RegisterFilter(filters["primary"]);
  primaryOrIon->RegisterFilter(filters["ion"]);
  filters["primary_or_ion"] = primaryOrIon;

  // aperture impact specifc filters
  filters["aper_min_ke"] = new G4SDKineticEnergyFilter("aper_min_ke", apertureImpactsMinimumKE);
  BDSSDFilterOr* primaryIonAperMinKE = new BDSSDFilterOr("primary_or_ion_aper_min_ke");
  primaryIonAperMinKE->RegisterFilter(filters["primary_or_ion"]);
  primaryIonAperMinKE->RegisterFilter(filters["aper_min_ke"]);
  filters["primary_or_ion_aper_min_ke"] = primaryIonAperMinKE;
  BDSSDFilterOr* primaryAperMinKE = new BDSSDFilterOr("primary_aper_min_ke");
  primaryAperMinKE->RegisterFilter(filters["primary"]);
  primaryAperMinKE->RegisterFilter(filters["aper_min_ke"]);

  // collimator impact specific filters
  filters["coll_min_ke"] = new G4SDKineticEnergyFilter("coll_min_ke", collimatorHitsMinimumKE);
  BDSSDFilterOr* primaryIonCollMinKE = new BDSSDFilterOr("primary_or_ion_coll_min_ke");
  primaryIonCollMinKE->RegisterFilter(filters["primary_or_ion"]);
  primaryIonCollMinKE->RegisterFilter(filters["coll_min_ke"]);
  filters["primary_or_ion_coll_min_ke"] = primaryIonCollMinKE;
  BDSSDFilterOr* primaryCollMinKE = new BDSSDFilterOr("primary_coll_min_ke");
  primaryCollMinKE->RegisterFilter(filters["primary"]);
  primaryCollMinKE->RegisterFilter(filters["coll_min_ke"]);
  
  G4SDManager* SDMan = G4SDManager::GetSDMpointer();
  
  samplerPlane = new BDSSDSampler("plane");
  SDMan->AddNewDetector(samplerPlane);
  
  samplerCylinder = new BDSSDSamplerCylinder("cylinder");
  SDMan->AddNewDetector(samplerCylinder);
  
  samplerSphere = new BDSSDSamplerSphere("sphere");
  SDMan->AddNewDetector(samplerSphere);

  samplerLink = new BDSSDSamplerLink("link");
  SDMan->AddNewDetector(samplerLink);

  // Terminator sd to measure how many times that primary has passed through the terminator
  terminator = new BDSSDTerminator("terminator");
  SDMan->AddNewDetector(terminator);

  energyDeposition = new BDSSDEnergyDeposition("general", storeELossExtras, killedParticleMassAddedToEloss);
  SDMan->AddNewDetector(energyDeposition);

  energyDepositionFull = new BDSSDEnergyDeposition("general_full", true, killedParticleMassAddedToEloss);
  SDMan->AddNewDetector(energyDepositionFull);
  
  energyDepositionVacuum = new BDSSDEnergyDeposition("vacuum", storeELossExtras, killedParticleMassAddedToEloss);
  SDMan->AddNewDetector(energyDepositionVacuum);

  energyDepositionTunnel = new BDSSDEnergyDeposition("tunnel", storeELossExtras, killedParticleMassAddedToEloss);
  SDMan->AddNewDetector(energyDepositionTunnel);

  energyDepositionWorld = new BDSSDEnergyDepositionGlobal("worldLoss", killedParticleMassAddedToEloss);
  SDMan->AddNewDetector(energyDepositionWorld);

  energyDepositionWorldContents = new BDSSDEnergyDepositionGlobal("worldLoss_contents", killedParticleMassAddedToEloss);
  SDMan->AddNewDetector(energyDepositionWorldContents);

  worldExit = new BDSSDVolumeExit("worldExit", true);
  SDMan->AddNewDetector(worldExit);

  apertureImpacts = new BDSSDApertureImpacts("aperture");
    // set up a filter for the collimator sensitive detector - always store primary hits
  G4VSDFilter* filterA = nullptr;
  G4bool applyApertureImpactsKEFilter = apertureImpactsMinimumKE > 0;
  if (storeApertureImpactsAll && applyApertureImpactsKEFilter)
    {filterA = filters["aper_min_ke"];}
  else if (storeApertureImpactsAll)
    {filterA = nullptr;} // no filter -> store all
  else if (storeApertureImpactsIons && applyApertureImpactsKEFilter) // primaries plus ion fragments
    {filterA = filters["primary_or_ion_aper_min_ke"];}
  else if (storeApertureImpactsIons)
    {filterA = filters["primary_or_ion"];}
  else if (applyApertureImpactsKEFilter)
    {filterA = filters["primary_aper_min_ke"];} // primaries + KE filter
  else
    {filterA = filters["primary"];} // just primaries
  apertureImpacts->SetFilter(filterA);
  SDMan->AddNewDetector(apertureImpacts);

#if G4VERSION_NUMBER > 1029
  // only multiple SDs since 10.3
  G4MultiSensitiveDetector* wcsd = new G4MultiSensitiveDetector("world_complete");
  SDMan->AddNewDetector(wcsd);
  wcsd->AddSD(energyDepositionWorld);
  wcsd->AddSD(worldExit);
  worldCompleteSD = wcsd;

  G4MultiSensitiveDetector* acsd = new G4MultiSensitiveDetector("aperture_complete");
  SDMan->AddNewDetector(acsd);
  acsd->AddSD(energyDeposition);
  acsd->AddSD(apertureImpacts);
  apertureCompleteSD = acsd;
#endif

  collimatorSD = new BDSSDCollimator("collimator");
  collimatorCompleteSD = new BDSMultiSensitiveDetectorOrdered("collimator_complete");
  collimatorCompleteSD->AddSD(energyDepositionFull); // always generate full edep hits
  collimatorCompleteSD->AddSD(collimatorSD);
  // set up a filter for the collimator sensitive detector - always store primary hits
  G4VSDFilter* filter = nullptr;
  G4bool applyCollimatorHitsKEFilter = collimatorHitsMinimumKE > 0;
  if (storeCollimatorHitsAll && applyCollimatorHitsKEFilter)
    {filter = filters["coll_min_ke"];}
  else if (storeCollimatorHitsAll)
    {filter = nullptr;} // no filter -> store all
  else if (storeCollimatorHitsIons && applyCollimatorHitsKEFilter) // primaries plus ion fragments
    {filter = filters["primary_or_ion_coll_min_ke"];}
  else if (storeCollimatorHitsIons)
    {filter = filters["primary_or_ion"];}
  else if (applyCollimatorHitsKEFilter)
    {filter = filters["primary_coll_min_ke"];} // primaries + KE filter
  else
    {filter = filters["primary"];} // just primaries

  // we only want to attach the filter to the collimator SD and not the energy counter SD
  // via the 'complete' SD. i.e. we always want energy deposition but collimator hits by
  // the filter.
  collimatorSD->SetFilter(filter);
  SDMan->AddNewDetector(collimatorSD);
  SDMan->AddNewDetector(collimatorCompleteSD);

  // thin things
  thinThingSD = new BDSSDThinThing("thinthing_general", g->StoreTrajectoryOptions());
  thinThingSD->SetFilter(filters["primary"]);
  SDMan->AddNewDetector(thinThingSD);

  // wire scanner wires SD
  wireCompleteSD = new BDSMultiSensitiveDetectorOrdered("wire_complete");
  wireCompleteSD->AddSD(energyDepositionFull);
  wireCompleteSD->AddSD(thinThingSD);
}

G4VSensitiveDetector* BDSSDManager::SensitiveDetector(const BDSSDType sdType,
						      G4bool applyOptions) const
{
  G4VSensitiveDetector* result = nullptr;
  switch (sdType.underlying())
    {
    case BDSSDType::samplerplane:
      {result = samplerPlane; break;}
    case BDSSDType::samplercylinder:
      {result = samplerCylinder; break;}
    case BDSSDType::samplerlink:
      {result = samplerLink; break;}
    case BDSSDType::terminator:
      {result = terminator; break;}
    case BDSSDType::energydep:
      {
	if (applyOptions)
	  {result = generateELossHits ? energyDeposition : nullptr;}
	else
	  {result = energyDeposition;}
	break;
      }
    case BDSSDType::energydepvacuum:
      {
	if (applyOptions)
	  {result = generateELossVacuumHits ? energyDepositionVacuum : nullptr;}
	else
	  {result = energyDepositionVacuum;}
	break;
      }
    case BDSSDType::energydeptunnel:
      {
	if (applyOptions)
	  {result = generateELossTunnelHits ? energyDepositionTunnel : nullptr;}
	else
	  {result = energyDepositionTunnel;}
	break;
      }
    case BDSSDType::energydepworld:
      {
	if (applyOptions)
	  {result = storeELossWorld ? energyDepositionWorld : nullptr;}
	else
	  {result = energyDepositionWorld;}
	break;
      }
    case BDSSDType::worldexit:
      {result = worldExit; break;}
    case BDSSDType::worldcomplete:
#if G4VERSION_NUMBER > 1029
      {result = worldCompleteSD; break;}
#else
      {result = nullptr; break;}
#endif
    case BDSSDType::energydepworldcontents:
      {
	if (applyOptions)
	  {result = generateELossWorldContents ? energyDepositionWorldContents : nullptr;}
	else
	  {result = energyDepositionWorldContents;}
	break;
      }
    case BDSSDType::collimator:
      {result = collimatorSD; break;}
    case BDSSDType::collimatorcomplete:
      {
	if (applyOptions)
	  {// if we're not going to store collimator specific information, just use
	    // regular energy deposition hits to save memory (collimator hits require
	    // full energy deposition hits, plus use extra memory themselves).
	    if (generateCollimatorHits)
	      {result = collimatorCompleteSD;}
	    else
	      {result = energyDeposition;}
	  }
	else
	  {result = collimatorCompleteSD;}
	break;
      }
    case BDSSDType::apertureimpacts:
      {
	if (applyOptions)
	  {result = generateApertureImpacts ? apertureImpacts : nullptr;}
	else
	  {result = apertureImpacts;}
	break;
      }
    case BDSSDType::aperturecomplete:
      {
#if G4VERSION_NUMBER > 1029
	if (applyOptions)
	  {
	    if (generateApertureImpacts && generateELossHits)
	      {result = apertureCompleteSD;}
	    else if (generateApertureImpacts)
	      {result = apertureImpacts;}
	  }
	else
	  {result = apertureCompleteSD;}
	break;
#else
	if (applyOptions)
	  {result = generateApertureImpacts ? apertureImpacts : nullptr;}
	else
	  {result = apertureImpacts;}
	break;
#endif
      }
    case BDSSDType::thinthing:
      {result = thinThingSD; break;}
    case BDSSDType::wirecomplete:
      {
	if (applyOptions)
	  {
	    result = generateELossHits
	      ? static_cast<G4VSensitiveDetector*>(wireCompleteSD)
	      : static_cast<G4VSensitiveDetector*>(thinThingSD);
	  }
	else
	  {result = wireCompleteSD;}
	break;
      }
    default:
      {result = nullptr; break;}
    }
  return result;
}

void BDSSDManager::RegisterPrimitiveScorerName(const G4String& nameIn,
					       G4double unit)
{
  primitiveScorerNamesComplete.emplace_back(nameIn);

  G4String primitivePartOnly = nameIn; // copy of full name
  auto search = nameIn.rfind("/");
  if (search != std::string::npos)
    {primitivePartOnly = nameIn.substr(search+1);}
  primitiveScorerNames.push_back(primitivePartOnly);
  primitiveScorerNameToUnit[primitivePartOnly] = unit;
}

void BDSSDManager::RegisterPrimitiveScorerNames(const std::vector<G4String>& namesIn,
						const std::vector<G4double>* units)
{
  if (units)
    {
      if (units->size() != namesIn.size())
	{throw BDSException(__METHOD_NAME__, "mismatching size of names and units.");}
      for (G4int i = 0; i < (G4int)namesIn.size(); i++)
	{RegisterPrimitiveScorerName(namesIn[i], (*units)[i]);}
    }
  else
    {
      for (const auto& name : namesIn)
	{RegisterPrimitiveScorerName(name);}
    }
}

void BDSSDManager::SetLinkRegistry(BDSLinkRegistry* registry)
{
  if (samplerLink)
    {samplerLink->SetLinkRegistry(registry);}
}

void BDSSDManager::ConstructSamplerSDsForParticleSets(const std::map<int, std::set<int>>& samplerFilterIDtoPDGSetIn)
{
  G4SDManager* SDMan = G4SDManager::GetSDMpointer();

  // we construct one of each sampler type per filter set as we don't know
  // from the parser how they'll be used. very small overhead.

  // plane sampler SDs
  for (const auto& IDAndSet : samplerFilterIDtoPDGSetIn)
    {
      G4String samplerName = "plane_with_PDG_set_" + std::to_string(IDAndSet.first);
      auto aSampler = new BDSSDSampler(samplerName);
      extraSamplerWithFilterNamesComplete.emplace_back(samplerName);
      auto filter = new BDSSDFilterPDGIDSet("pdgid_set_number_"+std::to_string(IDAndSet.first), IDAndSet.second);
      aSampler->SetFilter(filter);
      extraSamplersWithFilters[(G4int)IDAndSet.first] = aSampler;
      extraSamplerFilters[(G4int)IDAndSet.first] = filter; // keep a map of them for deleting later
      SDMan->AddNewDetector(aSampler);
    }
  // cylindrical sampler SDs
  for (const auto& IDAndSet : samplerFilterIDtoPDGSetIn)
    {
      G4String samplerName = "cylinder_with_PDG_set_" + std::to_string(IDAndSet.first);
      auto aSampler = new BDSSDSamplerCylinder(samplerName);
      extraSamplerCylinderWithFilterNamesComplete.emplace_back(samplerName);
      // filter already exists - reuse
      aSampler->SetFilter(extraSamplerFilters[(G4int)IDAndSet.first] );
      extraSamplerCylindersWithFilters[(G4int)IDAndSet.first] = aSampler;
      SDMan->AddNewDetector(aSampler);
    }
  // spherical sampler SDs
  for (const auto& IDAndSet : samplerFilterIDtoPDGSetIn)
    {
      G4String samplerName = "sphere_with_PDG_set_" + std::to_string(IDAndSet.first);
      auto aSampler = new BDSSDSamplerSphere(samplerName);
      extraSamplerSphereWithFilterNamesComplete.emplace_back(samplerName);
      // filter already exists - reuse
      aSampler->SetFilter(extraSamplerFilters[(G4int)IDAndSet.first] );
      extraSamplerSpheresWithFilters[(G4int)IDAndSet.first] = aSampler;
      SDMan->AddNewDetector(aSampler);
    }
}

BDSSDSampler* BDSSDManager::SamplerPlaneWithFilter(G4int ID) const
{
  auto search = extraSamplersWithFilters.find(ID);
  return search != extraSamplersWithFilters.end() ? search->second : nullptr;
}

BDSSDSamplerCylinder* BDSSDManager::SamplerCylinderWithFilter(G4int ID) const
{
  auto search = extraSamplerCylindersWithFilters.find(ID);
  return search != extraSamplerCylindersWithFilters.end() ? search->second : nullptr;
}

BDSSDSamplerSphere* BDSSDManager::SamplerSphereWithFilter(G4int ID) const
{
  auto search = extraSamplerSpheresWithFilters.find(ID);
  return search != extraSamplerSpheresWithFilters.end() ? search->second : nullptr;
}
