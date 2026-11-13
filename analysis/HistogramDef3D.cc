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
#include "HistogramDef3D.hh"

#include <string>

ClassImp(HistogramDef3D)

HistogramDef3D::HistogramDef3D():
  zBinning(BinSpecification())
{;}

HistogramDef3D::HistogramDef3D(const std::string&      treeNameIn,
                               const std::string&      histNameIn,
                               const BinSpecification& xBinningIn,
                               const BinSpecification& yBinningIn,
                               const BinSpecification& zBinningIn,
                               const std::string&      variableIn,
                               const std::string&      selectionIn,
                               bool                    perEntryIn):
  HistogramDef2D(treeNameIn, histNameIn, xBinningIn, yBinningIn, variableIn, selectionIn, perEntryIn),
  zBinning(zBinningIn)
{
  nDimensions = 3;
}

HistogramDef3D::~HistogramDef3D()
{;}

std::string HistogramDef3D::GetBinningString() const
{
  std::string result = "{" + std::to_string(xBinning.n) + "," + std::to_string(yBinning.n) + "," + std::to_string(zBinning.n) + "} {";
  result += xBinning.GetBinString() + "," + yBinning.GetBinString() + "," + zBinning.GetBinString() + "}";
  return result;
}

std::string HistogramDef3D::GetHistogramString() const
{
  std::string result = perEntry ? "Histogram3D" : "SimpleHistogram3D";
  if (xBinning.isLogSpaced || yBinning.isLogSpaced || zBinning.isLogSpaced)
    {result += xBinning.GetLogString() + yBinning.GetLogString() + zBinning.GetLogString();}
  return result;
}