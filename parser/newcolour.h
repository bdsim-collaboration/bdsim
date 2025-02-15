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
#ifndef NEWCOLOUR_H
#define NEWCOLOUR_H

#include <iomanip>
#include <iostream>
#include <string>

#include "published.h"

namespace GMAD
{
  /**
   * @brief Colour definition for parser.
   *
   * This is called "newcolour" instead of colour to avoid the 
   * conflict with Element::colour for the parser.
   * 
   * @author Laurie Nevay
   */
  
  class NewColour: public Published<NewColour>
  {
  public:
    std::string name;
    double red;
    double green;
    double blue;
    double alpha;
    
    /// Constructor
    NewColour();
    /// Reset
    void clear();
    /// Print some properties
    void print() const;
    /// Set methods by property name and value
    template <typename T>
    void set_value(std::string property, T value, bool bExit=true);

  private:
    /// publish members
    void PublishMembers();
  };
  
  template <typename T>
  void NewColour::set_value(std::string property, T value, bool bExit)
  {
#ifdef BDSDEBUG
    std::cout << "colour> Setting value " << std::setw(25) << std::left << property << value << std::endl;
#endif
    // member method can throw runtime_error, catch and exit gracefully
    try
      {set(this,property,value);}
    catch(const std::runtime_error&)
      {
        std::cerr << "Error: crystal> unknown option \"" << property << "\" with value \"" << value << "\"" << std::endl;
        if (bExit)
          {exit(1);}
        else
          {std::rethrow_exception(std::current_exception());} // to be caught by python
      }
  }
}

#endif
