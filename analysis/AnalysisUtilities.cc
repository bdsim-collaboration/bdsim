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
#include "AnalysisUtilities.hh"
#include "BinGeneration.hh"
#include "RBDSException.hh"

#include "Rtypes.h" // for classdef

#include "TH1D.h"

#include <algorithm>
#include <string>
#include <vector>
#include <iostream>

ClassImp(AnalysisUtilities)

AnalysisUtilities::AnalysisUtilities()
{;}

AnalysisUtilities::~AnalysisUtilities()
{;}

std::vector<double> AnalysisUtilities::LogSpace(double start,
						double stop,
						int    nBins,
						double base,
						bool   includeLastPoint)
{
  return RBDS::LogSpace(start, stop, nBins, base, includeLastPoint);
}

std::vector<double> AnalysisUtilities::LinSpace(double start,
						double stop,
						int    nBins,
						bool   includeLastPoint)
{
  return RBDS::LinSpace(start, stop, nBins, includeLastPoint);
}

void AnalysisUtilities::FillTH1D(TH1D& h1d,
				 const std::vector<float>& values,
				 const std::vector<float>& weights)
{
  for (int i=0; i < (int)values.size(); i++)
    {h1d.Fill(values[i],weights[i]);}
}

std::string RBDS::DefaultOutputName(const std::string& inputFilePath,
                                    const std::string& suffix)
{
  std::string result = inputFilePath;
  // protect against globbing
  std::replace(result.begin(), result.end(), '*', '_');
  // get only the filename - ie just write the file to the cwd
  auto foundSlash = result.rfind('/'); // find the last '/'
  if (foundSlash != std::string::npos)
    {result = result.substr(foundSlash + 1);} // the rest
  std::string key = ".root";
  auto found = result.rfind(key);
  if (found != std::string::npos)
    {result.replace(found, key.length(), suffix + ".root");}
  else
    {result += ".root";}
  return result;
}
