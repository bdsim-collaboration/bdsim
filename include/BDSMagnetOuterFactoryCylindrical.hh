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
#ifndef BDSMAGNETOUTERFACTORYCYLINDRICAL_H
#define BDSMAGNETOUTERFACTORYCYLINDRICAL_H

#include "BDSMagnetOuterFactoryBase.hh"

#include "globals.hh"           // geant4 globals / types

class BDSBeamPipe;
class BDSMagnetOuterInfo;
class G4Colour;
class G4Material;

/**
 * @brief Factory that produces cylindrical magnet geometry.
 * 
 * @author Laurie Nevay
 */

class BDSMagnetOuterFactoryCylindrical: public BDSMagnetOuterFactoryBase
{
public:
  BDSMagnetOuterFactoryCylindrical();
  virtual ~BDSMagnetOuterFactoryCylindrical(){;}

  virtual void CleanUp();
  
  /// sector bend outer volume
  virtual BDSMagnetOuter* CreateSectorBend(G4String           name,             // name
					   G4double           length,           // full length [mm]
					   const BDSBeamPipe* beamPipe,         // beampipe
					   G4double           containerLength,  // full length to make AccComp container
					   const BDSMagnetOuterInfo* recipe);   // recipe for geometry
  
  /// rectangular bend outer volume
  virtual BDSMagnetOuter* CreateRectangularBend(G4String           name,             // name
						G4double           length,           // full length [mm]
						const BDSBeamPipe* beamPipe,         // beampipe
						G4double           containerLength,  // full length to make AccComp container
						const BDSMagnetOuterInfo* recipe);   // recipe for geometry
  
  /// quadrupole outer volume
  virtual BDSMagnetOuter* CreateQuadrupole(G4String     name,                  // name
					   G4double     length,                // length
					   BDSBeamPipe* beamPipe,              // beampipe
					   G4double     containerLength,       // full length to make AccComp container
					   const BDSMagnetOuterInfo* recipe);  // geometry recipe


  /// sextupole outer volume
  virtual BDSMagnetOuter* CreateSextupole(G4String     name,                  // name
					  G4double     length,                // length
					  BDSBeamPipe* beamPipe,              // beampipe
					  G4double     containerLength,       // full length to make AccComp container
					  const BDSMagnetOuterInfo* recipe);  // geometry recipe
  
  /// octupole outer volume
  virtual BDSMagnetOuter* CreateOctupole(G4String     name,                  // name
					 G4double     length,                // length 
					 BDSBeamPipe* beamPipe,              // beampipe
					 G4double     containerLength,       // full length to make AccComp container
					 const BDSMagnetOuterInfo* recipe);  // geometry recipe
  
  /// decapole outer volume
  virtual BDSMagnetOuter* CreateDecapole(G4String     name,                  // name
					 G4double     length,                // length
					 BDSBeamPipe* beamPipe,              // beampipe
					 G4double     containerLength,       // full length to make AccComp container
					 const BDSMagnetOuterInfo* recipe);  // geometry recipe
  
  /// solenoid  outer volume
  virtual BDSMagnetOuter* CreateSolenoid(G4String     name,                  // name
					 G4double     length,                // length
					 BDSBeamPipe* beamPipe,              // beampipe
					 G4double     containerLength,       // full length to make AccComp
					 const BDSMagnetOuterInfo* recipe);  // geometry recipe
  
  /// general multipole outer volume - could be any 2N order multipole
  virtual BDSMagnetOuter* CreateMultipole(G4String     name,                  // name
					  G4double     length,                // length
					  BDSBeamPipe* beamPipe,              // beampipe
					  G4double     containerLength,       // full length to make AccComp container
					  const BDSMagnetOuterInfo* recipe);  // geometry recipe

  /// RF cavity outer volume
  virtual BDSMagnetOuter* CreateRfCavity(G4String     name,                  // name
					 G4double     length,                // length 
					 BDSBeamPipe* beamPipe,              // beampipe
					 G4double     containerLength,       // full length to make AccComp container
					 const BDSMagnetOuterInfo* recipe);  // geometry recipe
  
  /// muon spoiler outer volume
  virtual BDSMagnetOuter* CreateMuonSpoiler(G4String     name,                  // name
					    G4double     length,                // length
					    BDSBeamPipe* beamPipe,              // beampipe
					    G4double     containerLength,       // full length to make AccComp container
					    const BDSMagnetOuterInfo* recipe);  // geometry recipe

  /// horizontal and vertical kicker outer volume
  virtual BDSMagnetOuter* CreateKicker(G4String                  name,            // name
				       G4double                  length,          // length [mm]
				       const BDSBeamPipe*        beamPipe,        // beampipe
				       G4double                  containerLength, // full length to make AccComp container
				       const BDSMagnetOuterInfo* recipe,          // geometry recipe
				       G4bool                    vertical);       // is it a vertical kicker?
  
private:
  /// Only really one function needed for this factory private to this factory only.
  void CreateCylindricalSolids(G4String     name,                  // name
			       G4double     length,                // full length
			       const BDSBeamPipe* beamPipe,        // beampipe
			       G4double     magnetContainerLength, // magnet object container full length
			       G4double     boxSize);              // full width

  void CreateCylindricalSolidsAngled(G4String      name,                  // name
				     G4double      length,                // full length
				     const BDSBeamPipe*  beamPipe,        // beampipe
				     G4double      magnetContainerLength, // magnet object container full length
				     G4double      boxSize);              // full width

  /// Only the solids are unique, once we have those, the logical volumes and placement in the
  /// container are the same.  Group all this functionality together
  BDSMagnetOuter* CommonFinalConstructor(G4String    name,
					 G4double    length,
					 const BDSMagnetOuterInfo* recipe);

  /// test inputs for no null pointers or overlapping volumes due to poorly defined sizes
  void TestInputParameters(const BDSBeamPipe* beamPipe,
			   G4double&    horizontalWidthIn);

  G4double magnetContainerRadius;
};

#endif
