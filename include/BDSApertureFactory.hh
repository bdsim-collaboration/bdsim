/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2022.

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
#ifndef BDSAPERTUREFACTORY_H
#define BDSAPERTUREFACTORY_H
#include "BDSApertureType.hh"
#include "BDSBeamPipeType.hh"

#include "G4String.hh"
#include "G4ThreeVector.hh"
#include "G4TwoVector.hh"
#include "G4Types.hh"

#include <list>
#include <map>
#include <utility>
#include <set>
#include <utility>
#include <vector>

class BDSAperture;
class G4VSolid;

namespace GMAD
{
  class Aperture;
  struct Element;
  class SamplerPlacement;
}

/**
 * @brief Main interface for building basic shapes described by apertures.
 *
 * One part of the factory builds aperture definitions. The other builds either full
 * or hollow solids.
 *
 * For the aperture definitions, two are given: one for the incoming edge and one for
 * the outgoing edge. The second may be a nullptr, in which case the first is constant.
 * 
 * @author Laurie Nevay
 */

class BDSApertureFactory
{
public:
  BDSApertureFactory();
  explicit BDSApertureFactory(G4double nPointsPerTwoPiIn);
  ~BDSApertureFactory();

  /// Create an aperture from a parser element definition but fall back to a default model.
  std::pair<BDSAperture*, BDSAperture*> CreateAperture(BDSBeamPipeType bpt,
                                                       const GMAD::Element& el,
                                                       G4bool useElementVariables) const;

  /// Create it from a complete parser object.
  BDSAperture* CreateAperture(const GMAD::Aperture& ap) const;

  /// From a sampler definition.
  BDSAperture* CreateAperture(const GMAD::SamplerPlacement& sp) const;

  /// Full explicit constructor for a single instance that is used by many of the above.
  BDSAperture* CreateApertureSingle(BDSApertureType at,
                                    G4double        a1,
                                    G4double        a2,
                                    G4double        a3,
                                    G4double        a4,
                                    G4double        tilt = 0,
                                    G4double        offsetX = 0,
                                    G4double        offsetY = 0,
                                    unsigned int    nPoints = 0,
                                    const G4String& pointsFileAndString = "") const;

  /// Full explicit constructor for double instance that is used by many of the above.
  std::pair<BDSAperture*, BDSAperture*> CreateAperture(BDSApertureType at,
                                                       G4double        a1,
                                                       G4double        a2,
                                                       G4double        a3,
                                                       G4double        a4,
                                                       G4double        tilt = 0,
                                                       G4double        offsetX = 0,
                                                       G4double        offsetY = 0,
                                                       unsigned int    nPoints = 0,
                                                       const G4String& pointsFileAndString = "",
                                                       const G4String& apertureTypeOut = "",
                                                       const std::vector<G4double>* apertureOut = nullptr) const;

  /// Return type to give both the final solid but any transient solids on the way.
  struct Product
  {
    G4VSolid* product;
    std::set<G4VSolid*> otherSolids;

    void Extend(const Product& other)
    {
      otherSolids.insert(other.product);
      otherSolids.insert(other.otherSolids.begin(), other.otherSolids.end());
    }
  };

  /// Create any aperture shape with flat ends. The apertureOut is optionally
  /// for defining the possibly different aperture shape at the output end.
  Product CreateSolid(const G4String&    name,
                      G4double           length,
                      const BDSAperture* apertureIn,
                      const BDSAperture* apertureOut = nullptr,
                      const G4ThreeVector* normalIn  = nullptr,
                      const G4ThreeVector* normalOut = nullptr,
                      G4double           lengthExtraForBoolean = 0);
  
  /// Create a solid according to an aperture entrance ('in') and exit ('out') with
  /// a constant thickness. The aperture defines the interior edge of the solid. In
  /// effect, a beam pipe on its own; a tube.
  Product CreateSolidWithInner(const G4String&      name,
                               G4double             length,
                               const BDSAperture*   apertureInInside,
                               const BDSAperture*   apertureOutInside,
                               G4double             thickness,
                               const G4ThreeVector* normalIn,
                               const G4ThreeVector* normalOut,
                               G4double             lengthExtraForBoolean = 0);
  
  /// Create a solid like a pipe or tube, but where there are 4 shapes to specify. The inner
  /// entrance and exit shapes, and the outer entrance and exit shapes. These outer ones must
  /// be large enough to encompass the inner ones.
  Product CreateSolidWithInnerVariableThickness(const G4String& name,
                                                G4double length,
                                                const BDSAperture* apertureInOutside,
                                                const BDSAperture* apertureInInside,
                                                const BDSAperture* apertureOutOutside = nullptr,
                                                const BDSAperture* apertureOutInside  = nullptr,
                                                const G4ThreeVector* normalIn  = nullptr,
                                                const G4ThreeVector* normalOut = nullptr,
                                                G4double           lengthExtraForBoolean = 0);

  static std::vector<G4double> ApertureListToVector(const std::list<double>& l);

private:
  G4double CalculateExtraLength(G4double lengthExtraForBoolean);

  /// Default level of detail for each shape.
  G4int DefaultNPoints(BDSApertureType apt) const;

  /// Calculate the length on one side of a solid that is required for the
  /// Boolean operation for a given angle from the perpendicular.
  G4double RequiredLengthForBoolean(G4double angle, G4double radius) const;

  /// Check N points > 0 and throw an exception if not.
  void CheckNPoints(int nPoints,
                    const G4String& typeName,
                    const G4String& objectName) const;


  /// Return whether the input and output normal vectors are different from 0,0,-1
  /// and 0,0,1, i.e. one or both faces are angled.
  G4bool AngledFaces(const G4ThreeVector& v1, const G4ThreeVector& v2);
  
  void ParsePointsFileAndUnits(const G4String& beamPipeType,
                               G4String& pointsFileName,
                               G4String& pointsUnit) const;
  
  /// @{ Flat faced construction function.
  Product CreateCircle()      const;
  Product CreateRectangle()   const;
  Product CreateEllipse()     const;
  Product CreateRectCircle()  const;
  Product CreateRectEllipse() const;
  Product CreateRegularPolygon() const;
  /// @}

  /// Utility to make angled solid for intersection when cutting angled faces.
  /// Makes use of members assumed to be set for current factory product.
  G4VSolid* CutSolid(const G4String& name,
                     G4double halfLength,
                     G4double radiusToEncompass) const;

  /// Create a solid as an extruded solid
  Product CreateExtrudedSolid() const;

  /// Create a shape with different ends.
  Product CreateDifferentEnds() const;
  
  Product CreateTubeByPoints(const G4String& nameSuffix)  const;

  /// @{ Specialisation for particular solids.
  Product CreateDifferentEndsCircleToCircle() const;
  /// @}
  /// @{ Specialisation for particular solids for hollow solid.
  Product HollowCircleToCircle(G4double thickness) const;
  Product HollowVariedCircleToCircle(G4double thickness) const;
  Product HollowEllipseToEllipse(G4double thickness) const;
  Product HollowRectangleToRectangle(G4double thickness) const;
  Product HollowRectCircleToRectCircle(G4double thickness) const;
  Product HollowRectEllipseToRectEllipse(G4double thickness) const;
  /// @}

  G4double nPointsPerTwoPi;

  /// Ratio of radii used for an intersection solid - greater than 1 and defined arbitrarily.
  const G4double intersectionRadiusRatio;

  /// @{ Cache of input variable to simplify function arguments.
  G4String           productName;
  G4double           productLength;
  const BDSAperture* productApertureIn;
  const BDSAperture* productApertureOut;
  G4double           productLengthExtra;
  G4ThreeVector      productNormalIn;
  G4ThreeVector      productNormalOut;
  G4bool             angledFaces;
  /// @}

  /// Create a pair where the two values are in order. We do this so we can access
  /// a map of unique pairs.
  std::pair<BDSApertureType, BDSApertureType> MakePair(BDSApertureType a1,
                                                       BDSApertureType a2) const;

  /// Typedef for function pointers to simplify syntax.
  typedef Product(BDSApertureFactory::*Constructor)(void) const;

  /// Map of unique aperture type pairs to member function pointers.
  std::map<std::pair<BDSApertureType, BDSApertureType>, Constructor> specialisations;

  /// Typedef for function pointers to simplify syntax.
  typedef Product(BDSApertureFactory::*HollowConstructor)(G4double) const;

  /// Map of unique aperture type pairs to member function pointers for hollow constructors.
  std::map<std::pair<BDSApertureType, BDSApertureType>, HollowConstructor> hollowSpecialisations;

  /// Map of unique aperture type pairs to member function pointers for hollow constructors.
  std::map<std::pair<BDSApertureType, BDSApertureType>, HollowConstructor> hollowVariedSpecialisations;
};



#endif
