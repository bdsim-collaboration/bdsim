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
#ifndef CAVITYMODEL_H
#define CAVITYMODEL_H

#include <iomanip>
#include <iostream>
#include <string>

#include "published.h"

namespace GMAD
{
  /**
   * @brief RF CavityModel class for parser
   * 
   * @author Jochem Snuverink
   */
  class CavityModel: public Published<CavityModel> {
  public:
    std::string name;              ///< name
    std::string type;              ///< geometry type
    std::string material;          ///< material name
    double irisRadius;             ///< iris radius
    double equatorRadius;          ///< equator radius
    double halfCellLength;         ///< half cell length
    double equatorHorizontalAxis;  ///< equator ellipse vertical semi axis;
    double equatorVerticalAxis;    ///< equator ellipse horizontal semi axis;
    double irisHorizontalAxis;     ///< iris horizontal axis
    double irisVerticalAxis;       ///< Iris axis
    double tangentLineAngle;       ///< tangent angle
    double thickness;              ///< thickness

    /// number of points that will be used to make the cell shape
    /// more will be more accurate but will also increase tracking time
    int numberOfPoints;
   
    int numberOfCells;  ///< number of cells per cavity
    
    /// constructor
    CavityModel();
    /// reset
    void clear();
    /// publish members
    void PublishMembers();
    /// print some properties
    void print()const;

    /// set methods by property name and value
    template <typename T>
    void set_value(std::string property, T value, bool bExit = true);
  };
  template <typename T>
  void CavityModel::set_value(std::string property, T value, bool bExit)
  {
#ifdef BDSDEBUG
    std::cout << "cavitymodel> Setting value " << std::setw(25) << std::left << property << value << std::endl;
#endif
    // member method can throw runtime_error, catch and exit gracefully
    try
      {set(this,property,value);}
    catch(const std::runtime_error&)
      {
        std::cerr << "Error: cavitymodel> unknown option \"" << property << "\" with value \"" << value << "\"" << std::endl;
        if(bExit)
          {exit(1);}
        else
          {std::rethrow_exception(std::current_exception());}
      }
  }
}

#endif
