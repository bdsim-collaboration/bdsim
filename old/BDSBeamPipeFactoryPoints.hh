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
#ifndef BDSBEAMPIPEFACTORYPOINTS_H
#define BDSBEAMPIPEFACTORYPOINTS_H

#include "BDSBeamPipeFactoryBase.hh"
#include "BDSBeamPipe.hh"

#include "globals.hh"
#include "G4TwoVector.hh"

#include <vector>

class G4Material;

/**
 * @brief Factory for beam pipes defined by a series of x,y points that
 * are extruded.
 * 
 * This is a pure virtual class that provides common functionality
 * for aperture models derived from a series of points transversely.
 * The derived class need therefore only generate the vector of points
 * that will make the edges of the vacuum and inner & outer beam pipe 
 * surfaces. The solids are of type G4ExtrudedSolid, or in the case of angled 
 * faces, a G4ExtrudedSolid intersected with a G4CutTubs (cylinder with 
 * angled faces).
 * 
 * @author Laurie Nevay
 */

class BDSBeamPipeFactoryPoints: public BDSBeamPipeFactoryBase
{
public:
  BDSBeamPipeFactoryPoints();
  virtual ~BDSBeamPipeFactoryPoints();

  /// Required overloaded method from BDSBeamPipeFactoryBase to build straight piece of beam pipe
  virtual BDSBeamPipe* CreateBeamPipe(const G4String& nameIn,
				      G4double    lengthIn,
				      G4double    aper1In,
				      G4double    aper2In,
				      G4double    aper3In,
				      G4double    aper4In,
				      G4Material* vacuumMaterialIn,
				      G4double    beamPipeThicknessIn,
				      G4Material* beamPipeMaterialIn,
				      const G4String& pointsFileIn = "",
				      const G4String& pointsUnitIn = "");

  /// Required overloaded method from BDSBeamPipeFactoryBase to build angled piece of beam pipe
  virtual BDSBeamPipe* CreateBeamPipe(const G4String&      nameIn,
				      G4double             lengthIn,
				      const G4ThreeVector& inputFaceNormalIn,
				      const G4ThreeVector& outputFaceNormalIn,
				      G4double      aper1In,
				      G4double      aper2In,
				      G4double      aper3In,
				      G4double      aper4In,
				      G4Material*   vacuumMaterialIn,
				      G4double      beamPipeThicknessIn,
				      G4Material*   beamPipeMaterialIn,
				      const G4String& pointsFileIn = "",
				      const G4String& pointsUnitIn = "");
  
protected:
  /// Function to generate transverse points.  Should set member variables vacuumEdge,
  /// beamPipeInnerEdge, beamPipeOuterEdge, containerEdge and containerSubtractionEdge.
  /// This *must* also set extentX and extentY member variables for +ve symmetric extents.
  virtual void GeneratePoints(G4double aper1,
			      G4double aper2,
			      G4double aper3,
			      G4double aper4,
			      G4double beamPipeThickness,
			      G4int    pointsPerTwoPi = 40) = 0;

  /// Function derived class must provide to calculated the intersection radius, which
  /// is the radius of the intersection
  /// cylinder to create the aperture faces. This should be big enough to contain the
  /// cross-section of the beam pipe but not overly large, so as to avoid intersecting plane
  /// errors (and therefore exits) from large angles on short elements.
  virtual G4double CalculateIntersectionRadius(G4double aper1,
					       G4double aper2,
					       G4double aper3,
					       G4double aper4,
					       G4double beamPipeThickness) = 0;

  /// Clear member vectors and run base class clean up to clear pointers between runs
  virtual void CleanUp();

  void AppendPoint(std::vector<G4TwoVector>& vec,
		   G4double x,
		   G4double y);

  /// Generate 2-vector points (and append them) about a circle. Uses ellipse
  /// code with equal radii.
  void AppendAngle(std::vector<G4TwoVector>& vec,
		   G4double startAngle,
		   G4double finishAngle,
		   G4double radius,
		   G4int    nPoints = 10,
		   G4double xOffset = 0,
		   G4double yOffset = 0);

  /// Generate 2-vector points (and append them) about an ellipse.
  void AppendAngleEllipse(std::vector<G4TwoVector>& vec,
			  G4double startAngle,
			  G4double finishAngle,
			  G4double radiusA,   // radius in horizontal
			  G4double radiusB,   // radius in vertical
			  G4int    nPoints = 10,
			  G4double xOffset = 0,
			  G4double yOffset = 0);

  /// Vector of x,y coordinates for vacuum extruded solid edge.
  std::vector<G4TwoVector> vacuumEdge;

  /// Vector of x,y coordinates for beam pipe inner edge.
  std::vector<G4TwoVector> beamPipeInnerEdge;

  /// Vector of x,y coordinates for beam pipe outer edge.
  std::vector<G4TwoVector> beamPipeOuterEdge;

  /// Vector of x,y coordinates for the container volume.
  std::vector<G4TwoVector> containerEdge;

  /// Vector of x,y coordinates for the container subtraction volume.
  std::vector<G4TwoVector> containerSubtractionEdge;
  
  ///@{ Extents prepared by GeneratePoints function as only it knows the extents
  G4double extentX;
  G4double extentY;
  ///@}
  
  /// @{ Temporary copy of input variables.
  G4String pointsFile;
  G4String pointsUnit;
  /// @}

private:
  /// Radius of intersection cylinder for angled faced beam pipe pieces. Should
  /// be determined and set in GeneratePoints() by derived class. Should be big
  /// enough to enclose the beam pipe section transversely but not too large that
  /// overlapping z planes errors might occur with short pieces of beam pipe with
  /// large angled faces.
  G4double intersectionRadius;
  
  /// Solid for inner surface of beam pipe.
  G4VSolid* beamPipeInnerSolid;
  
  /// Solid for outer surface of beam pipe.
  G4VSolid* beamPipeOuterSolid;

  /// Actual clean up here for this class only.
  void CleanUpPoints();

  /// Create the vacuum, inner & outer, container and container subtraction solids.
  /// Optional flag buildLongForIntersection builds the solids longer by 1.5x length
  /// in preparation for them to be intersected for angled faces, saving code duplication.
  void CreateSolids(const G4String& name,
		    G4double length,
		    G4bool   buildLongForIntersection = false);

  /// Create angled solids.  Uses CreateSolids() method.
  void CreateSolidsAngled(const G4String&      name,
			  G4double             length,
			  const G4ThreeVector& inputFace,
			  const G4ThreeVector& outputFace);

  /// Overloads BDSBeamPipeFactoryBase method to do common construction tasks and
  /// constructs BDSBeamPipe instance to return.
  virtual BDSBeamPipe* CommonFinalConstruction(const G4String& nameIn,
					       G4Material* vacuumMaterialIn,
					       G4Material* beamPipeMaterialIn,
					       G4double    lengthIn);
};
  
#endif
