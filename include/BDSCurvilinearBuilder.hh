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
#include "BDSBeamlineElement.hh"
#include "BDSUtilities.hh"

#include "globals.hh"

class BDSBeamline;
class BDSCurvilinearFactory;
class BDSSimpleComponent;

/**
 * @brief Factory for simple parallel geometry for curvilinear coordinates.
 *
 * This builds a beam line of geometry w.r.t. a beam line that can be used
 * for curvilinear coordinates.
 *
 * @author Laurie Nevay
 */

class BDSCurvilinearBuilder
{
public:
  BDSCurvilinearBuilder();
  ~BDSCurvilinearBuilder();

  /// Build a beam line of curvilinear geometry based on another beam line.
  BDSBeamline* BuildCurvilinearBeamLine1To1(BDSBeamline const* const beamline,
					    const G4bool circular);

  /// Build bridging volumes to join the curvilinear ones
  BDSBeamline* BuildCurvilinearBridgeBeamLine(BDSBeamline const* const beamline);

private:
  BDSCurvilinearBuilder(const BDSCurvilinearBuilder&) = delete;
  BDSCurvilinearBuilder& operator=(const BDSCurvilinearBuilder&) = delete;

  /// Return the minimum of the (default) member curvilinearRadius and the radiusTolerance
  /// member * the radius from the arc length and the bending angle. If nullptr give, returns
  /// curvilinearRadius.
  G4double CurvilinearRadius(const BDSBeamlineElement* el) const;

  /// Create a curvilinear element for a beam line that represents the curvilinear
  /// coordinates between startElement and finishElement. This creates a BDSSimpleComponent
  /// first then wraps it in a pre-made BDSBeamlineElement. This must ONLY be used for a range
  /// of elements with the same tilt for the correct coordinate frame to be produced. The tilt
  /// is taken from the first element and assumed to be the same for all. The index is the
  /// index it'l have in the curvilinear beam line.
  BDSBeamlineElement* CreateCurvilinearElement(const G4String&             elementName,
					       BDSBeamline::const_iterator startElement,
					       BDSBeamline::const_iterator finishElement,
					       G4int                       index,
					       G4double                    crRadius);

  /// Create the BDSBeamlineElement by wrapping a BDSSimpleComponent.
  BDSBeamlineElement* CreateElementFromComponent(BDSSimpleComponent*         component,
						 BDSBeamline::const_iterator startElement,
						 BDSBeamline::const_iterator finishElement,
						 G4int                       index);


  /// @{ Create a small section to extend a linear beam line.
  BDSBeamlineElement* CreateBonusSectionStart(BDSBeamline const* const beamline);
  BDSBeamlineElement* CreateBonusSectionEnd(BDSBeamline const* const beamline);
  /// @}

  /// Create a pre-made beam line element (because it's a non-continuous beam line) for
  /// a bridge section between two curvilinear volumes. Can cope if nextElement == end.
  /// numberOfUniqueComponents will be incremented if a new accelerator component is required.
  /// This is only done if the two components that are being bridged have angled faces.
  /// beamlineIndex is the index the element will have in the non-continuous beam line;
  /// the indices should be continuous. crRadius controls the radius of the component.
  BDSBeamlineElement* CreateBridgeSection(BDSAcceleratorComponent*    defaultBridge,
					  BDSBeamline::const_iterator element,
					  BDSBeamline::const_iterator nextElement,
					  BDSBeamline::const_iterator end,
					  G4int&                      numberOfUniqueComponents,
					  const G4int                 beamlineIndex,
					  G4double                    crRadius);

  /// Safely give a pointer to the previous and next items, excluding items below 0.1mm (ie
  /// short items such as fringes). Sets pointers to nullptr if at beginning or end.
  void PreviousAndNext(BDSBeamline::const_iterator it,
		       BDSBeamline::const_iterator startIt,
		       BDSBeamline::const_iterator endIt,
		       const BDSBeamlineElement*&  previous,
		       const BDSBeamlineElement*&  next) const;

  /// Create a single flat sided accelerator component for a small bridge volume. Intended
  /// to be reused as the default.
  BDSAcceleratorComponent* CreateDefaultBridgeComponent();

  /// Create a unique accelerator component for an element that's straight, but with a
  /// specific width.
  BDSAcceleratorComponent* CreateStraightBridgeComponent(G4double                    width,
							 G4int&                      numberOfUniqueComponents);
  
  /// Create a unique accelerator component for an element with angled faces.
  BDSAcceleratorComponent* CreateAngledBridgeComponent(BDSBeamline::const_iterator element,
						       G4int&                      numberOfUniqueComponents,
						       G4double                    suggestedRadius);

  /// Package an accelerator component into a beam line element w.r.t. two particular elements.
  /// Again, can cope with nextElement == end.
  BDSBeamlineElement* CreateBridgeElementFromComponent(BDSAcceleratorComponent*    component,
						       BDSBeamline::const_iterator element,
						       BDSBeamline::const_iterator nextElement,
						       BDSBeamline::const_iterator end,
						       const G4int                 beamlineIndex);

  /// Simple interrogation function to determine if an element has a finite angle or not.
  inline G4bool Angled(BDSBeamlineElement const* const element) const;

  /// Whether an element is too short for its own curvilinear volume.
  inline G4bool TooShort(BDSBeamlineElement const* const element) const;

  /// Length that was used for padding on the beam line we're building with respesct to.
  G4double paddingLength;
  G4double defaultBridgeLength;
  G4double curvilinearRadius; ///< Radius for curvilinear geometry.
  G4double radiusTolerance;
  G4double bonusChordLength;  ///< Length of any possible bonus section added to beginning and end.

  /// Factory to build curvilinear geometry.
  BDSCurvilinearFactory* factory;
};

inline G4bool BDSCurvilinearBuilder::Angled(BDSBeamlineElement const* const element) const
{return BDS::IsFinite(element->GetAngle());}
