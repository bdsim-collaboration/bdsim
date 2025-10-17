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
#ifndef BDSLINKOPAQUEBOX_H
#define BDSLINKOPAQUEBOX_H
#include "BDSAcceleratorComponent.hh"
#include "BDSUtilities.hh"

#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"
#include "G4Types.hh"

#include "BDSGeometryComponent.hh"

class BDSAcceleratorComponent;
class BDSSamplerCustom;

/**
 * @brief Wrapper box for an accelerator component.
 * 
 * @author Laurie Nevay
 */

class BDSLinkOpaqueBox: public BDSGeometryComponent
{
public:
  BDSLinkOpaqueBox(BDSAcceleratorComponent* acceleratorComponentIn,
                   BDSTiltOffset* tiltOffsetIn,
                   G4double outputSamplerRadiusIn);
  virtual ~BDSLinkOpaqueBox();

  /// Default constructor
  BDSLinkOpaqueBox() = delete;

  /// Copy constructor
  BDSLinkOpaqueBox(const BDSLinkOpaqueBox &other) = delete;
  /// Copy assignment operator
  BDSLinkOpaqueBox& operator=(const BDSLinkOpaqueBox &other) = delete;

  inline const G4ThreeVector& OffsetToStart()    const {return offsetToStart;}
  inline const G4Transform3D& TransformToStart() const {return transformToStart;}

  /// Place the output sampler
  G4int PlaceOutputSampler();
  
  /// @{ Accessor
  G4double ArcLength()   const {return component ? component->GetArcLength() : 0.0;}
  G4double ChordLength() const {return component ? component->GetChordLength() : 0.0;}
  G4bool   Angled()      const {return component ? BDS::IsFinite(component->GetAngle()) : false;}
  G4String LinkName()    const {return component ? component->GetName() : "unknown";}
  /// @}

private:
  BDSAcceleratorComponent* component;
  G4double                 outputSamplerRadius;
  G4ThreeVector            offsetToStart;
  G4Transform3D            transformToStart;
  BDSSamplerCustom*        sampler;
};

#endif
