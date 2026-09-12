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
#ifndef BDSEXTENT_H
#define BDSEXTENT_H

#include <ostream>
#include <utility> // for pair and pair relational operators
#include <vector>

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"

class BDSParticleCoords;
class BDSTiltOffset;

/**
 * @brief Holder for +- extents in 3 dimensions.
 * 
 * @author Laurie Nevay
 */

class BDSExtent
{
public:
  /// Default constructor gives 0 in all extents - typically unphysical.
  BDSExtent();
  /// Construct with pairs of extents
  BDSExtent(const std::pair<G4double, G4double>& extXIn,
	    const std::pair<G4double, G4double>& extYIn,
	    const std::pair<G4double, G4double>& extZIn);
  /// Construct with individual extents
  BDSExtent(G4double extXNegIn, G4double extXPosIn,
	    G4double extYNegIn, G4double extYPosIn,
	    G4double extZNegIn, G4double extZPosIn);
  /// Symmetric variant - assumed to be +/- same amount
  BDSExtent(G4double extXIn, G4double extYIn, G4double extZIn);
  /// Threevectors of positive and negative extents
  BDSExtent(G4ThreeVector extInNeg, G4ThreeVector extInPos);
  /// Symmetric from three vector.
  explicit BDSExtent(G4ThreeVector extIn);
	    
  virtual ~BDSExtent();

  /// @{ Accessor
  inline std::pair<G4double, G4double> ExtentX() const {return std::make_pair(extXNeg, extXPos);}
  inline std::pair<G4double, G4double> ExtentY() const {return std::make_pair(extYNeg, extYPos);}
  inline std::pair<G4double, G4double> ExtentZ() const {return std::make_pair(extZNeg, extZPos);}
  
  inline G4double XNeg() const {return extXNeg;}
  inline G4double XPos() const {return extXPos;}
  inline G4double YNeg() const {return extYNeg;}
  inline G4double YPos() const {return extYPos;}
  inline G4double ZNeg() const {return extZNeg;}
  inline G4double ZPos() const {return extZPos;}
  
  inline G4ThreeVector ExtentPositive() const
  {return G4ThreeVector(extXPos, extYPos, extZPos);}

  inline G4ThreeVector ExtentNegative() const
  {return G4ThreeVector(extXNeg, extYNeg, extZNeg);}

  /// All 8 boundary points of the bounding box.
  std::vector<G4ThreeVector> AllBoundaryPoints() const;
  /// @}

  /// @{ The difference in a dimension.
  inline G4double DX() const {return extXPos - extXNeg;}
  inline G4double DY() const {return extYPos - extYNeg;}
  inline G4double DZ() const {return extZPos - extZNeg;}
  /// @}

  /// @{ Comparison operator.
  inline G4bool operator< (const BDSExtent& r) const;
  inline G4bool operator> (const BDSExtent& r) const {return r < (*this);}
  inline G4bool operator<=(const BDSExtent& r) const {return !((*this) > r);}
  inline G4bool operator>=(const BDSExtent& r) const {return !((*this) < r);}
  /// @}

  /// @{ Equality operator.
  inline G4bool operator==(const BDSExtent& r) const;
  inline G4bool operator!=(const BDSExtent& r) const {return !((*this) == r);}
  /// @}

  /// @{ Comparison operator for x,y only. Ignores z (length).
  inline G4bool TransverselyLessThan(const BDSExtent& r) const;
  inline G4bool TransverselyGreaterThan(const BDSExtent& r)   const {return !(this->TransverselyLessThan(r));}
  inline G4bool TransverselyLessEquals(const BDSExtent& r)    const {return !(this->TransverselyGreaterThan(r));}
  inline G4bool TransverselyGreaterEquals(const BDSExtent& r) const {return !(this->TransverselyLessThan(r));}
  /// @}

  /// Return whether the extent encompasses the point. True if point lies inside the extent.
  G4bool Encompasses(const G4ThreeVector& point) const;

  /// Return whether the boundary points given by a set of extents lie within this extent. True
  /// if all points lie within this extent; false if even one lies outside.
  G4bool Encompasses(const BDSExtent& other) const;

  /// Return whether the extent encompasses the point.  Similar, but with separate x,y,z coordinates.
  G4bool Encompasses(G4double x,
		     G4double y,
		     G4double z) const {return Encompasses(G4ThreeVector(x,y,z));}

  G4bool Encompasses(const BDSParticleCoords& coords) const;
  
  /// Provide a new copy of this extent but rotated along Z by a given tilt angle.
  BDSExtent Tilted(G4double angle) const;

  /// @{ Provide a new copy of this extent with an offset applied.
  BDSExtent Translate(const G4ThreeVector& offset) const {return Translate(offset.x(), offset.y(), offset.z());}
  BDSExtent Translate(G4double dx, G4double dy, G4double dz) const;
  /// @}

  /// Provide a new copy of this extent with both a tilt and an offset applied.
  BDSExtent TiltOffset(const BDSTiltOffset* tiltOffset) const;

  /// Output stream.
  friend std::ostream& operator<< (std::ostream &out, BDSExtent const &ext);
  
  /// Return the maximum absolute value considering all dimensions.
  G4double MaximumAbs() const;

  /// Return the minimum absolute value considering all dimensions.
  G4double MinimumAbs() const;

  G4double MaximumX() const {return std::max(std::abs(extXNeg), std::abs(extXPos));}
  G4double MaximumY() const {return std::max(std::abs(extYNeg), std::abs(extYPos));}
  G4double MaximumZ() const {return std::max(std::abs(extZNeg), std::abs(extZPos));}

  /// Return the maximum absolute value considering only x,y.
  G4double MaximumAbsTransverse() const;

  /// Return the minimum absolute value considering only x,y.
  G4double MinimumAbsTransverse() const;
  
  /// Return a radius that would encompass the maximum x,y extent.
  G4double TransverseBoundingRadius() const;

  /// Return a copy expanded in all dimensions by the given margin.
  BDSExtent ExpandBy(G4double margin) const;

  /// Return a copy expanded in x and y by the given margin.
  BDSExtent ExpandTransverselyBy(G4double margin) const;

  /// Expand this extent to encompass the other one. Takes the max of each extent
  /// in this and the other.
  void ExpandToEncompass(const BDSExtent& other);
  
private:
  /// @{ Extent.
  G4double extXNeg;
  G4double extXPos;
  G4double extYNeg;
  G4double extYPos;
  G4double extZNeg;
  G4double extZPos;
  /// @}
};

inline G4bool BDSExtent::operator<(const BDSExtent& r) const
{
  G4bool xOK = (extXNeg > r.extXNeg) && (extXPos < r.extXPos);
  G4bool yOK = (extYNeg > r.extYNeg) && (extYPos < r.extYPos);
  G4bool zOK = (extZNeg > r.extZNeg) && (extZPos < r.extZPos);
  return xOK && yOK && zOK;
}

inline G4bool BDSExtent::operator==(const BDSExtent& r) const
{
  G4bool xOK = (extXNeg == r.extXNeg) && (extXPos == r.extXPos);
  G4bool yOK = (extYNeg == r.extYNeg) && (extYPos == r.extYPos);
  G4bool zOK = (extZNeg == r.extZNeg) && (extZPos == r.extZPos);
  return xOK && yOK && zOK;
}

inline G4bool BDSExtent::TransverselyLessThan(const BDSExtent& r) const
{
  G4bool xOK = (extXNeg > r.extXNeg) && (extXPos < r.extXPos);
  G4bool yOK = (extYNeg > r.extYNeg) && (extYPos < r.extYPos);
  return xOK && yOK;
}

namespace BDS
{
  /// Returns the extent which is the greatest extent in all six directions.
  BDSExtent MaximumCombinedExtent(const BDSExtent& first, const BDSExtent& second);
}

#endif
