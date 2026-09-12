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
#include "BDSCurvilinearFactory.hh"
#include "BDSExtent.hh"
#include "BDSGlobalConstants.hh"
#include "BDSSimpleComponent.hh"
#include "BDSTiltOffset.hh"
#include "BDSUtilities.hh"

#include "globals.hh"
#include "G4CutTubs.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4Tubs.hh"
#include "G4VSolid.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>

BDSCurvilinearFactory::BDSCurvilinearFactory():
  lengthSafety(BDSGlobalConstants::Instance()->LengthSafety())
{;}

BDSCurvilinearFactory::~BDSCurvilinearFactory()
{;}

BDSSimpleComponent* BDSCurvilinearFactory::CreateCurvilinearVolume(const G4String& name,
                                                                   G4double        chordLength,
                                                                   G4double        radius)
{
  G4double halfLength = chordLength * 0.5 - lengthSafety;
  G4Tubs* solid = new G4Tubs(name + "_solid", // name
                             0,                  // inner radius
                             radius,             // outer radius
                             halfLength,         // z half width
                             0,                  // start angle
                             CLHEP::twopi);      // sweep angle

  G4ThreeVector inputFaceNormal  = G4ThreeVector(0, 0,-1);
  G4ThreeVector outputFaceNormal = G4ThreeVector(0, 0, 1);

  return CommonConstruction(name, chordLength, chordLength, radius,
                            solid, 0);
}

BDSSimpleComponent* BDSCurvilinearFactory::CreateCurvilinearVolume(const G4String&      name,
                                                                   const G4double       arcLength,
                                                                   const G4double       chordLength,
                                                                   const G4double       radius,
                                                                   const G4double       angle,
                                                                   const G4ThreeVector& inputFaceNormal,
                                                                   const G4ThreeVector& outputFaceNormal,
                                                                   const BDSTiltOffset* tiltOffset)
{
  // angle is finite!
  // factor of 0.8 here is arbitrary tolerance as g4 cut tubs seems to fail
  // with cutting entrance / exit planes close to limit.
  // s = r*theta -> r = s/theta
  G4double radiusFromAngleLength = radius;
  if (BDS::IsFinite(angle)) // it could be 0
    {radiusFromAngleLength =  std::abs(chordLength / angle) * 0.8;}
  G4double radiusLocal = std::min(radius, radiusFromAngleLength);

  // copy in case we need to modify in the case of tilt offset
  G4ThreeVector inputface  = inputFaceNormal;
  G4ThreeVector outputface = outputFaceNormal;

  if (tiltOffset)
    {// could be nullptr
      G4double tilt = tiltOffset->GetTilt();
      if (BDS::IsFinite(tilt))
        {// rotate normal faces
          inputface = inputface.rotateZ(tilt);
          outputface = outputface.rotateZ(tilt);
        }
    }
  
  G4double halfLength = chordLength * 0.5 - lengthSafety;
  G4CutTubs* solid = new G4CutTubs(name + "_solid", // name
                                   0,                  // inner radius
                                   radiusLocal,        // outer radius
                                   halfLength,         // half length (z)
                                   0,                  // rotation start angle
                                   CLHEP::twopi,       // rotation sweep angle
                                   inputface,          // input face normal vector
                                   outputface);        // output face normal vector

  return CommonConstruction(name, arcLength, chordLength, radiusLocal, solid, angle);
}

BDSSimpleComponent* BDSCurvilinearFactory::CreateCurvilinearVolume(const G4String& name,
                                                                   G4double        arcLength,
                                                                   G4double        chordLength,
                                                                   G4double        radius,
                                                                   G4double        angle,
                                                                   const BDSTiltOffset* tiltOffset)
{
  std::pair<G4ThreeVector,G4ThreeVector> faces = BDS::CalculateFaces(-0.5*angle, -0.5*angle);
  G4ThreeVector inputFaceNormal  = faces.first;
  G4ThreeVector outputFaceNormal = faces.second;

  return CreateCurvilinearVolume(name, arcLength, chordLength, radius, angle, inputFaceNormal, outputFaceNormal, tiltOffset);
}

BDSSimpleComponent* BDSCurvilinearFactory::CommonConstruction(const G4String& name,
                                                              G4double        arcLength,
                                                              G4double        chordLength,
                                                              G4double        radius,
                                                              G4VSolid*       solid,
                                                              G4double        angle)
{
  // nullptr for material ONLY ok in parallel world!
  G4LogicalVolume* lv =  new G4LogicalVolume(solid,            // solid
                                             nullptr,          // material
                                             name + "_lv"); // name

  // always debug visualisation for read out geometry - only viewed via explicit commands
  lv->SetVisAttributes(BDSGlobalConstants::Instance()->VisibleDebugVisAttr());

  // we don't specify the face normals as they are w.r.t the incoming or outgoing
  // reference trajectory - to which the curvilinear faces will be perpendicular
  BDSSimpleComponent* result = new BDSSimpleComponent(name,
                                                      arcLength,
                                                      angle,
                                                      solid,
                                                      lv,
                                                      BDSExtent(radius, radius, chordLength*0.5));
  // we don't bother calling Initialise() as we don't need to set the vis attributes (waste of memory)
  return result;
}
