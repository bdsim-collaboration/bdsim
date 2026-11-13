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
//eBremsstrahlung Leading Particle Biasing Method, M.D. Salt, R.B. Appleby, 15/10/09

#ifndef BDSEBREMSSTRAHLUNGLPB_H
#define BDSEBREMSSTRAHLUNGLPB_H

#include "G4WrapperProcess.hh"

class eBremsstrahlung_LPB: public G4WrapperProcess
{
  public:
    G4VParticleChange* PostStepDoIt(const G4Track& track, const G4Step& step);

  private:

};

class eBremsstrahlung_LPB_v2: public G4WrapperProcess
{
  public:
    G4VParticleChange* PostStepDoIt(const G4Track& track, const G4Step& step);

  private:

};
#endif
