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
#ifndef OPTIONS_H
#define OPTIONS_H

#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "published.h"
#include "optionsBase.h"

namespace GMAD
{
  /**
   * @brief Options class
   * 
   * Options passed with option command. This inherits
   * the OptionsBase class which contains all the members and 
   * provides templated filling functions. This separation allows
   * the OptionsBase class to be more easily written out to ROOT files
   * or other formats for strong reproducibility in a BDSIM run.
   *
   * @author I. Agapov, J. Snuverink
   */
  class Options: public Published<OptionsBase>, public OptionsBase
  {
  public:
    Options();
    explicit Options(const GMAD::OptionsBase& options);
    
    /// set methods by property name
    template<typename T>
    void set_value(std::string name, T value, bool bExit = false);
    
    /// get method (only for doubles)
    double get_value(std::string property_name) const;

    std::string get_value_string(std::string property_name) const;

    /// Take another instance of options and copy the values that have
    /// been set (through setKeys, which although private each instance
    /// has access to as C++ treats encapsulation at the class level).
    /// If override is true, the input option will override the existing
    /// one in this instance.
    void Amalgamate(const Options& optionsIn, bool override);

    /// Whether a parameter has been set using the set_value method or not.
    bool HasBeenSet(const std::string& name) const;

    inline const std::vector<std::string>& KeysOfSetValues() const {return setKeys;}
    
  private:
    /// publish members so these can be looked up from parser
    void PublishMembers();
  };

  template<typename T>
  void Options::set_value(std::string name, T value, bool bExit)
  {
#ifdef BDSDEBUG
    std::cout << "options> Setting value " << std::setw(25) << std::left << name << value << std::endl;
#endif
    // member method can throw runtime_error, catch and exit gracefully
    try
      {
        set(this, name, value);
        setKeys.push_back(name);
      }
    catch (const std::runtime_error&)
      {
        std::cerr << "Error: options> unknown option \"" << name << "\" with value \"" << value << "\"" << std::endl;
        if (bExit)
          {exit(1);}
        else
          {std::rethrow_exception(std::current_exception());} // to be caught by python
      }
  }
}

#endif
