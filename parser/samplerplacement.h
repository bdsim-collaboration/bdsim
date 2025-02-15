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
#ifndef SAMPLERPLACEMENT_H
#define SAMPLERPLACEMENT_H

#include <iomanip>
#include <iostream>
#include <list>
#include <string>

#include "published.h"

namespace GMAD
{
  /**
   * @brief Sampler placement class for parser.
   * 
   * @author Laurie Nevay
   */
  class SamplerPlacement : public Published<SamplerPlacement>
  {
  public:
    std::string name;         ///< Name of this samplerplacement.
    std::string samplerType;  ///< Plane, Cylinder, Sphere.
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
    bool   axisAngle;     ///< Flag to use the axis angle construction of rotation.

    std::string apertureModel;
    std::string shape;
    double aper1;
    double aper2;
    double aper3;
    double aper4;
    
    double startAnglePhi;
    double sweepAnglePhi;
    double startAngleTheta;
    double sweepAngleTheta;
    
    // This should be std::list<int> but the parser only supports double for numerical types in a list.
    /// List of PDG IDs of which particles to record for - default is empty, so all particles.
    std::list<double> partID;
    int partIDSetID; ///< The unique ID of the particle set given by the parser.
    
    /// constructor
    SamplerPlacement();
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
  void SamplerPlacement::set_value(std::string property, T value, bool bExit)
  {
#ifdef BDSDEBUG
    std::cout << "samplerplacement> Setting value " << std::setw(25) << std::left
              << property << value << std::endl;
#endif
    // member method can throw runtime_error, catch and exit gracefully
    try
      {
        set(this, property, value);
      }
    catch (const std::runtime_error &)
      {
        std::cerr << "Error: samplerplacement> unknown option \"" << property
                  << "\" with value \"" << value << "\"" << std::endl;
        if (property == "partID")
          {
            std::cerr << "Note \"partID\" should be a list {int,int...}" << std::endl;
            if (bExit)
              {exit(1);}
            else
              {std::rethrow_exception(std::current_exception());} // to be caught by python
          }
      }
  }
}

#endif
