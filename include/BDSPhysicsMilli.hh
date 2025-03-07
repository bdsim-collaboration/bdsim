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
#ifndef BDSPhysicsMilli_H
#define BDSPhysicsMilli_H

#include "G4VPhysicsConstructor.hh"
#include "globals.hh"

/**
 * @brief EM processes for millicharged particles.
 * 
 * Constructs:

 *
 * Uses the G4PhysicsListHelper to set and order processes.
 *
 */

class BDSPhysicsMilli: public G4VPhysicsConstructor
{
public:
    explicit BDSPhysicsMilli(const G4String& name="", G4int ver=1);
    virtual ~BDSPhysicsMilli();

    /// Overloaded particle constructor to construct millicharged.
    virtual void ConstructParticle();

    /// Construct and attach the physics processes.
    virtual void ConstructProcess();

private:
    G4int verbose;
};
#endif
