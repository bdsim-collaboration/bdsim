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
#ifndef BDSSDFILTERMATERIAL_H
#define BDSSDFILTERMATERIAL_H

#include "G4String.hh"
#include "G4Types.hh"
#include "G4VSDFilter.hh"

#include <vector>

class G4Material;

/**
 * @brief SD filter for a particular volume.
 *
 * @author Robin Tesse
 */

class BDSSDFilterMaterial: public G4VSDFilter
{
public:
  /// If inclusive only materials that *are* found in the input vector
  /// correspond to passing the filter, i.e. accepting the step. If false,
  /// then any material matching will be rejected (any random material will
  /// be accepted).
  BDSSDFilterMaterial(const G4String& name,
		      const std::vector<G4Material*>& materialsIn,
		      G4bool   inclusiveIn);
  
  virtual ~BDSSDFilterMaterial();
  
  /// Whether the step will be accepted or rejected.
  virtual G4bool Accept(const G4Step* aStep) const;
  
private:
  std::vector<G4Material*> materials;
  G4bool inclusive;
};

#endif
