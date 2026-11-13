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
#ifndef COMPRESULT_H
#define COMPRESULT_H

#include <ostream>
#include <sstream>
#include <string>

/**
 * @brief Base class of a result.
 *
 * The basic information each test result should have. Public
 * member class with compiler provided default constructor to use
 * as a struct.
 * 
 * @author Stewart Boogert
 */

class Result
{ 
public:
  Result(std::string nameIn, std::string objTypeIn):
    name(nameIn),
    objtype(objTypeIn),
    passed(true)
  {;}

  Result():
    name(""),
    objtype(""),
    passed(true)
  {;}
  
  virtual ~Result() {} ///< Virtual destructor

  std::string name;    ///< Name of object being compared in files.
  std::string objtype; ///< Name of class of object being compared in files.
  bool        passed;  ///< Whether it passed or not.

  /// Print the result of the test - ie information store in the class.
  virtual std::string print() const
  {
    std::stringstream ss;
    ss << "Comparison of \"" << name << "\" of type " << objtype << " ";
    passed ? ss<<"Passed" : ss<<"Failed";
    ss << "\n";
    return ss.str();
  }

  /// Overload ostream for easy printing.
  friend std::ostream& operator<<(std::ostream &ostr, const Result& rhs)
  {
    ostr << rhs.print(); 
    return ostr;
  }
};

#endif
