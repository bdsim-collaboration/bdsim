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
#ifndef BDSARRAY4DCOORDS_H
#define BDSARRAY4DCOORDS_H

#include "BDSArray4D.hh"
#include "BDSDimensionType.hh"
#include "BDSFourVector.hh"

#include "globals.hh"

#include <array>
#include <ostream>

class BDSExtent;

/**
 * @brief Overlay of 4D array that provides uniform only spatial coordinate mapping.
 * 
 * This class overlays (through inheritance) a BDSArray4D instance and provides
 * a mapping from array index coordinates to real spatial coordinates that are real
 * numbers.
 * 
 * Various interfaces are provided to find the nearest entry in an array or convert
 * to the coordinate system. This may seem a little more complicated than is perhaps
 * required, but the flexibility is required for interpolators that will need to 
 * query the array in both array and spatial coordinates.
 * 
 * @author Laurie Nevay
 */

class BDSArray4DCoords: public BDSArray4D
{
public:
  /// No default constructor as the array is not adjustable after construction and
  /// therefore the size must be known at construction time.
  BDSArray4DCoords() = delete;
  
  /// Constructor similar to BDSArray4D but with spatial limits in each dimension.
  /// The distance between the UNIFORMLY spaced data in spatial coordinates is
  /// calculated using the extents and the number of entries.
  BDSArray4DCoords(G4int nXIn, G4int nYIn, G4int nZIn, G4int nTIn,
		   G4double xMinIn, G4double xMaxIn,
		   G4double yMinIn, G4double yMaxIn,
		   G4double zMinIn, G4double zMaxIn,
		   G4double tMinIn, G4double tMaxIn,
                   BDSDimensionType xDimensionIn = BDSDimensionType::x,
                   BDSDimensionType yDimensionIn = BDSDimensionType::y,
                   BDSDimensionType zDimensionIn = BDSDimensionType::z,
                   BDSDimensionType tDimensionIn = BDSDimensionType::t);

  virtual ~BDSArray4DCoords(){;} 

  /// @{ The distance in spatial coordinates between any two points in the array.
  inline G4double XStep() const {return xStep;}
  inline G4double YStep() const {return yStep;}
  inline G4double ZStep() const {return zStep;}
  inline G4double TStep() const {return tStep;}
  /// @}

  /// Access the smallest spatial step between any two points in any dimension.
  /// Includes time dimension and assumes the speed of light for the distance.
  inline G4double SmallestSpatialStep() const {return smallestSpatialStep;}

  /// Whether the spatial coordinates lie outside the range of the array or not.
  virtual G4bool OutsideCoords(G4double x,
			       G4double y,
			       G4double z,
			       G4double t) const;

  /// Whether the spatial coordinates lie outside the range of the array or not and
  /// warn and exit if so. Uses OutsideCoords but warns and exits if the coordinates
  /// are outside the array.
  virtual void OutsideCoordsWarn(G4double x,
				 G4double y,
				 G4double z,
				 G4double t) const;

  /// @{ Not much point in being both virtual and inline (in our use case) but has to be virtual.
  virtual G4double ArrayCoordsFromX(G4double x) const {return (x - xMin) / xStep;}
  virtual G4double ArrayCoordsFromY(G4double y) const {return (y - yMin) / yStep;}
  virtual G4double ArrayCoordsFromZ(G4double z) const {return (z - zMin) / zStep;}
  virtual G4double ArrayCoordsFromT(G4double t) const {return (t - tMin) / tStep;}
  /// @}
  
  /// @{ Utility version to forward to individual function.
  void ArrayCoordsFromXY(G4double& x, G4double& xArr,
                         G4double& y, G4double& yArr) const
  {xArr = ArrayCoordsFromX(x); yArr = ArrayCoordsFromY(y);}
  void ArrayCoordsFromXYZ(G4double& x, G4double& xArr,
                          G4double& y, G4double& yArr,
                          G4double& z, G4double& zArr) const
  {xArr = ArrayCoordsFromX(x); yArr = ArrayCoordsFromY(y); zArr = ArrayCoordsFromZ(z);}
  void ArrayCoordsFromXYZT(G4double& x, G4double& xArr,
                           G4double& y, G4double& yArr,
                           G4double& z, G4double& zArr,
                           G4double& t, G4double& tArr) const
  {xArr = ArrayCoordsFromX(x); yArr = ArrayCoordsFromY(y); zArr = ArrayCoordsFromZ(z); tArr = ArrayCoordsFromT(t);}
  /// @}

  /// Convenience function to easily get array coords in all dimensions at once.
  inline BDSFourVector<G4double> ArrayCoordsFromXYZT(G4double x,
						     G4double y,
						     G4double z,
						     G4double t) const
  {
    return BDSFourVector<G4double>(ArrayCoordsFromX(x),
				   ArrayCoordsFromY(y),
				   ArrayCoordsFromZ(z),
				   ArrayCoordsFromT(t));
  }

  /// @{ Return spatial value from a continuous array coordinate in one dimension.
  inline G4double XFromArrayCoords(G4double xCoord) const {return xMin + xCoord*xStep;}
  inline G4double YFromArrayCoords(G4double yCoord) const {return yMin + yCoord*yStep;}
  inline G4double ZFromArrayCoords(G4double zCoord) const {return zMin + zCoord*zStep;}
  inline G4double TFromArrayCoords(G4double tCoord) const {return tMin + tCoord*tStep;}
  /// @}

  /// Convenience function to get the spatial coordinates from continuous array coordinates
  /// for all dimensions at once.
  inline BDSFourVector<G4double> XYZTFromArrayCoords(G4double x,
						     G4double y,
						     G4double z,
						     G4double t) const
  {
    return BDSFourVector<G4double>(XFromArrayCoords(x),
				   YFromArrayCoords(y),
				   ZFromArrayCoords(z),
				   TFromArrayCoords(t));
  }

  /// @{ Not much point in being both virtual and inline (in our use case) but has to be virtual.
  virtual G4int NearestX(G4double x) const {return (G4int)round((x - xMin) / xStep);}
  virtual G4int NearestY(G4double y) const {return (G4int)round((y - yMin) / yStep);}
  virtual G4int NearestZ(G4double z) const {return (G4int)round((z - zMin) / zStep);}
  virtual G4int NearestT(G4double t) const {return (G4int)round((t - tMin) / tStep);}
  /// @}

  /// Return the index of the nearest field value in space.
  inline BDSFourVector<G4int> NearestXYZT(G4double x,
					  G4double y,
					  G4double z,
					  G4double t) const
  {
    return BDSFourVector<G4int>(NearestX(x),
				NearestY(y),
				NearestZ(z),
				NearestT(t));
  }

  /// Return the index of the nearest field value in space via four-vector.
  inline BDSFourVector<G4int> NearestXYZT(const BDSFourVector<G4double>& pos) const
  {
    return BDSFourVector<G4int>(NearestX(pos[0]),
				NearestY(pos[1]),
				NearestZ(pos[2]),
				NearestT(pos[3]));
  }
  
  /// Extract 2x2x2x2 points lying around coordinate x.
  virtual void ExtractSection2x2x2x2(G4double x,
                                     G4double y,
                                     G4double z,
                                     G4double t,
                                     BDSFieldValue (&localData)[2][2][2][2],
                                     G4double& xFrac,
                                     G4double& yFrac,
                                     G4double& zFrac,
                                     G4double& tFrac) const;
  
  /// Extract 4x4x4x4 points lying around coordinate x.
  virtual void ExtractSection4x4x4x4(G4double x,
                                     G4double y,
                                     G4double z,
                                     G4double t,
                                     BDSFieldValue (&localData)[4][4][4][4],
                                     G4double& xFrac,
                                     G4double& yFrac,
                                     G4double& zFrac,
                                     G4double& tFrac) const;
  
  /// Extract nearest field value from array.
  virtual BDSFieldValue ExtractNearest(G4double x,
                                       G4double y = 0,
                                       G4double z = 0,
                                       G4double t = 0) const;

  /// Overloaded output function from BDSArray4D. Prints spatial map parameters, then
  /// underlying array.
  virtual std::ostream& Print(std::ostream& out) const;

  /// Output stream.
  friend std::ostream& operator<< (std::ostream& out, BDSArray4DCoords const &a);
  
  /// Return the SPATIAL (only) extent of this field without any offset. Ignores time.
  virtual BDSExtent Extent() const;
  
  inline G4double XMin() const {return xMin;}
  inline G4double YMin() const {return yMin;}
  inline G4double ZMin() const {return zMin;}
  inline G4double TMin() const {return tMin;}
  inline G4double XMax() const {return xMax;}
  inline G4double YMax() const {return yMax;}
  inline G4double ZMax() const {return zMax;}
  inline G4double TMax() const {return tMax;}
  
  /// @{ Accessor for each dimension label. e.g. array 'x' = spatial z.
  inline BDSDimensionType FirstDimension()  const {return xDimension;}
  inline BDSDimensionType SecondDimension() const {return yDimension;}
  inline BDSDimensionType ThirdDimension()  const {return zDimension;}
  inline BDSDimensionType FourthDimension() const {return tDimension;}
  /// @}

  /// Whether the array depends on the time coordinate. Can be determined
  /// differently for each derived class.
  virtual G4bool TimeVarying() const {return timeVarying;}
  
  /// Return the index to be used in array arguments for a given spatial dimension.
  /// e.g. DimensionIndex((spatial) z) -> index 0 for 1D array holding spatial z.
  G4int DimensionIndex(BDSDimensionType spatialDimension) const;

  /// Access all indices at once.
  inline std::array<G4int, 4> ArrayToSpatialDimensionIndices() const {return dimensions;}

protected:
  /// Build up an array of ints based on the order of dimensions stored in the array.
  void BuildDimensionIndex();
  
  /// @{ Dimension parameter - protected for derived class access.
  G4double xMin;
  G4double xMax;
  G4double yMin;
  G4double yMax;
  G4double zMin;
  G4double zMax;
  G4double tMin;
  G4double tMax;

  G4double xStep;
  G4double yStep;
  G4double zStep;
  G4double tStep;
  /// @}
  
  G4double smallestSpatialStep;
  
  /// Which dimension the contained data represents spatially. Always referred to
  /// locally as 'x' but may represent another dimension.
  BDSDimensionType xDimension;
  BDSDimensionType yDimension;
  BDSDimensionType zDimension;
  BDSDimensionType tDimension;
  std::array<G4int, 4> dimensions;
  G4bool timeVarying;
  
private:
  static void CheckStep(G4double step, const G4String& name);
};

#endif
