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

#include <utility>

class BDSAcceleratorComponent;
class BDSBeamline;
class BDSParallelWorldSampler;
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
                   G4double outputSamplerRadiusIn,
                   BDSAcceleratorComponent* inputGuardIn = nullptr,
                   BDSAcceleratorComponent* outputGuardIn = nullptr,
                   G4double inputGuardLengthIn = 0,
                   G4double outputGuardLengthIn = 0);
  virtual ~BDSLinkOpaqueBox();

  /// Default constructor
  BDSLinkOpaqueBox() = delete;

  /// Copy constructor
  BDSLinkOpaqueBox(const BDSLinkOpaqueBox &other) = delete;
  /// Copy assignment operator
  BDSLinkOpaqueBox& operator=(const BDSLinkOpaqueBox &other) = delete;

  inline const G4ThreeVector& OffsetToStart()    const {return offsetToStart;}
  inline const G4Transform3D& TransformToStart() const {return transformToStart;}
  inline const G4Transform3D& TransformToOutput() const {return transformToOutput;}
  inline G4double InputClearance() const {return inputClearance;}
  inline G4double OutputClearance() const {return outputClearance;}

  /// Determine how far a field-free guard must extend beyond the nominal
  /// input and output planes to contain angled faces completely.  A zero
  /// value means that the corresponding external face is perpendicular to
  /// the reference trajectory.
  static std::pair<G4double, G4double> FaceClearances(
    BDSAcceleratorComponent* component,
    const BDSTiltOffset* tiltOffset);

  /// Configure the delegated component field with its fixed world placement.
  void SetFieldLinkTransform(const G4Transform3D& opaqueToGlobal);

  /// Place the output sampler at the nominal exit in the sampler parallel world.
  G4int PlaceOutputSampler(BDSParallelWorldSampler* samplerWorld,
                           const G4Transform3D& outputToGlobal);

  /// @{ Accessor
  G4double ArcLength()   const {return component ? component->GetArcLength() : 0.0;}
  G4double ChordLength() const {return component ? component->GetChordLength() : 0.0;}
  G4bool   Angled()      const {return component ? BDS::IsFinite(component->GetAngle()) : false;}
  G4String LinkName()    const {return component ? component->GetName() : "unknown";}
  BDSAcceleratorComponent* Component() const {return component;}
  G4double Tilt() const {return tilt;}
  G4double OffsetX() const {return offsetX;}
  G4double OffsetY() const {return offsetY;}
  /// @}

private:
  BDSAcceleratorComponent* component;
  BDSBeamline*             componentBeamline;
  G4double                 outputSamplerRadius;
  G4double                 inputClearance;
  G4double                 outputClearance;
  G4double                 tilt;
  G4double                 offsetX;
  G4double                 offsetY;
  G4ThreeVector            offsetToStart;
  G4Transform3D            transformToStart;
  G4Transform3D            transformToOutput;
  G4Transform3D            nativeToOpaque;
  BDSSamplerCustom*        sampler;
};

#endif
