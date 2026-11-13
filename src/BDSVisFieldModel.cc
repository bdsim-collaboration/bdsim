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
#include "BDSColourScaleMagma.hh"
#include "BDSColourScaleViridis.hh"
#include "BDSFieldQueryForVis.hh"
#include "BDSFieldQueryInfo.hh"
#include "BDSVisFieldModel.hh"

#include "G4ArrowModel.hh"
#include "G4Box.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4String.hh"
#include "G4ThreeVector.hh"
#include "G4Types.hh"
#include "G4VGraphicsScene.hh"
#include "G4VMarker.hh"
#include "G4VisAttributes.hh"
#include "BDSWarning.hh"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdio>
#include <iomanip>
#include <ios>
#include <limits>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

G4int BDSVisFieldModel::instanceCounter = 0;

BDSVisFieldModel::BDSVisFieldModel(const std::vector<BDSFieldQueryInfo*>& queriesIn):
  queries(queriesIn),
  pointVisB(nullptr),
  pointVisE(nullptr)
{
  fGlobalTag = "BDSVisFieldModel";
  fGlobalDescription = "Field view #" + std::to_string(instanceCounter);
  instanceCounter++;
  
  auto bFieldColour = BDSColourScaleViridis();
  pointVisB = new G4VisAttributes(bFieldColour.GetValue(0));
  auto eFieldColour = BDSColourScaleMagma();
  pointVisE = new G4VisAttributes(eFieldColour.GetValue(0));
  
  pointVisB->SetForceSolid(true); // no default argument for g4 V < 10.3, so specify true always
  pointVisE->SetForceSolid(true);
}

BDSVisFieldModel::~BDSVisFieldModel()
{
  delete pointVisB;
  delete pointVisE;
}

void BDSVisFieldModel::DescribeYourselfTo(G4VGraphicsScene& sceneHandler)
{
  auto bFieldColour = BDSColourScaleViridis();
  auto eFieldColour = BDSColourScaleMagma();
  
  BDSFieldQueryForVis querier;
  for (const auto& query : queries)
    {
      querier.QueryField(query);
      G4double maxFieldB = querier.MaxFieldB();
      G4double maxFieldE = querier.MaxFieldE();
      const std::vector<std::array<G4double, 9>> xyzBEs = querier.Values();
      
      G4double arrowLength = CalculateArrowLength(query);
      if (arrowLength > 0.01 * std::numeric_limits<G4double>::max()) // This shouldn't happen, but is a sanity check
	{arrowLength = 1e-3 * sceneHandler.GetExtent().GetExtentRadius();}
      
      G4double arrowWidth = 0.3*arrowLength;
      G4double pointSize = std::max(0.1*arrowWidth, 5.0);
      
      G4bool drawArrows = query->drawArrows;
      G4bool drawBoxes = query->drawBoxes;
      
      if (!drawArrows && !drawBoxes)
        {
          BDS::Warning("Neither drawArrows or drawBoxes are turned on in query \"" + query->name + "\" -> skipping");
          continue; // skip this query
        }
      
      G4double boxAlpha = query->boxAlpha;
      if (boxAlpha <= 0)
        {
          BDS::Warning("boxAlpha in query \"" + query->name + "\" must be > 0. Setting to 0.01");
          boxAlpha = 0.01;
        }
      if (boxAlpha > 1)
	{
	  BDS::Warning("boxAlpha in query \"" + query->name + "\" must be <= 1. Setting to 1.0");
	  boxAlpha = 1.0;
	}
      
      G4ThreeVector boxHalfSize = BoxHalfSize(query);
      G4AffineTransform qTransform = query->globalTransform;
      G4RotationMatrix  qRotation  = qTransform.NetRotation().inverse();
      G4Box aBox("b", boxHalfSize.x(), boxHalfSize.y(), boxHalfSize.z());
      G4Polyhedron* baseBoxPoly = aBox.GetPolyhedron();
      
      if (query->queryMagnetic)
	{
    G4Colour boxZeroColourB = pointVisB->GetColor();
    G4Colour boxZeroColourB2(boxZeroColourB.GetRed(), boxZeroColourB.GetGreen(), boxZeroColourB.GetBlue(), boxAlpha);
    G4VisAttributes* boxZeroVisB = new G4VisAttributes(boxZeroColourB2);
	  G4String arrowPrefix = query->name + "_B_";
	  for (const auto& xyzBE: xyzBEs)
	    {
	      // x y z Bx By Bz Ex Ey Ez
	      G4ThreeVector B(xyzBE[3], xyzBE[4], xyzBE[5]);
	      G4ThreeVector unitB = B.unit();
	      G4double bMag = B.mag();
	      G4ThreeVector midPoint(xyzBE[0], xyzBE[1], xyzBE[2]);
    
	      // GetPolyhedron returns a pointer, but it's the same object always
	      // and there's no interface to allow rebuilding that polyhedron, so
	      // if we translate it we compound that transform. No method to just
	      // set the transform, so have to copy it.
	      G4Polyhedron boxPoly(*baseBoxPoly);
	      boxPoly.Transform(G4Transform3D(qRotation, midPoint));
	      
	      if (bMag == 0)
		{
		  sceneHandler.BeginPrimitives();
		  if (drawArrows)
		    {
		      G4Circle aPoint({xyzBE[0], xyzBE[1], xyzBE[2]});
		      aPoint.SetVisAttributes(pointVisB);
		      aPoint.SetDiameter(G4VMarker::SizeType::world, pointSize);
		      aPoint.SetFillStyle(G4VMarker::FillStyle::filled);
		      sceneHandler.AddPrimitive(aPoint);
		    }
		  if (drawBoxes)
		    {
		      boxPoly.SetVisAttributes(boxZeroVisB);
		      sceneHandler.AddPrimitive(boxPoly);
		    }
		  sceneHandler.EndPrimitives();
		  continue;
		} // don't add 0 field arrows
	      G4ThreeVector startPoint = midPoint - 0.5*arrowLength*unitB;
	      G4ThreeVector endPoint = midPoint + 0.5*arrowLength*unitB;
	      G4double normalisedValue = bMag / maxFieldB;
	      if (!std::isfinite(normalisedValue))
		{normalisedValue = 0.0;}
	      G4Colour arrowColour = bFieldColour.GetValue(normalisedValue);
	      if (drawBoxes)
		{
		  G4Colour boxColour(arrowColour.GetRed(), arrowColour.GetGreen(), arrowColour.GetBlue(), boxAlpha);
		  sceneHandler.BeginPrimitives();
		  boxPoly.SetVisAttributes(G4VisAttributes(boxColour));
		  sceneHandler.AddPrimitive(boxPoly);
		  sceneHandler.EndPrimitives();
		}
	      if (drawArrows)
		{
		  std::stringstream ss;
		  ss << std::setw(2) << std::fixed << xyzBE[0] << "_" << xyzBE[1] << "_" << xyzBE[2];
		  std::string arrowName = ss.str();
		  G4ArrowModel FArrow(startPoint.x(), startPoint.y(), startPoint.z(),
				      endPoint.x(), endPoint.y(), endPoint.z(),
				      arrowWidth, arrowColour, arrowName);
		  FArrow.DescribeYourselfTo(sceneHandler);
		}
	    }
    delete boxZeroVisB;
	}
      
      if (query->queryElectric)
	{
    G4Colour boxZeroColourE = pointVisE->GetColor();
    G4Colour boxZeroColourE2(boxZeroColourE.GetRed(),
                             boxZeroColourE.GetGreen(),
                             boxZeroColourE.GetBlue(),
                             boxAlpha);
    G4VisAttributes* boxZeroVisE = new G4VisAttributes(boxZeroColourE2);
	  G4String arrowPrefix = query->name + "_E_";
	  for (const auto& xyzBE: xyzBEs)
	    {
	      // x y z Bx By Bz Ex Ey Ez
	      G4ThreeVector E(xyzBE[6], xyzBE[7], xyzBE[8]);
	      G4ThreeVector unitE = E.unit();
	      G4double eMag = E.mag();
	      G4ThreeVector midPoint(xyzBE[0], xyzBE[1], xyzBE[2]);

	      G4Polyhedron boxPoly(*baseBoxPoly);
	      boxPoly.Transform(G4Transform3D(qRotation, midPoint));
	      if (eMag == 0)
		{
		  sceneHandler.BeginPrimitives();
		  if (drawArrows)
		    {
		      G4Circle aPoint({xyzBE[0], xyzBE[1], xyzBE[2]});
		      aPoint.SetVisAttributes(pointVisE);
		      aPoint.SetDiameter(G4VMarker::SizeType::world, pointSize);
		      aPoint.SetFillStyle(G4VMarker::FillStyle::filled);
		      sceneHandler.AddPrimitive(aPoint);
		    }
		  if (drawBoxes)
		    {
		      boxPoly.SetVisAttributes(boxZeroVisE);
		      sceneHandler.AddPrimitive(boxPoly);
		    }
		  sceneHandler.EndPrimitives();
		  continue;
		} // don't add 0 field arrows
	      G4ThreeVector startPoint = midPoint - 0.5*arrowLength*unitE;
	      G4ThreeVector endPoint = midPoint + 0.5*arrowLength*unitE;
	      G4double normalisedValue = eMag / maxFieldE;
	      if (!std::isfinite(normalisedValue))
		{normalisedValue = 0.0;}
	      G4Colour arrowColour = eFieldColour.GetValue(normalisedValue);
	      if (drawBoxes)
		{
		  G4Colour boxColour(arrowColour.GetRed(), arrowColour.GetGreen(), arrowColour.GetBlue(), boxAlpha);
		  sceneHandler.BeginPrimitives();
		  boxPoly.SetVisAttributes(G4VisAttributes(boxColour));
		  sceneHandler.AddPrimitive(boxPoly);
		  sceneHandler.EndPrimitives();
		}
	      if (drawArrows)
		{
		  std::stringstream ss;
		  ss << std::setw(2) << std::fixed << xyzBE[0] << "_" << xyzBE[1] << "_" << xyzBE[2];
		  std::string arrowName = ss.str();		  
		  G4ArrowModel FArrow(startPoint.x(), startPoint.y(), startPoint.z(),
				      endPoint.x(), endPoint.y(), endPoint.z(),
				      arrowWidth, arrowColour, arrowName);
		  FArrow.DescribeYourselfTo(sceneHandler);
		}
	    }
    delete boxZeroVisE;
	}
      
      querier.CleanUp();
    }
}

G4double BDSVisFieldModel::CalculateArrowLength(const BDSFieldQueryInfo* query) const
{
  G4double result = std::min({QIL(query->xInfo), QIL(query->yInfo), QIL(query->zInfo)});
  return result;
}

G4double BDSVisFieldModel::QIL(const BDSFieldQueryInfo::QueryDimensionInfo& qi) const
{
  if (qi.n == 1)
    {return std::numeric_limits<G4double>::max();}
  
  G4double step = std::abs((qi.max - qi.min) / ((G4double)qi.n));
  return 0.8 * step;
}

G4ThreeVector BDSVisFieldModel::BoxHalfSize(const BDSFieldQueryInfo* qi) const
{
  G4double minSize = std::numeric_limits<double>::max();
  G4ThreeVector result(0,0,0);
  std::array<G4bool,3> nonZero = {false, false, false};
  std::array<const BDSFieldQueryInfo::QueryDimensionInfo,3> dims = {qi->xInfo, qi->yInfo, qi->zInfo};

  G4int nDims = 0;
  for (G4int i = 0; i < 3; i++)
    {
      nonZero[i] = dims[i].n > 1;
      if (nonZero[i])
        {
	  nDims += 1;
          result[i] = dims[i].StepSize();
          minSize = std::min(minSize, result[i]);
        }
    }

  G4double thinSize;
  if (nDims == 1)
    {thinSize = 0.1 * minSize;} // 20% x 0.5
  else // 2 or 3 dimensions - only poosibly set 3rd dimension as thin-ish
    {thinSize = 1e-2 * minSize;}
  for (G4int i = 0; i < 3; i++)
    {result[i] = nonZero[i] ? result[i]*0.5 : thinSize;}
  
  return result;
}
