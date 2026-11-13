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
#ifndef HISTOGRAMDEF_H
#define HISTOGRAMDEF_H

#include <iostream>
#include <string>

#include "Rtypes.h" // for classdef

/**
 * @brief Common specification for a histogram.
 *
 * @author L. Nevay
 */

class HistogramDef
{
public:
  /// Public constructor only for compatibility with ROOT - not intended for use.
  HistogramDef();

  /// Use this constructor.
  HistogramDef(const std::string& treeNameIn,
               const std::string& histNameIn,
               int                nDimensionsIn,
               const std::string& variable,
               const std::string& selectionIn = "1",
               bool               perEnetry   = true);
  virtual ~HistogramDef(){;};

  /// Copy this instance. Virtual to be overridden in derived classes.
  virtual HistogramDef* Clone() const = 0;

  /// Print out the definition in the original syntax
  friend std::ostream& operator<< (std::ostream &out, const HistogramDef& s);

  /// Provide the binning string in the original input format for feedback.
  virtual std::string GetBinningString() const = 0;

  /// Get the first string that defines the histogram in rebdsim for feedback.
  virtual std::string GetHistogramString() const = 0;

  /// Little utility function.
  void ReplaceStringInVariable(const std::string& match,
                               const std::string& replacement);
  
  std::string treeName;
  std::string histName;
  int         nDimensions;
  std::string variable;
  std::string selection;
  bool        perEntry;

  ClassDef(HistogramDef, 1);
};

#endif
