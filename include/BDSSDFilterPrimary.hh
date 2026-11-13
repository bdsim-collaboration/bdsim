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
#ifndef BDSSDFILTERPRIMARY_H
#define BDSSDFILTERPRIMARY_H

#include "G4String.hh"
#include "G4Types.hh"
#include "G4VSDFilter.hh"

/**
 * @brief Filter for only primary particles.
 * 
 * @author Laurie Nevay
 */

class BDSSDFilterPrimary: public G4VSDFilter
{
public:
  BDSSDFilterPrimary(const G4String& name);
  virtual ~BDSSDFilterPrimary(){;}

  /// Return true if the particle is a primary.
  virtual G4bool Accept(const G4Step* step) const;

private:
  BDSSDFilterPrimary() = delete;
};

#endif
