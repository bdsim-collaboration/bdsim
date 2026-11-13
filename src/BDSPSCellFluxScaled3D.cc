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
#include "BDSHistBinMapper.hh"
#include "BDSScorerConversionLoader.hh"
#include "BDSPSCellFluxScaled3D.hh"
#include "BDSUtilities.hh"

#include "G4PhysicsVector.hh"
#include "G4String.hh"
#include "G4SystemOfUnits.hh"
#include "G4Types.hh"
#include "G4VSolid.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VPVParameterisation.hh"
#include "G4UnitsTable.hh"

#include <fstream>
#include <string>

#ifdef USE_GZSTREAM
#include "src-external/gzstream/gzstream.h"
#endif

BDSPSCellFluxScaled3D::BDSPSCellFluxScaled3D(const G4String&         scorerName,
                                             const BDSHistBinMapper* mapperIn,
                                             const G4String&         unitIn,
                                             G4int ni,
                                             G4int nj,
                                             G4int nk,
                                             G4int depi,
                                             G4int depj,
                                             G4int depk):
  G4VPrimitiveScorer(scorerName),
  HCID3D(-1),
  evtMap3D(nullptr),
  fDepthi(depi),fDepthj(depj),fDepthk(depk),
  conversionFactor(nullptr),
  mapper(mapperIn)
{
  DefineUnitAndCategory();
  CheckAndSetUnit(unitIn, "Per Unit Surface");
  fNi = ni; // set base class members
  fNj = nj;
  fNk = nk;
}

BDSPSCellFluxScaled3D::BDSPSCellFluxScaled3D(const G4String&         scorerName,
                                             const BDSHistBinMapper* mapperIn,
                                             const G4String&         filename,
                                             const G4String&         unitIn,
                                             G4int ni,
                                             G4int nj,
                                             G4int nk,
                                             G4int depi,
                                             G4int depj,
                                             G4int depk):
  BDSPSCellFluxScaled3D(scorerName, mapperIn, unitIn, ni, nj, nk, depi, depj, depk)
{
  if (filename.empty())
    {throw BDSException(__METHOD_NAME__, "no conversionFactorFile provided for \"" + scorerName + "\" - required");}

  G4String filePath = BDS::GetFullPath(filename);
  if (filePath.rfind("gz") != std::string::npos)
    {
#ifdef USE_GZSTREAM
      BDSScorerConversionLoader<igzstream> loader;
      conversionFactor = loader.Load(filePath);
#else
      throw BDSException(__METHOD_NAME__, "Compressed file loading - but BDSIM not compiled with ZLIB.");
#endif
    }
  else
    {
      BDSScorerConversionLoader<std::ifstream> loader;
      conversionFactor = loader.Load(filePath);
    }
}

BDSPSCellFluxScaled3D::~BDSPSCellFluxScaled3D()
{
  delete conversionFactor;
}

G4bool BDSPSCellFluxScaled3D::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{
  G4double stepLength = aStep->GetStepLength();
  G4double radiationQuantity = 0;

  if (!BDS::IsFinite(stepLength))
    {return false;}

  const G4VTouchable* touchable = aStep->GetPreStepPoint()->GetTouchable();
  G4VSolid* solid = touchable->GetSolid();
  G4double cubicVolume = 1;
  auto search = volumeCache.find(solid);
  if (search == volumeCache.end())
    {// cache the volume of the volume to avoid repeated calculation
      // this is simple for a cube, but expensive for a cuttubs
      cubicVolume = solid->GetCubicVolume();
      volumeCache[solid] = cubicVolume;
    }
  else
    {cubicVolume = search->second;}
  
  G4double cellFlux = stepLength / cubicVolume;
  cellFlux *= aStep->GetPreStepPoint()->GetWeight();

  G4double kineticEnergy = aStep->GetPreStepPoint()->GetKineticEnergy();
  G4double factor = GetConversionFactor(aStep->GetTrack()->GetDefinition()->GetPDGEncoding(), kineticEnergy);
  radiationQuantity = cellFlux * factor;
  G4int index = GetIndex(aStep);

  evtMap3D->add(index, radiationQuantity);
  return true;
}

G4double BDSPSCellFluxScaled3D::GetConversionFactor(G4int  /*particleID*/,
                                                    G4double kineticEnergy) const
{
  return conversionFactor ? conversionFactor->Value(kineticEnergy) : 1.0;
}

void BDSPSCellFluxScaled3D::Initialize(G4HCofThisEvent* HCE)
{
  evtMap3D = new G4THitsMap<G4double>(detector->GetName(),
				      GetName());
  if (HCID3D < 0)
    {HCID3D = GetCollectionID(0);}
  HCE->AddHitsCollection(HCID3D, evtMap3D);
}

void BDSPSCellFluxScaled3D::EndOfEvent(G4HCofThisEvent* /*HEC*/)
{;}

void BDSPSCellFluxScaled3D::clear()
{
  evtMap3D->clear();
}

G4int BDSPSCellFluxScaled3D::GetIndex(G4Step* aStep)
{
  const G4VTouchable* touchable = aStep->GetPreStepPoint()->GetTouchable();
  G4int i = touchable->GetReplicaNumber(fDepthi);
  G4int j = touchable->GetReplicaNumber(fDepthj);
  G4int k = touchable->GetReplicaNumber(fDepthk);
  
  if (i<0 || j<0 || k<0)
    {
      G4ExceptionDescription ED;
      ED << "GetReplicaNumber is negative" << G4endl
	 << "touchable->GetReplicaNumber(fDepthi) returns i,j,k = "
	 << i << "," << j << "," << k << " for volume "
	 << touchable->GetVolume(fDepthi)->GetName() << ","
	 << touchable->GetVolume(fDepthj)->GetName() << ","
	 << touchable->GetVolume(fDepthk)->GetName() << G4endl;
      G4Exception("BDSPSCellFluxScaled3D::GetIndex","DetPS0006",JustWarning,ED);
    }

  G4int globalIndex = mapper->GlobalFromIJKLIndex(i,j,k); // x,y,z
  //G4int oldResult = i*fNj*fNk+j*fNk+k;
  return globalIndex;
}

void BDSPSCellFluxScaled3D::DefineUnitAndCategory() const
{
  new G4UnitDefinition("percentimeter2", "percm2", "Per Unit Surface", (1./CLHEP::cm2));
  new G4UnitDefinition("permillimeter2", "permm2", "Per Unit Surface", (1./CLHEP::mm2));
  new G4UnitDefinition("permeter2",      "perm2",  "Per Unit Surface", (1./CLHEP::m2));
}
