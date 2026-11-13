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
#ifndef BDSSDFILTERPDGIDSET_H
#define BDSSDFILTERPDGIDSET_H

#include "G4String.hh"
#include "G4Types.hh"
#include "G4VSDFilter.hh"

#include <set>

/**
 * @brief Filter for a set of PDG IDs (ints).
 * 
 * @author Laurie Nevay
 */

class BDSSDFilterPDGIDSet: public G4VSDFilter
{
public:
  /// No default constructor.
  BDSSDFilterPDGIDSet() = delete;
  
  /// Construct with a set of PDG IDs to accept for a step and generate a hit.
  BDSSDFilterPDGIDSet(const G4String& name,
                      const std::set<G4int>& PDGIDsToAcceptIn);
  virtual ~BDSSDFilterPDGIDSet(){;}

  /// Return true if the particle is an ion or not.
  virtual G4bool Accept(const G4Step* step) const;

private:
  std::set<G4int> PDGIDsToAccept;
};

#endif
