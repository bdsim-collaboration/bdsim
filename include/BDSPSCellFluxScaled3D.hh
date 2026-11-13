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
#ifndef BDSPSCELLFLUXSCALED3D_H
#define BDSPSCELLFLUXSCALED3D_H

#include "globals.hh"
#include "G4THitsMap.hh"
#include "G4VPrimitiveScorer.hh"

class BDSHistBinMapper;
class G4PhysicsVector;

/**
 * @brief Primitive scorer for cell flux in a 3D mesh with a conversion factor.
 *
 * The cell flux (step length / cell volume) * weight is also multiplied by a
 * conversion factor (just a number) as listed vs energy in a supplied file. The
 * default is none and just a factor of 1.
 *
 * The implementation also differs from G4PSCellFlux3D as we cache the volume
 * to avoid repeated calculation.
 * 
 * @author Robin Tesse
 */

class BDSPSCellFluxScaled3D: public G4VPrimitiveScorer
{
public:
  /// Constructor where no conversion factor file is provided and all cell fluxes just
  /// use conversion factor 1.0.
  BDSPSCellFluxScaled3D(const G4String&         scorerName,
                        const BDSHistBinMapper* mapperIn,
                        const G4String&         unitIn = "percm2",
                        G4int ni=1, G4int nj=1, G4int nk=1,
                        G4int depi=2, G4int depj=1, G4int depk=0);

  /// Constructor where conversion factor file is provided and loaded into a physics vector.
  /// Cell fluxes are multiplied by the factor as a function of the particle kinetic energy.
  BDSPSCellFluxScaled3D(const G4String&         scorerName,
                        const BDSHistBinMapper* mapperIn,
                        const G4String&         filename,
                        const G4String&         unitIn = "percm2",
                        G4int ni=1, G4int nj=1, G4int nk=1,
                        G4int depi=2, G4int depj=1, G4int depk=0);

  virtual ~BDSPSCellFluxScaled3D() override;
  
public:
  void   Initialize(G4HCofThisEvent* HCE) override;
  void   EndOfEvent(G4HCofThisEvent* HCE) override;
  void   clear() override;
  G4bool ProcessHits(G4Step* aStep, G4TouchableHistory*) override;
  G4int  GetIndex(G4Step* aStep) override;
  
  virtual G4double GetConversionFactor(G4int    particleID,
				       G4double kineticEnergy) const;
  
private:
  /// Define units -> taken from G4PSCellFlux
  void DefineUnitAndCategory() const;

  G4int                 HCID3D;   ///< Collection ID.
  G4THitsMap<G4double>* evtMap3D; ///< Hits map.
  
  /// @{ Depth in replica to look for each dimension.
  G4int fDepthi;
  G4int fDepthj;
  G4int fDepthk;
  /// @}
  
  /// Conversion factor interpolator object.
  G4PhysicsVector* conversionFactor;
  
  /// Mapping from coordinate systems in mesh to global replica number.
  const BDSHistBinMapper* mapper; ///< We don't own this.
  
  std::map<G4VSolid*, G4double> volumeCache;
};

#endif
