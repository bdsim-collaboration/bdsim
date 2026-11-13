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
#ifndef BDSPSCELLFLUXSCALEDPERPARTICLE3D_H
#define BDSPSCELLFLUXSCALEDPERPARTICLE3D_H

#include "BDSPSCellFluxScaled3D.hh"

#include "G4String.hh"
#include "G4Types.hh"

#include <map>

class BDSHistBinMapper;
class G4PhysicsVector;

/**
 * @brief Primitive scorer for a 3D mesh with a conversion factor.
 *
 * @author Robin Tesse
 */

class BDSPSCellFluxScaledPerParticle3D: public BDSPSCellFluxScaled3D
{
public:
  BDSPSCellFluxScaledPerParticle3D(const G4String&         scorerName,
                                   const BDSHistBinMapper* mapperIn,
                                   const G4String&         pathname,
                                   const G4String&         unitIn = "percm2",
                                   G4int ni=1,
                                   G4int nj=1,
                                   G4int nk=1,
                                   G4int depi=2,
                                   G4int depj=1,
                                   G4int depk=0);
  
  virtual ~BDSPSCellFluxScaledPerParticle3D() override;
  
public:
  virtual G4double GetConversionFactor(G4int particleID, G4double kineticEnergy) const override;
  
private:
  std::map<G4int, G4PhysicsVector*> conversionFactors;
};

#endif
