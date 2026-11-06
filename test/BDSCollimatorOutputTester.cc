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
#include "BDSHitCollimator.hh"
#include "BDSHitEnergyDeposition.hh"
#include "BDSOutputROOTEventCollimator.hh"
#include "BDSOutputROOTEventCollimatorInfo.hh"

#include "G4ThreeVector.hh"
#include "G4Types.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include <iostream>
#include <string>
#include <utility>

/// Check that every per hit vector of the collimator output has length n, including
/// for a collimator hit without an energy deposition hit (e.g. a scattering point
/// where no energy was deposited), which previously left them shorter than n.
G4bool CheckSize(const std::string& label, std::size_t size, int n)
{
  G4bool ok = (int)size == n;
  if (!ok)
    {std::cout << label << " has length " << size << " but n is " << n << std::endl;}
  return ok;
}

int main()
{
  BDSOutputROOTEventCollimatorInfo info;
  info.length = 1*CLHEP::m;
  std::pair<G4double, G4double> differences(0, 0);

  // hit with an energy deposition hit
  BDSHitEnergyDeposition eHit(1*CLHEP::GeV, 0, 1.0, false, 0, 0, 0, 0, 0, 0, 0,
                              3*CLHEP::ns, 11, 1, 0, 1);
  BDSHitCollimator hitWithEDep(nullptr, 0, G4ThreeVector(), G4ThreeVector(0,0,1), 10*CLHEP::GeV,
                               3*CLHEP::ns, 1.0, 11, 0, 1, &eHit);
  // hit without an energy deposition hit
  BDSHitCollimator hitWithoutEDep(nullptr, 0, G4ThreeVector(), G4ThreeVector(0,0,1), 10*CLHEP::GeV,
                                  4*CLHEP::ns, 0.5, 22, 1, 1, nullptr);

  BDSOutputROOTEventCollimator coll;
  coll.Fill(&hitWithEDep, info, differences, true);
  coll.Fill(&hitWithoutEDep, info, differences, true);
  coll.Fill(&hitWithEDep, info, differences, true);

  G4bool ok = coll.n == 3;
  ok = CheckSize("energy",                  coll.energy.size(),                  coll.n) && ok;
  ok = CheckSize("energyDeposited",         coll.energyDeposited.size(),         coll.n) && ok;
  ok = CheckSize("xIn",                     coll.xIn.size(),                     coll.n) && ok;
  ok = CheckSize("T",                       coll.T.size(),                       coll.n) && ok;
  ok = CheckSize("weight",                  coll.weight.size(),                  coll.n) && ok;
  ok = CheckSize("partID",                  coll.partID.size(),                  coll.n) && ok;
  ok = CheckSize("parentID",                coll.parentID.size(),                coll.n) && ok;
  ok = CheckSize("turn",                    coll.turn.size(),                    coll.n) && ok;
  ok = CheckSize("firstPrimaryHitThisTurn", coll.firstPrimaryHitThisTurn.size(), coll.n) && ok;
  ok = CheckSize("impactParameterX",        coll.impactParameterX.size(),        coll.n) && ok;

  // the hit without an energy deposition hit takes the track information stored in it
  if (ok)
    {
      ok = coll.partID[1] == 22 && coll.parentID[1] == 1 && coll.energyDeposited[1] == 0
        && coll.weight[1] == 0.5f && coll.T[1] == 4.0f;
      if (!ok)
        {std::cout << "hit without an energy deposition hit has the wrong information" << std::endl;}
    }

  std::cout << (ok ? "All checks passed" : "Some checks failed") << std::endl;
  return ok ? 0 : 1;
}
