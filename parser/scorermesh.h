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
#ifndef SCORERMESH_H
#define SCORERMESH_H

#include <exception>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include "published.h"

namespace GMAD
{
  class SamplerPlacement;
  
  /**
   * @brief ScorerMesh class for parser
   * 
   * @author Laurie Nevay
   */
  class ScorerMesh : public Published<ScorerMesh>
  {
  public:
    std::string name;          ///< Name of this placement.
    std::string scoreQuantity; ///< Name of scorer object to use.
    std::string geometryType; ///< Name of scorermesh geometry to use.
    
    int nx;             ///< Number of bins in x.
    int ny;             ///< Number of bins in y.
    int nz;             ///< Number of bins in z.
    int nr;             ///< Number of bins in R.
    int nphi;           ///< Number of bins in Phi.
    int ne;             ///< Number of bins in E.
    double xsize;       ///< X total width.
    double ysize;       ///< Y total width.
    double zsize;       ///< Z total width.
    double rsize;       ///< R total length.
    double eLow;        ///< E Low limit.
    double eHigh;       ///< E High limit.
    std::string eScale; ///< E scaling type.
    std::string eBinsEdgesFilenamePath; ///< E bins edges filename path.

    // placement stuff
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
    bool   axisAngle;     ///< Flag to use the axis angle construction of rotation.
    
    /// constructor
    ScorerMesh();
    /// reset
    void clear();
    /// print some properties
    void print()const;
    /// set methods by property name and value
    template <typename T>
    void set_value(std::string property, T value, bool exceptionSafe = false);

  private:
    /// publish members
    void PublishMembers();
  };
  
  template <typename T>
  void ScorerMesh::set_value(std::string property, T value, bool exceptionSafe)
  {
#ifdef BDSDEBUG
    std::cout << "scorermesh> Setting value " << std::setw(25) << std::left
              << property << value << std::endl;
#endif
    // member method can throw runtime_error, catch and exit gracefully
    try
      {set(this,property,value);}
    catch (const std::runtime_error&)
      {
        std::stringstream ss;
        ss << "Error: scorermesh> unknown property \"" << property
           << "\" with value \"" << value << "\" in definition \"" << this->name << "\"";
        if (!exceptionSafe)
          {throw std::invalid_argument(ss.str());}
        else
          {std::cout << ss.str() << std::endl;}
      }
  }
}

#endif
