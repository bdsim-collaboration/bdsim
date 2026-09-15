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
#ifndef BDSPHYSICSSYNCHROTRONRADIATIONCUT_H
#define BDSPHYSICSSYNCHROTRONRADIATIONCUT_H

#include "BDSSingleUse.hh"

#include "G4Types.hh"
#include "G4VPhysicsConstructor.hh"

/**
* @brief Optical physics processes.
 *
* @author Siobhan Alden
 */

class BDSPhysicsSynchrotronRadiationCut: public G4VPhysicsConstructor, public BDSSingleUse
{
public:
    BDSPhysicsSynchrotronRadiationCut() = delete;
    explicit BDSPhysicsSynchrotronRadiationCut(G4double energyCutIn);
    virtual ~BDSPhysicsSynchrotronRadiationCut();

    /// Construct all leptons, photons (inc optical), and pion +- just in case.
    virtual void ConstructParticle();

    /// Construct and attach the processes to the relevant particles.
    virtual void ConstructProcess();

private:
    G4double energyCut;
};
#endif

