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
#ifndef BDSPARALLELWORLDSAMPER_H
#define BDSPARALLELWORLDSAMPER_H

#include "BDSBeamPipeType.hh"
#include "BDSSamplerType.hh"

#include "globals.hh"
#include "G4String.hh"
#include "G4Transform3D.hh"
#include "G4VUserParallelWorld.hh"

#include <map>
#include <vector>

class G4VisAttributes;
class G4VPhysicalVolume;
class BDSBeamlineElement;
class BDSSampler;
class BDSSamplerPlane;
namespace GMAD
{
  class SamplerPlacement;
}

/**
 * @brief A parallel world for sampler planes.
 *
 * @author Laurie Nevay
 */

class BDSParallelWorldSampler: public G4VUserParallelWorld
{
public:
  BDSParallelWorldSampler() = delete; ///< No default constructor.
  explicit BDSParallelWorldSampler(const G4String& name);
  virtual ~BDSParallelWorldSampler();

  /// Construct the required parallel world geometry. This must
  /// overload the pure virtual method in G4VUserParallelWorld.
  virtual void Construct();

  /// @{ Accessor.
  BDSSamplerPlane* GeneralPlane() const {return generalPlane;}
  G4LogicalVolume* WorldLV() const {return samplerWorldLV;}
  /// @}
  
  /// Place a sampler from a single element.
  void Place(const BDSBeamlineElement* element,
	     G4double                  samplerRadius);

  /// Register a sampler during mass-world construction and defer only its
  /// physical placement until this parallel world is constructed.
  G4int RegisterSamplerForConstruction(const G4String&      name,
                                       BDSSampler*          sampler,
                                       const G4Transform3D& transform);

private:
  struct DeferredSamplerPlacement
  {
    G4String      name;
    BDSSampler*   sampler;
    G4Transform3D transform;
    G4int         samplerID;
  };

  /// Utility function to reduce code.
  void ErrorNonPositive(G4double value,
                        const G4String& variableName,
                        const G4String& objectName) const;
  
  /// Construct the geometry for a sampler. Update 'radius' by reference if applicable.
  BDSSampler* BuildSampler(const GMAD::SamplerPlacement& samplerPlacement,
                           BDSSamplerType st,
                           G4double& radius) const;
  
  /// Compound the rotation of the placement transform for sphereforward and
  /// cylinderforward types of sampler only to orientate them along the beam line direction.
  void AdjustTransform(G4Transform3D& trans,
                       BDSSamplerType st) const;
  
  /// Cache of the placements to clean up at the end.
  std::vector<G4VPhysicalVolume*> placements;
  std::vector<DeferredSamplerPlacement> deferredPlacements;

  
  G4String suffix; ///< Just the input part of the world name.
  G4VisAttributes* samplerWorldVis; ///< Visualisation attributes for the sampler world.
  BDSSamplerPlane* generalPlane; ///< General single sampler we use for plane samplers.
  std::map<int, BDSSamplerPlane*> samplerInstances;
  G4LogicalVolume* samplerWorldLV;
};

#endif
