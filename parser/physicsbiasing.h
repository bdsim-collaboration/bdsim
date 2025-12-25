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
#ifndef PHYSICSBIASING_H
#define PHYSICSBIASING_H

#include <string>
#include <list>
#include <vector>
#if __cplusplus >= 201703L
#include <variant>
#endif
#include "published.h"


namespace GMAD
{
  class Array;
}

namespace GMAD
{
  enum class PhysicsBiasingType {ALL=1,PRIMARY=2,SECONDARY=3};
  
  /**
   * @brief Physics biasing class for parser
   * 
   * @author Jochem Snuverink
   */
  class PhysicsBiasing : public Published<PhysicsBiasing> {
    
  public:
    std::string name; ///< name
    /// particle name
    std::string particle;
    /// geant4 process: single string, but can have multiple processes separated with a space
    std::string process;
    std::vector<std::string> processList;
    /// factors corresponding to process
    std::vector<double> factor;
    /// flag which particles are biased
    std::vector<PhysicsBiasingType> flag;
    
    /// constructor
    PhysicsBiasing();
    /// publish members
    void PublishMembers();
    /// reset
    void clear();
    /// print some properties
    void print()const;

    /// set methods by property name, numeric values
    void set_value(const std::string& property, double value, bool bExit = true);
    /// set methods by property name, list of numeric values
    void set_value(const std::string& property, Array* value, bool bExit = true);
    /// set methods by property name, string values
    void set_value(const std::string& property, std::string value, bool bExit = true);
    /// Get method for lists
#if __cplusplus >= 201703L
    std::list<std::variant<bool, int, double, std::string>> get_value_array(const std::string &);
#endif
  };

}

#endif
