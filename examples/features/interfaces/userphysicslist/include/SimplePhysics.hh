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
#ifndef SIMPLEPHYSICS_H
#define SIMPLEPHYSICS_H

#include "G4VModularPhysicsList.hh"

/**
 * @brief A simple custom physics list.
 *
 * Includes only decay and em option2 - purely as an example.
 * 
 * @author Laurie Nevay
 */

class SimplePhysics: public G4VModularPhysicsList
{
public:
  SimplePhysics();
  virtual ~SimplePhysics();
};

#endif
