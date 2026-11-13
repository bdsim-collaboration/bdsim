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
#ifndef BINSPECIFICATION_H
#define BINSPECIFICATION_H
#include "Rtypes.h"

#include <string>
#include <vector>

/**
 * @brief Binning specification for a single dimension.
 * 
 * For logarithmic or uneven binning, a set of bin edges can be
 * used. Otherwise, a high and low limit and the number of bins
 * in between can be used.
 *
 * "n" is the number of bins, not the number of edges.
 *
 * @author Laurie Nevay
 */

class BinSpecification
{
public:
  /// Default constructor.
  BinSpecification();
  /// Linearly spaced bins.
  BinSpecification(double lowIn,
                   double highIn,
                   int    nIn);

  /// Uneven binning - suitable for logarithmic etc.
  explicit BinSpecification(const std::vector<double>& edgesIn);

  BinSpecification(const BinSpecification& other);
  BinSpecification(BinSpecification&& other) noexcept;
  
  ~BinSpecification();

  /// Return the filename or the binning.
  std::string GetBinString() const;

  /// Utility function.
  inline std::string GetLogString() const {return isLogSpaced ? "Log" : "Lin";}
  
  double low;
  double high;
  int    n;
  std::vector<double>* edges;
  bool   isLogSpaced;
  std::string edgesFileName;
};
  
#endif
