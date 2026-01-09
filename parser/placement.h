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
#ifndef PLACEMENT_H
#define PLACEMENT_H

#include <iomanip>
#include <iostream>
#include <string>

#include "published.h"

namespace GMAD
{
  class BLMPlacement;
  class Query;
  class SamplerPlacement;
  class ScorerMesh;
  
  /**
   * @brief Placement class for parser
   * 
   * @author Laurie Nevay
   */
  class Placement : public Published<Placement>
  {
  public:
    std::string name;         ///< Name of this placement.
    std::string geometryFile; ///< Geometry to load in format:path.
    std::string bdsimElement; ///< Name of bdsim-built component to place instead of piece of geometry.
    std::string sequence;     ///< Name of sequence to place.
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
    bool   sensitive;     ///< Whether to record hits or not.
    bool   axisAngle;     ///< Flag to use the axis angle construction of rotation.
    std::string side;     ///< which side to attach to: top, bottom, left, right.
    double sideOffset;    ///< Gap between side and component.
    bool   autoColour;    ///< Whether to auto colour the geometry.
    bool   stripOuterVolume; ///< Whether to place the world LV as a G4AssemblyVolume.
    bool   dontReloadGeometry; ///< Purposively don't reload for unique placements (must know what we're doing)
    
    std::string fieldAll; ///< Name of field object to apply to all of placement.
    
    /// constructor
    Placement();
    /// reset
    void clear();
    /// print some properties
    void print()const;
    /// set methods by property name and value
    template <typename T>
    void set_value(std::string property, T value, bool bExit = true);

    /// @{ Conversion constructor.
    Placement(const SamplerPlacement& samplerPlacement);
    Placement(const ScorerMesh&       scorerMesh);
    Placement(const BLMPlacement&     blmPlacement);
    Placement(const Query& qu);
    /// @}

  private:
    /// publish members
    void PublishMembers();
  };
  
  template <typename T>
  void Placement::set_value(std::string property, T value, bool bExit)
  {
#ifdef BDSDEBUG
    std::cout << "placement> Setting value " << std::setw(25) << std::left
	      << property << value << std::endl;
#endif
      // member method can throw runtime_error, catch and exit gracefully
    try
      {set(this,property,value);}
    catch (const std::runtime_error&)
      {
        std::cerr << "Error: placement> unknown option \"" << property
                  << "\" with value \"" << value << "\"" << std::endl;
        if(bExit)
          exit(1);
        else
          std::rethrow_exception(std::current_exception());
      }
  }
}

#endif
