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
#include "BDSScorerFactory.hh"
#include "BDSScorerInfo.hh"
#include "BDSPSCellFluxScaledPerParticle3D.hh"
#include "BDSPSPopulationScaled.hh"
#include "BDSPSBLMEnergyDeposit.hh"
#include "BDSPSCellFluxScaled3D.hh"
#include "BDSSDFilterAnd.hh"
#include "BDSSDFilterLogicalVolume.hh"
#include "BDSSDFilterMaterial.hh"
#include "BDSSDFilterPrimary.hh"
#include "BDSSDFilterTime.hh"
#include "BDSScorerType.hh"
#include "BDSUtilities.hh"

#include "G4PSCellCharge.hh"
#include "G4PSCellCharge3D.hh"
#include "G4PSCellFlux.hh"
#include "G4PSCellFlux3D.hh"
#include "BDSPSCellFlux4D.hh"
#include "G4PSDoseDeposit.hh"
#include "G4PSDoseDeposit3D.hh"
#include "G4PSEnergyDeposit.hh"
#include "G4PSEnergyDeposit3D.hh"
#include "G4PSPopulation.hh"
#include "G4PSPopulation3D.hh"
#include "G4SDParticleWithEnergyFilter.hh"
#include "G4String.hh"
#include "G4Types.hh"
#include "G4VPrimitiveScorer.hh"

BDSScorerFactory::BDSScorerFactory()
{;}

G4VPrimitiveScorer* BDSScorerFactory::CreateScorer(const BDSScorerInfo*    info,
						   const BDSHistBinMapper* mapper,
						   G4double*               unit,
						   G4LogicalVolume*        worldLV)
{
  // here we create the scorer with the information from BDSScorerInfo.
  G4VPrimitiveScorer* primitiveScorer = GetAppropriateScorer(*info, mapper, unit);

  BDSSDFilterAnd* filter = CreateFilter(info->name + "_scorer_filter", info, worldLV);
  if (filter)
    {primitiveScorer->SetFilter(filter);}

  return primitiveScorer;
}

G4VPrimitiveScorer* BDSScorerFactory::GetAppropriateScorer(const BDSScorerInfo&    info,
							   const BDSHistBinMapper* mapper,
							   G4double*               unit)
{
  G4VPrimitiveScorer* result = nullptr;
  switch (info.scorerType.underlying())
    {// if adding new 3D ones, remember to update the 3D upgrade mapping in BDSScorerInfo
    case BDSScorerType::cellcharge:
      {result = new G4PSCellCharge(info.name);    break;}
    case BDSScorerType::cellcharge3d:
      {result = new G4PSCellCharge3D(info.name);  break;}
    case BDSScorerType::depositeddose:
      {result = new G4PSDoseDeposit(info.name);   break;}
    case BDSScorerType::depositeddose3d:
      {result = new G4PSDoseDeposit3D(info.name); break;}
    case BDSScorerType::depositedenergy:
      {result = new G4PSEnergyDeposit(info.name, "GeV"); break;}
    case BDSScorerType::depositedenergyblm:
      {result = new BDSPSBLMEnergyDeposit(info.name, "GeV"); break;}
    case BDSScorerType::depositedenergy3d:
      {result = new G4PSEnergyDeposit3D(info.name, "GeV"); break;}
    case BDSScorerType::population:
      {
	G4PSPopulation* scorer = new G4PSPopulation(info.name);
	scorer->Weighted(true);
	result = scorer;
	break;
      }
    case BDSScorerType::population3d:
      {
	G4PSPopulation3D* scorer = new G4PSPopulation3D(info.name);
	scorer->Weighted(true);
	result = scorer;
	break;
      }
    case BDSScorerType::populationscaled:
      {result = new BDSPSPopulationScaled(info.name, info.pathname); break;}
    case BDSScorerType::cellflux:
      {
	G4PSCellFlux* scorer = new G4PSCellFlux(info.name, "percm2");
	scorer->Weighted(true);
	result = scorer;
	break;
      }
    case BDSScorerType::cellflux3d:
      {
	G4PSCellFlux3D* scorer = new G4PSCellFlux3D(info.name, "percm2");
	scorer->Weighted(true);
	result = scorer;
	break;
      }
    case BDSScorerType::cellflux4d:
      {
    BDSPSCellFlux4D* scorer = new BDSPSCellFlux4D(info.name, mapper, "percm2");
	scorer->Weighted(true);
	result = scorer;
	break;
      }
    case BDSScorerType::cellfluxscaledperparticle3d:
      {result = new BDSPSCellFluxScaledPerParticle3D(info.name, mapper, info.pathname); break;}
    case BDSScorerType::cellfluxscaled3d:
      {result = new BDSPSCellFluxScaled3D(info.name, mapper, info.filename, "percm2"); break;}
    case BDSScorerType::cellfluxscaled:
    case BDSScorerType::cellfluxscaledperparticle:
      {
	throw BDSException(__METHOD_NAME__, "unimplemented scorer \"" + info.scorerType.ToString() + "\"");
	break;
      }
    default:
      {
	throw BDSException(__METHOD_NAME__, "unknown scorer type \"" + info.scorerType.ToString() + "\"");
	break;
      }
    }
  if (unit)
    {*unit = result->GetUnitValue();}
  return result;
}

BDSSDFilterAnd* BDSScorerFactory::CreateFilter(const G4String&      name,
					                                     const BDSScorerInfo* info,
                                               G4LogicalVolume*     worldLV) const
{
  BDSSDFilterAnd* result = new BDSSDFilterAnd(name, /*ownsFilters=*/true);

  if (info->particle)
    {
      G4String particleName = info->particle->GetParticleName();
      auto pwkef = new G4SDParticleWithEnergyFilter("particle_filter",
						    info->minimumKineticEnergy,
						    info->maximumKineticEnergy);
      pwkef->add(particleName);
      result->RegisterFilter(pwkef);
    }
  if (BDS::IsFinite(info->maximumTime) || BDS::IsFinite(info->minimumTime))
    {
      auto timeFilter = new BDSSDFilterTime("time_filter",
					    info->minimumTime,
					    info->maximumTime);
      result->RegisterFilter(timeFilter);
    }
  if (!(info->materialsToInclude.empty()))
    {
      auto materialFilterInc = new BDSSDFilterMaterial("material_filter_include",
						       info->materialsToInclude,
						       /*inclusive*/true);
      result->RegisterFilter(materialFilterInc);
    }
  if (!(info->materialsToExclude.empty()))
    {
      auto materialFilterExc = new BDSSDFilterMaterial("material_filter_exclude",
						       info->materialsToExclude,
						       /*inclusive*/false);
      result->RegisterFilter(materialFilterExc);
    }
  if (info->worldVolumeOnly)
    {
      auto worldLVFilter = new BDSSDFilterLogicalVolume("world_lv_only", worldLV);
      result->RegisterFilter(worldLVFilter);
    }
  if (info->primariesOnly)
    {
      auto primaryFilter = new BDSSDFilterPrimary("primary_filter");
      result->RegisterFilter(primaryFilter);
    }

  // if we didn't register any filters, just delete it
  if (result->size() == 0)
    {
      delete result;
      return nullptr;
    }
  return result;
}
