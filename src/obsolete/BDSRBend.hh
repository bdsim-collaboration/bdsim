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
#ifndef BDSRBEND_H
#define BDSRBEND_H 

#include "globals.hh"

#include "BDSMagnet.hh"

class BDSBeamPipeInfo;
class BDSMagnetOuterInfo;

class BDSBeamPipe;

class BDSRBend: public BDSMagnet
{
public:
  BDSRBend(G4String            name,
	   G4double            length,
	   G4double            angle,
	   BDSBeamPipeInfo*    beamPipeInfo,
	   BDSMagnetOuterInfo* magnetOuterInfo,
	   BDSFieldInfo*       vacuumFieldInfo,
	   BDSFieldInfo*       outerFieldInfo = nullptr);

  virtual ~BDSRBend(){;}

private:
  G4double magFieldLength;

  /// radius of magnet body
  G4double outerRadius;

  /// Override method from BDSAcceleratorComponent to detail construction process.
  virtual void Build();

  /// Override method so we can build several bits of beam pipe
  virtual void BuildBeampipe();

  /// Override BDSMagnet::BuildOuter() so we can get a different length of central section
  /// and magnet container.
  virtual void BuildOuter();

  /// Override BDSMagnet::BuildContainerLogicalVolume() so we can conditionally build the
  /// correct container volume if no outer magnet geometry is built. In the case of an RBend
  /// we can't simply use the central beam pipe segment as the container volume as it'll be too
  /// small.
  virtual void BuildContainerLogicalVolume();

  /// Place the beam pipe and outer geometry in the overall container. If there's no outer
  /// geometry, then we don't need to place either as the beam pipe becomes the container.
  virtual void PlaceComponents();

  /// temporary function while old constructor still exists - used to avoid duplicating
  /// code in the mean time
  void CalculateLengths(G4double aLength);
};

#endif
