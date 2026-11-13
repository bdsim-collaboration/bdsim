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
#ifndef HISTOGRAMDEF4D_H
#define HISTOGRAMDEF4D_H
#include "BinSpecification.hh"
#include "HistogramDef3D.hh"

#include <string>

#include "Rtypes.h" // for classdef

/**
 * @brief Specification for 4D Histogram.
 *
 * @author E. Ramoisiaux
 */

class HistogramDef4D: public HistogramDef3D
{
public:
  /// Public constructor only for compatibility with ROOT - not indended for use.
  HistogramDef4D();
  
  /// Use this constructor.
  HistogramDef4D(const std::string& treeNameIn,
                 const std::string& histNameIn,
                 const BinSpecification& xBinningIn,
                 const BinSpecification& yBinningIn,
                 const BinSpecification& zBinningIn,
                 const BinSpecification& eBinningIn,
                 const std::string&      variableIn,
                 const std::string&      eScaleIn,
                 const std::string&      selectionIn = "1",
                 bool                    perEntryIn  = true);
  
  virtual ~HistogramDef4D();

  /// Return n bins and ranges.
  virtual std::string GetBinningString() const;

  /// Get the first string that defines the histogram in rebdsim for feedback.
  virtual std::string GetHistogramString() const;

  BinSpecification eBinning;
  std::string eScale;

  ClassDef(HistogramDef4D, 1);
};

#endif
