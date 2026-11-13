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
#include "HistogramDef2D.hh"

#include <string>

ClassImp(HistogramDef2D)

HistogramDef2D::HistogramDef2D():
  yBinning(BinSpecification())
{;}

HistogramDef2D::HistogramDef2D(const std::string&      treeNameIn,
                               const std::string&      histNameIn,
                               const BinSpecification& xBinningIn,
                               const BinSpecification& yBinningIn,
                               const std::string&      variableIn,
                               const std::string&      selectionIn,
                               bool                    perEntryIn):
  HistogramDef1D(treeNameIn, histNameIn, xBinningIn, variableIn, selectionIn, perEntryIn),
  yBinning(yBinningIn)
{
  nDimensions = 2;
}

HistogramDef2D::~HistogramDef2D()
{;}

std::string HistogramDef2D::GetBinningString() const
{
  std::string result = "{" + std::to_string(xBinning.n) + "," + std::to_string(yBinning.n) + "} {";
  result += xBinning.GetBinString() + "," + yBinning.GetBinString() + "}";
  return result;
}

std::string HistogramDef2D::GetHistogramString() const
{
  std::string result = perEntry ? "Histogram2D" : "SimpleHistogram2D";
  if (xBinning.isLogSpaced || yBinning.isLogSpaced)
    {result += xBinning.GetLogString() + yBinning.GetLogString();}
  return result;
}