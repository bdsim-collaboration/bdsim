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
#include "HistogramMerge.hh"
#include "rebdsim.hh"

ClassImp(HistogramMerge)

HistogramMerge::HistogramMerge():
debug(false)
{;}

HistogramMerge::HistogramMerge(BDSOutputROOTEventHistograms* h,
			       bool debugIn):
  debug(debugIn)
{
  auto h1i = h->Get1DHistograms();
  for(auto hist : h1i)
  {
    auto ch  = (TH1D*)hist->Clone();
    auto chE = (TH1D*)hist->Clone();
    ch->Reset();
    chE->Reset();
    this->histograms1D.push_back(ch);
    this->histograms1DN.push_back(0);
    this->histograms1DError.push_back(chE);
  }

  auto h2i = h->Get2DHistograms();
  for(auto hist : h2i)
  {
    auto ch  = (TH2D*)hist->Clone();
    auto chE = (TH2D*)hist->Clone();
    ch->Reset();
    chE->Reset();
    this->histograms2D.push_back(ch);
    this->histograms2DN.push_back(0);
    this->histograms2DError.push_back(chE);
  }

  auto h3i = h->Get3DHistograms();
  for (auto hist : h3i)
    {
      auto ch  = (TH3D*)hist->Clone();
      auto chE = (TH3D*)hist->Clone();
      ch->Reset();
      chE->Reset();
      histograms3D.push_back(ch);
      histograms3DN.push_back(0);
      histograms3DError.push_back(chE);
    }
  
  Add(h);
}

HistogramMerge::~HistogramMerge()
{;}

void HistogramMerge::Add(BDSOutputROOTEventHistograms *hIn)
{
  // loop over 1d histograms
  auto h1i = hIn->Get1DHistograms();
  for(size_t i=0;i<h1i.size();++i)
  {
    auto h1  = this->histograms1D[i];
    auto h1e = this->histograms1DError[i];
    h1e->SetName((std::string(h1->GetName())+"Error").c_str());
    auto h2  = hIn->Get1DHistograms()[i];
    // loop over all bins (0=underflow nbins+1=overflow)
    for(int j=0;j<=h1->GetNbinsX()+1;++j)
    {
      h1->SetBinContent(j,h1->GetBinContent(j)+h2->GetBinContent(j));
      h1e->SetBinContent(j,h1e->GetBinContent(j)+pow(h2->GetBinContent(j),2));
    }
    histograms1DN[i] = histograms1DN[i]+1;
  }

  // loop over 2d histograms
  auto h2i = hIn->Get2DHistograms();
  for(size_t i=0;i<h2i.size();++i)
  {
    auto h1 = this->histograms2D[i];
    auto h1e = this->histograms2DError[i];
    h1e->SetName((std::string(h1->GetName())+"Error").c_str());
    auto h2  = hIn->Get2DHistograms()[i];

    for(int j=0;j<=h1->GetNbinsX()+1;++j)
    {
      for(int k=0;k<=h1->GetNbinsY()+1;++k)
      {
        h1->SetBinContent(j,k,h1->GetBinContent(j,k)+h2->GetBinContent(j,k));
        h1e->SetBinContent(j,k,h1e->GetBinContent(j,k)+pow(h2->GetBinContent(j,k),2));
      }
    }
    histograms2DN[i] = histograms2DN[i]+1;
  }

  // loop over 3d histograms
  auto h3i = hIn->Get3DHistograms();
  for(size_t i=0;i<h3i.size();++i)
  {
    auto h1  = histograms3D[i];
    auto h1e = histograms3DError[i];
    h1e->SetName((std::string(h1->GetName())+"Error").c_str());
    auto h2  = hIn->Get3DHistograms()[i];

    for(int j = 0; j <= h1->GetNbinsX() + 1; ++j)
      {
	for(int k = 0; k <= h1->GetNbinsY() + 1; ++k)
	  {
	    for (int l = 0; l <= h1->GetNbinsZ()+1; ++l)
	      {
		h1->SetBinContent(j,k,l,h1->GetBinContent(j,k,l)+h2->GetBinContent(j,k,l));
		h1e->SetBinContent(j,k,l, h1e->GetBinContent(j,k,l)+pow(h2->GetBinContent(j,k,l),2));
	      }
	  }
      }
    histograms3DN[i] = histograms3DN[i]+1;
  }
  
}

void HistogramMerge::Terminate()
{
  if(debug)
    {std::cout << "HistogramMerge::Terminate" << std::endl;}
  
  // loop over 1d histograms
  for(unsigned int i=0;i<histograms1D.size();++i)
  {
    auto h1  = histograms1D[i];
    auto h1e = histograms1DError[i];
    int entries = histograms1DN[i];

    // loop over all bins (0=underflow nbins+1=overflow)
    for(int j=0;j<=h1->GetNbinsX()+1;++j)
    {
      double mean = h1->GetBinContent(j)/entries;
      double std  = sqrt((h1e->GetBinContent(j)/entries-pow(mean,2))/entries);
      h1->SetBinContent(j,mean);
      h1->SetBinError(j,std);
    }
    h1->SetEntries(entries);
  }

  // loop over 2d histograms
  for(unsigned int i=0;i<histograms2D.size();++i)
  {
    auto h1  = histograms2D[i];
    auto h1e = histograms2DError[i];
    int entries = histograms2DN[i];
    for(int j=0;j<=h1->GetNbinsX()+1;++j)
    {
      for(int k=0;k<=h1->GetNbinsY()+1;++k)
      {
        double mean = h1->GetBinContent(j,k)/entries;
        double std  = sqrt((h1e->GetBinContent(j,k)/entries-pow(mean,2))/entries);
        h1->SetBinContent(j,k,mean);
        h1->SetBinError(j,k,std);
      }
    }
    h1->SetEntries(entries);
  }

  // loop over 3d histograms
  for(unsigned int i=0;i<histograms3D.size();++i)
    {
      auto h1  = histograms3D[i];
      auto h1e = histograms3DError[i];
      int entries = histograms3DN[i];
      for(int j = 0; j <= h1->GetNbinsX() + 1; ++j)
	{
	  for(int k = 0; k <= h1->GetNbinsY() + 1; ++k)
	    {
	      for (int l = 0; l <= h1->GetNbinsZ()+1; ++l)
		{
		  double mean = h1->GetBinContent(j,k,l)/entries;
		  double std  = sqrt((h1e->GetBinContent(j,k,l)/entries-pow(mean,2))/entries);
		  h1->SetBinContent(j,k,l,mean);
		  h1->SetBinError(j,k,l,std);
		}
	    }
	}
      h1->SetEntries(entries);
    }
}

void HistogramMerge::Write(TFile* /*outputFile*/,
			   TDirectory* dir)
{
  if (dir)
    {// move to directory in output file
      for (auto& h : histograms1D)
	{dir->Add(h);}
      for (auto& h : histograms2D)
	{dir->Add(h);}
      for (auto& h : histograms3D)
	{dir->Add(h);}
    }

  // write to currently open file.
  for (auto& h : histograms1D)
    {h->Write();}
  for (auto& h : histograms2D)
    {h->Write();}
  for (auto& h : histograms3D)
    {h->Write();}
}
