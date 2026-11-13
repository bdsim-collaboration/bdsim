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
#include "BDSCollimatorElliptical.hh"
#include "BDSDebug.hh"
#include "BDSUtilities.hh"
#include "BDSWarning.hh"

#include "globals.hh" // geant4 globals / types
#include "G4EllipticalTube.hh"
#include "G4EllipticalCone.hh"
#include "G4VSolid.hh"

BDSCollimatorElliptical::BDSCollimatorElliptical(const G4String& nameIn,
                                                 G4double    lengthIn,
                                                 G4double    horizontalWidthIn,
                                                 G4Material* collimatorMaterialIn,
                                                 G4Material* vacuumMaterialIn,
                                                 G4double    xApertureIn,
                                                 G4double    yApertureIn,
                                                 G4double    xApertureOutIn,
                                                 G4double    yApertureOutIn,
                                                 G4Colour*   colourIn,
                                                 G4bool      circularOuterIn):
  BDSCollimator(nameIn, lengthIn, horizontalWidthIn, "ecol",
                collimatorMaterialIn, vacuumMaterialIn,
                xApertureIn, yApertureIn, xApertureOutIn, yApertureOutIn, colourIn, circularOuterIn)
{;}

void BDSCollimatorElliptical::CheckParameters()
{
  BDSCollimator::CheckParameters();
  if (BDS::IsFinite(xApertureOut) && BDS::IsFinite(yApertureOut) && BDS::IsFinite(xAperture) &&
      BDS::IsFinite(yAperture))
    {
      if ((xApertureOut / yApertureOut) != (xAperture / yAperture))
        {BDS::Warning(__METHOD_NAME__, "element: \"" + name + "\": X/Y half axes ratio at entrance and exit apertures are not equal");}
    }
}

void BDSCollimatorElliptical::BuildInnerCollimator()
{
  if (tapered)
    {
      // we use the notes from G4EllipticalCone.hh
      //   1. half length in Z = zTopCut
      //   2. Dx and Dy =  half length of ellipse axis  at  z = -zTopCut
      //   3. dx and dy =  half length of ellipse axis  at  z =  zTopCut
      //   ! Attention :  dx/dy=Dx/Dy
      //   xSemiAxis = (Dx-dx)/(2*zTopCut)
      //   ySemiAxis = (Dy-dy)/(2*zTopCut)
      //   z height  = (Dx+dx)/(2*xSemiAxis)
      G4double xSemiAxis = (xAperture - xApertureOut) / chordLength;
      G4double ySemiAxis = (yAperture - yApertureOut) / chordLength;
      G4double zHeight   = (xAperture + xApertureOut) / (2*xSemiAxis);

      innerSolid  = new G4EllipticalCone(name + "_inner_solid", // name
                                         xSemiAxis,             // major axis of largest ellipse
                                         ySemiAxis,             // minor axis of largest ellipse
                                         zHeight,               // height of cone
                                         1.02*chordLength*0.5); // cut
    
      vacuumSolid = new G4EllipticalCone(name + "_vacuum_solid",// name
                                         xSemiAxis,             // major axis of largest ellipse
                                         ySemiAxis,             // minor axis of largest ellipse
                                         zHeight,               // height of cone
                                         chordLength*0.5 - lengthSafety); // cut
    }
  else
    {
      innerSolid  = new G4EllipticalTube(name + "_inner_solid",    // name
                                         xAperture,                // x half width
                                         yAperture,                // y half width
                                         chordLength);             // z half length
      // z half length long for unambiguous subtraction

      vacuumSolid = new G4EllipticalTube(name + "_inner_solid",    // name
                                         xAperture - lengthSafety, // x half width
                                         yAperture - lengthSafety, // y half width
                                         chordLength * 0.5);       // z half length
    }
        
  RegisterSolid(innerSolid);
  RegisterSolid(vacuumSolid);
}
