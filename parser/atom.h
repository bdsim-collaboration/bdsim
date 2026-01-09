/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2024.

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
#ifndef ATOM_H
#define ATOM_H

#include <iomanip>
#include <iostream>
#include <string>

#include "published.h"

namespace GMAD
{
  /**
   * @brief Atom class
   * 
   * @author Jochem Snuverink
   */

  struct Atom : public Published<Atom>{
    /// name
    std::string name;

    double A; ///< g*mol^-1
    double Z; 
    std::string symbol;

    /// constructor
    Atom();
    /// reset
    void clear();
    /// printout
    void print()const;
    /// set methods by property name and value
    template <typename T>
    void set_value(std::string property, T value, bool bExit = false);

  private:
    /// publish members so these can be looked up from parser
    void PublishMembers();
  };

  template <typename T>
  void Atom::set_value(std::string property, T value, bool bExit )
  {
#ifdef BDSDEBUG
    std::cout << "parser> Setting value " << std::setw(25) << std::left << property << value << std::endl;
#endif
    // member method can throw runtime_error, catch and exit gracefully
    try
      {set(this,property,value);}
    catch(const std::runtime_error&)
      {
        std::cerr << "Error: parser> unknown atom option \"" << property << "\" with value \"" << value << "\"" << std::endl;
        if(bExit)
          {exit(1);}
        else
          {std::rethrow_exception(std::current_exception());}
      }
  }
}

#endif
