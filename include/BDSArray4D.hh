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
#ifndef BDSARRAY4D_H
#define BDSARRAY4D_H

#include "BDSFieldValue.hh"
#include "BDSFourVector.hh"

#include <ostream>
#include <vector>

/**
 * @brief 4D array and base class for 3,2 & 1D arrays. 
 * 
 * Data represents T,Z,Y,X dimensions. The data is stored 
 * as a (1D) vector and stored in that order. This means that
 * adjacent X values are stored next to each other. Storing the data
 * as a 1D vector improves performance for random access (through
 * behind the scenes caching) and significantly improves performance 
 * for copying.
 * 
 * Data can be accessed by index only in the array.  Testing of indices
 * is provided.
 *
 * The () operator is used for accessing and setting as the [] operator
 * can only ever accept one parameter by design.
 * https://isocpp.org/wiki/faq/operator-overloading#matrix-subscript-op
 * 
 * The size cannot be changed after construction.
 * 
 * @author Laurie Nevay
 */

class BDSArray4D
{
public:
  /// No default constructor as the array is not adjustable after construction and
  /// therefore the size must be known at construction time.
  BDSArray4D() = delete;
  /// At construction the size of the array must be known as this implementation
  /// does not allow the size to be changed afterwards.
  BDSArray4D(G4int nXIn, G4int nYIn, G4int nZIn, G4int nTIn);
  virtual ~BDSArray4D(){;}

  /// @{ Access the number of elements in a given dimension.
  inline G4int NX() const {return nX;}
  inline G4int NY() const {return nY;}
  inline G4int NZ() const {return nZ;}
  inline G4int NT() const {return nT;}
  inline BDSFourVector<G4int> NXYZT() const {return BDSFourVector<G4int>(NX(), NY(), NZ(), NT());}
  /// @}

  /// Setter & (technically, a non-const) accessor.
  virtual BDSFieldValue& operator()(G4int x,
				    G4int y = 0,
				    G4int z = 0,
				    G4int t = 0);

  /// Convenience accessor to operator().
  const BDSFieldValue& operator()(BDSFourVector<G4int>& pos)
  {return operator()(pos.x(), pos.y(), pos.z(), pos.t());}

  /// Accessor only as returns const reference to data. By being named
  /// this can be used explicitly to ensure const access - recommended main
  /// main interface.
  virtual const BDSFieldValue& GetConst(G4int x,
					G4int y = 0,
					G4int z = 0,
					G4int t = 0) const;

  /// Convenience shortcut to GetConst().
  virtual const BDSFieldValue& operator()(G4int x,
					  G4int y = 0,
					  G4int z = 0,
					  G4int t = 0) const;

  /// Convenience accessor to operator().
  const BDSFieldValue& operator()(const BDSFourVector<G4int>& pos) const
  {return operator()(pos.x(), pos.y(), pos.z(), pos.t());}

  /// Return whether the indices are valid and lie within the array boundaries or not.
  virtual G4bool Outside(G4int x,
			 G4int y,
			 G4int z,
			 G4int t) const;

  /// Use Outside() but warn and exit if the coordinates requested are outside the array.
  virtual void OutsideWarn(G4int x,
			   G4int y,
			   G4int z,
			   G4int t) const;

  /// Virtual function is more flexible than plain operator<< for ostreaming as the derived
  /// function may use the base class part of the print out first or in a different way.
  /// The operator<< for ostream uses this function.
  virtual std::ostream& Print(std::ostream& out) const;
  
  /// Output stream.
  friend std::ostream& operator<< (std::ostream& out, BDSArray4D const &a);

protected:
  
  /// @{ Dimension
  const G4int nX;
  const G4int nY;
  const G4int nZ;
  const G4int nT;
  /// @}

  /// Need to store a default value so it can be return by reference.
  BDSFieldValue defaultValue;
  
private:
  /// A 1D array representing all the data.
  std::vector<BDSFieldValue> data;
};

#endif
