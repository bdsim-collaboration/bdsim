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
#ifndef BDSMAGNETOUTERFACTORYPOLESFACETCROP_H
#define BDSMAGNETOUTERFACTORYPOLESFACETCROP_H

#include "BDSMagnetOuterFactoryPolesFacetCommon.hh"

#include "globals.hh" // geant4 globals / types

/**
 * @brief Factory class for outer volume of magnets. 
 *
 * Produces magnets
 * with 2N-poles around the beampipe with a faceted yoke - the yoke 
 * is perpendiular to the pole direction where the pole joins the yoke.
 * Between each pole there is a flat section angled between the two poles
 * 
 * Most magnets are 2N poles, but sector- and r-bends as well as 
 * muon spoilers, and h/v kickers are unique.
 *
 * @author Laurie Nevay
 */

class BDSMagnetOuterFactoryPolesFacetCrop: public BDSMagnetOuterFactoryPolesFacetCommon
{
public:
  BDSMagnetOuterFactoryPolesFacetCrop();
  virtual ~BDSMagnetOuterFactoryPolesFacetCrop(){;}

protected:
  /// Calculate start angle of polyhedra based on order and segment angle.
  virtual void CalculateStartAngles(); 
};

#endif
