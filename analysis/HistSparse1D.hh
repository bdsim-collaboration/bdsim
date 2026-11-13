/* 
Beam Delivery Simulation (BDSIM) Copyright (C) BDSIM Collaboration, 2001 - 2026.

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
#ifndef HISTSPARSE1D_H
#define HISTSPARSE1D_H

#include "Rtypes.h"
#include "TObject.h"

#include <cmath>
#include <map>
#include <string>

/**
 * @brief Sparse 1D histogram based on a map.
 *
 * Simple implementation for what we need. ROOT's THnSparse is really complicated
 * and couldn't get to work - overly complex for what we need. Boost histograms are
 * better but we don't have a dependency on that yet, so for now a super simple one
 * of our own.
 * 
 * This doesn't have any limits and just accumulates for each unique key forming a
 * set of keys.
 *
 * @author L. Nevay
 */

template<class T>
class HistSparse1D: public TObject
{
public:
  /// Public constructor only for compatibility with ROOT - not intended for use.
  HistSparse1D();

  /// Use this constructor.
  explicit HistSparse1D(const std::string& nameIn):
    name(nameIn),
    entries(0)
  {;}
  virtual ~HistSparse1D();
  
  struct BinWorking
  {
    double sumWeights = 0;
    double sumWeightsSquared = 0;
  
    friend BinWorking operator+(BinWorking& lhs,        // passing lhs by value helps optimize chained a+b+c
                                const BinWorking& rhs) // otherwise, both parameters may be const references
    {
      lhs += rhs; // reuse compound assignment
      return lhs;
    }
    BinWorking& operator+=(const BinWorking& rhs)
    {
      sumWeights += rhs.sumWeights;
      sumWeightsSquared += rhs.sumWeightsSquared;
      return *this;
    }
  };

  struct Bin
  {
    double value = 0;
    double error = 0;
  };

  inline void Fill(T x, double weight=1.0)
  {
    auto& v = data[x];
    v.sumWeights += weight; 
    v.sumWeightsSquared += weight*weight;
    entries++;
  }
  inline long long int Entries() const {return entries;}
  inline size_t        size()    const {return data.size();}
  std::map<T, Bin> Result() const
  {
    std::map<T, Bin> result;
    for (const auto& kv : data)
    {
      Bin v;
      v.value = kv.second.sumWeights;
      v.error = std::sqrt(kv.second.sumWeightsSquared);
      result[kv.first] = v;
    }
    return result;
  };
  
  /// Iterator mechanics
  typedef typename std::map<T, BinWorking>::iterator       iterator;
  typedef typename std::map<T, BinWorking>::const_iterator const_iterator;
  iterator               begin()        {return data.begin();}
  iterator               end()          {return data.end();}
  const_iterator         begin()  const {return data.begin();}
  const_iterator         end()    const {return data.end();}
  bool                   empty()  const {return data.empty();}
  
  BinWorking& operator[](const long long int key)             {return data[key];}
  const BinWorking& operator[](const long long int key) const {return data.at(key);}

  bool HasAbscissa(T value) const {return data.find(value) != data.end();}
  
  std::string name;
  std::map<T, BinWorking> data;
  long long int entries;

  ClassDef(HistSparse1D,1);
};

#endif
