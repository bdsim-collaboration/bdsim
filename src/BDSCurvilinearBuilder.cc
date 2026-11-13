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
#include "BDSAcceleratorComponent.hh"
#include "BDSAcceleratorComponentRegistry.hh"
#include "BDSBeamline.hh"
#include "BDSBeamlineElement.hh"
#include "BDSExtent.hh"
#include "BDSCurvilinearBuilder.hh"
#include "BDSCurvilinearFactory.hh"
#include "BDSGlobalConstants.hh"
#include "BDSSimpleComponent.hh"
#include "BDSSamplerType.hh"
#include "BDSTiltOffset.hh"
#include "BDSTunnelInfo.hh"
#include "BDSUtilities.hh"
#include "BDSWarning.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"

#include <algorithm>
#include <cmath>
#include <iterator>

BDSCurvilinearBuilder::BDSCurvilinearBuilder()
{
  const BDSGlobalConstants* globals = BDSGlobalConstants::Instance(); // shortcut

  paddingLength       = BDSBeamline::PaddingLength();
  defaultBridgeLength = paddingLength + 4*globals->LengthSafety();
  curvilinearRadius   = globals->CurvilinearDiameter()*0.5;
  radiusTolerance     = 0.8;
  if (globals->BuildTunnel() || globals->BuildTunnelStraight())
    {// query the default tunnel model
      BDSExtent tunnelExtent = globals->TunnelInfo()->IndicativeExtent();
      tunnelExtent = tunnelExtent.Translate(globals->TunnelOffsetX(), globals->TunnelOffsetY(), 0);
      G4double maxTunnelR = tunnelExtent.MaximumAbs();
      if (curvilinearRadius < maxTunnelR)
        {
          if (globals->CurvilinearDiameterShrunkForBends())
            {
              G4String message = "Strong bends necessitate smaller curvilinear geometry than the tunnel size, but\n";
              message += "tunnel hits will need transform for curvilinear coordinates.";
              BDS::Warning(message);
            }
          else
            {curvilinearRadius = std::max(curvilinearRadius, maxTunnelR);}
        }
    }
  bonusChordLength = 1*CLHEP::m;

  factory = new BDSCurvilinearFactory();
}

BDSCurvilinearBuilder::~BDSCurvilinearBuilder()
{
  delete factory;
}

BDSBeamline* BDSCurvilinearBuilder::BuildCurvilinearBeamLine1To1(BDSBeamline const* const beamline,
								 const G4bool circular)
{  
  BDSBeamline* result = new BDSBeamline();

  //prepend small sections to machine when non-circular
  G4bool bonusSections = (!circular && !beamline->empty());

  if (bonusSections)
    {//prepend small section to machine
      BDSBeamlineElement* bonusBit = CreateBonusSectionStart(beamline);
      result->AddBeamlineElement(bonusBit);
    }
  
  G4int i = 0;
  for (BDSBeamline::const_iterator element = beamline->begin(); element != beamline->end(); ++element)
    {
      G4String name = (*element)->GetName() + "_cl_" + std::to_string(i);
      i++;
      const BDSBeamlineElement* pstEl = nullptr;
      const BDSBeamlineElement* nxtEl = nullptr;
      PreviousAndNext(element, beamline->begin(), beamline->end(), pstEl, nxtEl);
      G4double crRadius = std::min({CurvilinearRadius(pstEl),
                                    CurvilinearRadius(*element),
                                    CurvilinearRadius(nxtEl)});
      BDSBeamlineElement* temp = CreateCurvilinearElement(name, element, element, i, crRadius);
      if (temp)
	{result->AddBeamlineElement(temp);}
    }

  if (bonusSections)
    {// append small section to machine
      BDSBeamlineElement* bonusBit = CreateBonusSectionEnd(beamline);
      if (bonusBit)
	{result->AddBeamlineElement(bonusBit);}
    }
  return result;
}

void BDSCurvilinearBuilder::PreviousAndNext(BDSBeamline::const_iterator it,
    BDSBeamline::const_iterator startIt,
    BDSBeamline::const_iterator endIt,
    const BDSBeamlineElement*& previous,
    const BDSBeamlineElement*& next) const
{
  G4bool isFirst = it == startIt;
  G4bool isLast  = std::next(it) == endIt; // end is one beyond the last

  G4double minimumSize = 0.1*CLHEP::mm;
  auto pst = std::prev(it);
  if (isFirst)
    {previous = nullptr;}
  else
    {
      previous = *pst;
      if (previous->GetArcLength() < minimumSize && pst != startIt)
        {
          pst = std::prev(pst);
          if (pst != startIt)
            {previous = *pst;}
        }
    }
  auto nxt = std::next(it);
  if (isLast)
    {next = nullptr;}
  else
    {
      next = *nxt;
      if (next->GetArcLength() < minimumSize && nxt != endIt)
        {
          nxt = std::next(nxt);
          if (nxt != endIt)
            {next = *nxt;}
        }
    }
}

BDSBeamline* BDSCurvilinearBuilder::BuildCurvilinearBridgeBeamLine(BDSBeamline const* const beamline)
{
  BDSBeamline* result = new BDSBeamline();

  BDSAcceleratorComponent* defaultBridge = CreateDefaultBridgeComponent();

  G4int numberOfUniqueComponents = 0;
  G4int beamlineIndex            = 0;
  for (BDSBeamline::const_iterator element = beamline->begin(); element != beamline->end(); ++element)
    {
      auto nxt = std::next(element);
      const BDSBeamlineElement* pstEl = nullptr;
      const BDSBeamlineElement* nxtEl = nullptr;
      PreviousAndNext(element, beamline->begin(), beamline->end(), pstEl, nxtEl);
      G4double crRadius = std::min({CurvilinearRadius(pstEl),
                                    CurvilinearRadius(*element),
                                    CurvilinearRadius(nxtEl)});
      BDSBeamlineElement* bridgeSection = CreateBridgeSection(defaultBridge, element, nxt,
							      beamline->end(), numberOfUniqueComponents,
							      beamlineIndex, crRadius);
      if (bridgeSection)
	{
	  result->AddBeamlineElement(bridgeSection);
	  beamlineIndex++;
	}
    }
  return result; 
}

G4double BDSCurvilinearBuilder::CurvilinearRadius(const BDSBeamlineElement* el) const
{
  if (!el)
    {return curvilinearRadius;} // default in case there's no element
  if (BDS::IsFinite(el->GetAngle()))
    {
      G4double radiusFromAngleLength = std::abs(el->GetArcLength()/el->GetAngle()); // s = r*theta -> r = s/theta
      radiusFromAngleLength *= radiusTolerance;
      return std::min(radiusFromAngleLength, curvilinearRadius);
    }
  else // no finite bending angle
    {return curvilinearRadius;}
}

BDSBeamlineElement* BDSCurvilinearBuilder::CreateCurvilinearElement(const G4String&             elementName,
								    BDSBeamline::const_iterator startElement,
								    BDSBeamline::const_iterator finishElement,
								    G4int    index,
								    G4double crRadius)
{ 
  BDSSimpleComponent* component = nullptr;

  // we'll take the tilt from the first element - they should only ever be the same when used here
  G4bool tilted = BDS::IsFinite((*startElement)->GetTilt());
  // variables to be defined to create component
  G4double chordLength=0.0, angle=0.0, arcLength=0.0;
  
  if (startElement == finishElement)
    {// build 1:1
      chordLength = (*startElement)->GetChordLength() + paddingLength;
      angle       = (*startElement)->GetAngle();
      if (Angled(*startElement))
	{
	  // Not strictly accurate to add on paddingLength to arcLength, but close for now.
	  arcLength = (*startElement)->GetArcLength() + paddingLength;
	}
    }
  else
    {// cover a few components
      G4ThreeVector positionStart = (*startElement)->GetReferencePositionStart();
      G4ThreeVector positionEnd   = (*finishElement)->GetReferencePositionEnd();
      chordLength                 = (positionEnd - positionStart).mag() + paddingLength;
      
      G4double accumulatedAngle = 0;
      for (auto it = startElement; it < finishElement; it++)
	{accumulatedAngle += (*it)->GetAngle();}
      
      angle = accumulatedAngle;
      if (BDS::IsFinite(angle))
	{
	  G4double meanBendingRadius = 0.5 * chordLength / std::sin(0.5*std::abs(angle));
	  arcLength                  = meanBendingRadius * std::abs(angle);
	}
    }

  if (!BDS::IsFinite(angle))
    {// straight
      component = factory->CreateCurvilinearVolume(elementName,
						   chordLength,
						   crRadius);
    }
  else
    {// angled - tilt matters
      BDSTiltOffset* to = nullptr;
      if (tilted)
	{to = (*startElement)->GetTiltOffset();}
      
      component = factory->CreateCurvilinearVolume(elementName,
						   arcLength,
						   chordLength,
						   crRadius,
						   angle,
						   to);
    }

  BDSAcceleratorComponentRegistry::Instance()->RegisterCurvilinearComponent(component);

  return CreateElementFromComponent(component, startElement, finishElement, index);
}

BDSBeamlineElement* BDSCurvilinearBuilder::CreateBridgeSection(BDSAcceleratorComponent*    defaultBridge,
							       BDSBeamline::const_iterator element,
							       BDSBeamline::const_iterator nextElement,
							       BDSBeamline::const_iterator end,
							       G4int&                      numberOfUniqueComponents,
							       const G4int                 beamlineIndex,
							       G4double                    crRadius)
{
  // we can safely assume faces match between two beam line elements so if one's angled, so is the other
  BDSAcceleratorComponent* component = defaultBridge;
  if ((*element)->AngledOutputFace()) // angled faces - make one to match to cover the angled gap
    {component = CreateAngledBridgeComponent(element, numberOfUniqueComponents, crRadius);}
  else if (BDS::IsFinite((*element)->GetAngle()))
    {// width may be reduced due to bend - check if required
      G4double width = (*element)->GetAcceleratorComponent()->GetExtent().DX();
      width = std::min(width, crRadius);
      component = CreateStraightBridgeComponent(width, numberOfUniqueComponents);
    }

  return CreateBridgeElementFromComponent(component, element, nextElement, end, beamlineIndex);
}


BDSAcceleratorComponent* BDSCurvilinearBuilder::CreateDefaultBridgeComponent()
{
  // we're ignoring any possible angled face of the curvilinear geometry
  BDSSimpleComponent* component = factory->CreateCurvilinearVolume("clb_flat_face",
								   defaultBridgeLength,
								   curvilinearRadius);

  BDSAcceleratorComponentRegistry::Instance()->RegisterCurvilinearComponent(component);
  
  return component;
}

BDSAcceleratorComponent* BDSCurvilinearBuilder::CreateStraightBridgeComponent(G4double width,
									      G4int&   numberOfUniqueComponents)
{
  BDSSimpleComponent* component = factory->CreateCurvilinearVolume("clb_" + std::to_string(numberOfUniqueComponents),
								   defaultBridgeLength,
								   width);
  BDSAcceleratorComponentRegistry::Instance()->RegisterCurvilinearComponent(component);
  numberOfUniqueComponents++;
  return component;
}

BDSAcceleratorComponent* BDSCurvilinearBuilder::CreateAngledBridgeComponent(BDSBeamline::const_iterator element,
									    G4int&                      numberOfUniqueComponents,
									    G4double                    suggestedRadius)
{
  G4ThreeVector outputFaceNormal = (*element)->OutputFaceNormal(); // outgoing face normal

  G4ThreeVector iFNormal = outputFaceNormal;
  iFNormal *= -1;
  G4ThreeVector oFNormal = outputFaceNormal; // we assume no angle for the bridge component so this is right.

  G4double width = (*element)->GetAcceleratorComponent()->GetExtent().DX();
  width = std::min(width, suggestedRadius);
  // we're ignoring any possible angled face of the curvilinear geometry
  BDSSimpleComponent* component = factory->CreateCurvilinearVolume("clb_" + std::to_string(numberOfUniqueComponents),
								   defaultBridgeLength,
								   defaultBridgeLength,
								   width*0.5,
								   0, /*angle*/
								   iFNormal,
								   oFNormal);

  BDSAcceleratorComponentRegistry::Instance()->RegisterCurvilinearComponent(component);
  numberOfUniqueComponents++;
  return component;
}

BDSBeamlineElement* BDSCurvilinearBuilder::CreateBridgeElementFromComponent(BDSAcceleratorComponent*    component,
									    BDSBeamline::const_iterator element,
									    BDSBeamline::const_iterator nextElement,
									    BDSBeamline::const_iterator end,
									    const G4int                 beamlineIndex)
{
  G4bool last = nextElement == end;
  BDSBeamlineElement* pel = (*element);     // convenience

  BDSTiltOffset* copyTiltOffset = nullptr;
  BDSTiltOffset* existingTiltOffset = pel->GetTiltOffset();
  if (existingTiltOffset)
    {copyTiltOffset = new BDSTiltOffset(*existingTiltOffset);}

  const G4RotationMatrix* refRotEnd = pel->GetReferenceRotationEnd();

  G4ThreeVector unitZ = G4ThreeVector(0,0,1);
  unitZ = unitZ.transform(*refRotEnd);
  
  G4ThreeVector startPosition = pel->GetReferencePositionEnd();
  G4ThreeVector endPosition;
  if (!last)
    {endPosition = (*nextElement)->GetReferencePositionStart();}
  else
    {endPosition = startPosition + ( (component->GetChordLength()) * unitZ );}
  G4ThreeVector midPosition   = (startPosition + endPosition) * 0.5;

  G4double startS = pel->GetSPositionEnd();
  G4double endS;
  if (!last)
    {endS = pel->GetSPositionStart();}
  else
    {endS = startS + component->GetChordLength();}
  G4double midS = (startS + endS) * 0.5;

  BDSBeamlineElement* result = new BDSBeamlineElement(component,
						      startPosition, midPosition, endPosition,
						      new G4RotationMatrix(*refRotEnd),
						      new G4RotationMatrix(*refRotEnd),
						      new G4RotationMatrix(*refRotEnd),
						      startPosition, midPosition, endPosition,
						      new G4RotationMatrix(*refRotEnd),
						      new G4RotationMatrix(*refRotEnd),
						      new G4RotationMatrix(*refRotEnd),
						      startS, midS, endS,
                  0, 0, 0,
						      copyTiltOffset,
						      nullptr, // sampler info
						      beamlineIndex);
  return result;
}

BDSBeamlineElement* BDSCurvilinearBuilder::CreateElementFromComponent(BDSSimpleComponent*         component,
								      BDSBeamline::const_iterator startElement,
								      BDSBeamline::const_iterator finishElement,
								      G4int                       index)
{
  BDSTiltOffset* copyTiltOffset = nullptr;
  BDSTiltOffset* existingTiltOffset = (*startElement)->GetTiltOffset();
  if (existingTiltOffset)
    {copyTiltOffset = new BDSTiltOffset(*existingTiltOffset);}

  BDSBeamlineElement* result = nullptr;
  
  if (startElement == finishElement)
    {// 1:1
      BDSBeamlineElement* element = *startElement; // convenience
      
      result = new BDSBeamlineElement(component,
				      element->GetReferencePositionStart(),
				      element->GetReferencePositionMiddle(),
				      element->GetReferencePositionEnd(),
				      new G4RotationMatrix(*(element->GetRotationStart())),
				      new G4RotationMatrix(*(element->GetRotationMiddle())),
				      new G4RotationMatrix(*(element->GetRotationEnd())),
				      element->GetReferencePositionStart(),
				      element->GetReferencePositionMiddle(),
				      element->GetReferencePositionEnd(),
				      new G4RotationMatrix(*(element->GetReferenceRotationStart())),
				      new G4RotationMatrix(*(element->GetReferenceRotationMiddle())),
				      new G4RotationMatrix(*(element->GetReferenceRotationEnd())),
				      element->GetSPositionStart(),
				      element->GetSPositionMiddle(),
				      element->GetSPositionEnd(),
              element->GetSynchronousTMiddle(),
              element->GetStartMomentum(),
              element->GetStartKineticEnergy(),
				      copyTiltOffset,
				      nullptr, // sampler info
				      index);
    }
  else
    {//must cover a few components
      G4double      sStart      = (*startElement)->GetSPositionStart();
      G4double      sEnd        = (*finishElement)->GetSPositionEnd();
      G4double      sMid        = 0.5 * (sEnd + sStart);
      G4ThreeVector posRefStart = (*startElement)->GetReferencePositionStart();
      G4ThreeVector posRefEnd   = (*finishElement)->GetReferencePositionEnd();
      G4ThreeVector posRefMid   = 0.5 * (posRefStart + posRefEnd);
      G4RotationMatrix* rotRefStart = (*startElement)->GetReferenceRotationStart();
      G4RotationMatrix* rotRefEnd   = (*finishElement)->GetReferenceRotationEnd();

      G4ThreeVector delta = posRefMid - posRefStart;
      G4ThreeVector newUnitZ      = delta.unit();
      G4ThreeVector unitXPrevious = G4ThreeVector(1,0,0).transform(*rotRefStart);
      G4ThreeVector newUnitY      = newUnitZ.cross(unitXPrevious).unit();
      G4ThreeVector unitYPrevious = G4ThreeVector(0,1,0).transform(*rotRefStart);
      G4ThreeVector newUnitX      = unitYPrevious.cross(newUnitZ).unit();

      // create mid point rotation matrix from unit vectors at mid point
      G4RotationMatrix rotRefMid = G4RotationMatrix(newUnitX, newUnitY, newUnitZ);

      result = new BDSBeamlineElement(component,
				      posRefStart,
				      posRefMid,
				      posRefEnd,
				      new G4RotationMatrix(*rotRefStart),
				      new G4RotationMatrix(rotRefMid),
				      new G4RotationMatrix(*rotRefEnd),
				      posRefStart,
				      posRefMid,
				      posRefEnd,
				      new G4RotationMatrix(*rotRefStart),
				      new G4RotationMatrix(rotRefMid),
				      new G4RotationMatrix(*rotRefEnd),
				      sStart,
				      sMid,
				      sEnd,
              0, 0, 0,
				      copyTiltOffset,
				      nullptr, // sampler info
				      index);
      
    }
  
  return result;
}


BDSBeamlineElement* BDSCurvilinearBuilder::CreateBonusSectionStart(BDSBeamline const* const beamline)
{  
  // we're ignoring any possible angled face of the curvilinear geometry
  BDSSimpleComponent* component = factory->CreateCurvilinearVolume("cl_start",
								   bonusChordLength,
								   curvilinearRadius);

  BDSAcceleratorComponentRegistry::Instance()->RegisterCurvilinearComponent(component);

  const BDSBeamlineElement* firstElement = beamline->GetFirstItem();
  const G4RotationMatrix*   rotStart     = firstElement->GetReferenceRotationStart();

  G4ThreeVector unitDir = G4ThreeVector(0,0,1).transform(*rotStart);
  G4ThreeVector endPos  = firstElement->GetReferencePositionStart() - unitDir*0.5*paddingLength;
  G4ThreeVector midPos  = endPos - unitDir*0.5*bonusChordLength;
  G4ThreeVector staPos  = endPos - unitDir*bonusChordLength;

  G4double sStart = firstElement->GetSPositionStart();

  BDSBeamlineElement*result = new BDSBeamlineElement(component,
						     staPos,
						     midPos,
						     endPos,
						     new G4RotationMatrix(*rotStart),
						     new G4RotationMatrix(*rotStart),
						     new G4RotationMatrix(*rotStart),
						     staPos,
						     midPos,
						     endPos,
						     new G4RotationMatrix(*rotStart),
						     new G4RotationMatrix(*rotStart),
						     new G4RotationMatrix(*rotStart),
						     sStart - bonusChordLength,
						     sStart - 0.5*bonusChordLength,
						     sStart,
                 0, 0, 0,
						     nullptr,
						     nullptr, // sampler info
						     -1); // artificial index of -1 for before beam line
  return result;
}

BDSBeamlineElement* BDSCurvilinearBuilder::CreateBonusSectionEnd(BDSBeamline const* const beamline)
{
  // we're ignoring any possible angled face of the curvilinear geometry
  BDSSimpleComponent* component = factory->CreateCurvilinearVolume("cl_end",
								   bonusChordLength,
								   curvilinearRadius);

  BDSAcceleratorComponentRegistry::Instance()->RegisterCurvilinearComponent(component);

  const BDSBeamlineElement* lastElement = beamline->GetLastItem();
  const G4RotationMatrix*   rotEnd      = lastElement->GetReferenceRotationEnd();
  const G4int lastIndex = lastElement->GetIndex();

  G4ThreeVector unitDir = G4ThreeVector(0,0,1).transform(*rotEnd);
  G4ThreeVector staPos  = lastElement->GetReferencePositionEnd() + unitDir*0.5*paddingLength;
  G4ThreeVector midPos  = staPos + unitDir*0.5*bonusChordLength;
  G4ThreeVector endPos  = staPos + unitDir*bonusChordLength;

  G4double sStart = lastElement->GetSPositionEnd();

  BDSBeamlineElement*result = new BDSBeamlineElement(component,
						     staPos,
						     midPos,
						     endPos,
						     new G4RotationMatrix(*rotEnd),
						     new G4RotationMatrix(*rotEnd),
						     new G4RotationMatrix(*rotEnd),
						     staPos,
						     midPos,
						     endPos,
						     new G4RotationMatrix(*rotEnd),
						     new G4RotationMatrix(*rotEnd),
						     new G4RotationMatrix(*rotEnd),
						     sStart,
						     sStart + 0.5*bonusChordLength,
						     sStart + bonusChordLength,
                 0, 0, 0,
						     nullptr,
						     nullptr, // sampler info
						     lastIndex + 1); // artificial index of -1 for before beam line
  return result;
}
