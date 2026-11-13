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
#ifndef BDSEXTENTGLOBAL_H
#define BDSEXTENTGLOBAL_H

#include "BDSExtent.hh"

#include <ostream>
#include <utility> // for pair and pair relational operators
#include <vector>

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"

class BDSTiltOffset;

/**
 * @brief Holder for +- extents in 3 dimensions with a rotation and translation.
 * 
 * Holds both a local extent as a 3D cuboid in local xyz as well as a rotation
 * and translation to give an extent in a different frame.
 * 
 * @author Laurie Nevay
 */

class BDSExtentGlobal: public BDSExtent
{
public:
  /// Default constructor gives 0 in all extents - typically unphysical.
  BDSExtentGlobal();

  /// Constructor with extent in only local frame + transform. We use only the
  /// G4Transform3D to avoid ambiguity about active or passive transforms.
  explicit BDSExtentGlobal(const BDSExtent&     extentIn,
		  const G4Transform3D& transformIn = G4Transform3D::Identity);

  /// Constructor with extents already in global frame. The transform here is the
  /// transform that the points were created with respect to. It is not applied to the points.
  /*
  BDSExtentGlobal(G4double extXNegIn, G4double extXPosIn,
		  G4double extYNegIn, G4double extYPosIn,
		  G4double extZNegIn, G4double extZPosIn,
		  const G4Transform3D& transformIn);
  */
	    
  virtual ~BDSExtentGlobal();

  /// @{ Accessor
  inline std::pair<G4double, G4double> ExtentXGlobal() const {return std::make_pair(extXNegG, extXPosG);}
  inline std::pair<G4double, G4double> ExtentYGlobal() const {return std::make_pair(extYNegG, extYPosG);}
  inline std::pair<G4double, G4double> ExtentZGlobal() const {return std::make_pair(extZNegG, extZPosG);}
  
  inline G4double XNegGlobal() const {return extXNegG;}
  inline G4double XPosGlobal() const {return extXPosG;}
  inline G4double YNegGlobal() const {return extYNegG;}
  inline G4double YPosGlobal() const {return extYPosG;}
  inline G4double ZNegGlobal() const {return extZNegG;}
  inline G4double ZPosGlobal() const {return extZPosG;}
  
  inline G4ThreeVector ExtentPositiveGlobal() const
  {return G4ThreeVector(extXPosG, extYPosG, extZPosG);}

  inline G4ThreeVector ExtentNegativeGlobal() const
  {return G4ThreeVector(extXNegG, extYNegG, extZNegG);}

  /// All 8 boundary points of the bounding box.
  std::vector<G4ThreeVector> AllBoundaryPointsGlobal() const;
  /// @}

  G4bool Overlaps(const BDSExtentGlobal& other) const;

  /// @{ The difference in a dimension.
  inline G4double DXGlobal() const {return extXPosG - extXNegG;}
  inline G4double DYGlobal() const {return extYPosG - extYNegG;}
  inline G4double DZGlobal() const {return extZPosG - extZNegG;}
  /// @}

  /// @{ Comparison operator for x,y only. Ignores z (length).
  inline G4bool TransverselyLessThanGlobal(const BDSExtentGlobal& r) const;
  inline G4bool TransverselyGreaterThanGlobal(const BDSExtentGlobal& r)   const {return !(this->TransverselyLessThanGlobal(r));}
  inline G4bool TransverselyLessEqualsGlobal(const BDSExtentGlobal& r)    const {return !(this->TransverselyGreaterThanGlobal(r));}
  inline G4bool TransverselyGreaterEqualsGlobal(const BDSExtentGlobal& r) const {return !(this->TransverselyLessThanGlobal(r));}
  /// @}
  
  /// Return whether the extent encompasses the point.  Similar, but with separate x,y,z coordinates.
  G4bool EncompassesGlobal(G4double x,
			   G4double y,
			   G4double z) const;

  /// Return whether the extent encompasses another extent
  G4bool Encompasses(const BDSExtentGlobal& otherExtent);

  /// Return whether the extent encompasses multiple other extents
  G4bool Encompasses(const std::vector<BDSExtentGlobal>& otherExtents);

  /// Get the maximum extent absolute in each dimension
  G4ThreeVector GetMaximumExtentAbsolute() const;

  /// @{ Provide a new copy of this extent with an offset applied.
  BDSExtentGlobal TranslateGlobal(G4ThreeVector offset) const
  {return TranslateGlobal(offset.x(), offset.y(), offset.z());}
  BDSExtentGlobal TranslateGlobal(G4double dx, G4double dy, G4double dz) const;
  /// @}

  /// Return a copy of this extent but expanded to encompass another global extent.
  BDSExtentGlobal ExpandToEncompass(const BDSExtentGlobal& other) const;
  
  /// Output stream.
  friend std::ostream& operator<< (std::ostream &out, BDSExtentGlobal const &ext);
  
  /// Return the maximum absolute value considering all dimensions.
  G4double MaximumAbsGlobal() const;
  
private:
  G4Transform3D transform;
  
  /// @{ Extent.
  G4double extXNegG;
  G4double extXPosG;
  G4double extYNegG;
  G4double extYPosG;
  G4double extZNegG;
  G4double extZPosG;
  /// @}
};

inline G4bool BDSExtentGlobal::TransverselyLessThanGlobal(const BDSExtentGlobal& r) const
{
  G4bool xOK = (extXNegG > r.extXNegG) && (extXPosG < r.extXPosG);
  G4bool yOK = (extYNegG > r.extYNegG) && (extYPosG < r.extYPosG);
  return xOK && yOK;
}

#endif
