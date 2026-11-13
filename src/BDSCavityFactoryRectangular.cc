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
#include "BDSCavity.hh"
#include "BDSCavityFactoryRectangular.hh"
#include "BDSCavityInfo.hh"

#include "globals.hh"
#include "G4Box.hh"
#include "G4SubtractionSolid.hh"
#include "G4Tubs.hh"
#include "G4UnionSolid.hh"
#include "G4VSolid.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include <set>

BDSCavityFactoryRectangular::BDSCavityFactoryRectangular()
{;}

BDSCavityFactoryRectangular::~BDSCavityFactoryRectangular()
{;}

G4double BDSCavityFactoryRectangular::CreateSolids(const G4String&      name,
						   G4double             totalChordLength,
						   const BDSCavityInfo* info)
{
  G4double thickness    = info->thickness;
  G4double chordLength  = totalChordLength;
  G4double cavityRadius = info->equatorRadius;
  G4double outerBoxHalf = cavityRadius + thickness;
  G4VSolid* outerSolid = new G4Box(name + "_outer_solid",           // name
				   outerBoxHalf,                    // dx
				   outerBoxHalf,                    // dy
				   0.5*chordLength - lengthSafety); // dz
  G4VSolid* sub = new G4Box(name + "_outer_sub_solid",    // name
			    outerBoxHalf - thickness,     // dx
			    outerBoxHalf - thickness,     // dy
			    0.5*chordLength - thickness); // dz
  allSolids.insert(outerSolid);
  allSolids.insert(sub);
  
  G4VSolid* innerSolid = new G4Tubs(name + "_inner_solid",  // name
				    0.0,                    // inner radius 
				    info->irisRadius,       // outer radius
				    chordLength,            // galf length
				    0.0,                    // star angle
				    CLHEP::twopi);          // sweep angle
  allSolids.insert(innerSolid);
  
  // subtraction
  G4VSolid* cavitySolid1 = new G4SubtractionSolid(name + "_cavity_sub_solid",//name
						  outerSolid,                //solid1
						  sub);               //minus solid2
  allSolids.insert(cavitySolid1);
  cavitySolid = new G4SubtractionSolid(name + "_cavity_solid", // name
				       cavitySolid1,
				       innerSolid);
  
  // vacuum: union of two solid - one cylinder (VacuumInnerCavity) to fill the centre,
  // and a longer, thinner cylinder (vacuumAperture) to fill the ends provided by the thickness.
  G4double vacuumHalfWidth    = outerBoxHalf - thickness - lengthSafety;
  G4VSolid* vacuumInnerCavity = new G4Box(name + "_vacuum_inner_cavity_solid",// name
					  vacuumHalfWidth,
					  vacuumHalfWidth,
					  0.5*chordLength - thickness - lengthSafety);

  G4VSolid* vacuumAperture = new G4Tubs(name + "_vacuum_aperture_solid",   // name
					0.0,                               // inner radius
					info->irisRadius - lengthSafety,   // outer radius
					0.5*chordLength - lengthSafety,    // half length
					0.0,                               // start angle
					CLHEP::twopi);                     // sweep angle
  
  // create the vacuum as a union of the two solides defined prior
  vacuumSolid = new G4UnionSolid(name + "_vacuum_solid",  // name
				 vacuumInnerCavity,       // solid one
				 vacuumAperture);         // added to solid two.
  allSolids.insert(vacuumInnerCavity);
  allSolids.insert(vacuumAperture);
  allSolids.insert(vacuumSolid);

  G4double containerRadius = cavityRadius + thickness + lengthSafety;
  containerSolid = new G4Box(name + "_container_solid",   // name
			     containerRadius,             // innerRadius
			     containerRadius,             // outerRadius
			     chordLength*0.5);            // half length
  return containerRadius;
}
