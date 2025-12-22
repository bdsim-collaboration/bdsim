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
#ifndef ARRAY_H
#define ARRAY_H

#include <iostream>
#include <list>
#include <string>
#include <vector>

namespace GMAD
{
  class Symtab;
}

namespace GMAD
{
  /**
   * @brief Representation of arrays used in tokens
   *
   * Used for both std::string and double
   */

  class Array {

  private:
    /// Representation of string array
    std::vector<std::string> symbols;
    /// Representation of double array
    std::vector<double> data;

    /// Helper constructor method, creates data array with size n
    explicit Array(unsigned int n);

  public:
    /// Symtabs and arrays are kind of interchangeable
    friend class Symtab;
    /// Default Constructor
    Array();
    /// Constructor from symbol
    explicit Array(Symtab*);
    /// Constructor from adding 2 double arrays
    static Array* Add(Array* a1, Array* a2);
    /// Constructor from subtracting 2 double arrays
    static Array* Subtract(Array* a1, Array* a2);
    /// Constructor from adding an array
    static Array* Add(Array* a, double d);
    /// Constructor from subtracting an array
    static Array* Subtract(Array* a, double d);
    /// Constructor from multiplying an array
    static Array* Multiply(Array* a, double d);
    /// Constructor from dividing an array
    static Array* Divide(Array* a, double d);
    /// Scalar vector product
    double Product(Array* a);

    /// Access
    const std::vector<std::string>& GetSymbols()     const {return symbols;}
    std::list<std::string>          GetSymbolsList() const;
    const std::vector<double>&      GetData()        const {return data;}
    std::list<double>               GetDataList()    const;
    
    /// Clear data
    void Clear();
    /// Print data
    void Print();
    
    /// Copy STL string containers into symbols
    template<template <typename, typename> class Container>
    void Copy(Container<std::string, std::allocator<std::string>>& cpy)
    {
      for (const std::string& name : cpy)
        {symbols.push_back(name);}
    }

    /// Copy STL numerical containers into data
    template<typename T, template <typename, typename> class Container>
    void Copy(Container<T, std::allocator<T>>& cpy)
    {
      for (T value : cpy)
        {data.push_back(value);}
    }

    /// Copy symbols into STL string containers
    template<template <typename, typename> class Container>
    void set_vector(Container<std::string, std::allocator<std::string>>& dst)const
    {
      for (std::string symbol : symbols)
	{
	  dst.push_back(symbol);
#ifdef BDSDEBUG 
	  std::cout << symbol << " ";
#endif
	}
#ifdef BDSDEBUG 
      std::cout << std::endl;
#endif
    }

    /// Copy data into STL numerical containers
    template<typename T, template <typename, typename> class Container>
    void set_vector(Container<T, std::allocator<T>>& dst)const
    {
      for (auto value : data)
	{
	  dst.push_back((T)value);

#ifdef BDSDEBUG
	  std::cout << (T)value << " ";
#endif
	}
#ifdef BDSDEBUG 
      std::cout << std::endl;
#endif
    }
  };
}

#endif
