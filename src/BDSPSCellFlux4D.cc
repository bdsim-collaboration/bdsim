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
#include "BDSPSCellFlux4D.hh"
#include "BDSHistBinMapper.hh"

#ifdef USE_BOOST
#include <boost/variant.hpp>
#endif

#include <iostream>

#include "G4String.hh"
#include "G4Types.hh"

BDSPSCellFlux4D::BDSPSCellFlux4D(const G4String&         name,
				 const BDSHistBinMapper* mapperIn,
				 G4int ni,   G4int nj,   G4int nk,
				 G4int depi, G4int depj, G4int depk):
  G4PSCellFlux3D(name,ni,nj,nk,depi,depj,depk),
  fDepthi(depi),
  fDepthj(depj),
  fDepthk(depk),
  mapper(mapperIn)
{;}

BDSPSCellFlux4D::BDSPSCellFlux4D(const G4String&         name,
				 const BDSHistBinMapper* mapperIn,
				 const G4String&         unit,
				 G4int ni,   G4int nj,   G4int nk,
				 G4int depi, G4int depj, G4int depk):
  G4PSCellFlux3D(name, unit, ni, nj, nk, depi, depj, depk),
  fDepthi(depi),
  fDepthj(depj),
  fDepthk(depk),
  mapper(mapperIn)
{;}

G4int BDSPSCellFlux4D::GetIndex(G4Step* aStep)
{
  const G4VTouchable* touchable = aStep->GetPreStepPoint()->GetTouchable();
  
  G4int i = touchable->GetReplicaNumber(fDepthi);
  G4int j = touchable->GetReplicaNumber(fDepthj);
  G4int k = touchable->GetReplicaNumber(fDepthk);

#ifdef USE_BOOST
  double energy = aStep->GetPostStepPoint()->GetKineticEnergy();
  G4int l = boost::apply_visitor([&energy](auto&& one){return (decltype(one)(one))->index(energy);}, mapper->GetEnergyAxis()) + 1; // + 1 to get the index starting from 0 (the underflow bin has -1 as index)
#else
  G4int l = 0;
#endif

  if (i<0 || j<0 || k<0)
    {
      G4ExceptionDescription ED;
      ED << "GetReplicaNumber is negative" << G4endl
	 << "touchable->GetReplicaNumber(fDepthi) returns i,j,k = "
	 << i << "," << j << "," << k << "," << l << " for volume "
	 << touchable->GetVolume(fDepthi)->GetName() << ","
	 << touchable->GetVolume(fDepthj)->GetName() << ","
	 << touchable->GetVolume(fDepthk)->GetName() << "," << G4endl;
      G4Exception("PSRadiationQuantity3D::GetIndex","DetPS0006",JustWarning,ED);
    }
  
  G4int globalIndex = mapper->GlobalFromIJKLIndex(i, j, k, l); // x,y,z,e
  return globalIndex;
}
