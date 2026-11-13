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
#ifndef BDSBEAMLINE_H
#define BDSBEAMLINE_H

#include "globals.hh" // geant4 globals / types
#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"

#include "BDSSamplerType.hh"
#include "BDSExtentGlobal.hh"

#include <iterator>
#include <ostream>
#include <set>
#include <vector>

class BDSAcceleratorComponent;
class BDSBeamlineElement;
class BDSBeamlineIntegral;
class BDSSamplerInfo;
class BDSSimpleComponent;
class BDSTiltOffset;
class BDSTransform3D;
namespace CLHEP {
  class HepRotation;
}
typedef CLHEP::HepRotation G4RotationMatrix;

/**
 * @brief A vector of BDSBeamlineElement instances - a beamline.
 * 
 * This will calculate and construct a beamline as BDSAcceleratorComponents
 * are added in sequence - ie it calculates their placement positions and 
 * rotations with respect to the start of the beamline as well as their s
 * position in curvilinear coordinates.
 * 
 * Note, this is not a singleton as geometry hierarchy can be introduced
 * by placing beamline components inside parent volumes and therefore creating
 * a new beamline of parents. It can also be used to create multiple beam lines.
 * 
 * @author Laurie Nevay
 */

class BDSBeamline
{
private:
  /// Typedefs up first so we can declare public iterators.
  typedef std::vector<BDSBeamlineElement*> BeamlineVector;

  /// Vector of beam line elements - the data.
  BeamlineVector beamline;
  
public:  
  /// assignment and copy constructor not implemented nor used.
  BDSBeamline& operator=(const BDSBeamline&) = delete;
  BDSBeamline(BDSBeamline&) = delete;
  
  /// Versatile basic constructor that allows a finite position and rotation to be applied
  /// at the beginning of the beamline in global coordinates. Remember the maximum
  /// extents of the beamline will also be displaced. The default constructor is in effect
  /// achieved via defaults.
  BDSBeamline(const G4ThreeVector& initialGlobalPosition = G4ThreeVector(0,0,0),
              G4RotationMatrix*    initialGlobalRotation = nullptr,
              G4double             initialSIn            = 0.0);

  /// Constructor with transform instance that uses other constructor.
  explicit BDSBeamline(G4Transform3D initialTransform,
                       G4double      initialSIn       = 0.0);
  
  ~BDSBeamline();

  /// Add a component, but check to see if it can be dynamically upcast to a line
  /// in which case, loop over it and apply
  /// AddSingleComponent(BDSAcceleratorComponent* component) to each component
  /// Returns vector of components added
  void AddComponent(BDSAcceleratorComponent*  component,
                    BDSTiltOffset*  tiltOffset  = nullptr,
                    BDSSamplerInfo* samplerInfo = nullptr,
                    const BDSBeamlineIntegral* beamlineIntegral = nullptr);

  /// Apply a Transform3D rotation and translation to the reference
  /// coordinates. Special method for the special case of unique component
  /// Transform3D. Modifies the end rotation and position of the last element
  /// in the beamline. Note this applies the offset dx,dy,dz first, then the rotation
  /// of coordinates
  void ApplyTransform3D(BDSTransform3D* component);

  /// Add a pre-assembled beam line element. In this case, the coordinates will have been
  /// calculated external to this class and as such, it's the responsibility of the
  /// developer to make sure the coordinates are correct and do not cause overlaps. This
  /// will be useful for tunnel construction for example or for a non-contiguous beamline.
  /// Subsequent components added via the AddComponent() method will be appended in the usual
  /// way to the end coordinates of this element.
  void AddBeamlineElement(BDSBeamlineElement* element);

  /// Iterate over the beamline and print out the name, position, rotation
  /// and s position of each beamline element
  void PrintAllComponents(std::ostream& out) const;

  /// Once the beamline element has been constructed and all positions and rotations
  /// use these to update the world extent of this beam line.
  void UpdateExtents(BDSBeamlineElement* element);

  /// Return a reference to the element at i
  inline const BDSBeamlineElement* at(int iElement) const { return beamline.at(iElement);}

  /// Return a reference to the first element
  inline const BDSBeamlineElement* GetFirstItem() const {return front();} 

  /// Return a reference to the last element
  inline const BDSBeamlineElement* GetLastItem() const {return back();}

  /// Get the ith placement of an element in the beam line. Returns null pointer if not found.
  const BDSBeamlineElement* GetElement(G4String acceleratorComponentName, G4int i = 0) const;

  /// Get the transform to the centre of the ith placement of element by name.  Uses
  /// GetElement(). Exits if no such element found.
  G4Transform3D GetTransformForElement(const G4String& acceleratorComponentName, G4int i = 0) const;
  
  /// Get the total length of the beamline - the sum of the chord length of each element
  inline G4double     GetTotalChordLength() const {return totalChordLength;}

  /// Get the total ARC length for the beamline - ie the maximum s position
  inline G4double     GetTotalArcLength() const {return totalArcLength;}

  /// Get the total angle of the beamline
  inline G4double GetTotalAngle() const {return totalAngle;}

  /// Check if the sum of the angle of all elements is two pi
  G4bool ElementAnglesSumToCircle() const;

  /// Get the number of elements
  BeamlineVector::size_type size() const {return beamline.size();}

  /// Get the maximum positive extent in all dimensions  
  G4ThreeVector GetMaximumExtentPositive() const {return maximumExtentPositive;}

  /// Get the maximum negative extent in all dimensions
  G4ThreeVector GetMaximumExtentNegative() const {return maximumExtentNegative;}

  /// Get the maximum extent absolute in each dimension
  G4ThreeVector GetMaximumExtentAbsolute() const;

  /// Get the global extents for this beamline
  BDSExtentGlobal GetExtentGlobal() const;

  ///@{ Iterator mechanics
  typedef BeamlineVector::iterator       iterator;
  typedef BeamlineVector::const_iterator const_iterator;
  typedef BeamlineVector::reverse_iterator reverse_iterator;
  typedef BeamlineVector::const_reverse_iterator const_reverse_iterator;
  iterator               begin()        {return beamline.begin();}
  iterator               end()          {return beamline.end();}
  const_iterator         begin()  const {return beamline.begin();}
  const_iterator         end()    const {return beamline.end();}
  reverse_iterator       rbegin()       {return beamline.rbegin();}
  reverse_iterator       rend()         {return beamline.rend();}
  const_reverse_iterator rbegin() const {return beamline.rbegin();}
  const_reverse_iterator rend()   const {return beamline.rend();}
  G4bool                 empty()  const {return beamline.empty();}
  ///@}

  /// Get the local to global transform for curvilinear coordinates
  /// to global coordinates. 0,0 transverse position by default. Optionally returns
  /// the index of the found element in the beam line (by reference variable).
  G4Transform3D GetGlobalEuclideanTransform(G4double s,
                                            G4double x = 0,
                                            G4double y = 0,
                                            G4int* indexOfFoundElement = nullptr) const;

  /// Return the element in this beam line according to a given s coordinate.
  const BDSBeamlineElement* GetElementFromGlobalS(G4double S,
                                                  G4int* indexOfFoundElement = nullptr) const;
  
  /// Returns an iterator to the beamline element at s.
  const_iterator FindFromS(G4double S) const;

  /// Get the global s position of each element all in one - used for histograms.
  /// For convenience, s positions are converted to metres in this function.
  std::vector<G4double> GetEdgeSPositions() const;

  G4double GetSMinimum() const {return sInitial;}
  G4double GetSMaximum() const {return sMaximum;}

  /// Return a pointer to the previous element. First this beamline is
  /// searched for the vector. If there is no such element or no previous
  /// element because it's the beginning, then a nullptr is returned. The
  /// caller should test on this.
  const BDSBeamlineElement* GetPrevious(const BDSBeamlineElement* element) const;
  const BDSBeamlineElement* GetNext(const BDSBeamlineElement* element) const;

  const BDSBeamlineElement* GetPrevious(G4int index) const;
  const BDSBeamlineElement* GetNext(G4int index) const;
  
  // Accessors in a similar style to std::vector
  /// Return a reference to the first element
  inline BDSBeamlineElement* front() const {return beamline.front();}
  /// Return a reference to the last element
  inline BDSBeamlineElement* back() const {return beamline.back();}
  
  /// output stream
  friend std::ostream& operator<< (std::ostream &out, BDSBeamline const &bl);

  /// Feedback about memory consumption for this beamline instance - container size,
  /// size of all BDSBeamlineElement() and size of all BDSAcceleratorComponent() stored.
  void PrintMemoryConsumption() const;

  BDSBeamlineElement* ProvideEndPieceElementBefore(BDSSimpleComponent* endPiece,
                                                   G4int    index) const;
  /// Calculate the placements for an end piece w.r.t. a particlar beam line element
  /// The optional flip flag is used for when the 'before' piece is used again and
  /// must be rotated.
  BDSBeamlineElement* ProvideEndPieceElementAfter(BDSSimpleComponent* endPiece,
                                                  G4int               index,
                                                  G4bool              flip = false) const;

  /// Whether the supplied index will lie within the beam line vector.
  G4bool IndexOK(G4int index) const;

  /// Access the padding length between each element added to the beamline.
  static G4double PaddingLength() {return paddingLength;}

  /// Return vector of indices for this beam line where element of type name 'type' is found.
  std::vector<G4int> GetIndicesOfElementsOfType(const G4String& type) const;

  /// Apply above function to get a set of types. These are in order as they appear in
  /// the beam line.
  std::vector<G4int> GetIndicesOfElementsOfType(const std::set<G4String>& types) const;

  /// Return indices in order of ecol, rcol, jcol and crystalcol elements.
  std::vector<G4int> GetIndicesOfCollimators() const;
  
private:
  /// Add a single component and calculate its position and rotation with respect
  /// to the beginning of the beamline. Returns pointer to the component added.
  void AddSingleComponent(BDSAcceleratorComponent* component,
                          BDSTiltOffset*           tiltOffset  = nullptr,
                          BDSSamplerInfo*          samplerInfo = nullptr,
                          const BDSBeamlineIntegral* beamlineIntegral = nullptr);
  
  /// Register the fully created element to a map of names vs element pointers. Used to
  /// look up transforms by name.
  void RegisterElement(BDSBeamlineElement* element);

  G4double sInitial; ///< Cache the initial S so we can tell if a requested S is too low.
  G4double sMaximum;

  /// Sum of all chord lengths
  G4double totalChordLength;
  /// Sum of all arc lengths
  G4double totalArcLength;
  /// Sum of all angles
  G4double totalAngle;

  G4ThreeVector maximumExtentPositive; ///< maximum extent in the positive coordinates in each dimension
  G4ThreeVector maximumExtentNegative; ///< maximum extent in the negative coordinates in each dimension

  /// Current reference rotation at the end of the previous element
  G4RotationMatrix* previousReferenceRotationEnd;

  /// Current reference position at the end of the previous element
  G4ThreeVector previousReferencePositionEnd;

  /// Current s coordinate at the end of the previous element
  G4double previousSPositionEnd;

  /// Flag to remember whether a BDSTransform3D which is not a real volume
  /// has been applied to make a discrete change in the beam line or not.
  G4bool transformHasJustBeenApplied;

  /// The gap added for padding between each component.
  static G4double paddingLength;

  /// Map of objects by placement name stored in this beam line.
  std::map<G4String, BDSBeamlineElement*> components;

  /// Vector of s coordinates at the end of each element. This is intended
  /// so that an iterator pointing to the s position will be the correct
  /// index for the beamline element in the main BDSBeamlineVector element.
  /// This is filled in order so it's sorted by design.
  std::vector<G4double> sEnd;
};

#endif
