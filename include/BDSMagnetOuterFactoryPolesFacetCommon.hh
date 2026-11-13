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
#ifndef BDSMAGNETOUTERFACTORYPOLESFACETCOMMON_H
#define BDSMAGNETOUTERFACTORYPOLESFACETCOMMON_H

#include "BDSMagnetOuterFactoryPolesBase.hh"

#include "globals.hh"           // geant4 globals / types

/**
 * @brief Factory class for outer volume of magnets. 
 *
 * Produces magnets
 * with 2N-poles around the beampipe with a faceted yoke - the yoke 
 * is perpendiular to the pole direction where the pole joins the yoke.
 * 
 * Most magnets are 2N poles, but sector- and r-bends as well as 
 * muon spoilers, and h/v kickers are unique.
 *
 * This class provides the common code for 'facet' and 'facetcrop'
 * styles as there's only one number different in the yoke code.
 *
 * @author Laurie Nevay
 */

class BDSMagnetOuterFactoryPolesFacetCommon: public BDSMagnetOuterFactoryPolesBase
{
public:
  explicit BDSMagnetOuterFactoryPolesFacetCommon(G4double factor);
  
  virtual ~BDSMagnetOuterFactoryPolesFacetCommon();

protected:
  /// Calculate start angle of polyhedra based on order and segment angle.
  virtual void CalculateStartAngles();
  
  /// Start angle for polyhedra.
  G4double polyStartAngle;

  /// Start angle for pole intersection.
  G4double poleIntersectionStartAngle;

private:
  /// Private constructor to force use of supplied one.
  BDSMagnetOuterFactoryPolesFacetCommon();
  
  /// Create yoke that connects poles and container to put them in
  virtual void CreateYokeAndContainerSolid(const G4String& name,
					   G4double length,
					   G4int    order,
                                           G4double magnetContainerLength,
					   G4double magnetContainerRadiusIn);
  
  /// Factor by which number of polyhedra vertices is multiplied by.
  G4double factor;
};

#endif
