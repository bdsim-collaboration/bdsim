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
#include "BDSAperture.hh"
#include "BDSApertureCircle.hh"
#include "BDSApertureClicPCL.hh"
#include "BDSApertureEllipse.hh"
#include "BDSApertureFactory.hh"
#include "BDSApertureOctagon.hh"
#include "BDSAperturePoints.hh"
#include "BDSApertureRaceTrack.hh"
#include "BDSApertureRectangle.hh"
#include "BDSApertureRectCircle.hh"
#include "BDSApertureRectEllipse.hh"
#include "BDSApertureRegularPolygon.hh"
#include "BDSApertureRhombus.hh"
#include "BDSApertureType.hh"
#include "BDSBeamPipeToApertureType.hh"
#include "BDSBeamPipeType.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSPolygon.hh"
#include "BDSTube.hh"
#include "BDSUtilities.hh"

#include "parser/aperture.h"
#include "parser/element.h"
#include "parser/samplerplacement.h"

#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4CutTubs.hh"
#include "G4EllipticalTube.hh"
#include "G4ExtrudedSolid.hh"
#include "G4IntersectionSolid.hh"
#include "G4Polyhedra.hh"
#include "G4String.hh"
#include "G4SubtractionSolid.hh"
#include "G4ThreeVector.hh"
#include "G4Tubs.hh"
#include "G4TwoVector.hh"
#include "G4TwoVector.hh"
#include "G4Types.hh"
#include "G4VSolid.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <map>
#include <utility>


BDSApertureFactory::BDSApertureFactory():
  BDSApertureFactory(24)
{;}

BDSApertureFactory::BDSApertureFactory(G4double nPointsPerTwoPiIn):
  nPointsPerTwoPi(nPointsPerTwoPiIn),
  intersectionRadiusRatio(1.3),
  productName(""),
  productLength(0),
  productApertureIn(nullptr),
  productApertureOut(nullptr),
  productLengthExtra(0),
  angledFaces(false)
{
  specialisations = {
    {MakePair(BDSApertureType::circle, BDSApertureType::circle), &BDSApertureFactory::CreateDifferentEndsCircleToCircle}
  };
  
  hollowSpecialisations = {
    {MakePair(BDSApertureType::circle, BDSApertureType::circle), &BDSApertureFactory::HollowCircleToCircle},
    {MakePair(BDSApertureType::ellipse, BDSApertureType::ellipse), &BDSApertureFactory::HollowEllipseToEllipse},
    {MakePair(BDSApertureType::rectangle, BDSApertureType::rectangle), &BDSApertureFactory::HollowRectangleToRectangle},
    {MakePair(BDSApertureType::rectcircle, BDSApertureType::rectcircle), &BDSApertureFactory::HollowRectCircleToRectCircle},
    {MakePair(BDSApertureType::rectellipse, BDSApertureType::rectellipse), &BDSApertureFactory::HollowRectEllipseToRectEllipse},
  };

  hollowVariedSpecialisations = {
    {MakePair(BDSApertureType::circle, BDSApertureType::circle), &BDSApertureFactory::HollowVariedCircleToCircle},
  };
}

BDSApertureFactory::~BDSApertureFactory()
{;}

G4int BDSApertureFactory::DefaultNPoints(BDSApertureType apt) const
{
  std::map<BDSApertureType, G4int> npoints = {
    {BDSApertureType::circle, nPointsPerTwoPi},
    {BDSApertureType::ellipse, nPointsPerTwoPi},
    {BDSApertureType::rectangle, 4},
    {BDSApertureType::rectcircle, nPointsPerTwoPi},
    {BDSApertureType::rectellipse, nPointsPerTwoPi},
    {BDSApertureType::racetrack, nPointsPerTwoPi},
    {BDSApertureType::octagon, 8},
    {BDSApertureType::clicpcl, nPointsPerTwoPi},
    {BDSApertureType::rhombus, 8},
    {BDSApertureType::points, 0}
  };
  return npoints.at(apt);
}

std::vector<G4double> BDSApertureFactory::ApertureListToVector(const std::list<double>& l,
                                                               BDSBeamPipeType bpt)
{
  G4double m = CLHEP::m;
  std::vector<G4double> values = {0, 0, 0, 0, 0, 0, 0, 0};
  std::vector<G4double> apertures = {l.begin(), l.end()};
  std::vector<G4double> units = {Units(bpt, 1), Units(bpt, 2), Units(bpt, 3), Units(bpt, 4), CLHEP::rad, m, m, 1};
  for (G4int i = 0; i < (G4int)apertures.size(); i++)
    {values[i] = apertures[i] * units[i];}
  return values;
}

G4double BDSApertureFactory::Units(BDSApertureType apt,
                                   G4int aperN)
{
  auto bpt = BDS::BeamPipeTypeFromApertureType(apt);
  return Units(bpt, aperN);
}

G4double BDSApertureFactory::Units(BDSBeamPipeType bpt,
                                   G4int aperN)
{
  G4double result = 1;
  switch (bpt.underlying())
    {
      case BDSBeamPipeType::regularpolygon:
      {
        std::array<G4double, 4> units = {1.0, CLHEP::m, CLHEP::rad, 1.0};
        result = units[aperN-1];
        break;
      }
      default:
        {result = CLHEP::m; break;}
    }
  return result;
}

std::pair<BDSAperture*, BDSAperture*> BDSApertureFactory::CreateAperture(BDSBeamPipeType bpt,
                                                                         const GMAD::Element& el,
                                                                         G4bool useElementVariables) const
{
  BDSApertureType apt = BDS::ApertureTypeFromBeamPipeType(bpt);
  auto apOut = ApertureListToVector(el.apertureParamsOut, bpt);
  if (bpt == BDSBeamPipeType::pointsfile)
    {return CreateAperture(apt, 0, 0, 0, 0, 0, 0, 0, 0, el.apertureType);}
  else if (useElementVariables)
    {
      return CreateAperture(apt,
                            el.aper1 * Units(bpt, 1),
                            el.aper2 * Units(bpt, 2),
                            el.aper3 * Units(bpt, 3),
                            el.aper4 * Units(bpt, 4),
                            0, 0, 0, 0,
                            el.apertureType,
                            el.apertureTypeOut,
                            &apOut);
    }
  else
    {
      auto values = ApertureListToVector(el.apertureParams, bpt);
      return CreateAperture(apt, values[0], values[1], values[2], values[3],
                            values[4], values[5], values[6], (G4int)values[7],
                            el.apertureType,
                            el.apertureTypeOut,
                            &apOut);
    }
}

BDSAperture* BDSApertureFactory::CreateAperture(const GMAD::Aperture& ap) const
{
  CheckNPoints(ap.nPoints, "aperture", ap.name);
  auto apt = BDS::DetermineApertureType(ap.apertureType);
  return CreateApertureSingle(apt,
                              ap.aper1 * Units(apt, 1),
                              ap.aper2 * Units(apt, 2),
                              ap.aper3 * Units(apt, 3),
                              ap.aper4 * Units(apt, 4),
                              ap.tilt  * CLHEP::rad,
                              ap.offsetX * CLHEP::m,
                              ap.offsetY * CLHEP::m,
                              (unsigned int)ap.nPoints,
                              ap.apertureType);
}

BDSAperture* BDSApertureFactory::CreateAperture(const GMAD::SamplerPlacement& sp) const
{
  CheckNPoints(sp.nPoints, "samplerplacement", sp.name);
  auto apt = BDS::DetermineApertureType(sp.shape);
  return CreateApertureSingle(apt,
                              sp.aper1 * Units(apt, 1),
                              sp.aper2 * Units(apt, 2),
                              sp.aper3 * Units(apt, 3),
                              sp.aper4 * Units(apt, 4),
                              sp.tilt  * CLHEP::rad,
                              0,
                              0,
                              (unsigned int)sp.nPoints,
                              sp.shape);
}

BDSAperture* BDSApertureFactory::CreateApertureSingle(BDSApertureType at,
                                                      G4double        a1,
                                                      G4double        a2,
                                                      G4double        a3,
                                                      G4double        a4,
                                                      G4double        tilt,
                                                      G4double        offsetX,
                                                      G4double        offsetY,
                                                      unsigned int    nPoints,
                                                      const G4String& pointsFileString) const
{
  BDSAperture* result = nullptr;
  if (nPoints == 0)
    {nPoints = DefaultNPoints(at);}
  switch (at.underlying())
    {
    case BDSApertureType::circle:
      {result = new BDSApertureCircle(a1, nPoints);                  break;}
    case BDSApertureType::ellipse:
      {result = new BDSApertureEllipse(a1, a2, nPoints);             break;}
    case BDSApertureType::rectangle:
      {result = new BDSApertureRectangle(a1, a2);                    break;}
    case BDSApertureType::rectcircle:
      {result = new BDSApertureRectCircle(a1, a2, a3, nPoints);      break;}
    case BDSApertureType::rectellipse:
      {result = new BDSApertureRectEllipse(a1, a2, a3, a4, nPoints); break;}
    case BDSApertureType::racetrack:
      {result = new BDSApertureRaceTrack(a1, a2, a3, nPoints);       break;}
    case BDSApertureType::octagon:
      {result = new BDSApertureOctagon(a1, a2, a3, a4);              break;}
    case BDSApertureType::clicpcl:
      {result = new BDSApertureClicPCL(a1, a2, a3, a4, nPoints);     break;}
    case BDSApertureType::rhombus:
      {result = new BDSApertureRhombus(a1, a2, a3, nPoints);         break;}
    case BDSApertureType::points:
      {
        G4String pointsFile;
        G4String unitsStr;
        ParsePointsFileAndUnits(pointsFileString, pointsFile, unitsStr);
        result = new BDSAperturePoints(pointsFile, unitsStr);
        break;
      }
    default:
      {break;}
    }
  if (result)
    {
      result->CheckInfoOK();
      result->SetTiltOffset(BDSTiltOffset(tilt, offsetX, offsetY));
      result->LoadData(); // default does nothing for apertures that don't need it
    }
  return result;
}

std::pair<BDSAperture*, BDSAperture*> BDSApertureFactory::CreateAperture(BDSApertureType at,
                                                                         G4double        a1,
                                                                         G4double        a2,
                                                                         G4double        a3,
                                                                         G4double        a4,
                                                                         G4double        tilt,
                                                                         G4double        offsetX,
                                                                         G4double        offsetY,
                                                                         unsigned int    nPoints,
                                                                         const G4String& pointsFileString,
                                                                         const G4String& apertureTypeOut,
                                                                         const std::vector<G4double>* apertureOut) const
{
  auto* aIn = CreateApertureSingle(at, a1, a2, a3, a4, tilt, offsetX, offsetY, nPoints, pointsFileString);
  BDSAperture* aOut = nullptr;
  if (!apertureTypeOut.empty())
    {
      BDSApertureType atOut = BDS::DetermineApertureType(apertureTypeOut);
      if (!apertureOut)
        {throw BDSException(__METHOD_NAME__, "apertureOut must be filled if using apertureTypeOut");}
      if (apertureOut->size() != 8)
        {throw BDSException(__METHOD_NAME__, "apertureOut must have all 8 variables given");}
      auto v = *apertureOut;
      aOut = CreateApertureSingle(atOut, v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7], apertureTypeOut);
    }
  return std::make_pair(aIn, aOut);
}

G4double BDSApertureFactory::RequiredLengthForBoolean(G4double angle,
                                                      G4double radius) const
{
  // use the absolute as for either positive or negative angle the solid must be extended
  G4double rl = radius*std::tan(std::abs(angle));
  return rl*1.1; // 10% margin
}

void BDSApertureFactory::CheckNPoints(int nPoints,
                                      const G4String& typeName,
                                      const G4String& objectName) const
{
  if (nPoints < 0)
    {throw BDSException(__METHOD_NAME__, "negative \"nPoints\" in " + typeName +" definition \"" + objectName + "\"");}
}

void BDSApertureFactory::ParsePointsFileAndUnits(const G4String& beamPipeType,
                                                 G4String& pointsFileName,
                                                 G4String& pointsUnit) const
{
  auto typeAndFileName = BDS::SplitOnColon(beamPipeType); // find first colon
  G4String fname = typeAndFileName.second;
  if (BDS::StrContains(fname, ":"))
    {// optional second colon with units after it
      auto fileNameAndUnit = BDS::SplitOnColon(fname);
      pointsFileName = fileNameAndUnit.first;
      pointsUnit = fileNameAndUnit.second;
    }
  else
    {
      pointsFileName = fname;
      pointsUnit = "mm";
    }
}

G4bool BDSApertureFactory::AngledFaces(const G4ThreeVector& v1,
                                       const G4ThreeVector& v2)
{
  G4bool straight = (v1.x() == 0.0) && (v1.y() == 0.0) && (v2.x() == 0.0) && (v2.y() == 0.0);
  return !straight;
}

BDSApertureFactory::Product BDSApertureFactory::CreateSolid(const G4String&    name,
                                                            G4double           length,
                                                            const BDSAperture* apertureIn,
                                                            const BDSAperture* apertureOut,
                                                            const G4ThreeVector* normalIn,
                                                            const G4ThreeVector* normalOut,
                                                            G4double           lengthExtraForBoolean)
{
  productNormalIn  = normalIn  ? *normalIn : G4ThreeVector(0, 0, -1);
  productNormalOut = normalOut ? *normalOut : G4ThreeVector(0, 0, 1);
  angledFaces      = AngledFaces(productNormalIn, productNormalOut);
  
  if (!apertureIn)
    {throw BDSException(__METHOD_NAME__, "no aperture specified.");}

  G4bool variedAperture = (G4bool)apertureOut ? apertureOut != apertureIn : false;

  productName        = name;
  productLength      = length;
  productApertureIn  = apertureIn;
  productApertureOut = apertureOut;

  lengthExtraForBoolean = CalculateExtraLength(lengthExtraForBoolean);
  productLengthExtra = lengthExtraForBoolean;

  if (variedAperture)
    {return CreateDifferentEnds();}

  // else create non-tapering piece of geometry
  Product product;
  switch (apertureIn->apertureType.underlying())
    {
    case BDSApertureType::circle:
      {product = CreateCircle();      break;}
    case BDSApertureType::rectangle:
      {product = CreateRectangle();   break;}
    case BDSApertureType::ellipse:
      {product = CreateEllipse();     break;}
    case BDSApertureType::rectcircle:
      {product = CreateRectCircle();  break;}
    case BDSApertureType::rectellipse:
      {product = CreateRectEllipse(); break;}
    case BDSApertureType::racetrack:
    case BDSApertureType::octagon:
    case BDSApertureType::clicpcl:
    case BDSApertureType::rhombus:
    case BDSApertureType::points:
      {product = CreateExtrudedSolid(); break;}
    case BDSApertureType::regularpolygon:
      {product = CreateRegularPolygon(); break;}
    default:
      {break;}
    }
  return product;
}

G4double BDSApertureFactory::CalculateExtraLength(G4double lengthExtraForBoolean)
{
  if (!BDS::IsFinite(lengthExtraForBoolean))
    {
      G4ThreeVector copy(productNormalIn);
      copy.setZ(std::abs(copy.z())); // as otherwise '0' will be pi
      G4double a1 = copy.theta();
      G4double a2 = productNormalOut.theta();
      G4double angle = std::max(std::abs(a1), std::abs(a2));
      G4double r1 = productApertureIn->RadiusToEncompass();
      G4double r2 = productApertureOut ? productApertureOut->RadiusToEncompass() : r1;
      G4double radius = std::max(std::abs(r1), std::abs(r2));
      lengthExtraForBoolean = RequiredLengthForBoolean(angle, radius);
      lengthExtraForBoolean = std::max(lengthExtraForBoolean, 0.1*productLength);
    }
  return lengthExtraForBoolean;
}

BDSApertureFactory::Product BDSApertureFactory::CreateSolidWithInner(const G4String&      name,
                                                                     G4double             length,
                                                                     const BDSAperture*   apertureInInside,
                                                                     const BDSAperture*   apertureOutInside,
                                                                     G4double             thickness,
                                                                     const G4ThreeVector* normalIn,
                                                                     const G4ThreeVector* normalOut,
                                                                     G4double             lengthExtraForBoolean)
{
  productNormalIn  = normalIn  ? *normalIn : G4ThreeVector(0,0,-1);
  productNormalOut = normalOut ? *normalOut : G4ThreeVector(0, 0, 1);
  angledFaces      = AngledFaces(productNormalIn, productNormalOut);
  
  if (!apertureInInside)
    {throw BDSException(__METHOD_NAME__, "no aperture specified.");}
  G4bool variedAperture = (G4bool)apertureOutInside && apertureOutInside != apertureInInside;
  
  productName        = name;
  productLength      = length;
  productApertureIn  = apertureInInside;
  productApertureOut = variedAperture ? apertureOutInside : apertureInInside;

  lengthExtraForBoolean = CalculateExtraLength(lengthExtraForBoolean);
  productLengthExtra = lengthExtraForBoolean;
  
  // check specialisations - these are more optimal combinations
  auto key = MakePair(productApertureIn->apertureType, productApertureOut->apertureType);

  // varied specialisations
  if (variedAperture)
    {
      auto search = hollowVariedSpecialisations.find(key);
      if (search != hollowVariedSpecialisations.end())
        {
          auto mem = search->second;
          return (this->*mem)(thickness);
        }
      else // no specialisation and different types -> use polygons
        {
          Product result;
          Product inner = CreateTubeByPoints("_inner");
          result.Extend(inner); // keep track of all solids
          productLengthExtra = 0;
          BDSAperture* apInOutside = apertureInInside->Plus(thickness);
          BDSAperture* apOutOutside = apertureOutInside->Plus(thickness); // must be different from "in" as variedAperture==true
          productApertureIn = apInOutside; // assign to members for factory action
          productApertureOut = apOutOutside;
          Product outer = CreateTubeByPoints("_outer");
          result.Extend(outer);
          G4VSolid* product = new G4SubtractionSolid(productName, outer.product, inner.product);
          result.product = product;
          delete apInOutside;
          delete apOutOutside;
          return result;
        }
    }
  else
    {
      // constant aperture specialisations
      auto search = hollowSpecialisations.find(key);
      if (search != hollowSpecialisations.end())
        {
          auto mem = search->second;
          return (this->*mem)(thickness);
        }
      else
        {// use two instances of the same shape but with one expanded to make a subtraction
          Product result;
          Product inner = CreateSolid(productName+"_inner", length + productLengthExtra,
                                      productApertureIn, productApertureOut, normalIn, normalOut, 0);
          result.Extend(inner); // keep track of all solids
          productLengthExtra = 0;
          BDSAperture* apInOutside = apertureInInside->Plus(thickness);
          Product outer = CreateSolid(productName+"_outer", length, apInOutside,
                                      apInOutside, normalIn, normalOut, 0);
          result.Extend(outer);
          G4VSolid* product = new G4SubtractionSolid(productName, outer.product, inner.product);
          result.product = product;
          delete apInOutside;
          return result;
        }
    }
}

BDSApertureFactory::Product BDSApertureFactory::CreateSolidWithInnerVariableThickness(const G4String&      name,
                                                                                      G4double             length,
                                                                                      const BDSAperture*   apertureInOutside,
                                                                                      const BDSAperture*   apertureInInside,
                                                                                      const BDSAperture*   apertureOutOutside,
                                                                                      const BDSAperture*   apertureOutInside,
                                                                                      const G4ThreeVector* normalIn,
                                                                                      const G4ThreeVector* normalOut,
                                                                                      G4double             lengthExtraForBoolean)
{
  productNormalIn  = normalIn  ? *normalIn : G4ThreeVector(0, 0, -1);
  productNormalOut = normalOut ? *normalOut : G4ThreeVector(0, 0, 1);
  angledFaces      = normalIn || normalOut;
  
  // If the apertures are twisted, it would be possible to create a shape where both the inner apertures
  // were smaller than the outer apertures, but part way through they're bigger due to the rotation. This
  // should be handled.

  // TBC
  return {nullptr, {}};
}

BDSApertureFactory::Product BDSApertureFactory::CreateCircle() const
{
  const BDSApertureCircle* ap = dynamic_cast<const BDSApertureCircle*>(productApertureIn);
  if (!ap)
    {return {nullptr, {}};}
  if (!angledFaces)
    {
      G4VSolid* product = new G4Tubs(productName+"_so", 0, ap->radius, 0.5 * productLength, 0, CLHEP::twopi);
      return {product, {}};
    }
  else
    {
      G4VSolid* product = new G4CutTubs(productName+"_so", 0, ap->radius, 0.5 * productLength,
                                        0, CLHEP::twopi, productNormalIn, productNormalOut);
      return {product, {}};
    }
}

BDSApertureFactory::Product BDSApertureFactory::CreateRectangle() const
{
  const BDSApertureRectangle* ap = dynamic_cast<const BDSApertureRectangle*>(productApertureIn);
  if (!ap)
    {return {nullptr, {}};}
  if (!angledFaces)
    {
      G4VSolid* product = new G4Box(productName, ap->a, ap->b, 0.5 * productLength);
      return {product, {}};
    }
  else
    {
      G4VSolid* box = new G4Box(productName+"_square_so", ap->a, ap->b,
                                productLength + productLengthExtra); // factor 2 here
      G4double maxRadius = ap->RadiusToEncompass();
      G4VSolid* cut = CutSolid(productName+"_cut_so", 0.5*productLength, maxRadius);
      G4VSolid* product = new G4IntersectionSolid(productName+"_so", box, cut);
      return {product, {box, cut}};
    }
}

G4VSolid* BDSApertureFactory::CutSolid(const G4String& name,
                                       G4double halfLength,
                                       G4double radiusToEncompass) const
{
  G4double intersectionRadius = intersectionRadiusRatio * radiusToEncompass;
  G4VSolid* cut;
  if (!angledFaces)
    {cut = new G4Tubs(name, 0, intersectionRadius, halfLength, 0, CLHEP::twopi);}
  else
    {
      cut = new G4CutTubs(name, 0, intersectionRadius, halfLength,
                          0, CLHEP::twopi, productNormalIn, productNormalOut);
    }
  return cut;
}

BDSApertureFactory::Product BDSApertureFactory::CreateEllipse() const
{
  const BDSApertureEllipse* ap = dynamic_cast<const BDSApertureEllipse*>(productApertureIn);
  if (!ap)
    {return {nullptr, {}};}
  if (!angledFaces)
    {
      G4VSolid* product = new G4EllipticalTube(productName+"_so", ap->a, ap->b, 0.5*productLength);
      return {product, {}};
    }
  else
    {
      G4double maxRadius = ap->RadiusToEncompass();
      G4VSolid* cut = CutSolid(productName+"_cut_so", 0.5*productLength, maxRadius);
      G4VSolid* tube = new G4EllipticalTube(productName+"_square_so", ap->a, ap->b,
                                            productLength + productLengthExtra);
      G4VSolid* product = new G4IntersectionSolid(productName+"_so", tube, cut);
      return {product, {}};
    }
}

BDSApertureFactory::Product BDSApertureFactory::CreateExtrudedSolid() const
{
  BDSPolygon p = productApertureIn->Polygon();
  G4TwoVector zOffsets(0,0); // the transverse offset of each plane from 0,0
  G4double zScale = 1; // the scale at each end of the points = 1
  if (!angledFaces)
    {
      G4VSolid* product = new G4ExtrudedSolid(productName+"_so", p.Points(), 0.5 * productLength,
                                              zOffsets, zScale,  // dx,dy offset for each face, scaling
                                              zOffsets, zScale); // dx,dy offset for each face, scaling
      return {product, {}};
    }
  else
    {
      G4double maxRadius = productApertureIn->RadiusToEncompass();
      G4VSolid* cut = CutSolid(productName+"_cut_so", 0.5*productLength, maxRadius);
      G4VSolid* square = new G4ExtrudedSolid(productName+"_straight_so", p.Points(),
                                            productLength + productLengthExtra,
                                            zOffsets, zScale,  // dx,dy offset for each face, scaling
                                            zOffsets, zScale); // dx,dy offset for each face, scaling
      G4VSolid* product = new G4IntersectionSolid(productName+"_so", square, cut);
      return {product, {cut, square}};
    }
}

BDSApertureFactory::Product BDSApertureFactory::CreateRectCircle() const
{
  const BDSApertureRectCircle* ap = dynamic_cast<const BDSApertureRectCircle*>(productApertureIn);
  if (!ap)
    {return {nullptr, {}};}
  if (!angledFaces)
    {
      G4VSolid* circle = new G4Tubs(productName + "_circle_so", 0, ap->radius, 0.5*productLength, 0, CLHEP::twopi);
      G4VSolid* rect = new G4Box(productName + "_rectangle_so", ap->a, ap->b, productLength+productLengthExtra);
      G4VSolid* product = new G4IntersectionSolid(productName+"_so", circle, rect);
      return {product, {circle, rect}};
    }
  else
    {
      G4VSolid* circle = new G4CutTubs(productName + "_circle_so", 0, ap->radius,
                                       0.5*productLength, 0, CLHEP::twopi,
                                       productNormalIn, productNormalOut);
      G4VSolid* rect = new G4Box(productName + "_rect_so", ap->a, ap->b,
                                 1.1*(productLength+productLengthExtra));
      G4VSolid* product = new G4IntersectionSolid(productName+"_so", circle, rect);
      return {product, {circle, rect}};
    }
}

BDSApertureFactory::Product BDSApertureFactory::CreateRectEllipse() const
{
  const BDSApertureRectEllipse* ap = dynamic_cast<const BDSApertureRectEllipse*>(productApertureIn);
  if (!ap)
    {return {nullptr, {}};}
  if (!angledFaces)
    {
      G4VSolid* ellipse = new G4EllipticalTube(productName+"_ellipse_so", ap->ellipseA, ap->ellipseB, 0.5*productLength);
      G4VSolid* rect = new G4Box(productName+"_rect_so", ap->rectangleA, ap->rectangleB, productLength);
      G4VSolid* product = new G4IntersectionSolid(productName+"_so", ellipse, rect);
      return {product, {ellipse, rect}};
    }
  else
    {
      G4double maxRadius = ap->RadiusToEncompass();
      G4VSolid* cut = CutSolid(productName + "_angled_so", 0.5*productLength, maxRadius);
      G4VSolid* ellipse = new G4EllipticalTube(productName + "_ellipse_so", ap->ellipseA, ap->ellipseB,
                                               productLength + productLengthExtra);
      G4VSolid* rect = new G4Box(productName + "_rect_so", ap->rectangleA, ap->rectangleB,
                                 1.5*productLength + productLengthExtra);
      G4VSolid* ellipseRect = new G4IntersectionSolid(productName, ellipse, rect);
      G4VSolid* product = new G4IntersectionSolid(productName, ellipseRect, cut);
      return {product, {cut, ellipse, rect, ellipseRect}};
    }
}

BDSApertureFactory::Product BDSApertureFactory::CreateRegularPolygon() const
{
  const BDSApertureRegularPolygon* ap = dynamic_cast<const BDSApertureRegularPolygon*>(productApertureIn);
  if (!ap)
    {return {nullptr, {}};}
  G4double innerRadii[2] = {0, 0};
  G4double outerRadii[2] = {ap->radius, ap->radius};
  if (!angledFaces)
    {
      G4double zPlanes[2]    = {-0.5*productLength, 0.5*productLength};

      G4VSolid* product = new G4Polyhedra(productName+"_so", ap->polygonTilt, CLHEP::twopi,
                                          ap->nSides, 2, zPlanes, innerRadii, outerRadii);
      return {product, {}};
    }
  else
    {
      G4double maxRadius = ap->RadiusToEncompass();
      G4VSolid* cut = CutSolid(productName + "_angled_so", 0.5*productLength, maxRadius);
      G4double zPlanes[2]    = {-productLength, productLength};
      G4VSolid* polygon = new G4Polyhedra(productName+"_poly_so", ap->polygonTilt, CLHEP::twopi,
                                          ap->nSides, 2, zPlanes, innerRadii, outerRadii);
      G4VSolid* product = new G4IntersectionSolid(productName, polygon, cut);
      return {product, {cut, polygon}};
    }
}

BDSApertureFactory::Product BDSApertureFactory::CreateDifferentEnds() const
{
  G4bool outTiltOffset = (G4bool)productApertureOut ? productApertureOut->TiltOrOffset() : false;
  if (productApertureIn->TiltOrOffset() || outTiltOffset)
    {return CreateTubeByPoints("");}

  // check specialisations
  auto key    = MakePair(productApertureIn->apertureType, productApertureOut->apertureType);
  auto search = specialisations.find(key);
  if (search != specialisations.end())
    {
      auto mem = search->second;
      return (this->*mem)();
    }
  else // no specialisation, so use high number polygons
    {return CreateTubeByPoints("");}
}

BDSApertureFactory::Product BDSApertureFactory::CreateTubeByPoints(const G4String& nameSuffix) const
{
  unsigned int nPointsIn  = productApertureIn->MinimumNumberOfPoints();
  unsigned int nPointsOut = productApertureOut->MinimumNumberOfPoints();

  G4bool isAMultiple = (nPointsIn % nPointsOut == 0) || (nPointsOut % nPointsIn == 0);
  if (!isAMultiple)
    {
      unsigned int lowestCommonMultiple = BDS::LowestCommonMultiple(nPointsIn, nPointsOut);
      nPointsIn  = lowestCommonMultiple;
      nPointsOut = lowestCommonMultiple;
    }

  BDSPolygon startingPoints  = productApertureIn->Polygon(nPointsIn);
  BDSPolygon finishingPoints = productApertureOut->Polygon(nPointsOut);

  // choose more z points in tube if it twists - approximately 1 z plane per 10-ish degrees
  unsigned int nZ = 2;
  if (productApertureIn->FiniteTilt() || productApertureOut->FiniteTilt())
    {
      G4double dTilt = productApertureOut->tiltOffset.Tilt() - productApertureIn->tiltOffset.Tilt();
      nZ += std::ceil(std::abs(dTilt) / 0.2*CLHEP::radian);
    }

  G4VSolid* result = new BDSTube(productName+nameSuffix+"_so", productLength,
                                 startingPoints, finishingPoints, nZ);
  return {result, {}};
}

BDSApertureFactory::Product BDSApertureFactory::CreateDifferentEndsCircleToCircle() const
{
  const BDSApertureCircle* ap1 = dynamic_cast<const BDSApertureCircle*>(productApertureIn);
  const BDSApertureCircle* ap2 = dynamic_cast<const BDSApertureCircle*>(productApertureOut);
  if (!ap1 || !ap2)
    {return {nullptr, {}};}
  G4VSolid* product;
  if (angledFaces)
    {
      // make longer cone, then intersect with cut tubs
      G4Cons* cons = new G4Cons(productName+"_base_cons_so", 0, ap1->radius, 0, ap2->radius,
                                0.5*(productLength + productLengthExtra), 0, CLHEP::twopi);
      G4double maxRadius = std::max(ap1->radius, ap2->radius);
      G4VSolid* cut = CutSolid(productName+"_cut_so", productLength, maxRadius);
      product = new G4IntersectionSolid(productName+"_so", cons, cut);
      return {product, {cons, cut}};
    }
  else
    {
      product = new G4Cons(productName+"_so", 0, ap1->radius, 0, ap2->radius,
                           0.5*productLength, 0, CLHEP::twopi);
      return {product, {}};
    }
}

BDSApertureFactory::Product BDSApertureFactory::HollowCircleToCircle(G4double thickness) const
{
  const BDSApertureCircle* ap = dynamic_cast<const BDSApertureCircle*>(productApertureIn);
  if (!ap)
    {return {nullptr, {}};}
  if (!angledFaces)
    {
      G4VSolid* product = new G4Tubs(productName+"_so", ap->radius, ap->radius + thickness,
                                     0.5*productLength, 0, CLHEP::twopi);
      return {product, {}};
    }
  else
    {
      G4VSolid* product = new G4CutTubs(productName+"_so", ap->radius, ap->radius + thickness,
                                        0.5*productLength, 0, CLHEP::twopi,
                                        productNormalIn, productNormalOut);
      return {product, {}};
    }
}

BDSApertureFactory::Product BDSApertureFactory::HollowVariedCircleToCircle(G4double thickness) const
{
  const BDSApertureCircle* ap1 = dynamic_cast<const BDSApertureCircle*>(productApertureIn);
  const BDSApertureCircle* ap2 = dynamic_cast<const BDSApertureCircle*>(productApertureOut);
  if (!ap1 || !ap2)
    {return {nullptr, {}};}
  G4VSolid* product;
  if (angledFaces)
    {
      // make longer cone, then intersect with cut tubs
      G4Cons* cons = new G4Cons(productName+"_base_cons_so", ap1->radius, ap1->radius+thickness,
                                ap2->radius, ap2->radius+thickness,
                                0.5*(productLength + productLengthExtra), 0, CLHEP::twopi);
      G4double maxRadius = std::max(ap1->radius, ap2->radius);
      G4VSolid* cut = CutSolid(productName+"_cut_so", productLength, maxRadius);
      product = new G4IntersectionSolid(productName+"_so", cons, cut);
      return {product, {cons, cut}};
    }
  else
    {
      product = new G4Cons(productName+"_so", ap1->radius, ap1->radius+thickness, ap2->radius,
                           ap2->radius+thickness, 0.5*productLength, 0, CLHEP::twopi);
      return {product, {}};
    }
}

BDSApertureFactory::Product BDSApertureFactory::HollowEllipseToEllipse(G4double thickness) const
{
  const auto* ap = dynamic_cast<const BDSApertureEllipse*>(productApertureIn);
  if (!ap)
    {return {nullptr, {}};}
  if (!angledFaces)
    {
      G4VSolid* inner = new G4EllipticalTube(productName+"_inner_so", ap->a, ap->b, 0.5*productLength + productLengthExtra);
      G4VSolid* outer = new G4EllipticalTube(productName+"_outer_so", ap->a+thickness, ap->b+thickness, 0.5*productLength);
      G4VSolid* product = new G4SubtractionSolid(productName+"_so", outer, inner);
      return {product, {inner, outer}};
    }
  else
    {
      G4double maxRadius = ap->RadiusToEncompass();
      G4VSolid* cut = CutSolid(productName+"_cut_so", 0.5*productLength, maxRadius);
      G4VSolid* inner = new G4EllipticalTube(productName+"_inner_so", ap->a, ap->b,
                                             0.5*productLength + 1.5*productLengthExtra);
      G4VSolid* outer = new G4EllipticalTube(productName+"_outer_so", ap->a+thickness, ap->b+thickness,
                                             0.5*productLength + productLengthExtra);
      G4VSolid* straight = new G4SubtractionSolid(productName+"_straight_so", outer, inner);
      G4VSolid* product = new G4IntersectionSolid(productName+"_so", straight, cut);
      return {product, {cut, inner, outer, straight}};
    }
}

BDSApertureFactory::Product BDSApertureFactory::HollowRectangleToRectangle(G4double thickness) const
{
  const auto* ap = dynamic_cast<const BDSApertureRectangle*>(productApertureIn);
  if (!ap)
    {return {nullptr, {}};}
  if (!angledFaces)
    {
      G4VSolid* inner = new G4Box(productName+"_inner_so", ap->a, ap->b, 0.5*productLength + productLengthExtra);
      G4VSolid* outer = new G4Box(productName+"_outer_so", ap->a + thickness, ap->b + thickness,
                                  0.5*productLength);
      G4VSolid* product = new G4SubtractionSolid(productName+"_so", outer, inner);
      return {product, {inner, outer}};
    }
  else
    {
      G4VSolid* inner = new G4Box(productName+"_inner_so", ap->a, ap->b,
                                  0.5*productLength + 1.1*productLengthExtra);
      G4VSolid* outer = new G4Box(productName+"_outer_so", ap->a + thickness, ap->b + thickness,
                                  0.5*productLength + productLengthExtra);
      G4VSolid* part1 = new G4SubtractionSolid(productName+"_straight_so", outer, inner);
      
      G4double maxRadius = ap->RadiusToEncompass();
      G4VSolid* cut = CutSolid(productName + "_cut_so", 0.5*productLength, maxRadius);
      G4VSolid* product = new G4IntersectionSolid(productName+"_so", part1, cut);
      return {product, {inner, outer, part1, cut}};
    }
}

BDSApertureFactory::Product BDSApertureFactory::HollowRectCircleToRectCircle(G4double thickness) const
{
  const BDSApertureRectCircle* ap = dynamic_cast<const BDSApertureRectCircle*>(productApertureIn);
  if (!ap)
    {return {nullptr, {}};}
  if (!angledFaces)
    {
      G4VSolid* circleInside = new G4Tubs(productName + "_inner_circle_so", 0, ap->radius, productLength, 0, CLHEP::twopi);
      G4VSolid* rectInside = new G4Box(productName + "_inner_rect_so", ap->a, ap->b, 1.1*productLength);
      G4VSolid* inner = new G4IntersectionSolid(productName+"_inner_so", circleInside, rectInside);

      G4VSolid* circleOutside = new G4Tubs(productName + "_outer_circle_so", 0, ap->radius+thickness,
                                           0.5*productLength, 0, CLHEP::twopi);
      G4VSolid* rectOutside = new G4Box(productName + "_outer_rect_so", ap->a+thickness,
                                        ap->b+thickness, 0.6*productLength);
      G4VSolid* outer = new G4IntersectionSolid(productName+"_outer_so", circleOutside, rectOutside);

      G4VSolid* product = new G4SubtractionSolid(productName+"_so", outer, inner);
      return {product, {circleInside, rectInside, inner, circleOutside, rectOutside, outer}};
    }
  else
    {
      G4VSolid* circleInside = new G4CutTubs(productName + "_inner_circle_so", 0, ap->radius,
                                             productLength+productLengthExtra, 0, CLHEP::twopi,
                                             productNormalIn, productNormalOut);
      G4VSolid* rectInside = new G4Box(productName + "_inner_rect_so", ap->a, ap->b,
                                       1.1*(productLength+productLengthExtra));
      G4VSolid* inner = new G4IntersectionSolid(productName+"_inner_so", circleInside, rectInside);

      G4VSolid* circleOutside = new G4CutTubs(productName + "_outer_circle_so", 0, ap->radius+thickness, 0.5*productLength,
                                              0, CLHEP::twopi, productNormalIn, productNormalOut);
      G4VSolid* rectOutside = new G4Box(productName + "_outer_rect_so", ap->a+thickness, ap->b+thickness,
                                        0.6*productLength+productLengthExtra);
      G4VSolid* outer = new G4IntersectionSolid(productName+"_outer_so", circleOutside, rectOutside);

      G4VSolid* product = new G4SubtractionSolid(productName+"_so", outer, inner);
      return {product, {circleInside, rectInside, inner, circleOutside, rectOutside, outer}};
    }
}

BDSApertureFactory::Product BDSApertureFactory::HollowRectEllipseToRectEllipse(G4double thickness) const
{
  const BDSApertureRectEllipse* ap = dynamic_cast<const BDSApertureRectEllipse*>(productApertureIn);
  if (!ap)
    {return {nullptr, {}};}
  if (!angledFaces)
    {
      G4VSolid* ellipseInside = new G4EllipticalTube(productName + "_inner_ellipse_so", ap->ellipseA,
                                                     ap->ellipseB, productLength);
      G4VSolid* rectInside = new G4Box(productName + "_inner_rect_so", ap->rectangleA, ap->rectangleB,
                                       1.1*productLength);
      G4VSolid* inner = new G4IntersectionSolid(productName+"_inner_so", ellipseInside, rectInside);

      G4VSolid* ellipseOutside = new G4EllipticalTube(productName + "_outer_ellipse_so", ap->ellipseA+thickness,
                                                      ap->ellipseB+thickness, 0.5*productLength);
      G4VSolid* rectOutside = new G4Box(productName + "_outer_rect_so", ap->rectangleA+thickness,
                                        ap->rectangleB+thickness, 0.6*productLength);
      G4VSolid* outer = new G4IntersectionSolid(productName+"_outer_so", ellipseOutside, rectOutside);

      G4VSolid* product = new G4SubtractionSolid(productName+"_so", outer, inner);
      return {product, {ellipseInside, rectInside, inner, ellipseOutside, rectOutside, outer}};
    }
  else
    {
      G4double lOuter = 0.5*productLength + productLengthExtra;
      G4double lInner = 1.1*lOuter;
      G4VSolid* ellipseInside = new G4EllipticalTube(productName + "_inner_ellipse_so", ap->ellipseA,
                                                     ap->ellipseB, lInner);
      G4VSolid* rectInside = new G4Box(productName + "_inner_rect_so", ap->rectangleA, ap->rectangleB,
                                       1.1*lInner);
      G4VSolid* inner = new G4IntersectionSolid(productName+"_inner_so", ellipseInside, rectInside);

      G4VSolid* ellipseOutside = new G4EllipticalTube(productName + "_outer_ellipse_so", ap->ellipseA+thickness,
                                                      ap->ellipseB+thickness, lOuter);
      G4VSolid* rectOutside = new G4Box(productName + "_outer_rect_so", ap->rectangleA+thickness,
                                        ap->rectangleB+thickness, 1.1*lOuter);
      G4VSolid* outer = new G4IntersectionSolid(productName+"_outer_so", ellipseOutside, rectOutside);

      G4VSolid* straight = new G4SubtractionSolid(productName+"_straight_so", outer, inner);

      G4double maxRadius = ap->RadiusToEncompass();
      G4VSolid* cut = CutSolid(productName + "_cut_so", 0.5*productLength, maxRadius);
      G4VSolid* product = new G4IntersectionSolid(productName+"_so", straight, cut);

      return {product, {ellipseInside, rectInside, inner, ellipseOutside, rectOutside, outer, straight, cut}};
    }
}

std::pair<BDSApertureType,BDSApertureType> BDSApertureFactory::MakePair(BDSApertureType a1,
                                                                        BDSApertureType a2) const
{
  return std::make_pair(std::min(a1, a2), std::max(a1, a2));
}