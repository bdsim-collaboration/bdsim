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
#ifndef BDSMAGNETOUTERFACTORYNONE_H
#define BDSMAGNETOUTERFACTORYNONE_H

#include "BDSMagnetOuter.hh"
#include "BDSMagnetOuterFactoryBase.hh"

#include "globals.hh"           // geant4 globals / types

class BDSBeamPipe;
class G4Material;

/**
 * @brief Factory that produces null outer geometry.
 * 
 * @author Laurie Nevay
 */

class BDSMagnetOuterFactoryNone: public BDSMagnetOuterFactoryBase
{
public:
  BDSMagnetOuterFactoryNone();
  virtual ~BDSMagnetOuterFactoryNone(){;}
  
  /// sector bend outer volume
  virtual BDSMagnetOuter* CreateSectorBend(G4String           name,             // name
					   G4double           length,           // full length
					   const BDSBeamPipe* beamPipe,         // beampipe
					   G4double           containerLength,  // full length to make AccComp container
					   const BDSMagnetOuterInfo* recipe);   // recipe for geometry

  /// rectangular bend outer volume
  virtual BDSMagnetOuter* CreateRectangularBend(G4String           name,             // name
						G4double           length,           // full length
						const BDSBeamPipe* beamPipe,         // beampipe
						G4double           containerLength,  // full length to make AccComp container
						const BDSMagnetOuterInfo* recipe);   // recipe for geometry
  
  /// quadrupole outer volume
  virtual BDSMagnetOuter* CreateQuadrupole(G4String     name,                  // name
					   G4double     length,                // length
					   BDSBeamPipe* beamPipe,              // beampipe
					   G4double     containerLength,       // full length to make AccComp container
					   const BDSMagnetOuterInfo* recipe);  // recipe for geometry
  

  /// sextupole outer volume
  virtual BDSMagnetOuter* CreateSextupole(G4String     name,                  // name
					  G4double     length,                // length
					  BDSBeamPipe* beamPipe,              // beampipe
					  G4double     containerLength,       // full length to make AccComp container
					  const BDSMagnetOuterInfo* recipe);  // recipe for geometry

  /// octupole outer volume
  virtual BDSMagnetOuter* CreateOctupole(G4String     name,                  // name
					 G4double     length,                // length
					 BDSBeamPipe* beamPipe,              // beampipe
					 G4double     containerLength,       // full length to make AccComp container
					 const BDSMagnetOuterInfo* recipe);  // recipe for geometry
  
  /// decapole outer volume
  virtual BDSMagnetOuter* CreateDecapole(G4String     name,                  // name
					 G4double     length,                // length
					 BDSBeamPipe* beamPipe,              // beampipe
					 G4double     containerLength,       // full length to make AccComp container
					 const BDSMagnetOuterInfo* recipe);  // recipe for geometry
  
  /// solenoid  outer volume
  virtual BDSMagnetOuter* CreateSolenoid(G4String     name,                  // name
					 G4double     length,                // length
					 BDSBeamPipe* beamPipe,              // beampipe
					 G4double     containerLength,       // full length to make AccComp container
					 const BDSMagnetOuterInfo* recipe);  // recipe for geometry
  
  /// general multipole outer volume - could be any 2N order multipole
  virtual BDSMagnetOuter* CreateMultipole(G4String     name,                  // name
					  G4double     length,                // length
					  BDSBeamPipe* beamPipe,              // beampipe
					  G4double     containerLength,       // full length to make AccComp container
					  const BDSMagnetOuterInfo* recipe);  // recipe for geometry
  
  /// RF cavity outer volume
  virtual BDSMagnetOuter* CreateRfCavity(G4String     name,                  // name
					 G4double     length,                // length
					 BDSBeamPipe* beamPipe,              // beampipe
					 G4double     containerLength,       // full length to make AccComp container
					 const BDSMagnetOuterInfo* recipe);  // recipe for geometry

  /// muon spoiler outer volume
  virtual BDSMagnetOuter* CreateMuonSpoiler(G4String     name,                  // name
					    G4double     length,                // length
					    BDSBeamPipe* beamPipe,              // beampipe
					    G4double     containerLength,       // full length to make AccComp container
					    const BDSMagnetOuterInfo* recipe);  // recipe for geometry
  
  /// horizontal and vertical kicker outer volume
  virtual BDSMagnetOuter* CreateKicker(G4String                  name,            // name
				       G4double                  length,          // length
				       const BDSBeamPipe*        beamPipe,        // beampipe
				       G4double                  containerLength, // full length to make AccComp container
				       const BDSMagnetOuterInfo* recipe,          // geometry recipe
				       G4bool                    vertical);       // is it a vertical kicker?
};

#endif
