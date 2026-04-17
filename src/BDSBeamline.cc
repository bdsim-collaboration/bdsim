/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2024.

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
#include "BDSDebug.hh"
#include "BDSAcceleratorComponent.hh"
#include "BDSBeamline.hh"
#include "BDSBeamlineElement.hh"
#include "BDSBeamlineIntegral.hh"
#include "BDSException.hh"
#include "BDSExtentGlobal.hh"
#include "BDSGlobalConstants.hh"
#include "BDSLine.hh"
#include "BDSOutput.hh"
#include "BDSSamplerPlane.hh"
#include "BDSSimpleComponent.hh"
#include "BDSTiltOffset.hh"
#include "BDSTransform3D.hh"
#include "BDSUtilities.hh"
#include "BDSWarning.hh"

#include "globals.hh" // geant4 globals / types
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"

#include "CLHEP/Vector/AxisAngle.h"

#include <algorithm>
#include <iterator>
#include <ostream>
#include <set>
#include <vector>

G4double BDSBeamline::paddingLength = -1;

BDSBeamline::BDSBeamline(const G4ThreeVector& initialGlobalPosition,
                         G4RotationMatrix*    initialGlobalRotation,
                         G4double             initialSIn):
  sInitial(initialSIn),
  sMaximum(initialSIn),
  totalChordLength(0),
  totalArcLength(0),
  totalAngle(0),
  previousReferencePositionEnd(initialGlobalPosition),
  previousSPositionEnd(sInitial),
  transformHasJustBeenApplied(false)
{
  // initialise extents
  maximumExtentPositive = G4ThreeVector(0,0,0);
  maximumExtentNegative = G4ThreeVector(0,0,0);
  
  // initial rotation matrix
  if (initialGlobalRotation) // default is null
    {previousReferenceRotationEnd = initialGlobalRotation;}
  else
    {previousReferenceRotationEnd = new G4RotationMatrix();}

  // gap between each element added to the beam line
  if (paddingLength <= 0)
    {paddingLength = 3 * BDSGlobalConstants::Instance()->LengthSafety();}
  //paddingLength = 3*CLHEP::mm;
  
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "with initial position and rotation" << G4endl;
  G4cout << "Initial position: " << initialGlobalPosition << G4endl;
  G4cout << "Initial rotation: " << *previousReferenceRotationEnd << G4endl;
#endif  
}

BDSBeamline::BDSBeamline(G4Transform3D initialTransform,
                         G4double      initialSIn):
  BDSBeamline(initialTransform.getTranslation(),
              new G4RotationMatrix(initialTransform.getRotation()),
              initialSIn)
{;}

BDSBeamline::~BDSBeamline()
{
  for (auto it : *this)
    {delete it;}
  // special case, if empty then previousReferenceRotationEnd is not used in the first element
  if (size()==0)
    {delete previousReferenceRotationEnd;}
  // components map goes out of scope - elements are already deleted - no need to
  // explicitly delete
}

void BDSBeamline::PrintAllComponents(std::ostream& out) const
{
  for (const auto& element : *this)
    {out << element;}
}

void BDSBeamline::PrintMemoryConsumption() const
{
  G4cout << __METHOD_NAME__ << "container size: " << sizeof(beamline) << G4endl;
  G4cout << __METHOD_NAME__ << "beamline element cumulative size: " << sizeof(BDSBeamlineElement) * beamline.size() << G4endl;
  G4cout << __METHOD_NAME__ << "full usage including components:  " << (sizeof(BDSBeamlineElement) + sizeof(BDSAcceleratorComponent)) * beamline.size() << G4endl;
}

std::ostream& operator<< (std::ostream& out, BDSBeamline const &bl)
{
  out << "Beamline with "       << bl.size()           << " elements" << G4endl;
  out << "Total arc length:   " << bl.totalArcLength   << " mm"       << G4endl;
  out << "Total chord length: " << bl.totalChordLength << " mm"       << G4endl;

  return out;
}

void BDSBeamline::AddComponent(BDSAcceleratorComponent* component,
                               BDSTiltOffset*           tiltOffset,
                               BDSSamplerInfo*          samplerInfo,
                               const BDSBeamlineIntegral* beamlineIntegral)
{
  if (!component)
    {
      G4String samplerName = samplerInfo ? samplerInfo->name : "no_sampler_name_given";
      throw BDSException(__METHOD_NAME__, "invalid accelerator component " + samplerName);
    }

  // check the sampler name is allowed in the output
  if (samplerInfo)
    {
      G4String samplerName = samplerInfo->name;
      if (BDSOutput::InvalidSamplerName(samplerName))
        {
          G4cerr << __METHOD_NAME__ << "invalid sampler name \"" << samplerName << "\"" << G4endl;
          BDSOutput::PrintProtectedNames(G4cerr);
          throw BDSException(__METHOD_NAME__, "");
        }
    }
  
  if (BDSLine* line = dynamic_cast<BDSLine*>(component))
    {
      // in the case a single component has become a line, when we have a cylindrical
      // sample we should flag that it should cover the full 'line', however with a plane
      // one it is only attached to the last element.
      BDSBeamlineElement* first = nullptr;
      BDSBeamlineElement* last  = nullptr;
      G4int sizeLine = (G4int)line->size();
      for (G4int i = 0; i < sizeLine; ++i)
        {
          if (i < sizeLine-1)
            {
              AddSingleComponent((*line)[i], tiltOffset, nullptr, beamlineIntegral);
        if (i == 0)
          {first = back();}
            }
          else 
            {// only attach the desired sampler to the last one in the line
              AddSingleComponent((*line)[i], tiltOffset, samplerInfo, beamlineIntegral);
              last = back();
            }
        }
      if (samplerInfo) // could be nullptr
        {
          if (samplerInfo->samplerType == BDSSamplerType::cylinder) // only a cylinder or plane can be attached to an element
            {// cache the range it should cover as a cylinder
              last->GetSamplerInfo()->startElement = first;
              last->GetSamplerInfo()->finishElement = last;
              // calculate the mid (i.e. mean) position and rotation
              G4ThreeVector midRefPosition = (last->GetReferencePositionEnd() + first->GetReferencePositionStart()) / 2.0;
              G4ThreeVector aaMidAxis;
              G4double aaMidAngle;
              auto aaStart = first->GetReferenceRotationStart()->axisAngle();
              auto aaFinish = last->GetReferenceRotationEnd()->axisAngle();
              // careful of identity rotations in AA form (axis=(0,0,1),angle=0) as our average of these would be wrong
              if (first->GetReferenceRotationStart()->isIdentity())
                {
                  aaMidAxis = aaFinish.axis();
                  aaMidAngle = 0.5 * aaFinish.delta();
                }
              else if (last->GetReferenceRotationEnd()->isIdentity())
                {
                  aaMidAxis = aaStart.axis();
                  aaMidAngle = 0.5 * aaStart.delta();
                }
              else
                {
                  aaMidAxis = (aaFinish.axis() + aaStart.axis()) / 2.0;
                  aaMidAngle = (aaFinish.delta() + aaStart.delta()) / 2.0;
                }
              auto aaCSampler = CLHEP::HepAxisAngle(aaMidAxis, aaMidAngle);
              G4RotationMatrix rmCSampler = G4RotationMatrix(aaCSampler);
              G4Transform3D trCSampler(rmCSampler, midRefPosition);
              last->UpdateSamplerPlacementTransform(trCSampler);
            }
        }
    }
  else
    {AddSingleComponent(component, tiltOffset, samplerInfo, beamlineIntegral);}
  // free memory - as once the rotations are calculated, this is no longer needed
  delete tiltOffset;
}

void BDSBeamline::AddSingleComponent(BDSAcceleratorComponent* component,
                                     BDSTiltOffset*           tiltOffset,
                                     BDSSamplerInfo*          samplerInfo,
                                     const BDSBeamlineIntegral* beamlineIntegral)
{
#ifdef BDSDEBUG
  G4cout << G4endl << __METHOD_NAME__ << "adding component to beamline and calculating coordinates" << G4endl;
  G4cout << "component name:      " << component->GetName() << G4endl;
#endif
  // Test if it's a BDSTransform3D instance - this is a unique component that requires
  // rotation in all dimensions and can skip normal addition as isn't a real volume
  // that can be placed.  Apply the transform and skip the rest of this function by returning
  // This modifies the "end" coordinates, rotation and axes of the last element in the beamline
  if (BDSTransform3D* transform = dynamic_cast<BDSTransform3D*>(component))
    {
      ApplyTransform3D(transform);
      transformHasJustBeenApplied = true;
      return;
    }

  // if it's not a transform3d instance, continue as normal
  // interrogate the item
  G4double chordLength   = component->GetChordLength();
  G4double      angle    = component->GetAngle();
  G4bool hasFiniteLength = BDS::IsFinite(chordLength);
  G4bool hasFiniteAngle  = BDS::IsFinite(angle);
  G4bool hasFiniteTilt, hasFiniteOffset;
  G4ThreeVector offset;
  if (tiltOffset)
    {
      hasFiniteTilt   = tiltOffset->HasFiniteTilt();
      hasFiniteOffset = tiltOffset->HasFiniteOffset();
      offset          = tiltOffset->GetOffset(); // returns 3Vector
    }
  else
    {
      hasFiniteTilt   = false;
      hasFiniteOffset = false;
      offset          = G4ThreeVector();
    }
  G4ThreeVector eP       = component->GetExtentPositive() + offset;
  G4ThreeVector eN       = component->GetExtentNegative() + offset;
  G4ThreeVector placementOffset   = component->GetPlacementOffset();
  G4bool hasFinitePlacementOffset = BDS::IsFinite(placementOffset);
  G4ThreeVector oFNormal = component->InputFaceNormal();
  
#ifdef BDSDEBUG
  G4cout << "chord length                " << chordLength << " mm"         << G4endl;
  G4cout << "angle                       " << angle       << " rad"        << G4endl;
  if (tiltOffset)
    {G4cout << "tilt offsetX offsetY        " << *tiltOffset << " rad mm mm " << G4endl;}
  else
    {G4cout << "no tilt offset" << G4endl;}
  G4cout << "has finite length           " << hasFiniteLength              << G4endl;
  G4cout << "has finite angle            " << hasFiniteAngle               << G4endl;
  G4cout << "has finite tilt             " << hasFiniteTilt                << G4endl;
  G4cout << "has finite offset           " << hasFiniteOffset              << G4endl;
  G4cout << "extent positive             " << eP                           << G4endl;
  G4cout << "extent negative             " << eN                           << G4endl;
  G4cout << "object placement offset     " << placementOffset              << G4endl;
  G4cout << "has finite placement offset " << hasFinitePlacementOffset     << G4endl;
  G4cout << "output face normal          " << oFNormal                     << G4endl;
#endif

  // Check this won't overlap with any previous geometry. This is only done for elements
  // that aren't drifts as they should be built by the component factory to match any angles.
  if (!empty() && (component->GetType() != "drift") && (component->GetType() != "thinmultipole"))
    {// can only look back if there is an element - won't clash if no element; also add drifts always
      G4bool   keepGoing   = true;
      G4bool   checkFaces  = true;
      G4double zSeparation = 0;
      const BDSBeamlineElement* inspectedElement = back(); // remember we haven't added this new element yet
      // find previous non drift output face.
      G4ThreeVector iFNormal;
      G4String clasherName = "Unknown";
      while (keepGoing)
        {
          if (inspectedElement) // valid element
            {// decrement could return nullptr so have to check if valid element
              if ((inspectedElement->GetType() == "drift")||(inspectedElement->GetType() == "thinmultipole")) // leave keepGoing true
                {
                  zSeparation += inspectedElement->GetChordLength();
                  inspectedElement = GetPrevious(inspectedElement); // decrement
                }
              else
                {
                  keepGoing   = false; // found a non drift - stop here
                  iFNormal    = inspectedElement->GetAcceleratorComponent()->OutputFaceNormal();
                  clasherName = inspectedElement->GetAcceleratorComponent()->GetName();
                }
            }
          else
            {
              keepGoing  = false;
              checkFaces = false; // got to the beginning with only drifts - don't check
            }
        }
#ifdef BDSDEBUG
      G4cout << "input face normal           " << iFNormal << G4endl; // matches above debug formatting
#endif

      if (checkFaces)
        {
          // now do checks
          BDSExtent extOF = inspectedElement->GetAcceleratorComponent()->GetExtent(); // output face
          BDSExtent extIF = component->GetExtent(); // input face
          
          G4bool willIntersect = BDS::WillIntersect(iFNormal, oFNormal, zSeparation, extIF, extOF);
          if (willIntersect)
            {
              G4cout << __METHOD_NAME__ << "Error - angled faces of objects will cause overlap in beam line geometry" << G4endl;
              G4cout << "\"" << component->GetName() << "\" will overlap with \""
                     << clasherName << "\"" << G4endl;
              throw BDSException(__METHOD_NAME__, "");
            }
        }
    }
  
  // Calculate the reference placement rotation
  // rotations are done first as they're required to transform the spatial displacements.
  // if not the first element in the beamline, copy the rotation matrix (cumulative along line)
  // from end of last component, else use initial rotation matrix (no copy to prevent memory leak)
  G4RotationMatrix* referenceRotationStart;
  if (empty())
    {referenceRotationStart = previousReferenceRotationEnd;}
  else
    {
      if (!transformHasJustBeenApplied)
        {previousReferenceRotationEnd = back()->GetReferenceRotationEnd();}
      referenceRotationStart  = new G4RotationMatrix(*previousReferenceRotationEnd); // always create a new copy
    }

  G4RotationMatrix* referenceRotationMiddle = new G4RotationMatrix(*referenceRotationStart);
  G4RotationMatrix* referenceRotationEnd    = new G4RotationMatrix(*referenceRotationStart);
  
  // if the component induces an angle in the reference trajectory, rotate the mid and end point
  // rotation matrices appropriately
  if (hasFiniteAngle)
    {
      // remember our definition of angle - +ve angle bends in -ve x direction in right
      // handed coordinate system
      // rotate about cumulative local y axis of beamline
      // middle rotated by half angle in local x,z plane
      G4ThreeVector rotationAxisOfBend = G4ThreeVector(0,1,0); // nominally about local unit Y
      G4ThreeVector rotationAxisOfBendEnd = rotationAxisOfBend; // a copy
      if (hasFiniteTilt)
        {
          G4double tilt = tiltOffset->GetTilt();
          G4RotationMatrix rotationAxisRM = G4RotationMatrix();
          rotationAxisRM.rotateZ(tilt);
          rotationAxisOfBend.transform(rotationAxisRM);
          rotationAxisOfBendEnd.transform(rotationAxisRM);
        }
      referenceRotationMiddle->rotate(angle*0.5, rotationAxisOfBend.transform(*previousReferenceRotationEnd));
      // end rotated by full angle in local x,z plane
      referenceRotationEnd->rotate(angle, rotationAxisOfBendEnd.transform(*previousReferenceRotationEnd));
    }
  
  G4RotationMatrix* rotationStart  = new G4RotationMatrix(*referenceRotationStart);
  G4RotationMatrix* rotationMiddle = new G4RotationMatrix(*referenceRotationMiddle);
  G4RotationMatrix* rotationEnd    = new G4RotationMatrix(*referenceRotationEnd);
  // add the tilt to the rotation matrices (around z axis)
  if (hasFiniteTilt)
    {
      G4double tilt = tiltOffset->GetTilt();

      // transform a unit z vector with the rotation matrices to get the local axes
      // of rotation to apply the tilt.
      G4ThreeVector unitZ = G4ThreeVector(0,0,1);
      rotationStart ->rotate(tilt, unitZ.transform(*referenceRotationStart));
      unitZ = G4ThreeVector(0,0,1);
      rotationMiddle->rotate(tilt, unitZ.transform(*referenceRotationMiddle));
      unitZ = G4ThreeVector(0,0,1);
      rotationEnd   ->rotate(tilt, unitZ.transform(*referenceRotationEnd));
    }
  
  // calculate the reference placement position
  // if not the first item in the beamline, get the reference trajectory global position
  // at the end of the previous element
  if (!empty())
    {
      // if a transform has been applied, the previousReferencePositionEnd variable is already calculated
      if (!transformHasJustBeenApplied)
	{previousReferencePositionEnd = back()->GetReferencePositionEnd();}
      // leave a small gap for unambiguous geometry navigation. Transform that length
      // to a unit z vector along the direction of the beam line before this component.
      // increase it by sampler length if we're placing a sampler there.
      G4ThreeVector pad = G4ThreeVector(0,0,paddingLength);
      if (samplerInfo)
      {
        BDSSamplerType samplerType = samplerInfo->samplerType;
        if (samplerType != BDSSamplerType::none)
	{pad += G4ThreeVector(0,0,BDSSamplerPlane::ChordLength());}
      }

      // even if a transform has been applied that might induce a rotation, we introduce
      // the padding length along the outgoing vector of the previous component to ensure
      // the padding length is respected - hence we get the rotation from back() and not
      // from the previousReferenceRotationEnd member variable
      auto previousReferenceRotationEnd2 = back()->GetReferenceRotationEnd();
      G4ThreeVector componentGap = pad.transform(*previousReferenceRotationEnd2);
      previousReferencePositionEnd += componentGap;
    }
  
  G4ThreeVector referencePositionStart, referencePositionMiddle, referencePositionEnd;
  if (hasFiniteLength)
    {
      referencePositionStart = previousReferencePositionEnd;
      
      // calculate delta to mid point
      G4ThreeVector md = G4ThreeVector(0, 0, 0.5 * chordLength);
      md.transform(*referenceRotationMiddle);
      referencePositionMiddle = referencePositionStart + md;
      // remember the end position is the chord length along the half angle, not the full angle
      // the particle achieves the full angle though by the end position.
      G4ThreeVector delta = G4ThreeVector(0, 0, chordLength).transform(*referenceRotationMiddle);
      referencePositionEnd = referencePositionStart + delta;
    }
  else
    {
      // element has no finite size so all positions are previous end position
      // likely this is a transform3d or similar - but not hard coded just for transform3d
      referencePositionStart  = previousReferencePositionEnd;
      referencePositionMiddle = previousReferencePositionEnd;
      referencePositionEnd    = previousReferencePositionEnd;
    }
  
  // add the placement offset
  G4ThreeVector positionStart, positionMiddle, positionEnd;
  if (hasFiniteOffset || hasFinitePlacementOffset)
    {
      if (hasFiniteOffset && hasFiniteAngle) 
        {// do not allow x offsets for bends as this will cause overlaps
          G4String name = component->GetName();
          G4String message = "element has x offset, but this will cause geometry overlaps: " + name
            + " - omitting x offset";
          BDS::Warning(__METHOD_NAME__, message);
          offset.setX(0.0);
        }
      // note the displacement is applied in the accelerator x and y frame so use
      // the reference rotation rather than the one with tilt already applied
      G4ThreeVector total = offset + placementOffset;
      G4ThreeVector displacement   = total.transform(*referenceRotationMiddle);
      positionStart  = referencePositionStart  + displacement;
      positionMiddle = referencePositionMiddle + displacement;
      positionEnd    = referencePositionEnd    + displacement;
    }
  else
    {
      positionStart  = referencePositionStart;
      positionMiddle = referencePositionMiddle;
      positionEnd    = referencePositionEnd;
    }
  
  // calculate the s position
  // if not the first element in the beamline, get the s position at the end of the previous element
  if (!empty())
    {previousSPositionEnd = back()->GetSPositionEnd();}

  // chord length set earlier
  G4double arcLength   = component->GetArcLength();

  // integrate lengths
  totalChordLength += chordLength;
  totalArcLength   += arcLength;

  // advance s coordinate
  G4double sPositionStart  = previousSPositionEnd;
  G4double sPositionMiddle = previousSPositionEnd + 0.5 * arcLength;
  G4double sPositionEnd    = previousSPositionEnd + arcLength;
  sMaximum += arcLength;

  // integrate angle
  totalAngle += component->GetAngle();

#ifdef BDSDEBUG
  // feedback about calculated coordinates
  G4cout << "calculated coordinates in mm and rad are " << G4endl;
  G4cout << "reference position start:  " << referencePositionStart   << G4endl;
  G4cout << "reference position middle: " << referencePositionMiddle  << G4endl;
  G4cout << "reference position end:    " << referencePositionEnd     << G4endl;
  G4cout << "reference rotation start:  " << *referenceRotationStart;
  G4cout << "reference rotation middle: " << *referenceRotationMiddle;
  G4cout << "reference rotation end:    " << *referenceRotationEnd;
  G4cout << "position start:            " << positionStart            << G4endl;
  G4cout << "position middle:           " << positionMiddle           << G4endl;
  G4cout << "position end:              " << positionEnd              << G4endl;
  G4cout << "rotation start:            " << *rotationStart;
  G4cout << "rotation middle:           " << *rotationMiddle;
  G4cout << "rotation end:              " << *rotationEnd;
#endif
  
  // construct beamline element
  BDSTiltOffset* tiltOffsetToStore = nullptr;
  if (tiltOffset)
    {tiltOffsetToStore = new BDSTiltOffset(*tiltOffset);} // copy as can be used multiple times

  G4double midT = 0;
  G4double staP = 0;
  G4double staEk = 0;
  if (beamlineIntegral)
    {
      midT = beamlineIntegral->synchronousTAtMiddleOfLastElement;
      staP = beamlineIntegral->MomentumAtStartOfLastElement();
      staEk = beamlineIntegral->KineticEnergyAtStartOfLastElement();
    }

  BDSBeamlineElement* element;
  element = new BDSBeamlineElement(component,
                                   positionStart,
                                   positionMiddle,
                                   positionEnd,
                                   rotationStart,
                                   rotationMiddle,
                                   rotationEnd,
                                   referencePositionStart,
                                   referencePositionMiddle,
                                   referencePositionEnd,
                                   referenceRotationStart,
                                   referenceRotationMiddle,
                                   referenceRotationEnd,
                                   sPositionStart,
                                   sPositionMiddle,
                                   sPositionEnd,
                                   midT,
                                   staP,
                                   staEk,
                                   tiltOffsetToStore,
                                   samplerInfo,
                                   (G4int)beamline.size());

  // calculate extents for world size determination
  UpdateExtents(element);
  
  // append it to the beam line
  beamline.push_back(element);

  // register the s position at the end for curvilinear transform
  sEnd.push_back(sPositionEnd);

  // register it by name
  RegisterElement(element);

  // reset flag for transform since we've now added a component
  transformHasJustBeenApplied = false;
}

void BDSBeamline::ApplyTransform3D(BDSTransform3D* component)
{
  G4double dx = component->dx;
  G4double dy = component->dy;
  G4double dz = component->dz;
  
  // test validity for potential overlaps
  if (dz < 0)
    {
      G4cerr << __METHOD_NAME__ << "Caution: Transform3d: " << component->GetName() << G4endl;
      G4cerr << __METHOD_NAME__ << "dz = " << dz << " < 0 -> could overlap previous element" << G4endl;
    }
  
  // if not the first element in the beamline, get information from
  // the end of the last element in the beamline
  if (!empty() && !transformHasJustBeenApplied)
    {
      BDSBeamlineElement* last = back();
      previousReferenceRotationEnd = last->GetReferenceRotationEnd();
      previousReferencePositionEnd = last->GetReferencePositionEnd();
    }
  
  // apply position
  // transform the local dx,dy,dz displacement into the global frame then apply
  G4ThreeVector delta = G4ThreeVector(dx, dy, dz).transform(*previousReferenceRotationEnd);
  previousReferencePositionEnd = previousReferencePositionEnd + delta;
  
  // apply rotation
  G4RotationMatrix trRotInverse = component->rotationMatrix.inverse();
  (*previousReferenceRotationEnd) *= trRotInverse;
}

void BDSBeamline::AddBeamlineElement(BDSBeamlineElement* element)
{
  if (!element)
    {throw BDSException(__METHOD_NAME__, "invalid BDSBeamlineElement");}
  if (!(element->GetAcceleratorComponent()))
    {throw BDSException(__METHOD_NAME__, "invalid BDSAcceleratorComponent");}
  
  // update world extent for this beam line
  UpdateExtents(element);
  
  // append it to the beam line
  beamline.push_back(element);

  // register it by name
  RegisterElement(element);

  // no need to update any internal variables - that's done by AddSingleComponent()
}

G4ThreeVector BDSBeamline::GetMaximumExtentAbsolute() const
{
  G4ThreeVector mEA;
  for (int i=0; i<3; i++)
    {mEA[i] = std::max(std::abs(maximumExtentPositive[i]), std::abs(maximumExtentNegative[i]));}
  return mEA;
}

G4Transform3D BDSBeamline::GetGlobalEuclideanTransform(G4double s, G4double x, G4double y,
                                                       G4int* indexOfFoundElement) const
{
  // check if s is in the range of the beamline
  G4double sStart = at(0)->GetSPositionStart();
  if (s-sStart > totalArcLength) // need to offset start S position
    {
      G4String msg = "s position " + std::to_string(s/CLHEP::m) + " m is beyond length of accelerator (";
      msg += std::to_string(totalArcLength/CLHEP::m) + " m)\nReturning identify transform";
      BDS::Warning(__METHOD_NAME__, msg);
      return G4Transform3D();
    }

  const auto element = GetElementFromGlobalS(s, indexOfFoundElement);

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
  G4cout << "S position requested: " << s     << G4endl;
  G4cout << "Index:                " << indexOfFoundElement << G4endl;
  G4cout << "Element: " << *element << G4endl;
#endif

  G4double dx = 0;
  // G4double dy = 0; // currently magnets can only bend in local x so avoid extra calculation

  // difference from centre of element to point in local coords)
  // difference in s from centre, normalised to arcLength and scaled to chordLength
  // as s is really arc length, but we must place effectively in chord length coordinates
  const BDSAcceleratorComponent* component = element->GetAcceleratorComponent();
  G4double arcLength   = component->GetArcLength();
  G4double chordLength = component->GetChordLength();
  G4double dS          = s - element->GetSPositionMiddle();
  G4double localZ      = dS * (chordLength / arcLength);
  G4double angle       = component->GetAngle();
  G4RotationMatrix rotation; // will be interpolated rotation
  G4RotationMatrix* rotMiddle = element->GetReferenceRotationMiddle();
  // find offset of point from centre of volume - 2 methods
  if (BDS::IsFinite(angle))
    {
      // finite bend angle - interpolate position and angle along arc due to change in angle
      // local unit z at start of element
      G4ThreeVector localUnitY = G4ThreeVector(0,1,0);
      localUnitY.transform(*(element->GetReferenceRotationStart()));
      // linearly interpolate angle -> angle * (s from beginning into component)/arcLength
      G4double partialAngle = angle * std::fabs(( (0.5*arcLength + dS) / arcLength));
      rotation = G4RotationMatrix(*(element->GetReferenceRotationStart())); // start rotation
      rotation.rotate(partialAngle, localUnitY); // rotate it by the partial angle about local Y
      dx = localZ*tan(partialAngle); // calculate difference of arc from chord at that point
    }
  else
    {rotation = G4RotationMatrix(*rotMiddle);}

  // note, magnets only bend in local x so no need to add dy as always 0
  G4ThreeVector dLocal    = G4ThreeVector(x + dx, y /*+ dy*/, localZ);
#ifdef BDSDEBUG
  G4cout << "Local offset from middle: " << dLocal << G4endl;
#endif
  // note, rotation middle is also the same as the coordinate frame of the g4 solid
  G4ThreeVector globalPos = element->GetReferencePositionMiddle() + dLocal.transform(*rotMiddle);
  // construct transform3d from global position and rotation matrix
  G4Transform3D result    = G4Transform3D(rotation, globalPos);
  
#ifdef BDSDEBUG
  G4cout << "Global offset from middle: " << dLocal    << G4endl;
  G4cout << "Resultant global position: " << globalPos << G4endl;
#endif
  return result;
}

const BDSBeamlineElement* BDSBeamline::GetElementFromGlobalS(G4double S,
                                                             G4int*   indexOfFoundElement) const
{
  // find element that s position belongs to
  auto lower = std::lower_bound(sEnd.begin(), sEnd.end(), S);
  G4int index = G4int(lower - sEnd.begin()); // subtract iterators to get index
  if (indexOfFoundElement)
    {*indexOfFoundElement = index;}
  return beamline.at(index);
}

BDSBeamline::const_iterator BDSBeamline::FindFromS(G4double S) const
{
  auto lower = std::lower_bound(sEnd.begin(), sEnd.end(), S);
  auto iter = begin();
  std::advance(iter, std::distance(sEnd.begin(), lower));
  return iter;
}

const BDSBeamlineElement* BDSBeamline::GetPrevious(const BDSBeamlineElement* element) const
{
  // search for element
  auto result = find(beamline.begin(), beamline.end(), element);
  if (result != beamline.end())
    {// found
      return GetPrevious(G4int(result - beamline.begin()));
    }
  else
    {return nullptr;}
}

const BDSBeamlineElement* BDSBeamline::GetPrevious(G4int index) const
{
  if (index < 1 || index > (G4int)(beamline.size()-1))
    {return nullptr;} // invalid index - inc beginning or end
  else
    {return beamline[index-1];}
}

const BDSBeamlineElement* BDSBeamline::GetNext(const BDSBeamlineElement* element) const
{
  // search for element
  auto result = find(beamline.begin(), beamline.end(), element);
  if (result != beamline.end())
    {// found
      return GetNext(G4int(result - beamline.begin()));
    }
  else
    {return nullptr;}
}

const BDSBeamlineElement* BDSBeamline::GetNext(G4int index) const
{
  if (index < 0 || index > (G4int)(beamline.size()-2))
    {return nullptr;} // invalid index - inc beginning or end
  else
    {return beamline[index+1];}
}

void BDSBeamline::RegisterElement(BDSBeamlineElement* element)
{
  // check if base name already registered (can be single component placed multiple times)
  const auto search = components.find(element->GetName());
  if (search == components.end())
    {// not registered
      components[element->GetPlacementName()] = element;
    }
}

const BDSBeamlineElement* BDSBeamline::GetElement(G4String acceleratorComponentName,
                                                  G4int    i) const
{
  // build placement name based on acc component name and ith placement
  // matches construction in BDSBeamlineElement
  G4String suffix        = "_" + std::to_string(i);
  G4String placementName = acceleratorComponentName + suffix;
  const auto search = components.find(placementName);
  if (search == components.end())
    {
      // Try again but search including naming convention of uniquely built
      // components. Sometimes we modify an element or build it uniquely for
      // that position in the lattice, so we therefore add a suffix to the
      // name for storing in the component registry.
      // Naming will be NAME_MOD_MODNUMBER_PLACEMENTNUMBER
      // Why not search registry? -> should be found from this beam line
      // 1) search with starts with NAME
      std::vector<const BDSBeamlineElement*> candidates;
      std::for_each(this->begin(),
                    this->end(),
                    [&acceleratorComponentName,&candidates](const BDSBeamlineElement* el)
                    {if (BDS::StartsWith(el->GetPlacementName(), acceleratorComponentName)){candidates.push_back(el);};});
      
      if (candidates.empty())
        {return nullptr;} // nothing found
      else
        {// 2) of things that start with NAME, search for ones that end in _PLACEMENTNUMBER
          auto foundItem = std::find_if(candidates.begin(),
                                        candidates.end(),
                                        [&suffix](const BDSBeamlineElement* el)
                                        {return BDS::EndsWith(el->GetPlacementName(), suffix);});
          return foundItem != candidates.end() ? *foundItem : nullptr;
        }
    }
  else
    {return search->second;}
}

G4Transform3D BDSBeamline::GetTransformForElement(const G4String& acceleratorComponentName,
                                                  G4int    i) const
{
  const BDSBeamlineElement* result = GetElement(acceleratorComponentName, i);
  if (!result)
    {
      G4cerr << __METHOD_NAME__ << "No element named \""
             << acceleratorComponentName << "\" found for placement number "
             << i << G4endl;
      G4cout << "Note, this may be because the element is a bend and split into " << G4endl;
      G4cout << "multiple sections with unique names." << G4endl;
      throw BDSException(__METHOD_NAME__, "");
    }
  else
    {return G4Transform3D(*(result->GetRotationMiddle()), result->GetPositionMiddle());}
}

void BDSBeamline::UpdateExtents(BDSBeamlineElement* element)
{
  // calculate extents for world size determination
  // get the boundary points in global coordinates.
  BDSExtentGlobal extG = element->GetExtentGlobal();
  const auto boundaryPoints = extG.AllBoundaryPointsGlobal();

  // expand maximums based on the boundary points.
  for (const auto& point : boundaryPoints)
    {
      for (int i = 0; i < 3; ++i)
        {
          if (point[i] > maximumExtentPositive[i])
            {maximumExtentPositive[i] = point[i];}
          if (point[i] < maximumExtentNegative[i])
            {maximumExtentNegative[i] = point[i];}
        }
    }
#ifdef BDSDEBUG
  G4cout << "new global extent +ve:         " << maximumExtentPositive << G4endl;
  G4cout << "new global extent -ve:         " << maximumExtentNegative << G4endl;
#endif
}

BDSBeamlineElement* BDSBeamline::ProvideEndPieceElementBefore(BDSSimpleComponent* endPiece,
                                                              G4int               index) const
{
  if (!IndexOK(index))
    {return nullptr;}
  
  const G4double pl = BDSGlobalConstants::Instance()->LengthSafetyLarge(); // shortcut - 'padding length'
  G4double endPieceLength      = endPiece->GetChordLength();
  BDSBeamlineElement*  element = beamline[index];
  G4RotationMatrix* elRotStart = element->GetRotationMiddle();
  G4ThreeVector     elPosStart = element->GetPositionStart() - G4ThreeVector(0,0,2*pl).transform(*elRotStart);
  G4ThreeVector positionMiddle = elPosStart - G4ThreeVector(0,0,endPieceLength*0.5).transform(*elRotStart);
  G4ThreeVector  positionStart = elPosStart - G4ThreeVector(0,0,endPieceLength).transform(*elRotStart);
  G4double         elSPosStart = element->GetSPositionStart();
  BDSTiltOffset*  elTiltOffset = element->GetTiltOffset();
  BDSTiltOffset*   forEndPiece = nullptr;
  if (elTiltOffset)
    {forEndPiece = new BDSTiltOffset(*elTiltOffset);}
  BDSBeamlineElement* result = new BDSBeamlineElement(endPiece,
                                                      positionStart,
                                                      positionMiddle,
                                                      elPosStart,
                                                      new G4RotationMatrix(*elRotStart),
                                                      new G4RotationMatrix(*elRotStart),
                                                      new G4RotationMatrix(*elRotStart),
                                                      positionStart,// for now the same - ie no tilt offset
                                                      positionMiddle,
                                                      elPosStart,
                                                      new G4RotationMatrix(*elRotStart),
                                                      new G4RotationMatrix(*elRotStart),
                                                      new G4RotationMatrix(*elRotStart),
                                                      elSPosStart - endPieceLength,
                                                      elSPosStart - 0.5*endPieceLength,
                                                      elSPosStart,
                                                      element->GetSynchronousTMiddle(),
                                                      element->GetStartMomentum(),
                                                      element->GetStartKineticEnergy(),
                                                      forEndPiece);
  return result;
}

BDSBeamlineElement* BDSBeamline::ProvideEndPieceElementAfter(BDSSimpleComponent* endPiece,
                                                             G4int               index,
                                                             G4bool              flip) const
{
  if (!IndexOK(index))
    {return nullptr;}

  const G4double pl = paddingLength; // shortcut
  G4double endPieceLength      = endPiece->GetChordLength();
  BDSBeamlineElement*  element = beamline[index];
  G4RotationMatrix*   elRotEnd = new G4RotationMatrix(*(element->GetRotationMiddle()));
  G4ThreeVector       elPosEnd = element->GetPositionEnd() + G4ThreeVector(0,0,pl).transform(*elRotEnd);
  G4ThreeVector positionMiddle = elPosEnd + G4ThreeVector(0,0,endPieceLength*0.5).transform(*elRotEnd);
  G4ThreeVector    positionEnd = elPosEnd + G4ThreeVector(0,0,endPieceLength).transform(*elRotEnd);
  if (flip)
    {// rotate about local unit Y direction
      G4ThreeVector localUnitY = G4ThreeVector(0,1,0).transform(*elRotEnd);
      elRotEnd->rotate(CLHEP::pi, localUnitY);
    }
  G4double           elSPosEnd = element->GetSPositionEnd();
  BDSTiltOffset*  elTiltOffset = element->GetTiltOffset();
  BDSTiltOffset*   forEndPiece = nullptr;
  if (elTiltOffset)
    {forEndPiece = new BDSTiltOffset(*elTiltOffset);}
  BDSBeamlineElement* result = new BDSBeamlineElement(endPiece,
                                                      elPosEnd,
                                                      positionMiddle,
                                                      positionEnd,
                                                      new G4RotationMatrix(*elRotEnd),
                                                      new G4RotationMatrix(*elRotEnd),
                                                      new G4RotationMatrix(*elRotEnd),
                                                      elPosEnd,
                                                      positionMiddle,
                                                      positionEnd,
                                                      new G4RotationMatrix(*elRotEnd),
                                                      new G4RotationMatrix(*elRotEnd),
                                                      new G4RotationMatrix(*elRotEnd),
                                                      elSPosEnd,
                                                      elSPosEnd + 0.5*endPieceLength,
                                                      elSPosEnd + endPieceLength,
                                                      element->GetSynchronousTMiddle(),
                                                      element->GetStartMomentum(),
                                                      element->GetStartKineticEnergy(),
                                                      forEndPiece);
  delete elRotEnd;
  return result;
}

G4bool BDSBeamline::IndexOK(G4int index) const
{
  if (index < 0 || index > (G4int)(beamline.size()-1))
    {return false;}
  else
    {return true;}
}

std::vector<G4double> BDSBeamline::GetEdgeSPositions()const
{
  std::vector<G4double> sPos;
  sPos.reserve(beamline.size()+1);
  // add start position
  sPos.push_back(GetSMinimum()/CLHEP::m);
  for (auto element : beamline)
    {sPos.push_back(element->GetSPositionEnd()/CLHEP::m);}
  if (sPos.size() == 1)
    {sPos.push_back(1*CLHEP::m);}
  return sPos;
}

G4bool BDSBeamline::ElementAnglesSumToCircle() const
{
  return (std::abs(totalAngle) > 0.99 * 2.0 * CLHEP::pi) and (std::abs(totalAngle) < 1.01 * 2.0 * CLHEP::pi);
}

BDSExtentGlobal BDSBeamline::GetExtentGlobal() const
{
  const BDSExtent ext = BDSExtent(maximumExtentNegative, maximumExtentPositive);
  BDSExtentGlobal extG = BDSExtentGlobal(ext, G4Transform3D());
  return extG;
}

std::vector<G4int> BDSBeamline::GetIndicesOfElementsOfType(const G4String& type) const
{
  std::vector<G4int> result;
  for (auto element : beamline)
    {
      if (element->GetType() == type)
        {result.push_back(element->GetIndex());}
    }
  return result;
}

std::vector<G4int> BDSBeamline::GetIndicesOfElementsOfType(const std::set<G4String>& types) const
{
  std::vector<G4int> result;
  for (auto element : beamline)
    {
      G4String type = element->GetType();
      if (types.find(type) != types.end())
        {result.push_back(element->GetIndex());}
    }
  return result;
}

std::vector<G4int> BDSBeamline::GetIndicesOfCollimators() const
{
  std::set<G4String> collimatorTypes = {"ecol", "rcol", "jcol", "jcoltip", "crystalcol", "element-collimator"};
  return GetIndicesOfElementsOfType(collimatorTypes);
}
