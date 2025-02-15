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
#ifndef MATERIAL_H
#define MATERIAL_H

#include <iomanip>
#include <iostream>
#include <string>

#include "published.h"

namespace GMAD
{
  class Array;
  
  /**
   * @brief Material class
   * 
   * @author Jochem Snuverink
   *
   */

  struct Material : public Published<Material>{
    /// name
    std::string name;

    double A; ///< g*mol^-1
    double Z;
    double density; ///< g*cm-3 
    double temper; ///< Kelvin
    double pressure; ///<atm
    std::string state; ///< "solid", "liquid", or "gas"
    std::list<std::string> components;
    std::list<double> componentsFractions;
    std::list<int> componentsWeights;

    /// constructor
    Material();
    /// reset
    void clear();
    /// printout
    void print()const;
    /// set methods by property name and value
    template <typename T>
    void set_value(const std::string& property, T value, bool bExit = true);
    // Template overloading for Array pointers
    /// Set method for lists
    void set_value(const std::string& property, Array* value, bool bExit = true);

  private:
    /// publish members so these can be looked up from parser
    void PublishMembers();
  };

  template <typename T>
  void Material::set_value(const std::string& property, T value, bool bExit)
  {
#ifdef BDSDEBUG
    std::cout << "parser> Setting value " << std::setw(25) << std::left << property << value << std::endl;
#endif
    // member method can throw runtime_error, catch and exit gracefully
    try
      {set(this,property,value);}
    catch (const std::runtime_error&)
      {
        std::cerr << "Error: parser> unknown material option \"" << property << "\" with value \"" << value << "\"" << std::endl;
        if (bExit)
          {exit(1);}
        else
          {std::rethrow_exception(std::current_exception());} // to be caught by python
      }
  }
}

#endif
