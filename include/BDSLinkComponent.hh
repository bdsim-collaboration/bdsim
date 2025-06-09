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
#ifndef BDSLINKCOMPONENT_H
#define BDSLINKCOMPONENT_H

#include "BDSAcceleratorComponent.hh"
#include "BDSLinkOpaqueBox.hh"

#include "globals.hh" // geant4 types / globals

/**
 * @brief A BDSAcceleratorComponent wrapper for BDSLinkOpaqueBox.
 * 
 * This is similar to BDSSimpleComponent but specialised for a link
 * box containing a component. This is to allow it to be used in a beam line.
 *
 * Note, the lengths reported for this are for the full thing and not the component
 * inside as this is required to comply with beam line building mechanics.
 *
 * @author Laurie Nevay
 */

class BDSLinkComponent: public BDSAcceleratorComponent
{
public:
  BDSLinkComponent() = delete; ///< No default constructor.
  /// Constructor that keeps the component as a daughter geometry component.
  /// Face normal (unit) vectors are w.r.t. the incoming / outgoing reference 
  /// trajectory and NOT the local geometry of the component.
  BDSLinkComponent(const G4String&      nameIn,
                   BDSLinkOpaqueBox*    componentIn,
                   G4double             arcLengthIn,
                   G4double             angleIn = 0,
                   const G4ThreeVector& inputFaceNormalIn  = G4ThreeVector(0,0,-1),
                   const G4ThreeVector& outputFaceNormalIn = G4ThreeVector(0,0, 1));
  
  /// Default destructor suffices as this calls base class which
  /// clears everything up.
  virtual ~BDSLinkComponent() noexcept {;}

  /// @{ Accessor.
  inline BDSLinkOpaqueBox* Component() const {return component;}
  inline G4String LinkName() const {return component ? component->LinkName() : "unknown";}
  inline G4double ComponentArcLength() const {return component ? component->ArcLength() : -1.0;}
  inline G4double ComponentChordLength() const {return component ? component->ChordLength() : -1.0;}
  /// @}

private:
  /// @{ Assignment and copy constructor not implemented nor used
  BDSLinkComponent& operator=(const BDSLinkComponent&) = delete;
  BDSLinkComponent(BDSLinkComponent&) = delete;
  /// @}

  /// Required implementation from base class.
  virtual void BuildContainerLogicalVolume(){;}

  BDSLinkOpaqueBox* component;
};

#endif
