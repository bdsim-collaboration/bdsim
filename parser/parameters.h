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
#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <string>
#include <iostream>
#include <iomanip>
#include <map>
#include "element.h"

namespace GMAD
{
  extern bool willExit;

  class Array;

  /**
   * @brief Parameters - Element class with booleans
   * 
   * This is a temporary storage class of the parameters before the actual Element is created. 
   * The booleans are needed for inheritance and extension of already created Elements.
   * The class is a singleton.
   *
   * @author Jochem Snuverink (based on I. Agapov)
   */

  struct Parameters : public Element {

    /// Map that holds booleans for every member of element
    std::map<std::string,bool> setMap;

    /// Reset the parameters to defaults and setMap
    void flush();

    /// Copy parameters into temporary buffer params from element e
    /// Parameters already set in params have priority and are not overridden
    void inherit_properties(const Element& e);

    /// Set method by property name and value
    template <typename T>
    void set_value(std::string property, T value, bool bExit = true);

    /// Constructor
    Parameters();
  };

  template <typename T>
  void Parameters::set_value(std::string property, T value, bool bExit)
  {
#ifdef BDSDEBUG
    std::cout << "element> Setting value " << std::setw(25) << std::left << property << value << std::endl;
#endif
    // member method can throw runtime_error, catch and exit gracefully
    try
      {
        Published<Element>::set(this,property,value);
      }
    catch(const std::runtime_error&)
      {
        // not implemented mad parameters will be ignored
        if (property == "harmon" || property == "lag" || property == "volt")
          {return;}
        
        std::cerr << "Error: element> unknown parameter \"" << property << "\" with value " << value  << std::endl;

        willExit = bExit;
        return;
      }
    // record property set
    // property name can be different, so look up in alternate names
    std::string publishedName = getPublishedName(property);
    setMap.at(publishedName) = true;
  }
}

#endif
