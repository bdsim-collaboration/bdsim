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
#include "globals.hh" // geant4 globals / types

#include "BDSAcceleratorComponent.hh"
#include "BDSBeamPipeInfo.hh"
#include "BDSBendBuilder.hh"
#include "BDSComponentFactory.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSFieldType.hh"
#include "BDSGlobalConstants.hh"
#include "BDSIntegratorSet.hh"
#include "BDSIntegratorType.hh"
#include "BDSIntegratorSetType.hh"
#include "BDSMagnetStrength.hh"
#include "BDSLine.hh"
#include "BDSMagnet.hh"
#include "BDSMagnetOuterInfo.hh"
#include "BDSModulatorInfo.hh"
#include "BDSUtilities.hh"

#include "parser/element.h"
#include "parser/elementtype.h"

#include "G4Transform3D.hh"

using namespace GMAD;

G4bool BDS::ZeroStrengthDipole(const BDSMagnetStrength* st)
{
  G4bool finiteAngle = BDS::IsFinite((*st)["angle"]);
  G4bool finiteField = BDS::IsFinite((*st)["field"]);
  return !(finiteAngle || finiteField);
}

BDSAcceleratorComponent* BDS::BuildSBendLine(const G4String&         elementName,
					     const Element*          element,
					     BDSMagnetStrength*      st,
					     G4double                brho,
					     const BDSIntegratorSet* integratorSet,
					     G4double                incomingFaceAngle,
					     G4double                outgoingFaceAngle,
					     G4bool                  buildFringeFields,
					     const GMAD::Element*    prevElement,
					     const GMAD::Element*    nextElement,
					     BDSModulatorInfo*       fieldModulator)
{
  const G4String             baseName = elementName;
  const G4double thinElementArcLength = BDSGlobalConstants::Instance()->ThinElementLength();
  const G4bool             yokeOnLeft = BDSComponentFactory::YokeOnLeft(element,st); 
  const G4double            arcLength = element->l  * CLHEP::m;
  const G4double                angle = (*st)["angle"];
  G4double                       fint = element->fint;
  G4double                      fintx = element->fintx;
  G4double                       hgap = element->hgap * CLHEP::m;

  // Note for tilted dipoles, the geometry is tilted but the curvilinear world isn't,
  // therefore we tilt the field to match the geometry.
  G4Transform3D fieldTiltOffset = BDSComponentFactory::CreateFieldTransform(element);

  G4bool buildFringeIncoming = buildFringeFields;
  G4bool buildFringeOutgoing = buildFringeFields;

  G4bool finiteK1 = BDS::IsFinite((*st)["k1"]);
  BDSFieldType dipoleFieldType = finiteK1 ? BDSFieldType::dipolequadrupole : BDSFieldType::dipole;

  if (buildFringeFields)
    {
      // perform series of checks on fringe field parameters
      // nominally, don't build the fringe element if there's no face angle
      if (!BDS::IsFinite(element->e1))
	{buildFringeIncoming = false;} // by default false
      if (!BDS::IsFinite(element->e2))
	{buildFringeOutgoing = false;}

      // fintx = -1 is how madx writes out that fintx should default to fint
      // it's also our default. If by default fint=0 and fintx=0, no fringes
      // would be built. If finite fint and -1 for fintx both will be built.
      // if fint=0 and fintx != -1, only the outgoin will be built
      if (fintx == -1)
	{fintx = fint;}
      
      // however if finite hgap and fint or fintx specified, there is an effect
      if (BDS::IsFinite(fint) && BDS::IsFinite(hgap))
	{buildFringeIncoming = true;}
      if (BDS::IsFinite(fintx) && BDS::IsFinite(hgap))
	{buildFringeOutgoing = true;}      

    // don't build fringe elements if a non-matrix integrator set is used and
    // only a pole face effect is specified. The angled pole face geometry is
    // constructed so a thin integrated pole face kick shouldn't be applied as well
    if (!integratorSet->IsMatrixIntegratorSet() && BDSGlobalConstants::Instance()->BuildPoleFaceGeometry())
      {
        // no hgap check - finite hgap but 0 fint/x will not produce a fringe kick so fringes won't be built anyway
        if (BDS::IsFinite(element->e1) && !BDS::IsFinite(fint))
          {buildFringeIncoming = false;}
        if (BDS::IsFinite(element->e2) && !BDS::IsFinite(fintx))
          {buildFringeOutgoing = false;}
      }

      // overriding checks - don't build fringe field if we're butt against another
      // sbend.
      if (prevElement)
	{
	  if (prevElement->type == ElementType::_SBEND)
	    {
	      buildFringeIncoming = false;
	      if (BDS::IsFinite(prevElement->e2 - element->e1))
		{throw BDSException( __METHOD_NAME__, prevElement->name + " e2 clashes with " + elementName + " e1");}
	    }
	}
      if (nextElement)
	{
	  if (nextElement->type == ElementType::_SBEND)
	    {buildFringeOutgoing = false;}
	  // check above on clashing sbends with e1 e2 will be used for next bend.
	}
      
      if (!BDS::IsFinite(angle))
	{// no fringe fields if magnet of zero angle, ie strength
	  buildFringeIncoming = false;
	  buildFringeOutgoing = false;
	}
    }// end of checks
  
  // Calculate number of sbends to split parent into
  G4int nSBends = BDS::CalculateNSBendSegments(arcLength, angle, incomingFaceAngle,
					       outgoingFaceAngle);
  
  G4bool zeroStrength = BDS::ZeroStrengthDipole(st);
  if (zeroStrength)
    {
      buildFringeIncoming = false;
      buildFringeOutgoing = false;
    }
  
  // If no angle, means no fringe strength, so even if there are pole face rotations, we don't
  // need the fringe elements. If there's only 1 segment we may still have a finite
  // angle and therefore finite strength and the pole face rotations will require fringe
  // elements
  if (!BDS::IsFinite(angle))
    {      
      BDSIntegratorType intType = BDS::GetDipoleIntegratorType(integratorSet, element);
      BDSFieldInfo* vacuumField = nullptr;
      BDSFieldInfo* outerField = nullptr;
      // prepare one sbend segment
      auto bpInfo = BDSComponentFactory::PrepareBeamPipeInfo(element, -incomingFaceAngle,
							     -outgoingFaceAngle);
      auto mgInfo = BDSComponentFactory::PrepareMagnetOuterInfo(baseName, element,
								-incomingFaceAngle,
								-outgoingFaceAngle,
								bpInfo,
								yokeOnLeft);
      if (!zeroStrength)
	{
	  vacuumField = new BDSFieldInfo(dipoleFieldType,
					 brho,
					 intType,
					 st,
					 true,
					 fieldTiltOffset);
	  vacuumField->SetModulatorInfo(fieldModulator);
	  outerField = BDSComponentFactory::PrepareMagnetOuterFieldInfo(st,
									BDSFieldType::dipole,
									bpInfo,
									mgInfo,
									fieldTiltOffset,
									integratorSet,
									brho,
									BDSComponentFactory::ScalingFieldOuter(element),
									fieldModulator);
	}
      BDSMagnet* oneBend = new BDSMagnet(BDSMagnetType::sectorbend,
					 baseName,
					 arcLength,
					 bpInfo,
					 mgInfo,
					 vacuumField,
					 -angle,
					 outerField); // angle 0, but should be minus for 3d cart.
      return oneBend;
    }

  // We're definitely building a line now.
  BDSLine* sbendline  = new BDSLine(baseName); // line for resultant sbend
  
  // length of bends minus the fringe segments
  G4double centralArcLength = arcLength;
  G4double centralAngle     = angle;
  G4double oneFringeAngle   = 0;
  if (BDS::IsFinite(angle))
    {oneFringeAngle = (thinElementArcLength / arcLength) * angle;}
  
  if (buildFringeIncoming)
    {
      centralArcLength -= thinElementArcLength;
      centralAngle     -= oneFringeAngle;
    }
  if (buildFringeOutgoing)
    {
      centralArcLength -= thinElementArcLength;
      centralAngle     -= oneFringeAngle;
    }
  
  // calculate their angles and length
  G4double semiAngle     = centralAngle / (G4double) nSBends;
  G4double semiArcLength = centralArcLength / (G4double) nSBends;

  BDSMagnetStrength* semiStrength = new BDSMagnetStrength(*st); // the copy is crucial to copy the field strength
  (*semiStrength)["angle"]  = semiAngle;
  (*semiStrength)["length"] = semiArcLength;
  
  G4double zExtentIn  = 0;
  G4double zExtentOut = 0;
  G4bool fadeIn = true;
  G4bool fadeOut = true;

  // unlike an rbend, the sbend will mostly likely be split up into segments.
  // we must check that the faces of each segment (varying from e1 to e2) will
  // not overlap given the outer diameter.
  // calculate extent along z due poleface rotation at half the horizontal width.
  // note this is only w.r.t. pole face rotation and not the general bending angle
  G4double horizontalWidth = BDSComponentFactory::PrepareHorizontalWidth(element);
  if (incomingFaceAngle > 0)
    {zExtentIn = 0.5*horizontalWidth*std::tan(incomingFaceAngle - 0.5*std::abs(semiAngle));}
  else if (incomingFaceAngle < 0)
    {zExtentIn = 0.5*horizontalWidth*std::tan(0.5*std::abs(semiAngle) + incomingFaceAngle);}
  if (outgoingFaceAngle > 0)
    {zExtentOut = 0.5*horizontalWidth*std::tan(outgoingFaceAngle - 0.5*std::abs(semiAngle));}
  else if (outgoingFaceAngle < 0)
    {zExtentOut = 0.5*horizontalWidth*std::tan(0.5*std::abs(semiAngle) + outgoingFaceAngle);}
  
  // decide if segment angles fade or not depending on the extents
  if (std::abs(zExtentIn) < semiArcLength/4.0)
    {fadeIn = false;}
  if (std::abs(zExtentOut) < semiArcLength/4.0)
    {fadeOut = false;}

  // field recipe for one segment of the sbend
  G4String centralName = baseName + "_even_ang";
  BDSIntegratorType intType = BDS::GetDipoleIntegratorType(integratorSet, element);
  BDSFieldInfo* semiVacuumField = nullptr;
  BDSFieldInfo* semiOuterField = nullptr;
  auto bpInfo = BDSComponentFactory::PrepareBeamPipeInfo(element, 0.5*semiAngle, 0.5*semiAngle);
  auto mgInfo = BDSComponentFactory::PrepareMagnetOuterInfo(centralName, element,
                                                            0.5*semiAngle, 0.5*semiAngle, bpInfo,
                                                            yokeOnLeft);
  if (!zeroStrength)
    {
      semiVacuumField = new BDSFieldInfo(dipoleFieldType,
					 brho,
					 intType,
					 semiStrength,
					 true,
					 fieldTiltOffset);
      semiVacuumField->SetModulatorInfo(fieldModulator);
      semiOuterField = BDSComponentFactory::PrepareMagnetOuterFieldInfo(semiStrength,
									BDSFieldType::dipole,
									bpInfo,
									mgInfo,
									fieldTiltOffset,
									integratorSet,
									brho,
									BDSComponentFactory::ScalingFieldOuter(element),
                                                                        fieldModulator);
    }
  mgInfo->name = centralName;
  BDSMagnet* centralWedge = new BDSMagnet(BDSMagnetType::sectorbend,
					  centralName,
					  semiArcLength,
					  bpInfo,
					  mgInfo,
					  semiVacuumField,
					  -semiAngle,
					  semiOuterField); // minus for 3d cartesian conversion
  
  // check magnet outer info
  auto magnetOuterInfoCheck = BDSComponentFactory::PrepareMagnetOuterInfo("checking", element,
									  -incomingFaceAngle,
                                                                          -outgoingFaceAngle,
									  bpInfo, yokeOnLeft);
  // minus for conversion to 3d cartesian
  G4double minimalRadius = 2*magnetOuterInfoCheck->MinimumIntersectionRadiusRequired();
  // extra pedantic check for dipoles with certain geometry types
  if (!magnetOuterInfoCheck->hStyle)
    {// in the case of C-shaped poled dipoles, the full 'horizontalWidth' is shifted to the yoke side
      switch (magnetOuterInfoCheck->geometryType.underlying())
        {
          case BDSMagnetGeometryType::polescircular:
          case BDSMagnetGeometryType::polesfacet:
          case BDSMagnetGeometryType::polesfacetcrop:
          case BDSMagnetGeometryType::polessquare:
            {
              minimalRadius *= element->yokeOnInside ? 2.0 : 0.5;
              break;
            }
          default:
            {break;}
        }
    }
  BDSComponentFactory::CheckBendLengthAngleWidthCombo(semiArcLength, -semiAngle, minimalRadius, element->name);
  delete magnetOuterInfoCheck; // clean up
  
  // build incoming fringe field if required
  if (buildFringeIncoming)
    {
      G4double e1 = element->e1;
      // if using a non-matrix integrator set, this code should only be reached if a finite fint is specified.
      // As the angled face geometry is constructed, a thin pole face kick shouldn't also be applied, so set
      // e1 to 0 in the magnet strength object so only fringe effects are applied
      if (!integratorSet->IsMatrixIntegratorSet()
	  && BDSGlobalConstants::Instance()->BuildPoleFaceGeometry()
	  && BDS::IsFinite(fint))
        {e1 = 0;}

      BDSMagnetStrength* fringeStIn = BDS::GetFringeMagnetStrength(element, st, oneFringeAngle,
                                                                   e1, element->e2, fintx, true);
      G4String segmentName           = baseName + "_e1_fringe";
      G4double fringeAngleIn         = 0.5*oneFringeAngle - incomingFaceAngle;
      G4double fringeAngleOut        = 0.5*oneFringeAngle + incomingFaceAngle;
      BDSMagnet* startfringe = BDS::BuildDipoleFringe(element, fringeAngleIn, fringeAngleOut,
						      segmentName, fringeStIn, brho,
						      integratorSet, dipoleFieldType, fieldModulator);
      sbendline->AddComponent(startfringe);
    }
  
  // logic for elements in the sbend line:
  // reuse central segment for all segment of in/out half if no poleface angle(s)
  // if small poleface, new first/last segment, reuse central wedge for remainder of in/out half
  // otherwise fade in/out faces for all segments in app. halves.
  // 'central' one is definitely used for the central part, but also it's just a segment
  // with even incoming and outgoing face angles w.r.t. the chord.
  G4double segmentAngleIn  = 0;
  G4double segmentAngleOut = 0;
  G4int    numberOfUniqueComponents = 1; // used for naming purposes
  BDSMagnet* oneBend = nullptr;
  G4bool   centralWedgeUsed = false; // keep track to avoid memory leak
  const G4int numSegmentsEitherSide = (nSBends - 1) / 2;
  for (G4int i = 0; i < nSBends; ++i)
    {
      G4String name = baseName;
      if (i < numSegmentsEitherSide)
        {// first half of magnet
          if (!BDS::IsFinite(incomingFaceAngle)) // no pole face rotation so just repeat central segment
            {oneBend = centralWedge;}
          else if (fadeIn) // build incremented angled segment
            {
	      name += "_"+std::to_string(numberOfUniqueComponents);
	      numberOfUniqueComponents++;
	      BDS::UpdateSegmentAngles(i,nSBends,semiAngle,incomingFaceAngle,outgoingFaceAngle,segmentAngleIn,segmentAngleOut);
	      oneBend = BDS::BuildSingleSBend(element, name, semiArcLength, semiAngle,
					      segmentAngleIn, segmentAngleOut, semiStrength,
					      brho, integratorSet, yokeOnLeft, semiOuterField);
	    }
          else
            {// finite pole face, but not strong so build one angled, then repeat the rest to save memory
              if (i == 0) // the first one is unique
                {
		  name += "_"+std::to_string(numberOfUniqueComponents);
		  numberOfUniqueComponents++;
		  segmentAngleIn  = 0.5*semiAngle - incomingFaceAngle; // whole pole face angle
		  segmentAngleOut = 0.5*semiAngle;      // even matching angle
		  oneBend = BDS::BuildSingleSBend(element, name, semiArcLength, semiAngle,
						  segmentAngleIn, segmentAngleOut, semiStrength,
						  brho, integratorSet, yokeOnLeft, semiOuterField);
		}
              else // others afterwards are a repeat of the even angled one
                {oneBend = centralWedge;}
            }
        }
      else if (i > numSegmentsEitherSide) // i is zero counting
        {// second half of magnet
          if (!BDS::IsFinite(outgoingFaceAngle)) // no pole face rotation so just repeat central segment
            {oneBend = centralWedge;}
          else if (fadeOut) // build incremented angled segment
	    {
	      name += "_"+std::to_string(numberOfUniqueComponents);
	      numberOfUniqueComponents++;
	      BDS::UpdateSegmentAngles(i,nSBends,semiAngle,incomingFaceAngle,outgoingFaceAngle,segmentAngleIn,segmentAngleOut);
	      oneBend = BDS::BuildSingleSBend(element, name, semiArcLength, semiAngle,
					      segmentAngleIn, segmentAngleOut, semiStrength,
					      brho, integratorSet, yokeOnLeft, semiOuterField);
	    }
          else
            {// finite pole face, but not strong so build only one unique angled on output face
              if (i == (nSBends-1)) // last one
                {
		  name += "_"+std::to_string(numberOfUniqueComponents);
		  numberOfUniqueComponents++;
		  segmentAngleIn  = 0.5*semiAngle;
		  segmentAngleOut = 0.5*semiAngle - outgoingFaceAngle;
		  oneBend = BDS::BuildSingleSBend(element, name, semiArcLength, semiAngle,
						  segmentAngleIn, segmentAngleOut, semiStrength,
						  brho, integratorSet, yokeOnLeft, semiOuterField);
		}
              else // after central, but before unique end piece - even angled.
                {oneBend = centralWedge;}
            }
        }
      else // the middle piece
        {oneBend = centralWedge;}

      // append to the line
      sbendline->AddComponent(oneBend);

      centralWedgeUsed = centralWedgeUsed || (oneBend == centralWedge);
    }

  if (!centralWedgeUsed)
    {delete centralWedge;}
  
  //Last element should be fringe if poleface specified
  if (buildFringeOutgoing)
    {
      G4double e2 = element->e2;
      // if using a non-matrix integrator set, this code should only be reached if a finite fintx is specified.
      // As the angled face geometry is constructed, a thin pole face kick shouldn't also be applied, so set
      // e2 to 0 in the magnet strength object so only fringe effects are applied
      if (!integratorSet->IsMatrixIntegratorSet() && BDSGlobalConstants::Instance()->BuildPoleFaceGeometry()
          && BDS::IsFinite(fintx))
        {e2 = 0;}

      BDSMagnetStrength* fringeStOut = BDS::GetFringeMagnetStrength(element, st, oneFringeAngle,
                                                                    element->e1, e2, fintx, false);
      G4double fringeAngleIn          = 0.5*oneFringeAngle + outgoingFaceAngle;
      G4double fringeAngleOut         = 0.5*oneFringeAngle - outgoingFaceAngle;
      G4String segmentName            = baseName + "_e2_fringe";
      
      BDSMagnet* endfringe = BDS::BuildDipoleFringe(element, fringeAngleIn, fringeAngleOut,
						    segmentName, fringeStOut, brho,
						    integratorSet, dipoleFieldType);
      sbendline->AddComponent(endfringe);
    }
  return sbendline;
}

void BDS::UpdateSegmentAngles(G4int index,
			      G4int nSBends,
			      G4double semiAngle,
			      G4double incomingFaceAngle,
			      G4double outgoingFaceAngle,
			      G4double& segmentAngleIn,
			      G4double& segmentAngleOut)
{
  // nSBends is always odd.
  // index assumed 0 counting.
  G4bool central = index == (nSBends - 1) / 2;
  if (central)
    {// shouldn't happen, but just in case
      segmentAngleIn = 0.5*semiAngle;
      segmentAngleOut = 0.5*semiAngle;
      return;
    }
  
  G4bool       firstHalf = index + 1 < (nSBends + 1) / 2;
  G4int numberToFadeOver = (nSBends - 1) / 2;
  if (firstHalf)
    {// fade from incomingFaceAngle to 0
      G4double delta = incomingFaceAngle / (G4double) numberToFadeOver;
      G4double inputFaceAngle  = incomingFaceAngle - ((G4double)index * delta);
      G4double outputFaceAngle = incomingFaceAngle - ((G4double)(index+1) * delta);
      // if incomingFaceAngle was 0, each would have 1/2 of semiAngle
      segmentAngleIn  = 0.5*semiAngle - inputFaceAngle;
      segmentAngleOut = 0.5*semiAngle + outputFaceAngle;
    }
  else
    {// second half - fade from 0 to outgoingFaceAngle
      G4double delta = outgoingFaceAngle / (G4double) numberToFadeOver;
      G4int secondHalfIndex = index - ((nSBends + 1) / 2);
      // here we fade from 0 contribution to maximum (opposite of above if statement)
      G4double inputFaceAngle  = outgoingFaceAngle - ( (G4double)(numberToFadeOver - secondHalfIndex) * delta);
      G4double outputFaceAngle = outgoingFaceAngle - ( (G4double)(numberToFadeOver - (secondHalfIndex + 1)) * delta);
      // if incomingFaceAngle was 0, each would have 1/2 of semiAngle
      segmentAngleIn  = 0.5*semiAngle + inputFaceAngle;
      segmentAngleOut = 0.5*semiAngle - outputFaceAngle;
    }
}

BDSMagnet* BDS::BuildSingleSBend(const GMAD::Element*     element,
				 const G4String&          name,
				 G4double                 arcLength,
				 G4double                 angle,
				 G4double                 angleIn,
				 G4double                 angleOut,
				 const BDSMagnetStrength* strength,
				 G4double                 brho,
				 const BDSIntegratorSet*  integratorSet,
				 G4bool                   yokeOnLeft,
				 const BDSFieldInfo*      outerFieldIn,
                                 BDSModulatorInfo*        fieldModulator)
{
  auto bpInfo = BDSComponentFactory::PrepareBeamPipeInfo(element, angleIn, angleOut);
  
  BDSMagnetStrength* strengthCopy = new BDSMagnetStrength(*strength); // the copy is crucial to copy the field strength
  auto  magnetOuterInfo = BDSComponentFactory::PrepareMagnetOuterInfo(name, element, angleIn, angleOut, bpInfo, yokeOnLeft);
  // set the name to the desired one rather than the one from the element
  magnetOuterInfo->name = name;
  
  G4Transform3D fieldTiltOffset = BDSComponentFactory::CreateFieldTransform(element);

  G4bool finiteK1 = BDS::IsFinite((*strength)["k1"]);
  BDSFieldType dipoleFieldType = finiteK1 ? BDSFieldType::dipolequadrupole : BDSFieldType::dipole;
  BDSIntegratorType intType = BDS::GetDipoleIntegratorType(integratorSet, element);
  BDSFieldInfo* vacuumField = nullptr;
  BDSFieldInfo* outerField  = nullptr;
  G4bool zeroStrength = BDS::ZeroStrengthDipole(strengthCopy);
  if (!zeroStrength)
    {
      vacuumField = new BDSFieldInfo(dipoleFieldType,
				     brho,
				     intType,
				     strengthCopy,
				     true,
				     fieldTiltOffset);
      vacuumField->SetModulatorInfo(fieldModulator);
      outerField = new BDSFieldInfo(*outerFieldIn);
    }
  
  BDSMagnet* magnet = new BDSMagnet(BDSMagnetType::sectorbend,
				    name,
				    arcLength,
				    bpInfo,
				    magnetOuterInfo,
				    vacuumField,
				    -angle,
				    outerField);
  
  return magnet;
}

BDSLine* BDS::BuildRBendLine(const G4String&         elementName,
			     const Element*          element,
			     const Element*          prevElement,
			     const Element*          nextElement,
			     G4double                brho,
			     BDSMagnetStrength*      st,
			     const BDSIntegratorSet* integratorSet,
			     G4double                incomingFaceAngle,
			     G4double                outgoingFaceAngle,
			     G4bool                  buildFringeFields,
                             BDSModulatorInfo*       fieldModulator)
{
  const G4String name = elementName;
  BDSLine* rbendline  = new BDSLine(name); // line for resultant rbend
  
  const G4double thinElementArcLength = BDSGlobalConstants::Instance()->ThinElementLength();
  const G4bool             yokeOnLeft = BDSComponentFactory::YokeOnLeft(element, st);
  G4bool          buildFringeIncoming = buildFringeFields;
  G4bool          buildFringeOutgoing = buildFringeFields;
  G4double                       fint = element->fint;
  G4double                      fintx = element->fintx;
  G4double                       hgap = element->hgap * CLHEP::m;
  
  // Angle here is in the 'strength' convention of +ve angle -> -ve x deflection
  const G4double       angle = (*st)["angle"];
  const G4double   arcLength = (*st)["length"];
  
  G4bool zeroStrength = BDS::ZeroStrengthDipole(st);
  
  G4Transform3D fieldTiltOffset = BDSComponentFactory::CreateFieldTransform(element);

  G4bool finiteK1 = BDS::IsFinite((*st)["k1"]);
  BDSFieldType dipoleFieldType = finiteK1 ? BDSFieldType::dipolequadrupole : BDSFieldType::dipole;

  // Here, 'no face angle' really means that the rbend becomes an sbend.
  // Calculate how far away we are from an sbend.
  G4double incomingFaceAngleWRTSBend = incomingFaceAngle + angle*0.5;
  G4double outgoingFaceangleWRTSBend = outgoingFaceAngle + angle*0.5;
  if (!BDS::IsFinite(incomingFaceAngleWRTSBend) && (!integratorSet->IsMatrixIntegratorSet()))
    {buildFringeIncoming = false;}
  if (!BDS::IsFinite(outgoingFaceangleWRTSBend) && (!integratorSet->IsMatrixIntegratorSet()))
    {buildFringeOutgoing = false;}

  // fintx = -1 is how madx writes out that fintx should default to fint
  // it's also our default. If by default fint=0 and fintx=0, no fringes
  // would be built. If finite fint and -1 for fintx both will be built.
  // if fint=0 and fintx != -1, only the outgoin will be built
  if (fintx == -1)
    {fintx = fint;}
  
  // however if finite hgap and fint or fintx specified, there is an effect
  if (BDS::IsFinite(fint) && BDS::IsFinite(hgap))
    {buildFringeIncoming = true;}
  if (BDS::IsFinite(fintx) && BDS::IsFinite(hgap))
    {buildFringeOutgoing = true;} 
  
  // the poleface angles to be used in tracking only.
  G4double trackingPolefaceAngleIn = element->e1;
  G4double trackingPolefaceAngleOut = element->e2;
  if (integratorSet->IsMatrixIntegratorSet())
    {// for this integrator set we track the rbend as an sbend with extra pole face rotation
      trackingPolefaceAngleIn += angle*0.5;
      trackingPolefaceAngleOut += angle*0.5;
    }

  G4double e1 = -incomingFaceAngle;
  G4double e2 = -outgoingFaceAngle;

  if (prevElement)
    {
      if (BDS::IsFinite(prevElement->e2) && BDS::IsFinite(element->e1))
	{
	  throw BDSException(__METHOD_NAME__, prevElement->name + " has finite e2!\n Clashes with " + elementName + " with finite e1");
	}
      if (prevElement->type == ElementType::_RBEND)
	{
	  buildFringeIncoming = false;
	  e1 = angle*0.5;
	}
    }
  if (nextElement)
    {
      if (BDS::IsFinite(nextElement->e1) && BDS::IsFinite(element->e2))
	{
	  throw BDSException(__METHOD_NAME__ + nextElement->name + " has finite e1!\n Clashes with " + elementName + " with finite e2");
	}
      if (nextElement->type == ElementType::_RBEND)
	{
	  buildFringeOutgoing = false;
	  e2 = angle*0.5;
	}
    }
  
  if (zeroStrength)
    {
      buildFringeIncoming = false;
      buildFringeOutgoing = false;
    }
  
  // used for debugging purposes to forefully try out one and not the other fringe
  //buildFringeIncoming = false;
  //buildFringeOutgoing = false;
  
  // if we're building the fringe elements, we reduce the length of the central section
  // and proportion the bending by their length w.r.t. the full length of the total rbend.
  // angles of the faces are in order:
  // angleIn / fringeInOutputAngle / centralInputFaceAngle / centralOutputFaceAngle
  // fringeOutInputAngle / angleOut
  // default face angles for an rbend are 0 - ie parallel faces, plus any pole face rotation
  // angle in and out of total rbend are nominally the face angles.
  G4double angleIn  = incomingFaceAngle;
  G4double angleOut = outgoingFaceAngle;
  G4double fringeInOutputAngle    = 0;
  G4double centralInputFaceAngle  = angleIn;
  G4double centralOutputFaceAngle = angleOut;
  G4double fringeOutInputAngle    = 0;
  G4double centralArcLength       = arcLength; // full length to start with
  G4double centralAngle           = angle;
  G4double oneFringeAngle         = 0;
  if (BDS::IsFinite(angle))
    {oneFringeAngle = (thinElementArcLength / arcLength) * angle;}
  
  if (buildFringeIncoming && buildFringeOutgoing)
    {// both
      centralArcLength      -= 2*thinElementArcLength;
      centralAngle          -= 2*oneFringeAngle;
      angleIn                = e1 + (0.5*oneFringeAngle - 0.5*angle);
      fringeInOutputAngle    = -angleIn;
      centralInputFaceAngle  = e1;
      centralOutputFaceAngle = e2;
      fringeOutInputAngle    = - (e2 + (0.5*oneFringeAngle - 0.5*angle));
      angleOut               = -fringeOutInputAngle;
    }
  else if (buildFringeIncoming)
    {// incoming only
      centralArcLength      -= thinElementArcLength;
      centralAngle          -= oneFringeAngle;
      angleIn                = e1 + (0.5*oneFringeAngle - 0.5*angle);
      fringeInOutputAngle    = -angleIn; // fringe built the same
      centralInputFaceAngle  = e1 + 0.5*oneFringeAngle;
      centralOutputFaceAngle = e2 - 0.5*oneFringeAngle;
    }
  else if (buildFringeOutgoing)
    {// outgoing only
      centralArcLength      -= thinElementArcLength;
      centralAngle          -= oneFringeAngle;
      centralInputFaceAngle  = e1 - 0.5*oneFringeAngle;
      centralOutputFaceAngle = e2 + 0.5*oneFringeAngle;
      fringeOutInputAngle    = - (e2 + (0.5*oneFringeAngle - 0.5*angle));;
      angleOut               = e2 + (0.5*oneFringeAngle - 0.5*angle);;
    }
  else
    {// neither
      // centralArcLength the same
      // centralAngle the same
      centralInputFaceAngle  = e1;
      centralOutputFaceAngle = e2;
    }
  
  if (buildFringeIncoming)
    {
      G4double trackingPolefaceAngle = trackingPolefaceAngleIn;
      // if using a non-matrix integrator set, this code should only be reached if a finite fint is specified.
      // As the angled face geometry is constructed, a thin pole face kick shouldn't also be applied, so subtract
      // e1 for in the magnet strength object so only the fringe & the natural rbend angled face effects are applied
      if (!integratorSet->IsMatrixIntegratorSet() && BDSGlobalConstants::Instance()->BuildPoleFaceGeometry()
          && BDS::IsFinite(fint))
        {trackingPolefaceAngle -= element->e1;}

        BDSMagnetStrength* fringeStIn = BDS::GetFringeMagnetStrength(element, st, oneFringeAngle,
                                                                     trackingPolefaceAngle, trackingPolefaceAngleOut,
                                                                   fintx, true);
      G4String fringeName = name + "_e1_fringe";

      // element used for beam pipe materials etc - not strength, angle or length.
      BDSMagnet* startfringe = BDS::BuildDipoleFringe(element, angleIn, fringeInOutputAngle,
						      fringeName,
						      fringeStIn, brho,
						      integratorSet, dipoleFieldType, fieldModulator);
      rbendline->AddComponent(startfringe);
    }
  
  // override copied length and angle
  (*st)["length"] = centralArcLength;
  (*st)["angle"]  = centralAngle;

  BDSIntegratorType intType = BDS::GetDipoleIntegratorType(integratorSet, element);
  BDSFieldInfo* vacuumField = nullptr;
  BDSFieldInfo* outerField = nullptr;
  auto bpInfo = BDSComponentFactory::PrepareBeamPipeInfo(element, centralInputFaceAngle, centralOutputFaceAngle);
  auto mgInfo = BDSComponentFactory::PrepareMagnetOuterInfo(elementName, element, centralInputFaceAngle, centralOutputFaceAngle, bpInfo, yokeOnLeft);
  mgInfo->name = elementName;
  if (!zeroStrength)
    {
      vacuumField = new BDSFieldInfo(dipoleFieldType,
				     brho,
				     intType,
				     st,
				     true,
				     fieldTiltOffset);
      vacuumField->SetModulatorInfo(fieldModulator);
      outerField = BDSComponentFactory::PrepareMagnetOuterFieldInfo(st,
								    BDSFieldType::dipole,
								    bpInfo,
								    mgInfo,
								    fieldTiltOffset,
								    integratorSet,
								    brho,
								    BDSComponentFactory::ScalingFieldOuter(element),
                                                                    fieldModulator);
    }
  
  // Here we change from the strength angle convention of +ve angle corresponds to
  // deflection in negative x, to correct 3d +ve angle corresponds to deflection in
  // positive x. Hence, angle sign flip for construction.
  BDSMagnet* oneBend = new BDSMagnet(BDSMagnetType::rectangularbend,
				     elementName+"_centre",
				     centralArcLength,
				     bpInfo,
				     mgInfo,
				     vacuumField,
				     -centralAngle,
                                     outerField);
  
  rbendline->AddComponent(oneBend);
  
  //Last element should be fringe if poleface specified
  if (buildFringeOutgoing)
    {
      G4double trackingPolefaceAngle = trackingPolefaceAngleOut;
      // if using a non-matrix integrator set, this code should only be reached if a finite fintx is specified.
      // As the angled face geometry is constructed, a thin pole face kick shouldn't also be applied, so subtract
      // e2 for in the magnet strength object so only the fringe & the natural rbend angled face effects are applied
      if (!integratorSet->IsMatrixIntegratorSet() && BDSGlobalConstants::Instance()->BuildPoleFaceGeometry()
          && BDS::IsFinite(fintx))
        {trackingPolefaceAngle -= element->e2;}
      BDSMagnetStrength* fringeStOut = BDS::GetFringeMagnetStrength(element, st, oneFringeAngle,
                                                                    trackingPolefaceAngleIn, trackingPolefaceAngle,
                                                                    fintx, false);
      G4String fringeName = name + "_e2_fringe";
      
      BDSMagnet* endfringe = BDS::BuildDipoleFringe(element, fringeOutInputAngle, angleOut,
						    fringeName,
						    fringeStOut, brho,
						    integratorSet, dipoleFieldType);
      rbendline->AddComponent(endfringe);
    }
  
  return rbendline;
}

BDSMagnet* BDS::BuildDipoleFringe(const GMAD::Element*     element,
				  G4double                 angleIn,
				  G4double                 angleOut,
				  const G4String&          name,
				  BDSMagnetStrength*       st,
				  G4double                 brho,
				  const BDSIntegratorSet*  integratorSet,
				  BDSFieldType             dipoleFieldType,
                                  BDSModulatorInfo*        fieldModulator)
{
  BDSBeamPipeInfo* beamPipeInfo = BDSComponentFactory::PrepareBeamPipeInfo(element, angleIn, angleOut);
  beamPipeInfo->beamPipeType = BDSBeamPipeType::circularvacuum;
  auto magnetOuterInfo = BDSComponentFactory::PrepareMagnetOuterInfo(name, element,
								     angleIn, angleOut, beamPipeInfo);
  magnetOuterInfo->geometryType   = BDSMagnetGeometryType::none;
  magnetOuterInfo->name           = name;
  magnetOuterInfo->buildEndPieces = false;
  
  G4Transform3D fieldTiltOffset = BDSComponentFactory::CreateFieldTransform(element);
  
  BDSIntegratorType intType = integratorSet->dipoleFringe;
  BDSFieldInfo* vacuumField = new BDSFieldInfo(dipoleFieldType,
					       brho,
					       intType,
					       st,
					       true,
					       fieldTiltOffset);
  vacuumField->SetModulatorInfo(fieldModulator);
  vacuumField->SetFieldAsThin();

  return new BDSMagnet(BDSMagnetType::dipolefringe,
		       name,
		       (*st)["length"],
		       beamPipeInfo,
		       magnetOuterInfo,
		       vacuumField,
		       -(*st)["angle"],
		       nullptr,
		       true);
}

G4int BDS::CalculateNSBendSegments(G4double length,
				   G4double angle,
				   G4double incomingFaceAngle,
				   G4double outgoingFaceAngle,
				   G4double aperturePrecision)
{
  // Split a bend into equal segments such that the maximum distance between the
  // chord and arc is 1mm.
  
  // from formula: L/2 / N tan (angle/N) < precision. (L=physical length)
  // add poleface rotations onto angle as absolute number (just to be safe)
  G4double totalAngle = std::abs(angle) + std::abs(incomingFaceAngle) + std::abs(outgoingFaceAngle);
  G4int nSBends = (G4int) ceil(std::sqrt(length*totalAngle/2/aperturePrecision));
  if (nSBends==0)
    {nSBends = 1;} // can happen in case angle = 0
  if (BDSGlobalConstants::Instance()->DontSplitSBends())
    {nSBends = 1;}  // use for debugging
  if (nSBends % 2 == 0)
    {nSBends += 1;} // always have odd number of poles for poleface rotations
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " splitting sbend into " << nSBends << " sbends" << G4endl;
#endif
  return nSBends;
}

BDSIntegratorType BDS::GetDipoleIntegratorType(const BDSIntegratorSet* integratorSet,
                                               const Element*          element)
{
  // default is dipole integrator
  BDSIntegratorType intType = integratorSet->Integrator(BDSFieldType::dipole);

  // check for finite k1 and change integrator type if needed
  if (BDS::IsFinite(element->k1))
    {intType = integratorSet->Integrator(BDSFieldType::dipolequadrupole);}

  //if (BDS::IsFinite(element->tilt))
  //  {intType = BDSIntegratorType::dipolerodrigues2;}

  return intType;
}

BDSMagnetStrength* BDS::GetFringeMagnetStrength(const Element*           element,
                                                const BDSMagnetStrength* st,
                                                G4double           fringeAngle,
                                                G4double           e1,
                                                G4double           e2,
                                                G4double           fintx,
                                                G4bool             isEntrance)
{
  BDSMagnetStrength* fringeSt   = new BDSMagnetStrength(*st);
  (*fringeSt)["length"]         = BDSGlobalConstants::Instance()->ThinElementLength();
  (*fringeSt)["angle"]          = fringeAngle;
  (*fringeSt)["e1"]             = e1;     // supply separately as it may be modified for rbends
  (*fringeSt)["e2"]             = e2;     // supply separately as it may be modified for rbends
  (*fringeSt)["fint"]           = element->fint;
  (*fringeSt)["fintx"]          = fintx;  // supply separately as it may be modified to match madx behaviour
  (*fringeSt)["fintk2"]         = element->fintK2;
  (*fringeSt)["fintk2"]         = element->fintxK2;
  (*fringeSt)["hgap"]           = element->hgap * CLHEP::m;
  (*fringeSt)["isentrance"]     = isEntrance;
  (*fringeSt)["h1"]             = element->h1 / CLHEP::m;     
  (*fringeSt)["h2"]             = element->h2 / CLHEP::m;
  return fringeSt;
}
