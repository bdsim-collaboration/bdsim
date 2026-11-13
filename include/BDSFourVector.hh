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
#ifndef BDSFOURVECTOR_H
#define BDSFOURVECTOR_H
#include "BDSException.hh"

#include "globals.hh" // geant4 types / globals

#include <ostream>
#include <vector>

/**
 * @brief A simple templated four vector class.
 * 
 * @author Laurie Nevay
 */

template<typename Type>
class BDSFourVector
{
public:
  /// Default constructor.
  BDSFourVector()
  {values[0] = 0; values[1] = 0; values[2] = 0; values[3] = 0;}

  /// Alternate constructor.
  BDSFourVector(Type xIn, Type yIn, Type zIn, Type tIn)
  {values[0] = xIn; values[1] = yIn; values[2] = zIn; values[3] = tIn;}

  /// Copy constructor.
  BDSFourVector(const BDSFourVector& other)
  {
    for (const auto& i : indices)
      {values[i] = other.values[i];}
  }

  /// Assignment operator.
  BDSFourVector& operator=(const BDSFourVector& rhs)
  {
    if (this == &rhs)
      {return *this;}
    for (const G4int& i : indices)
      {values[i] = rhs.values[i];}
    return *this;
  }

  /// Access / set a single element.
  Type& operator[](const G4int index)
  {
    if (index > (G4int)indices.size()-1)
    {throw BDSException("BDSFourVector::operator[]> index outside array");}
    return values[index];
  }

  /// Access a single element.
  const Type& operator[](const G4int index) const
  {
    if (index > (G4int)indices.size()-1)
    {throw BDSException("BDSFourVector::operator[]> index outside array");}
    return values[index];
  }

  /// @{ Accessor by name.
  inline const Type& x() const {return values[0];}
  inline const Type& y() const {return values[1];}
  inline const Type& z() const {return values[2];}
  inline const Type& t() const {return values[3];}
  /// @}

  /// Output stream.
  friend std::ostream& operator<< (std::ostream& out, BDSFourVector const &v)
  {
    out << "("
	<< v.values[0] << ", "
	<< v.values[1] << ", "
	<< v.values[2] << ", "
	<< v.values[3]
	<< ")";
    return out;
  }

private:
  /// Member data x,y,z,t
  Type values[4];

  std::vector<G4int> indices = {0,1,2,3};
};

#endif
