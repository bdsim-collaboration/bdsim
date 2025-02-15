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
#ifndef TUNNEL_H
#define TUNNEL_H

#include <iomanip>
#include <iostream>
#include <string>

#include "published.h"

namespace GMAD
{
  /**
   * @brief Tunnel class for parser
   * 
   * @author Jochem Snuverink
   */
  class Tunnel: public Published<Tunnel> {

  public:
    std::string name; ///< name
    /// geometry type
    std::string type;

    /// radius, aperture parameters
    double   aper1;
    double   aper2;
    /// offset x and y
    double   offsetX, offsetY;
    /// tunnel geometry parameters
    double   thickness;
    double   soilThickness;
    double   floorOffset;

    /// visibility
    int      visible;
  
    /// material
    std::string material;
    /// soil
    std::string soilMaterial;

    /// start and end element by name
    std::string startElement, endElement;
  
    /// constructor
    Tunnel();
    /// reset
    void clear();
    /// print some properties
    void print()const;
    /// set methods by property name and value
    template <typename T>
    void set_value(std::string property, T value, bool bExit = true);

  private:
    /// Publish members
    void PublishMembers();
  };
  
  template <typename T>
  void Tunnel::set_value(std::string property, T value, bool bExit)
    {
#ifdef BDSDEBUG
      std::cout << "tunnel> Setting value " << std::setw(25) << std::left << property << value << std::endl;
#endif
      // member method can throw runtime_error, catch and exit gracefully
      try
	{set(this,property,value);}
      catch(const std::runtime_error&)
	{
	  std::cerr << "Error: tunnel> unknown option \"" << property << "\" with value " << value << "\"" << std::endl;
          if (bExit)
            {exit(1);}
          else
            {std::rethrow_exception(std::current_exception());} // to be caught by python
	}
    }
}

#endif
