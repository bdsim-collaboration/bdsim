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
#include "BDSBunchFileBased.hh"

#include "parser/beam.h"

BDSBunchFileBased::BDSBunchFileBased(const G4String& distributionName):
  BDSBunch(distributionName),
  nOriginalEvents(0),
  nEventsInFile(0),
  nEventsInFileSkipped(0),
  distrFileLoop(false),
  distrFileLoopNTimes(0)
{;}

BDSBunchFileBased::~BDSBunchFileBased() 
{;}

void BDSBunchFileBased::SetOptions(const BDSParticleDefinition* beamParticle,
                                   const GMAD::Beam& beam,
                                   const BDSBunchType& distrType,
                                   G4Transform3D beamlineTransformIn,
                                   const G4double beamlineSIn)
{
  BDSBunch::SetOptions(beamParticle, beam, distrType, beamlineTransformIn, beamlineSIn);
  distrFileLoop = beam.distrFileLoop || beam.distrFileLoopNTimes > 1;
  distrFileLoopNTimes = beam.distrFileLoopNTimes;
}

void BDSBunchFileBased::BeginOfRunAction(G4int /*numberOfEvents*/,
                                         G4bool batchMode)
{
  // If interactive, we must permit looping as the user may request more
  // events than are in a file, and they may not have explicitly set the
  // right combination of options in the input.
  distrFileLoop = !batchMode || distrFileLoop;
}