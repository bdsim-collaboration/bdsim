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
#ifndef BDSAPERTUREINFO_H
#define BDSAPERTUREINFO_H

#include "BDSApertureType.hh"

#include "globals.hh"         // geant4 types / globals
#include "G4ThreeVector.hh"

class BDSExtent;
class G4Material;

/**
 * @brief Holder class for all information required to describe an aperture. 
 * 
 * @author Laurie Nevay
 */

class BDSApertureInfo
{
public:
  /// Constructor to assign all members at once
  BDSApertureInfo(BDSApertureType apertureTypeIn,
		  G4double        aper1In,
		  G4double        aper2In,
		  G4double        aper3In,
		  G4double        aper4In,
		  G4String        nameForError = "");

  /// Constructor with string descriptors of aperture type. Automatically determined.
  BDSApertureInfo(G4String apertureTypeIn,
		  G4double aper1In,
		  G4double aper2In,
		  G4double aper3In,
		  G4double aper4In,
		  G4String nameForError = "");

  /// Constructor that allows a default model to be used as backup. Checks on parameter
  /// validity are done after substituting unset values by values from defaultInfo.
  BDSApertureInfo(BDSApertureInfo* defaultInfo,
		  G4String         apertureTypeIn,
		  G4double         aper1In,
		  G4double         aper2In,
		  G4double         aper3In,
		  G4double         aper4In,
		  G4String         nameForError = "");

  /// Function to check relevant aperture values are set.  This is really a dispatch function
  /// for other aperture specific methods below
  void CheckApertureInfo(const G4String& nameForError);

  /// Return a BDSExtent instance for the transverse extents - extent in Z is 0 as this is
  /// only aperture information. Includes beam pipe thickness.
  BDSExtent Extent() const;

  /// Return an indicative extent of the beam pipe - typically the maximum of x or y extent.
  G4double IndicativeRadius() const;

  ///@{ Public member for direct access
  BDSApertureType apertureType;
  G4double        aper1;
  G4double        aper2;
  G4double        aper3;
  G4double        aper4;
  ///@}
  
private:
  /// Private default constructor to ensure one of supplied constructors is used.
  BDSApertureInfo() = delete;
  
  /// Function to check whether a parameter is set (using BDSUtilities BDS::IsFinite() ). If the
  /// accompanying G4bool setAper1 (for example) is true, the parameter will be checked for
  /// being set. All parameters required to be checked are checked and appropriate warnings
  /// printed before exiting if any fail that test.
  void CheckRequiredParametersSet(G4bool setAper1, G4bool setAper2,
				  G4bool setAper3, G4bool setAper4);
  
  /// Aperture info check for circular aperture.
  void InfoOKForCircular();

  /// Aperture info check for elliptical aperture.
  void InfoOKForElliptical();

  /// Aperture info check for rectangular aperture.
  void InfoOKForRectangular();

  /// Aperture info check for lhc aperture.
  void InfoOKForLHC();

  /// Aperture info check for lhc detailed aperture.
  void InfoOKForLHCDetailed();

  /// Aperture info check for rectellipse aperture.
  void InfoOKForRectEllipse();

  /// Aperture info check for racetrack aperture.
  void InfoOKForRaceTrack();

  /// Aperture info check for octagonal aperture.
  void InfoOKForOctagonal();

  /// Aperture info check for CLIC PCL aperture.
  void InfoOKForClicPCL();
};

#endif
