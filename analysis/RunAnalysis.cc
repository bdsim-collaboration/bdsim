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
#include "HistogramMeanFromFile.hh"
#include "rebdsim.hh"  // for debug __METHOD_NAME__
#include "Run.hh"
#include "RunAnalysis.hh"

#include "TChain.h"

#include <iostream>

ClassImp(RunAnalysis)

RunAnalysis::RunAnalysis():
  Analysis("Run.", nullptr, "RunHistogramsMerged"),
  run(nullptr)
{;}

RunAnalysis::RunAnalysis(Run*    runIn,
			 TChain* chainIn,
			 bool    perEntryAnalysis,
			 bool    debugIn):
  Analysis("Run.", chainIn, "RunHistogramsMerged", perEntryAnalysis, debugIn),
  run(runIn)
{;}

RunAnalysis::~RunAnalysis() noexcept
{;}

void RunAnalysis::Process()
{
  if (debug)
    {std::cout << __METHOD_NAME__ << chain->GetEntries() << " " << std::endl;}

  // loop over events
  for (int i = 0; i < chain->GetEntries(); ++i)
    {
      chain->GetEntry(i);
      
      if (i == 0)
        {histoSum = new HistogramMeanFromFile(run->Histos);}
      else
	      {histoSum->Accumulate(run->Histos);}

      // per event histograms
      AccumulatePerEntryHistograms(i);
      
      UserProcess();
    }
}
