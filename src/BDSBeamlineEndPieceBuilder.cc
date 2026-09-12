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
#include "BDSBeamline.hh"
#include "BDSBeamlineEndPieceBuilder.hh"
#include "BDSBeamlineElement.hh"
#include "BDSExtent.hh"
#include "BDSGlobalConstants.hh"
#include "BDSSimpleComponent.hh"
#include "BDSTiltOffset.hh"
#include "BDSUtilities.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"

BDSBeamline* BDS::BuildEndPieceBeamline(const BDSBeamline* beamline,
                                        const G4bool circularMachine)
{
  const G4double lengthSafetyLarge = BDSGlobalConstants::Instance()->LengthSafetyLarge();
  
  // the beamline of end pieces to be placed.
  BDSBeamline* endPieces = new BDSBeamline();
  if (beamline->empty())
    {return endPieces;}

  // references to first and last item for checking
  const BDSBeamlineElement* firstItem = beamline->GetFirstItem();
  const BDSBeamlineElement* lastItem  = beamline->GetLastItem();
  
  // loop over main beamline
  G4int currentIndex = 0;
  for (auto element : *beamline)
    {
      const auto accComponent   = element->GetAcceleratorComponent();
      const auto accComponentTO = element->GetTiltOffset();
      const auto endPieceBefore = accComponent->EndPieceBefore();
      const auto endPieceAfter  = accComponent->EndPieceAfter();
      
      G4bool placeBefore = true;
      G4bool placeAfter  = true;
      if (!endPieceBefore && !endPieceAfter)
        {// no end piece on either side - skip this element
          currentIndex++;
          continue;
        }
      else if (!endPieceBefore)
        {placeBefore = false;}
      else if (!endPieceAfter)
        {placeAfter  = false;}

      G4double requiredBeforeLength = 0; // required length for previous end piece
      G4double requiredAfterLength  = 0; // required length for next end piece
      if (endPieceBefore)
        {requiredBeforeLength = endPieceBefore->GetChordLength() + 2*lengthSafetyLarge;}
      if (endPieceAfter)
        {requiredAfterLength  = endPieceAfter->GetChordLength() + 2*lengthSafetyLarge;}

      if (placeBefore)
        {
          // search behind current element until we hit a previous component that's not a drift.
          // ie work out the amount of drift space available. If we hit a previous element, check
          // if it has an end piece and take that length out of the available space. if there's then
          // available space for this end piece we place it.
          G4bool keepGoing = true;
          const BDSBeamlineElement* inspectedElement = element; // start with current element
          G4double             availableLength = 0;
          G4double   previousNonDriftEndPieceL = 0;
          G4bool              driftIsFirstItem = false;
          BDSExtent        endPieceInnerExtent = endPieceBefore->GetInnerExtent();
          G4bool         driftsAreTooBigBefore = false;
          while (keepGoing)
            {
              inspectedElement = beamline->GetPrevious(inspectedElement);
              if (inspectedElement)
                { // there is a previous element - inspect it
                  G4String inspectedElementType = inspectedElement->GetType();
                  if (inspectedElementType == "drift" || inspectedElementType == "dipolefringe" || inspectedElementType == "element")
                    {// leave keepGoing true here to keep going
                      // check extents first
                      BDSExtent extPipe  = inspectedElement->GetAcceleratorComponent()->GetExtent();
                      G4double  tiltPipe = inspectedElement->GetTilt();
                      if (accComponentTO) // could be nullptr
                        {extPipe = extPipe.Tilted(accComponentTO->GetTilt() - tiltPipe);}
                      if (extPipe.TransverselyGreaterThan(endPieceInnerExtent))
                        {
                          keepGoing             = false;
                          if (inspectedElementType != "element")
                            {driftsAreTooBigBefore = true;}
                        }
                      if (keepGoing)
                        {
                          availableLength += inspectedElement->GetChordLength();
                          driftIsFirstItem = inspectedElement == firstItem;
                        }
                    }
                  else
                    {
                      keepGoing = false;
                      auto previousNonDriftAccCompEndPiece = inspectedElement->GetAcceleratorComponent()->EndPieceAfter();
                      if (previousNonDriftAccCompEndPiece)
                        {previousNonDriftEndPieceL = previousNonDriftAccCompEndPiece->GetChordLength();}
                    } // not a drift - can't be used
                }
              else
                {keepGoing = false;} // quit the loop - no previous element
            }
          
          // don't place if there isn't enough space for BOTH endpieces - the previous one and this one
          if (requiredBeforeLength > (availableLength - previousNonDriftEndPieceL))
            {placeBefore = false;}

          // don't place if the drifts on either side are too big for the end pieces to fit around
          if (driftsAreTooBigBefore)
            {placeBefore = false;}

          // place the last item anyway even if there technically isn't space - either too short
          // a drift or the magnet is first in the line
          if ( (element == firstItem) || (driftIsFirstItem && !driftsAreTooBigBefore) )
            {placeBefore = true;}

          // If it's a circular machine the end piece will overlap with the teleporter or
          // terminator or previous element which we don't check against, so play it safe
          // and don't build it.
          if ( (element == firstItem) && circularMachine )
            {placeBefore = false;}

          // Now check if the coil volumes will overlap
          // This check is only done on before, as something can always be naturally added
          // afterwards - this just sets a precedence of after over before.
          if (placeBefore && !endPieces->empty() ) // only look backwards if we have already placed an end piece
            {
              // endPieces is a beam line of only end pieces
              BDSAcceleratorComponent* prevEndPiece = (endPieces->back())->GetAcceleratorComponent();
              // OF = outgoing face, IF = incoming face
              // outgoing is end of next element being added, incoming is front face previous already added
              BDSExtent extOF = endPieceBefore->GetExtent();
              BDSExtent extIF = prevEndPiece->GetExtent();
              G4ThreeVector oFNormal = endPieceBefore->InputFaceNormal();
              G4ThreeVector iFNormal = prevEndPiece->OutputFaceNormal();
              G4double lastEndPieceL = prevEndPiece->GetChordLength();
              G4double zSeparation   = availableLength - requiredBeforeLength - lastEndPieceL;
              G4bool willIntersect = BDS::WillIntersect(iFNormal, oFNormal, zSeparation, extIF, extOF);
              if (willIntersect)
                {placeBefore = false;}

              /*
              // 3d check
              auto thisEl = beamline->ProvideEndPieceElementBefore(endPieceBefore,
                                                                   currentIndex);

              G4bool willIntersect3D = endPieces->back()->Overlaps(thisEl);
              if (willIntersect3D)
                {
                  placeBefore = false;
                  delete thisEl;
                }
              */
            }
          if (placeBefore)
            { // provide a BDSBeamlineElement for the end piece w.r.t. the original beam line
              auto beforeEl = beamline->ProvideEndPieceElementBefore(endPieceBefore,
                                                                     currentIndex);
              endPieces->AddBeamlineElement(beforeEl); // append to end piece beam line
            }
        }

      if (placeAfter)
        {
          // search after current element until we hit another component that's not a drift.
          // ie work out the amount of drift space available. If we hit another element, check
          // if it has an end piece and take that length out of the available space. if
          // there's then available space for this end piece we place it.
          G4bool keepGoing = true;
          const BDSBeamlineElement* inspectedElement = element; // start with current element
          G4double             availableLength = 0;
          G4double       nextNonDriftEndPieceL = 0;
          G4bool               driftIsLastItem = false;
          BDSExtent        endPieceInnerExtent = endPieceAfter->GetInnerExtent();
          G4bool          driftsAreTooBigAfter = false;
          while (keepGoing)
            {
              inspectedElement = beamline->GetNext(inspectedElement);
              if (inspectedElement)
                { // there is a next element - inspect it
                  G4String inspectedElementType = inspectedElement->GetType();
                  if (inspectedElementType == "drift" || inspectedElementType == "dipolefringe" || inspectedElementType == "element")
                    {// leave keepGoing true here to keep going
                      // check extents first
                      BDSExtent extPipe  = inspectedElement->GetAcceleratorComponent()->GetExtent();
                      G4double  tiltPipe = inspectedElement->GetTilt();
                      if (accComponentTO) // could be nullptr
                        {extPipe = extPipe.Tilted(accComponentTO->GetTilt() - tiltPipe);}
                      if (extPipe.TransverselyGreaterThan(endPieceInnerExtent))
                        {
                          keepGoing            = false;
                          if (inspectedElementType != "element")
                            {driftsAreTooBigAfter = true;}
                        }
                      if (keepGoing)
                        {
                          availableLength += inspectedElement->GetChordLength();
                          driftIsLastItem = inspectedElement == lastItem;
                        }
                    }
                  else
                    {
                      keepGoing = false;
                      auto nextNonDriftAccCompEndPiece = inspectedElement->GetAcceleratorComponent()->EndPieceBefore();
                      if (nextNonDriftAccCompEndPiece)
                        {nextNonDriftEndPieceL = nextNonDriftAccCompEndPiece->GetChordLength();}
                    } // not a drift - can't be used
                }
              else
                {keepGoing = false;} // quit the loop - no previous element
            }

          // don't place if there isn't enough space for BOTH endpieces - this one and the upcoming one
          if (requiredAfterLength > (availableLength - nextNonDriftEndPieceL))
            {placeAfter = false;}

          // don't place if the drifts on either side are too big for the end pieces to fit around
          if (driftsAreTooBigAfter)
            {placeAfter = false;}
          
          // place the last item anyway even if there technically isn't space - either too short
          // a drift or the magnet is last in the line
          if ( (element == lastItem) || (driftIsLastItem && !driftsAreTooBigAfter) )
            {placeAfter = true;}

          if (placeAfter)
            { // provide a BDSBeamlineElement for the end piece w.r.t. the original beam line
              G4bool flip = endPieceAfter == endPieceBefore; // sometimes the after piece is a copy of the before one
              auto afterEl = beamline->ProvideEndPieceElementAfter(endPieceAfter,
                                                                   currentIndex,
                                                                   flip); // flip it round for placement
              endPieces->AddBeamlineElement(afterEl); // append to end piece beam line
            }
        }
      currentIndex++; // increment iterator index on beamline
    }

  return endPieces;
}
