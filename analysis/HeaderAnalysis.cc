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
#include "BDSOutputROOTEventHeader.hh"
#include "Header.hh"
#include "HeaderAnalysis.hh"

#include "TChain.h"
#include "TFile.h"
#include "TTree.h"

#include <set>
#include <stdexcept>
#include <string>
#include <vector>

HeaderAnalysis::HeaderAnalysis(const std::vector<std::string>& filenamesIn,
                               Header* headerIn,
                               TChain* chainIn):
  filenames(filenamesIn),
  header(headerIn),
  chain(chainIn)
{;}

HeaderAnalysis::~HeaderAnalysis() noexcept
{;}

unsigned long long int HeaderAnalysis::CountNOriginalEvents(unsigned long long int& nEventsInFileIn,
                                                            unsigned long long int& nEventsInFileSkippedIn,
                                                            unsigned long long int& nEventsRequestedIn,
                                                            unsigned int& distrFileLoopNTimesIn)
{
  unsigned long long int nOriginalEvents = 0;
  nEventsInFileIn = 0;
  nEventsInFileSkippedIn = 0;
  nEventsRequestedIn = 0;
  distrFileLoopNTimesIn = 0;

  for (const auto& fn : filenames)
    {
      TFile* ft = nullptr;
      try
        {ft = new TFile(fn.c_str(), "READ");}
      catch (const std::exception& e)
        {continue;}
      if (ft->IsZombie())
        {
          delete ft;
          continue;
        }
      Header* ha = new Header();
      TTree* ht = dynamic_cast<TTree*>(ft->Get("Header"));
      if (!ht)
        {
          delete ft;
          continue;
        }
      ha->SetBranchAddress(ht);
      ht->GetEntry(ht->GetEntries()-1); // get the last entry

      nOriginalEvents += ha->header->nOriginalEvents;
      nEventsInFileIn += ha->header->nEventsInFile;
      nEventsInFileSkippedIn += ha->header->nEventsInFileSkipped;
      nEventsRequestedIn += ha->header->nEventsRequested;
      distrFileLoopNTimesIn += ha->header->distrFileLoopNTimes;
      ft->Close();
      delete ft;
      delete ha;
  }
  return nOriginalEvents;
}
