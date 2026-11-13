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
#ifndef BDSMAGNETOUTER_H
#define BDSMAGNETOUTER_H

#include "BDSGeometryComponent.hh"

#include "globals.hh"
#include "G4ThreeVector.hh"

class BDSGeometryExternal;
class BDSSimpleComponent;
class G4LogicalVolume;
class G4VSolid;

/**
 * @brief An object for both the returned magnet outer body but also a
 * tight fitting container for the whole magnet.
 * 
 * Mostly BDSGeometryComponent but with the 
 * addition of a suggested container volume for the whole magnet body
 * as the magnet outer section is typically the outside of the magnet
 * and only the magnet factory knows its true shape and can make a well
 * fitting container.
 * 
 * @author Laurie Nevay
 */

class BDSMagnetOuter: public BDSGeometryComponent
{
public:
  BDSMagnetOuter(G4VSolid*             containerSolidIn,
		 G4LogicalVolume*      containerLVIn,
		 const BDSExtent&      extent,
		 BDSGeometryComponent* magnetContainerIn,
		 const G4ThreeVector&  placementOffset    = G4ThreeVector(0,0,0),
		 BDSSimpleComponent*   endPieceBeforeIn   = nullptr,
		 BDSSimpleComponent*   endPieceAfterIn    = nullptr,
		 const G4ThreeVector&  inputFaceNormalIn  = G4ThreeVector(0,0,-1),
		 const G4ThreeVector&  outputFaceNormalIn = G4ThreeVector(0,0, 1));
  BDSMagnetOuter(BDSGeometryComponent* component,
		 BDSGeometryComponent* magnetContainerIn,
		 BDSSimpleComponent*   endPieceBeforeIn   = nullptr,
		 BDSSimpleComponent*   endPieceAfterIn    = nullptr,
		 const G4ThreeVector&  inputFaceNormalIn  = G4ThreeVector(0,0,-1),
		 const G4ThreeVector&  outputFaceNormalIn = G4ThreeVector(0,0, 1));
  BDSMagnetOuter(BDSGeometryExternal*  external,
		 BDSGeometryComponent* magnetContainerIn);
  virtual ~BDSMagnetOuter();

  /// Access the magnet container - a BDSGeometryComponent instance that has a suggested
  /// container solid (null pointer LV! - constructed from solid in BDSMagnet and saves doing it
  /// repeatedly in factories) that would contain both the magnet outer section and the
  /// beam pipe - so a solid container. Use as a way to pass solid + extents
  BDSGeometryComponent* GetMagnetContainer() const {return magnetContainer;}

  /// @{ Access the end piece.
  BDSSimpleComponent* EndPieceBefore() const {return endPieceBefore;}
  BDSSimpleComponent* EndPieceAfter()  const {return endPieceAfter;}
  /// @}

  void SetEndPieceBefore(BDSSimpleComponent* endPieceIn) {endPieceBefore = endPieceIn;}
  void SetEndPieceAfter(BDSSimpleComponent* endPieceIn)  {endPieceAfter = endPieceIn;}
  
  /// Clear the memory of the now unneeded magnet container object.
  void ClearMagnetContainer();

  /// Clear the memory of the possibly unneeded end piece objects.
  void ClearEndPieces();

  /// @{ Accessor.
  inline G4ThreeVector InputFaceNormal()  const {return inputFaceNormal;}
  inline G4ThreeVector OutputFaceNormal() const {return outputFaceNormal;}
  /// @}

  /// @{ Setter for face normals.
  void SetInputFaceNormal(const G4ThreeVector& input);
  void SetOutputFaceNormal(const G4ThreeVector& output);
  /// @}
  
  /// Optional external geometry. An instance of this class may be based on one. In this
  /// case, the is the full object. Could be nullptr.
  BDSGeometryExternal* ExternalGeometry() const {return externalGeometry;}
  
protected:
  BDSGeometryComponent* magnetContainer;
  BDSSimpleComponent* endPieceBefore;
  BDSSimpleComponent* endPieceAfter;
  G4ThreeVector    inputFaceNormal;
  G4ThreeVector    outputFaceNormal;
  BDSGeometryExternal* externalGeometry;
};

#endif
