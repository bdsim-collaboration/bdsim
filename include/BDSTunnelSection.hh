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
#ifndef BDSTUNNELSECTION_H
#define BDSTUNNELSECTION_H

#include "BDSAcceleratorComponent.hh"
#include "globals.hh" // geant4 types / globals

class BDSGeometryComponent;
class G4VSolid;

/**
 * @brief Class that represents a section of tunnel.
 *
 * @author Laurie Nevay
 */

class BDSTunnelSection: public BDSAcceleratorComponent
{
public:
  /// Constructor takes BDSGeometryComponent instance of already
  /// constructed geometry. The inner intersection solid is a solid that
  /// represents the cavity inside the tunnel. It can be used to intersect
  /// with any geometry such as the supports to ensure they always fit without
  /// having detailed knowledge about the floor and tunnel geometry itself.
  BDSTunnelSection(G4String              name,
		   G4double              chordLength,
		   G4double              angle,
		   BDSGeometryComponent* tunnelGeometry,
		   G4VSolid*             innerIntersectionSolidIn);

  virtual ~BDSTunnelSection();

  /// Override BDSAcceleratorComponent::Initialise() method to avoid doing
  /// anything. Doing this avoids wrong and useless read out volume being created.
  /// Note, this will mean that the private internal variable 'initialised' will always
  /// be false even though it effectively 'is' initialised.
  virtual void Initialise();

  G4VSolid* InnerIntersectionSolid() const {return innerIntersectionSolid;}

protected:
  /// Have to provide implementation - does nothing
  virtual void BuildContainerLogicalVolume();

  G4VSolid* innerIntersectionSolid;

private:
  /// Private default constructor to force the use of the supplied one.
  BDSTunnelSection() = delete;

  /// @{ Assignment and copy constructor not implemented nor used
  BDSTunnelSection& operator=(const BDSTunnelSection&) = delete;
  BDSTunnelSection(BDSTunnelSection&) = delete;
  /// @}
};

#endif
