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
#ifndef BDSMAGNETOUTERFACTORYPOLESBASE_H
#define BDSMAGNETOUTERFACTORYPOLESBASE_H

#include "BDSMagnetOuter.hh"
#include "BDSMagnetOuterFactoryBase.hh"

#include "globals.hh"           // geant4 globals / types
#include "G4TwoVector.hh"

#include <vector>

class BDSBeamPipe;
class BDSMagnetOuterInfo;
class G4Material;
class G4VSolid;

/**
 * @brief Factory class for outer volume of magnets. Produces magnets
 * with 2N-poles around the beampipe with a yoke of various shapes.
 * 
 * Most magnets are 2N poles, but sector and r-bends as well as 
 * muon spoilers, and h/v kickers are unique.
 *
 * NOTE this is a base class in that there are derived classes
 * with different outer shapes - all have poles, but the return 
 * yoke can for example, circular, square or faceted (with 4N facets)
 *
 * @author Laurie Nevay
 */

class BDSMagnetOuterFactoryPolesBase: public BDSMagnetOuterFactoryBase
{
public:
  BDSMagnetOuterFactoryPolesBase();
  explicit BDSMagnetOuterFactoryPolesBase(G4double poleStopFactorIn);
  virtual ~BDSMagnetOuterFactoryPolesBase();
  
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
					   G4double     length,                // length [mm]
					   BDSBeamPipe* beamPipe,              // beampipe
					   G4double     containerLength,       // full length to make AccComp container
					   const BDSMagnetOuterInfo* recipe);  // geometry recipe

  /// sextupole outer volume
  virtual BDSMagnetOuter* CreateSextupole(G4String     name,                  // name
					  G4double     length,                // length [mm]
					  BDSBeamPipe* beamPipe,              // beampipe
					  G4double     containerLength,       // full length to make AccComp container
					  const BDSMagnetOuterInfo* recipe);  // geometry recipe

  /// octupole outer volume
  virtual BDSMagnetOuter* CreateOctupole(G4String     name,                  // name
					 G4double     length,                // length [mm]
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
  // geometry parameters
  /// The fraction of the distance from the beam pipe to the horizontalWidth/2 that each pole
  /// will take - always < 1
  const G4double poleFraction;

  /// Fraction of 2pi/Npoles that the pole will occupy - always < 1
  const G4double poleAngularFraction;

  /// Fraction of length from pole tip to horizontalWidth that pole tip ellisoid will
  /// take up
  const G4double poleTipFraction;

  /// Fraction of length from pole tip to outerDiamter that the expanding section of
  /// the pole will take up. There's the tip, this bit, then a straight bit.
  const G4double poleAnnulusFraction;

  /// Bends are often wider in the bending plane. As we only have one parameter to pass
  /// which is outerDimaeter, make the non bending dimension a fixed (< 1) fraction of
  /// the outerDimaeter.
  const G4double bendHeightFraction;

  /// Factor by which the pole length is multiplied for the raw pole length before it's
  /// intersected with the inside of the yoke. Where the pole would normally stop at
  /// yokeStartRadius - lengthSaftey, it runs to yokeStartRadius x poleStopFactor.
  const G4double poleStopFactor;
  
  G4double yokeStartRadius;        ///< Start radius of yoke geometry from magnet cetnre.
  G4double yokeFinishRadius;       ///< Finish radius of yoke geometry from magnet centre - less than horizontalWidth.
  G4double magnetContainerRadius;  ///< Radius of the container solid for the outer geometry.
  G4bool   buildPole;              ///< Whether or not to build poles (and therefore coils).
  G4double poleStartRadius;        ///< Start radius of the pole from magnet centre.
  G4double poleFinishRadius;       ///< Finish radius of the pole from magnet centre.
  G4double poleSquareWidth;        ///< Full width of pole in constant width section.
  G4double poleSquareStartRadius;  ///< Radius from magnet centre that constant width section starts.
  G4double segmentAngle;           ///< 2PI / # of poles - angle per segment allocated for each pole.
  G4double poleAngle;              ///< The angle allowed for the pole to occupy - less than segmentAngle.
  G4ThreeVector poleTranslation;   ///< Offste of pole for placement from magnet centre.
  G4double coilHeight;             ///< Height along y for coil for coil beside 1 upgright pole aligned with y axis.
  G4double coilCentreRadius;       ///< Radius from magnet centre that the centre of the coils exist at.
  G4double endPieceLength;         ///< Length of the coil end piece along what will be curvilinear S.
  G4double endPieceInnerR;         ///< Inner radius for end piece container.
  G4double endPieceOuterR;         ///< Outer radius for end piece container.

  G4VSolid* poleIntersectionSolid; ///< Solid used to chop off pole
  G4VSolid* coilLeftSolid;         ///< Left coil solid for one pole built upright along y axis.
  G4VSolid* coilRightSolid;        ///< Right coil solid.
  G4VSolid* endPieceContainerSolid;///< End piece container solid.
  G4LogicalVolume* coilLeftLV;     ///< Logical volume for left coil.
  G4LogicalVolume* coilRightLV;    ///< Logical volume for right coil.
  G4LogicalVolume* endPieceCoilLV; ///< Logical volume for end piece single coil piece.
  G4LogicalVolume* endPieceContainerLV; ///< Logical volume for end piece container.
  BDSSimpleComponent* endPiece;    ///< Fully constructed end piece.
  
  std::vector<G4TwoVector> leftPoints;    ///< Vector of 2D points for left coil.
  std::vector<G4TwoVector> rightPoints;   ///< Vector of 2D points for right coil.
  std::vector<G4TwoVector> endPiecePoints;///< Vector of 2D points for end piece looking from above down z.

  /// Empty containers for next use - this class is never deleted so can't rely on scope
  virtual void CleanUp();

  /// Non-virtual clean up to be used in constructor.
  void CleanUpPolesBase();

  /// Common construction tasks to all methods - assemble yoke and poles in container
  virtual BDSMagnetOuter* CommonConstructor(const G4String& name,
					    G4double        length,
					    BDSBeamPipe*    beamPipe,
					    G4int           order,
					    G4double        magnetContainerLength,
					    const BDSMagnetOuterInfo* recipe);
  
  /// Calculate the length of the pole and yoke radii based on the design. This is only
  /// responsible for calculating the gross proportions of the yoke and pole, not all the
  /// geometrical parameters that may be required for the final geometry.
  virtual void CalculatePoleAndYoke(G4double     horizontalWidth,
				    BDSBeamPipe* beamPipe,
				    G4int        order);
  
  /// Create pole for magnet of order N where npoles = Nx2. This contains some calcultion
  /// of geometrical parameters pertinent to the exact geometry being required.
  /// NOTE the poles are not joined (boolean union) to the outer yoke - there is a
  /// gap of length safety. This won't affect physics results and speeds up tracking
  /// as the solid is not a boolean of order Npoles + 1.
  virtual void CreatePoleSolid(const G4String& name,                 // name
			       G4double        length,               // length [mm]
			       G4int           order);               // Nx2 poles

  /// Create the coil solids corresponding to the pole solid.
  virtual void CreateCoilSolids(const G4String& name,
				G4double        length);
  
  /// Create all the points that make up the extruded solid of the pole.
  virtual void CreateCoilPoints();
  
  /// Create yoke that connects poles and container to put them in. Also create the
  /// poleIntersectionSolid that will be used to chop the extended pole in
  /// IntersectPoleWithYoke().
  virtual void CreateYokeAndContainerSolid(const G4String& name,
					   G4double        length,
					   G4int           order,
					   G4double        magnetContainerLength,
					   G4double        magnetContainerRadiusIn); // so as not to clash with member name

  /// Chop off the top of the pole to match the appropriate yoke geometry.
  virtual void IntersectPoleWithYoke(const G4String& name,
				     G4double        length,
				     G4int           order);

  virtual void CreateLogicalVolumes(const G4String& name,
				    G4Colour*       colour,
				    G4Material*     outerMaterial);

  /// Discretise the coil logical volumes as even though derived factories from this one
  /// may complete override CreateLogicalVolumes as the poles can be individually unique,
  /// the coils will be the same and this allows reuse of code and lack of duplication.
  virtual void CreateLogicalVolumesCoil(const G4String& name);

  /// Create the solids, logical volumes for the end piece - everything
  /// but the placement. Also, create the geometry component now.
  virtual void CreateEndPiece(const G4String& name);
  
  /// Place the poles and yoke in the container volume.
  virtual void PlaceComponents(const G4String& name,
			       G4int           order);

  /// If we're building coils, place two coils for each pole.
  virtual void PlaceComponentsCoils(const G4String& name,
				    G4int           order);

  /// Ensure the coil fractions lie with [0.05, 0.98] and if they're negative set them to
  /// a provided default. -ve is assumed to require the default parameter and allows different
  /// usages of the function (in C and H dipoles) to control the defaults.
  void TestCoilFractions(G4double& coilWidthFraction,
			 G4double& coilHeightFraction);
  
  /// Common task to both dipole construction routines. Clean up, test  inputs and check
  /// if faces will intersect and warn user. Note reference to material pointer so it can
  /// be fixed if needs be to the default.
  void DipoleCommonPreConstruction(const G4String& name,
				   G4double        angleIn,
				   G4double        angleOut,
				   G4double        length,
				   G4double&       horizontalWidth,
				   G4Material*&    material,
				   G4double&       vhRatio);

  /// Common calculations to both dipole construction routines in one place. Pass by reference
  /// to modify variables declared in each function.
  void DipoleCalculations(G4bool    hStyle,
			  G4bool    buildVertically,
			  const BDSBeamPipe* beamPipe,
			  G4double  length,
			  G4double  horizontalWidth,
			  G4double  angleIn,
			  G4double  angleOut,
			  G4double  yokeThicknessFraction,
			  G4double  vhRatio,
			  G4double  coilWidthFraction,
			  G4double  coilHeightFraction,
			  G4double& cShapeOuterEdge,
			  G4double& poleHalfGap,
			  G4double& poleWidth,
			  G4double& poleHeight,
			  G4double& yokeWidth,
			  G4double& yokeHalfHeight,
			  G4double& yokeThickness,
			  G4double& yokeOverHang,
			  G4double& coilWidth,
			  G4double& coilHeightIn, // to avoid shadowing member variable
			  G4double& coilToYokeGap,
			  G4double& coilToPoleGap,
			  G4double& sLength,
			  G4double& containerSLength,
			  G4double& intersectionRadius);

  /// Calculate the placement offsets for each of the four coil placements. Common to both dipole
  /// construction routines.
  std::vector<G4ThreeVector> CalculateCoilDisplacements(G4double  poleHalfWidthIn,
							G4double  poleHalfGapIn,
							G4double  coilWidthIn,
							G4double  coilHeightIn,
							G4double  cDY,
							G4double& coilDY);
    
  /// Routine to construct a C shaped dipole magnet with the yoke either to the left or right
  /// and can optionally be built vertically.
  BDSMagnetOuter* CreateDipoleC(const G4String&           name,
				G4double                  length,
				const BDSBeamPipe*        beamPipe,
				G4double                  containerLength,
				const BDSMagnetOuterInfo* recipe,
				G4bool                    buildVertically);

  /// Routine to construct an H shaped dipole magnet and can optionally be built vertically.
  BDSMagnetOuter* CreateDipoleH(const G4String&           name,
				G4double                  length,
				const BDSBeamPipe*        beamPipe,
				G4double                  containerLength,
				const BDSMagnetOuterInfo* recipe,
				G4bool                    buildVertically);

  BDSMagnetOuter* DipoleCommonConstruction(const G4String& name,
					   G4double    horizontalWidth,
					   G4bool      buildEndPiece,
					   G4double    coilWidth,
					   G4double    length,
					   G4double    containerLength,
					   G4double    sLength,
					   G4double    angleIn,
					   G4double    angleOut,
					   G4Colour*   colour,
					   G4Material* material,
					   std::vector<G4ThreeVector>& coilDisps,
					   G4bool      buildVertically,
					   BDSExtent&  ext,
					   G4double    poleHalfWidth,
					   G4double    poleHalfGap,
					   G4double    cDY,
					   G4double    coilDY,
					   G4double    intersectionRadius);

  BDSMagnetOuterFactoryBase* cylindrical; ///< Default factory to fall back to.
};

#endif
