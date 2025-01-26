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
#include "HistogramAccumulator.hh"
#include "HistogramMeanFromFile.hh"

#include "BDSOutputROOTEventHistograms.hh"

#include "TDirectory.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"
#include "BDSBH4DBase.hh"

#include "TTree.h"
#include "TFile.h"

#include <string>
#include <vector>

ClassImp(HistogramMeanFromFile)

HistogramMeanFromFile::HistogramMeanFromFile()
{;}

HistogramMeanFromFile::HistogramMeanFromFile(BDSOutputROOTEventHistograms* h)
{
  for (auto hist : h->Get1DHistograms())
    {
      std::string name  = std::string(hist->GetName());
      std::string title = std::string(hist->GetTitle());
      histograms1d.push_back(new HistogramAccumulator(hist, 1, name, title));
    }

  for (auto hist : h->Get2DHistograms())
    {
      std::string name  = std::string(hist->GetName());
      std::string title = std::string(hist->GetTitle());
      histograms2d.push_back(new HistogramAccumulator(hist, 2, name, title));
    }

  for (auto hist : h->Get3DHistograms())
    {
      std::string name  = std::string(hist->GetName());
      std::string title = std::string(hist->GetTitle());
      histograms3d.push_back(new HistogramAccumulator(hist, 3, name, title));
    }

  for (auto hist : h->Get4DHistograms())
    {
      std::string name  = hist->GetName();
      std::string title = hist->GetTitle();
      histograms4d.push_back(new HistogramAccumulator(hist, 4, name, title));
    }

  Accumulate(h);
}

HistogramMeanFromFile::~HistogramMeanFromFile()
{
  for (auto h : histograms1d)
    {delete h;}
  for (auto h : histograms2d)
    {delete h;}
  for (auto h : histograms3d)
    {delete h;}
  for (auto h : histograms4d)
    {delete h;}
}

void HistogramMeanFromFile::Accumulate(BDSOutputROOTEventHistograms* hNew)
{
  auto h1i = hNew->Get1DHistograms();
  for (unsigned int i = 0; i < (unsigned int)histograms1d.size(); ++i)
    {histograms1d[i]->Accumulate(h1i[i]);}
  auto h2i = hNew->Get2DHistograms();
  for (unsigned int i = 0; i < (unsigned int)histograms2d.size(); ++i)
    {histograms2d[i]->Accumulate(h2i[i]);}
  auto h3i = hNew->Get3DHistograms();
  for (unsigned int i = 0; i < (unsigned int)histograms3d.size(); ++i)
    {histograms3d[i]->Accumulate(h3i[i]);}
  auto h4i = hNew->Get4DHistograms();
  for (unsigned int i = 0; i < (unsigned int)histograms4d.size(); ++i)
    {histograms4d[i]->Accumulate(h4i[i]);}
}

void HistogramMeanFromFile::Terminate()
{
  // terminate each accumulator
  // this returns a pointer to the result but no need to store
  for (auto& h : histograms1d)
    {h->Terminate();}
  for (auto& h : histograms2d)
    {h->Terminate();}
  for (auto& h : histograms3d)
    {h->Terminate();}
  for (auto& h : histograms4d)
    {h->Terminate();}
}

void HistogramMeanFromFile::Write(TDirectory* dir)
{
  if (dir)
    {// move to directory in output file
      for (auto& h : histograms1d)
        {dir->Add(h->Result());}
      for (auto& h : histograms2d)
        {dir->Add(h->Result());}
      for (auto& h : histograms3d)
        {dir->Add(h->Result());}
      for (auto& h : histograms4d)
        {dir->Add(h->Result());}
    }

  // write to currently open file.
  for (auto& h : histograms1d)
    {h->Result()->Write();}
  for (auto& h : histograms2d)
    {h->Result()->Write();}
  for (auto& h : histograms3d)
    {h->Result()->Write();}
  for (auto& h : histograms4d)
    {h->Result()->Write();}
}

