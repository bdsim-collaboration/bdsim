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
#ifndef BDSPSCELLFLUX4D_H
#define BDSPSCELLFLUX4D_H
#include "BDSBH4DTypeDefs.hh"

#include "G4PSCellFlux3D.hh"
#include "G4String.hh"
#include "G4Types.hh"

class BDSHistBinMapper;

/** @brief Primitive scorer for cell flux in a 4D mesh.
 *
 * @author Eliott Ramoisiaux
 */

class BDSPSCellFlux4D: public G4PSCellFlux3D
{
public:
  BDSPSCellFlux4D(const G4String&         name,
		  const BDSHistBinMapper* mapperIn,
		  G4int ni = 1, G4int nj = 1, G4int nk = 1,
		  G4int depi = 2, G4int depj = 1, G4int depk = 0);
  
  BDSPSCellFlux4D(const G4String&         name,
		  const BDSHistBinMapper* mapperIn,
		  const G4String&         unit,
		  G4int ni = 1, G4int nj = 1, G4int nk = 1,
		  G4int depi = 2, G4int depj = 1, G4int depk = 0);
  
  virtual ~BDSPSCellFlux4D() override {;}
  
protected:
  G4int GetIndex(G4Step* aStep) override;
  
private:
  G4int fDepthi;
  G4int fDepthj;
  G4int fDepthk;
  const BDSHistBinMapper* mapper;
};

#endif
