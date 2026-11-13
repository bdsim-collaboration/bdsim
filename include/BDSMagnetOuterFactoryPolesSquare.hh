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
#ifndef BDSMAGNETOUTERFACTORYPOLESSQUARE_H
#define BDSMAGNETOUTERFACTORYPOLESSQUARE_H

#include "BDSMagnetOuterFactoryPolesBase.hh"

#include "globals.hh"           // geant4 globals / types
#include <vector>

class BDSBeamPipe;
class G4Colour;
class G4LogicalVolume;
class G4Material;
class G4VSolid;

/**
 * @brief Factory class for outer volume of magnets. 
 * 
 * Produces magnets
 * with 2N-poles around the beampipe with a square yoke.
 * 
 * Most magnets are 2N poles, but sector- and r-bends as well as 
 * muon spoilers, and h/v kickers are unique.
 *
 * @author Laurie Nevay
 */

class BDSMagnetOuterFactoryPolesSquare: public BDSMagnetOuterFactoryPolesBase
{
public:
  BDSMagnetOuterFactoryPolesSquare();
  virtual ~BDSMagnetOuterFactoryPolesSquare(){;}

private:
  /// Clean up all memebers used.
  virtual void CleanUp();

  /// Clean up for this factory. Non-virtual as used in constructor.
  void CleanUpPolesSquare();

  /// Create yoke that connects poles and container to put them in
  virtual void CreateYokeAndContainerSolid(const G4String& name,
					   G4double        length,
					   G4int           order,
					   G4double        magnetContainerLength,
					   G4double        magnetContainerRadiusIn);

  virtual void IntersectPoleWithYoke(const G4String& name,
				     G4double        length,
				     G4int           orderIn);

  /// Build the logical volumes from the solids assigning materials and colours and cuts.
  /// This doesn't make use of any base class implementation as this class creates a
  /// vector of unique poles that must all be built individually into logical volumes.
  /// It does however make use of BDSMagnetOuterFactoryPolesBase::CreateLogicalVolumesCoil.
  virtual void CreateLogicalVolumes(const G4String& name,
				    G4Colour*       colour,
				    G4Material*     outerMaterial);

  /// Place the poles and yoke in the container volume
  virtual void PlaceComponents(const G4String& name,
			       G4int           order);
  
  /// Common construction tasks to all methods - assemble yoke and poles in container
  /// have to override this as we have multiple pole logical volumes and they need to
  /// be registered
  virtual BDSMagnetOuter* CommonConstructor(const G4String& name,
					    G4double        length,
					    BDSBeamPipe*    beamPipe,
					    G4int           order,
					    G4double        magnetContainerLength,
					    const BDSMagnetOuterInfo* recipe);
  
  /// Poles have to be different lengths to fit in a square - have to be unique
  /// therefore keep a vector of them
  std::vector<G4RotationMatrix*> poleRotations;
  std::vector<G4VSolid*> poleSolids;
  std::vector<G4LogicalVolume*> poleLVs;

  /// Need to pass order to CreateLogicalVolumes but should override for neatness and
  /// therefore can't change the signature of the function, so pass by internal variable.
  G4int order;
};

#endif
