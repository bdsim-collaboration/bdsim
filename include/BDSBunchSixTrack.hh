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
#ifndef BDSBUNCHSIXTRACK_H
#define BDSBUNCHSIXTRACK_H

#include <vector>

#include "BDSBunch.hh"

/**
 * @brief A bunch distribution that reads a SixTrack hits file.
 * 
 * @author Regina Kwee-Hinzmann
 */

class BDSBunchSixTrack: public BDSBunch
{ 
public: 
  BDSBunchSixTrack();
  explicit BDSBunchSixTrack(G4String fileNameIn);
  virtual ~BDSBunchSixTrack(); 
  virtual void SetOptions(const BDSParticleDefinition* beamParticle,
			  const GMAD::Beam& beam,
			  const BDSBunchType& distrType,
			  G4Transform3D beamlineTransformIn = G4Transform3D::Identity,
			  const G4double beamlineS = 0);

  virtual BDSParticleCoordsFull GetNextParticleLocal();

private:
  void LoadSixTrackFile();
  
  G4int    nPart;
  G4String fileName;
  G4int    iPart;    ///< Current ray.
  std::vector<double*> sixtrackData; 
};

#endif
