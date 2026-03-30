/*
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway,
University of London 2001 - 2024.

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
#ifndef BDSWRAPPERSYNCHROTRONRADIATIONCUT_H
#define BDSWRAPPERSYNCHROTRONRADIATIONCUT_H
#include "BDSWrapperProcess.hh"

#include "G4Types.hh"

class BDSPhysicsVectorLinear;
class G4Step;
class G4Track;
class G4VParticleChange;
class G4VProcess;

/**
* @brief Wrapper process to reduce number of optical photons produced in G4Scintillation.
 *
* The reduction factor is linearly applied to optical photon production per step.
* CLHEP random shoot used to uniformly select photons to keep from those produced to
* minimise statistical biasing. NOriginal/scaleFactor added to weight of optical photons kept.
* @author Siobhan Alden
 */

class BDSWrapperSynchrotronRadiationCut: public BDSWrapperProcess
{
public:
    BDSWrapperSynchrotronRadiationCut() = delete;
    BDSWrapperSynchrotronRadiationCut(G4VProcess* originalProcess,
                            G4double    energyCutIn);
    virtual ~BDSWrapperSynchrotronRadiationCut();

    /// Do the splitting operation.
    virtual G4VParticleChange* PostStepDoIt(const G4Track& track,
                                            const G4Step& step);



private:
    G4double energyCut;
};

#endif

