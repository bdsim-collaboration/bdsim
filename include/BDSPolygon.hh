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
#ifndef BDSPOLYGON_H
#define BDSPOLYGON_H
#include "BDSInterpolatorType.hh"

#include "G4TwoVector.hh"
#include "G4Types.hh"

#include <iterator>
#include <list>
#include <vector>

class BDSExtent;
class BDSTiltOffset;

/**
 * @brief A 2D polygon made of line segments.
 *
 * @author Laurie Nevay
 */

class BDSPolygon
{
public:
  BDSPolygon() = delete;
  explicit BDSPolygon(const std::vector<G4TwoVector>& pointsIn);
  ~BDSPolygon() noexcept;
  /// Implement the copy constructor because we may have an extent object.
  BDSPolygon(const BDSPolygon& other);

  /// Enum to help us label points in a polygon for Boolean operations.
  enum class PointIs {in, out, intersection};

  /// Accessor.
  const std::vector<G4TwoVector>& Points() const {return points;}
  
  /// Return true if point inside this polygon. This assumes linear interpolation between
  /// points and ignores the interpolation type member variable. This is for ease of calculating
  /// normals.
  G4bool Inside(const G4TwoVector& point) const;

  /// Labelled version of Inside function where we return an enum. Doesn't return on boundary.
  BDSPolygon::PointIs InsideLabelled(const G4TwoVector& point) const;
  
  /// Return true if polygon is completely contained inside this one.
  /// If any lines intersect, this will return false.
  G4bool Inside(const BDSPolygon& other) const;

  /// What type of interpolation to use between points (e.g. linear, cubic).
  BDSInterpolatorType Interpolation() const {return interpolation;}
  
  BDSExtent Extent() const;           ///< Access the extent.
  G4double RadiusToEncompass() const; ///< Maximum radius of any point with no margin. Dynamically calculated on first access.
  G4double MinimumInscribedCricleRadius() const;  ///< Minimum radius of a circle that will fit inside the polygon.

  /// Return whether the axis-aligned bounding box given by ext lies fully within the polygon.
  G4bool EncompassesExtentXY(const BDSExtent& ext) const;
  /// Return whether the polygon will fit inside the axis-aligned bounding box ext.
  G4bool FitsInside(const BDSExtent& ext) const;

  /// Return true if any segments that make this polgyon intersect with
  /// any others. Optional arguments for indices of lines that intersect.
  G4bool SelfIntersecting(G4int* const indexOfIntersectionA = nullptr,
                          G4int* const indexOfIntersectionB = nullptr) const;
  
  /// Return a copy with a tilt and offset applied.
  BDSPolygon ApplyTiltOffset(const BDSTiltOffset& to) const;
  
  /// Expands the polygon points by 'value' along the direction of the
  /// vertex normal.
  BDSPolygon ExpandByValueUsingVertexNormals(G4double value) const;
  
  /// Same as above but done on this instance.
  void ExpandByValueUsingVertexNormalsInPlace(G4double value);
  
  /// Multiply each 2-vector point by scale. Return a new polygon.
  BDSPolygon ScaleByValue(G4double scale) const;
  
  /// Same as above but apply in place to the points in this instance.
  void ScaleByValueInPlace(G4double scale);
  
  // Boolean operations
  /// Calculate the union of 'other' polygon with this one and return as a copy.
  BDSPolygon Union(const BDSPolygon& other) const;
  
  /// Calculate subtraction of other from this polygon. This may produce more than
  /// one resultant polygon, therefore a vector is returned.
  std::vector<BDSPolygon*> Subtraction(const BDSPolygon& other) const;
  
  /// Calculate the intersection of other and this polygon. Throws BDSException
  /// if no valid intersection found - ie shapes not overlapping.
  BDSPolygon Intersection(const BDSPolygon& other) const;
  
  /// Resample the polygon and generate a new one using nPoints points.
  BDSPolygon InterpolateWithNPoints(unsigned int nPoints) const;

  ///@{ Iterator mechanics.
  typedef std::vector<G4TwoVector>::iterator               iterator;
  typedef std::vector<G4TwoVector>::const_iterator         const_iterator;
  typedef std::vector<G4TwoVector>::reverse_iterator       reverse_iterator;
  typedef std::vector<G4TwoVector>::const_reverse_iterator const_reverse_iterator;
  iterator               begin()        {return points.begin();}
  iterator               end()          {return points.end();}
  const_iterator         begin()  const {return points.begin();}
  const_iterator         end()    const {return points.end();}
  reverse_iterator       rbegin()       {return points.rbegin();}
  reverse_iterator       rend()         {return points.rend();}
  const_reverse_iterator rbegin() const {return points.rbegin();}
  const_reverse_iterator rend()   const {return points.rend();}
  G4bool                 empty()  const {return points.empty();}
  ///@}

  inline G4TwoVector front() const {return points.front();}
  inline G4TwoVector back()  const {return points.back();}
  std::vector<G4TwoVector>::size_type size() const {return points.size();}

  /// Algorithm to test if two straight line segments (segments are finite lines
  /// with a start and end point) intersect at any point. Returns 0 if no intersection,
  /// 1 if a unique intersection and 2 if the same line. This algorithm is from
  /// Geometric Tools for Computer Graphics by Philip J. Schneider David H. Eberly,
  /// ISBN 1-55860-594-0. Geant4 has a similar one in
  /// G4TessellatedGeometryAlgorithms::IntersectLineAndLineSegment2D, however they've
  /// parameterised it in a different way that's relative to the first point with gradients.
  static G4int SegmentsIntersect(const G4TwoVector& p1,
                                 const G4TwoVector& p2,
                                 const G4TwoVector& q1,
                                 const G4TwoVector& q2,
                                 G4TwoVector* intersectionPoint = nullptr);

protected:
  /// Simple structure of a 2D point (by pointer to avoid copy) and a label as defiend by enum.
  struct LabelledPoint
  {
    const G4TwoVector* point;
    PointIs label;
  };

  /// Generate a list of labelled points as to whether each point in 'test' is in 'reference'.
  /// If the optional G4int* is given, the number is updated with the number inside reference.
  static std::list<LabelledPoint> GenerateLabelled(const BDSPolygon& reference,
                                                   const BDSPolygon& test,
                                                   G4int* nInside = nullptr);
  
private:
  /// Shared traversal kernel for Union and Intersection. Builds the augmented
  /// point lists, then walks the boundary switching polygons at every intersection.
  /// Starting outside other (startInsideOther==false) yields the union boundary;
  /// starting inside other (true) yields the intersection boundary.
  BDSPolygon BooleanTraversal(const BDSPolygon& other, bool startInsideOther, bool reverseOther = false) const;

  /// Calculate the normal at each vertex (point) in the polygon. Populates the
  /// member vector vertexNormals. If it already exists, it will be deleted and
  /// calculated afresh.
  void CalculateVertexNormals() const;
  
  /// Calculate the determinant of 2x G4TwoVectors.
  G4double Determinant(const G4TwoVector& v1,
                       const G4TwoVector& v2) const;
  
  /// Polygon 2D coordinates.
  std::vector<G4TwoVector> points;
  
  /// Outward-pointing normals. Calculated only when needed.
  mutable std::vector<G4TwoVector>* vertexNormals;

  /// Cache of extent if calculated - initialised on first call to Extent().
  mutable BDSExtent* extent;
  
  /// Cache of maximum radius of any point.
  mutable G4double radiusToEncompass;
  mutable bool     recalculateRadiusToEncompass;

  /// Cache of minimum radius of any point.
  mutable G4double radiusInterior;
  mutable bool     recalculateRadiusInterior;

  /// Which interpolation scheme to use if interpolating between points.
  BDSInterpolatorType interpolation;
};

#endif
