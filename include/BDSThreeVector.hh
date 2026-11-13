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
#ifndef BDSTHREEVECTOR_H
#define BDSTHREEVECTOR_H
#include "BDSException.hh"

#include "globals.hh" // geant4 types / globals

#include <cmath>
#include <ostream>

/**
 * @brief Simple three vector that's templated so the right type can be used.
 * 
 * The G4ThreeVector(HEP::ThreeVector) is much better, but this only uses
 * a double. Here, a simple three vector that's templated allows the
 * possibility of using float or double precision, which could make a 
 * significant difference to the memory usage - very important for large
 * arrays of this class.
 *
 * Should more complex features be required - it's best to use an actual
 * G4ThreeVector - constructed from this - and then translated back.
 * 
 * @author Laurie Nevay
 */

template <typename T>
class BDSThreeVector
{
public:
  /// Default constructor.
  BDSThreeVector()
  {values[0] = 0; values[1] = 0; values[2] = 0;}

  /// Alternate constructor.
  BDSThreeVector(T xIn, T yIn, T zIn)
  {values[0] = xIn; values[1] = yIn; values[2] = zIn;}

  /// Copy constructor.
  BDSThreeVector(const BDSThreeVector& other)
  {
    for (G4int i : {0, 1, 2})
      {values[i] = other.values[i];}
  }

  /// Assignment operator.
  BDSThreeVector& operator=(const BDSThreeVector& rhs)
  {
    if (&rhs != this)
      {
	for (G4int i : {0, 1, 2})
	  {values[i] = rhs.values[i];}
      }
    return *this;
  }

  /// Access / set a single element.
  T& operator[](const G4int index)
  {
    if (index > 2)
      {throw BDSException("BDSThreeVector::operator[]> index outside array");}
    return values[index];
  }

  /// Access a single element.
  const T& operator[](const G4int index) const
  {
    if (index > 2)
      {throw BDSException("BDSThreeVector::operator[]> index outside array");}
    return values[index];
  }

  /// @{ Accessor by name.
  inline const T& x() const {return values[0];}
  inline const T& y() const {return values[1];}
  inline const T& z() const {return values[2];}
  /// @}

  /// Output stream.
  friend std::ostream& operator<< (std::ostream& out, BDSThreeVector const &v)
  {out << "(" << v.values[0] << ", " << v.values[1] << ", " << v.values[2] << ")"; return out;}

  T dot(const BDSThreeVector& b) const
  {return x()*b.x() + y()*b.y() + z()*b.z();}
  
  /// Get the magnitude of it.
  inline T mag() const {return std::sqrt(values[0]*values[0] + values[1]*values[1] + values[2]*values[2]);}

  /// @{ Multiplication
  inline friend BDSThreeVector operator * (const BDSThreeVector& a, const double& b)
  {return BDSThreeVector(b*a.x(), b*a.y(), b*a.z());}
  inline friend BDSThreeVector operator * (const double& b, const BDSThreeVector& a)
  {return BDSThreeVector(b*a.x(), b*a.y(), b*a.z());}
  inline friend T operator * (const BDSThreeVector& a, const BDSThreeVector& b)
  {return a.dot(b);}
  /// @}

  /// @{ Addition, subtraction
  inline friend BDSThreeVector operator + (const BDSThreeVector& a, const G4double& b)
  {return BDSThreeVector(a.x() + b, a.y() + b, a.z() + b);}
  inline friend BDSThreeVector operator + (const BDSThreeVector& a, const BDSThreeVector& b)
  {return BDSThreeVector(a.x() + b.x(), a.y() + b.y(), a.z() + b.z());}
  inline friend BDSThreeVector operator - (const BDSThreeVector& a, const G4double& b)
  {return BDSThreeVector(a.x() - b, a.y() - b, a.z() - b);}
  inline friend BDSThreeVector operator - (const BDSThreeVector& a, const BDSThreeVector& b)
  {return BDSThreeVector(a.x() - b.x(), a.y() - b.y(), a.z() - b.z());}
  /// @}

  inline BDSThreeVector& operator *= (G4double a)
  {
    values[0] *= a;
    values[1] *= a;
    values[2] *= a;
    return *this;
  }
  
private:
  /// Member data - x,y,z.
  T values[3];
};

#endif
