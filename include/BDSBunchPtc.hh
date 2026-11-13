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
#ifndef BDSBUNCHPTC_H
#define BDSBUNCHPTC_H

#include "BDSBunchFileBased.hh"

#include "G4String.hh"
#include "G4Types.hh"

#include <array>
#include <vector>

/**
 * @brief A bunch distribution that reads a PTC inrays file.
 * 
 * @author Stewart Boogert
 */

class BDSBunchPtc: public BDSBunchFileBased
{  
public: 
  BDSBunchPtc();
  virtual ~BDSBunchPtc(); 
  virtual void SetOptions(const BDSParticleDefinition* beamParticle,
			  const GMAD::Beam& beam,
			  const BDSBunchType& distrType,
			  G4Transform3D beamlineTransformIn = G4Transform3D::Identity,
			  const G4double beamlineS = 0);
  
  virtual void Initialise();
  
  virtual BDSParticleCoordsFull GetNextParticleLocal();
  
  virtual void RecreateAdvanceToEvent(G4int eventOffset);
  
private:
  /// Load the PTC file into memory
  void LoadPtcFile();

  G4bool   matchDistrFileLength; ///< Whether to only run the number of particles in the file.
  G4int    nRays;     ///< Number of rays in file (1 counting).
  G4String fileName;  ///< File name.
  G4int    iRay;      ///< Iterator counter for current ray.
  std::vector<std::array<double, 6>> ptcData; ///< Data.
  G4double beta;      ///< Velocity w.r.t. speed of light. Needed to convert mom. to energy.
  G4int nlinesSkip;
  G4int lineCounter;
};

#endif
