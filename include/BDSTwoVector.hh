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
#ifndef BDSTWOVECTOR_H
#define BDSTWOVECTOR_H
#include "BDSException.hh"

#include "globals.hh" // geant4 types / globals

#include <ostream>

/**
 * @brief Simple two vector that's templated so the right type can be used.
 * 
 * Only the assigment operator is provided, other than that it's simple access
 * and setting only.
 * 
 * @author Laurie Nevay
 */

template <typename T>
class BDSTwoVector
{
public:
  /// Default constructor.
  BDSTwoVector()
  {values[0] = 0; values[1] = 0;}

  /// Alternate constructor.
  BDSTwoVector(T x, T y)
  {values[0] = x; values[1] = y;}

  /// Copy constructor.
  BDSTwoVector(const BDSTwoVector& other)
  {
    for (G4int i : {0, 1})
      {values[i] = other.values[i];}
  }

  /// Assignment operator.
  BDSTwoVector& operator=(const BDSTwoVector& rhs)
  {
    if (this == &rhs)
      {return *this;}
    values[0] = rhs.values[0];
    values[1] = rhs.values[1];
    return *this;
  }

  /// Access / set a single element.
  T& operator[](const G4int index)
  {
    if (index > 1)
      {throw BDSException("BDSTwoVector::operator[]> index outside array");}
    return values[index];
  }

  // Access a single element.
  const T& operator[](const G4int index) const
  {
    if (index > 1)
      {throw BDSException("BDSTwoVector::operator[]> index outside array");}
    return values[index];
  }

  /// @{ Accessor by name.
  inline const T& x() const {return values[0];}
  inline const T& y() const {return values[1];}
  /// @}
  
  /// Output stream.
  friend std::ostream& operator<< (std::ostream& out, BDSTwoVector const &v)
  {out << "(" << v.values[0] << ", " << v.values[1] << ")"; return out;}

private:
  /// Member data - x,y.
  T values[2];
};


#endif
