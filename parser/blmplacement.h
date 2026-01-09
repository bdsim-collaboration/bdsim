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
#ifndef BLMPLACEMENT_H
#define BLMPLACEMENT_H

#include <iomanip>
#include <iostream>
#include <string>

#include "published.h"

namespace GMAD
{
  /**
   * @brief blm for parser.
   *
   * This has to be called something other than "BLM" because the parser
   * can't distinguish between the class name and the token "BLM".
   * 
   * @author Laurie Nevay
   */
  class BLMPlacement : public Published<BLMPlacement>
  {
  public:
    std::string name;         ///< Name of this samplerplacement.
    std::string referenceElement; ///< Name of reference element w.r.t. to place to.
    int         referenceElementNumber; ///< Index of repetition of element if there are multiple uses.
    double s; ///< Curvilinear s position to place w.r.t..
    double x; ///< Offset in x.
    double y; ///< Offset in y.
    double z; ///< Offset in z.
    /// @{ Euler angle for rotation.
    double phi;
    double theta;
    double psi;
    /// @}
    /// @{ Axis angle rotation.
    double axisX;
    double axisY;
    double axisZ;
    double angle;
    /// @}
    bool   axisAngle;  ///< Flag to use the axis angle construction of rotation.
    std::string side;  ///< which side to attach to: top, bottom, left, right.
    double sideOffset; ///< Offset between the geometry and the BLM.

    std::string geometryFile;
    std::string geometryType;
    std::string blmMaterial;
    double blm1;
    double blm2;
    double blm3;
    double blm4;
    std::string scoreQuantity;
    std::string bias;

    /// constructor
    BLMPlacement();
    /// reset
    void clear();
    /// print some properties
    void print()const;
    /// set methods by property name and value
    template <typename T>
    void set_value(std::string property, T value, bool bExit = true);

  private:
    /// publish members
    void PublishMembers();
  };

  template <typename T>
  void BLMPlacement::set_value(std::string property, T value, bool bExit)
    {
#ifdef BDSDEBUG
      std::cout << "blmplacement> Setting value " << std::setw(25) << std::left
		<< property << value << std::endl;
#endif
      // member method can throw runtime_error, catch and exit gracefully
      try
        {set(this,property,value);}
      catch (const std::runtime_error&)
        {
          std::cerr << "Error: blmplacement> unknown option \"" << property
                    << "\" with value \"" << value << "\"" << std::endl;
          if(bExit)
            {exit(1);}
          else
            {std::rethrow_exception(std::current_exception());}
        }
    }
}

#endif
