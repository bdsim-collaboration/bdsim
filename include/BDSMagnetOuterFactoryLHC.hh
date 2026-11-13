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
#ifndef BDSMAGNETOUTERFACTORYLHC_H
#define BDSMAGNETOUTERFACTORYLHC_H

#include "BDSMagnetOuterFactoryBase.hh"

#include "globals.hh"           // geant4 globals / types

class BDSBeamPipe;
class G4Material;

/**
 * @brief The base factory for the LHC magnet geometry factory.
 * 
 * This class is not a singleton unlike the other factories
 * as it will be constructed twice by each derived class
 * 
 * @author Laurie Nevay
 */

class BDSMagnetOuterFactoryLHC: public BDSMagnetOuterFactoryBase
{
private:
  /// Enum up front for yoke colouring.
  enum class YokeColour {dipole, kicker};
public:
  explicit BDSMagnetOuterFactoryLHC(G4bool isLeftOffset);
  virtual ~BDSMagnetOuterFactoryLHC();
  
  /// sector bend outer volume
  virtual BDSMagnetOuter* CreateSectorBend(G4String           name,             // name
					   G4double           length,           // full length [mm]
					   const BDSBeamPipe* beamPipe,         // beampipe
					   G4double           containerLength,  // full length to make AccComp container
					   const BDSMagnetOuterInfo* recipe);   // recipe for geometry
        
  /// Used in many places - make it a constant in the code and put here as most relevant.
  static const G4double beamSeparation;
private:
  /// Our own function in this class that has the yoke colour so
  /// we can reuse the code for kickers
  BDSMagnetOuter* CreateLHCDipole(const G4String&    name,             // name
				  G4double           length,           // full length [mm]
				  const BDSBeamPipe* beamPipe,         // beampipe
				  G4double           containerLength,  // full length to make AccComp container
				  const BDSMagnetOuterInfo* recipe,    // recipe for geometry
				  YokeColour         colourIn);
public:
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
					 G4double     containerLength,       // full length to make AccComp container
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
				       G4double                  length,          // length
				       const BDSBeamPipe*        beamPipe,        // beampipe
				       G4double                  containerLength, // full length to make AccComp container
				       const BDSMagnetOuterInfo* recipe,          // geometry recipe
				       G4bool                    vertical);       // is it a vertical kicker?
  
protected:
  G4bool isLeftOffset;
  G4bool IsLeftOffset(){return isLeftOffset;}
  
private:
  /// test inputs for no null pointers or overlapping volumes due to poorly defined sizes
  void TestInputParameters(const BDSBeamPipe* beamPipe,
			   G4double&    horizontalWidthIn);

  BDSMagnetOuterFactoryBase* cylindrical; ///< Default factory to fall back to.
};

#endif
