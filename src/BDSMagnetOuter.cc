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
#include "BDSGeometryComponent.hh"
#include "BDSGeometryExternal.hh"
#include "BDSMagnetOuter.hh"
#include "BDSSimpleComponent.hh"

#include "G4ThreeVector.hh"

BDSMagnetOuter::BDSMagnetOuter(G4VSolid*             containerSolidIn,
			       G4LogicalVolume*      containerLVIn,
			       const BDSExtent&      extentIn,
			       BDSGeometryComponent* magnetContainerIn,
			       const G4ThreeVector&  placementOffsetIn,
			       BDSSimpleComponent*   endPieceBeforeIn,
			       BDSSimpleComponent*   endPieceAfterIn,
			       const G4ThreeVector&  inputFaceNormalIn,
			       const G4ThreeVector&  outputFaceNormalIn):
  BDSGeometryComponent(containerSolidIn, containerLVIn, extentIn, BDSExtent(),
		       placementOffsetIn),
  magnetContainer(magnetContainerIn),
  endPieceBefore(endPieceBeforeIn),
  endPieceAfter(endPieceAfterIn),
  inputFaceNormal(inputFaceNormalIn),
  outputFaceNormal(outputFaceNormalIn),
  externalGeometry(nullptr)
{;}

BDSMagnetOuter::BDSMagnetOuter(BDSGeometryComponent* componentIn,
			       BDSGeometryComponent* magnetContainerIn,
			       BDSSimpleComponent*   endPieceBeforeIn,
			       BDSSimpleComponent*   endPieceAfterIn,
			       const G4ThreeVector&  inputFaceNormalIn,
			       const G4ThreeVector&  outputFaceNormalIn):
  BDSGeometryComponent(*componentIn),
  magnetContainer(magnetContainerIn),
  endPieceBefore(endPieceBeforeIn),
  endPieceAfter(endPieceAfterIn),
  inputFaceNormal(inputFaceNormalIn),
  outputFaceNormal(outputFaceNormalIn),
  externalGeometry(nullptr)
{;}

BDSMagnetOuter::BDSMagnetOuter(BDSGeometryExternal*  external,
			       BDSGeometryComponent* magnetContainerIn):
  BDSGeometryComponent(*(static_cast<BDSGeometryComponent*>(external))),
  magnetContainer(magnetContainerIn),
  endPieceBefore(nullptr),
  endPieceAfter(nullptr),
  inputFaceNormal(G4ThreeVector(0,0,-1)),
  outputFaceNormal(G4ThreeVector(0,0,1)),
  externalGeometry(external)
{;}

BDSMagnetOuter::~BDSMagnetOuter()
{
  ClearMagnetContainer();
  ClearEndPieces();
}

void BDSMagnetOuter::ClearMagnetContainer()
{
  if (magnetContainer)
    {
      delete magnetContainer;
      magnetContainer = nullptr;
    }
}

void BDSMagnetOuter::ClearEndPieces()
{
  if (endPieceAfter && (endPieceAfter != endPieceBefore))
    {delete endPieceAfter; endPieceAfter = nullptr;}
  if (endPieceBefore)
    {delete endPieceBefore; endPieceBefore = nullptr;}
}

void BDSMagnetOuter::SetInputFaceNormal(const G4ThreeVector& input)
{
  inputFaceNormal = input;
  if (endPieceBefore)
    {endPieceBefore->SetInputFaceNormal(input);}
}

void BDSMagnetOuter::SetOutputFaceNormal(const G4ThreeVector& output)
{
  outputFaceNormal = output;
  if (endPieceAfter)
    {endPieceBefore->SetInputFaceNormal(output);}
}
