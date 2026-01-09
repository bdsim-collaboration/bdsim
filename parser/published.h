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
#ifndef PUBLISHED_H
#define PUBLISHED_H

#include <cmath>
#include <list>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

#include "array.h" // our array header

namespace GMAD
{
/** 
 * @brief Class that provides introspection to its members
 *
 * In the parser the keywords in the ASCII files need to be matched to the correct member variable.
 * However, C++ has no introspection ability. This class provides this artificially.
 * 
 * All members need to be added explicitly to a hash table (unordered_map) with the method publish.
 * Note that the name can be different from the actual member name
 * 
 * adapted into a class from http://stackoverflow.com/questions/19557881/convert-string-character-to-class-member-method-in-c
 *
 * @author Jochem Snuverink
 */

template<typename C>
class Published
{
public:
  bool NameExists(const std::string& name) const {return allNames.count(name) > 0;}
  std::set<std::string> AllNames() {return allNames;}
  /// Get method for class C
  template <typename T>
  T get(const C* instance, const std::string& name) const;

protected:
  /// Make pointer to member from class C and type T with accessible with a name
  template<typename T>
  void publish(const std::string& name, T C::*mp);
  ///@{
  /// Set member with name of class instance to value.
  /// Throws std::runtime_error if not found
  void set(C* instance, const std::string& name, double value);
  void set(C* instance, const std::string& name, GMAD::Array* const& value);
  template<typename T>
  void set(C* instance, const std::string& name, const T& value);
  ///@}
  
  /// Set member with name of class instance to value of second instance
  void set(C* instance, const C* instance2, const std::string& name);
  
  /// Define AttributeMap of string and class member pointer
  template <typename T>
  using AttributeMap = typename std::unordered_map<std::string, T C::*>;
  
  /// Access method to static map for type T and class C
  template <typename T>
  AttributeMap<T>& attribute_map() const;

private:
  /// Access to member pointer
  template<typename T>
  T C::* member(const std::string& name) const;
  
  /// A cache of all names defined through publish().
  std::set<std::string> allNames;
};
  
// implementation for templated class needs to be in header
  
template<typename C>
template<typename T>
void Published<C>::publish(const std::string& name, T C::*mp)
{
  attribute_map<T>()[name] = mp;
  allNames.insert(name);
}
  
template<typename C>
void Published<C>::set(C* instance, const std::string& name, double value)
{
  // check maps for double, int, bool
  // tried to do this more general (e.g. with a single map),
  // but difficult since member pointers have different types
  // better would be to keep a list of arithmetic types published and loop over those
  try
    {
      double C::* mp = member<double>(name);
      (instance)->*mp = value;
    }
  catch (const std::runtime_error&)
    {
      try
        {
          int C::* mp = member<int>(name);
          (instance)->*mp = std::round(value);
        }
      catch (const std::runtime_error&)
        {
          try
            {
              bool C::* mp = member<bool>(name);
              (instance)->*mp = value;
            }
          catch (const std::runtime_error&)
            {
              try
                {
                  long C::* mp = member<long>(name);
                  (instance)->*mp = value;
                }
              catch (const std::runtime_error&)
                {
                  // if not found throw error
                  throw std::runtime_error("Unknown member " + name);
                }
            }
        }
    }
}

template<typename C>
template<typename T>
void Published<C>::set(C* instance, const std::string& name, const T& value)
{
  try
    {
      T C::* mp = member<T>(name);
      (instance)->*mp = value;
    }
  catch (const std::runtime_error&)
    {throw std::runtime_error("Unknown member " + name);}
}

template<typename C>
void Published<C>::set(C* instance, const std::string& name, GMAD::Array* const& value)
{
  try
    {
      std::list<double> valueNew = value->GetDataList();
      std::list<double> C::* mp = member<std::list<double>>(name);
      (instance)->*mp = valueNew;
    }
  catch (const std::runtime_error&)
    {
      try
        {
          std::list<std::string> valueNew = value->GetSymbolsList();
          std::list<std::string> C::* mp = member<std::list<std::string>>(name);
          (instance)->*mp = valueNew;
        }
      catch (const std::runtime_error &)
        {
          // if not found throw error
          throw std::runtime_error("Unknown member " + name);
        }
    }
}
  
template<typename C>
void Published<C>::set(C* instance, const C* instance2, const std::string& name)
{
  // unfortunately, have to try all existing types
  try
    {
      // pointer to member
      double C::* mp = member<double>(name);
      // set value
      (instance)->*mp = (instance2)->*mp;
    }
  catch (const std::runtime_error&)
    {
      try
        {
          int C::* mp = member<int>(name);
          (instance)->*mp = (instance2)->*mp;
        }
      catch (const std::runtime_error&)
        {
          try
            {
              bool C::* mp = member<bool>(name);
              (instance)->*mp = (instance2)->*mp;
            }
          catch (const std::runtime_error&)
            {
              try
                {
                  std::string C::* mp = member<std::string>(name);
                  (instance)->*mp = (instance2)->*mp;
                }
              catch (const std::runtime_error&)
                {
                  try
                    {
                      std::list<std::string> C::* mp = member<std::list<std::string>>(name);
                      (instance)->*mp = (instance2)->*mp;
                    }
                  catch (const std::runtime_error&)
                    {
                      try
                        {
                          std::list<int> C::* mp = member<std::list<int>>(name);
                          (instance)->*mp = (instance2)->*mp;
                        }
                      catch (const std::runtime_error&)
                        {
                          try
                            {
                              std::list<double> C::* mp = member<std::list<double>>(name);
                              (instance)->*mp = (instance2)->*mp;
                            }
                          catch (const std::runtime_error&)
                            {
                              try
                                {
                                  long C::* mp = member<long>(name);
                                  (instance)->*mp = (instance2)->*mp;
                                }
                              catch (const std::runtime_error&)
                                {
                                  // if not found throw error
                                  throw std::runtime_error("Unknown member " + name);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

template <typename C>
template <typename T>
T Published<C>::get(const C* instance, const std::string& name) const
{
  // pointer to member
  T C::* mp = member<T>(name);
  return (instance)->*mp;
}
  
template<typename C>
template<typename T>
T C::* Published<C>::member(const std::string& name) const
{
  AttributeMap<T>& m = attribute_map<T>();
  typename AttributeMap<T>::const_iterator it=m.find(name);
  if (it == m.end())
    {
      // if not found throw error
      throw std::runtime_error("Unknown member " + name);
    }
  else
    {
        return it->second;
      }
    }

  template<typename C>
    template<typename T>
    typename Published<C>::template AttributeMap<T>& Published<C>::attribute_map() const
    {
      // static initialisation for flexibility
      static AttributeMap<T> m;
      return m;
    }
}

#endif
