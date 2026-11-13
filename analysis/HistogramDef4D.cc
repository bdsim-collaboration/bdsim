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
#include "BinSpecification.hh"
#include "HistogramDef4D.hh"

#include <string>

ClassImp(HistogramDef4D)

HistogramDef4D::HistogramDef4D():
  eBinning(BinSpecification())
{;}

HistogramDef4D::HistogramDef4D(const std::string&      treeNameIn,
                               const std::string&      histNameIn,
                               const BinSpecification& xBinningIn,
                               const BinSpecification& yBinningIn,
                               const BinSpecification& zBinningIn,
                               const BinSpecification& eBinningIn,
                               const std::string&      variableIn,
                               const std::string&      eScaleIn,
                               const std::string&      selectionIn,
                               bool                    perEntryIn):
  HistogramDef3D(treeNameIn, histNameIn, xBinningIn, yBinningIn, zBinningIn, variableIn, selectionIn, perEntryIn),
  eBinning(eBinningIn),
  eScale(eScaleIn)
{
  nDimensions = 4;
}

HistogramDef4D::~HistogramDef4D()
{;}

std::string HistogramDef4D::GetBinningString() const
{
  std::string result = "{" + std::to_string(xBinning.n) + "," + std::to_string(yBinning.n) + ",";
  result += std::to_string(zBinning.n) + "," + std::to_string(eBinning.n) + "} {";
  result += xBinning.GetBinString() + "," + yBinning.GetBinString() + "," + zBinning.GetBinString() + "," + eBinning.GetBinString() +"}";
  return result;
}

std::string HistogramDef4D::GetHistogramString() const
{
  std::string result = perEntry ? "Histogram3D" : "SimpleHistogram3D";
  if (xBinning.isLogSpaced || yBinning.isLogSpaced || zBinning.isLogSpaced || eBinning.isLogSpaced)
    {result += xBinning.GetLogString() + yBinning.GetLogString() + zBinning.GetLogString() + eBinning.GetLogString();}
  return result;
}