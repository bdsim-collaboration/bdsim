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
#include "HistSparse1D.hh"
#include "TH1Set.hh"

#include "TH1.h"
#include "TMultiGraph.h"
#include "TClass.h"
#include "TList.h"
#include "TMath.h"

ClassImp(TH1Set)

TH1Set::TH1Set()
{;}

TH1Set::TH1Set(const char* name,
	       const char* title):
  TH1D(name, title, 1, 0, 1)
{;}

TH1Set::~TH1Set()
{;}

Int_t TH1Set::Fill(Double_t x, Double_t w)
{
  fEntries++;

  Int_t bin = 0;
  if (data.HasAbscissa(x))
    {bin = abscissaToBinIndex[x];}
  else
    {bin = AddNewBin(x);}

  data.Fill(x,w); // fill our data structure

  // fill theirs
  if (!fSumw2.fN && w != 1.0 && !TestBit(TH1::kIsNotW) )  Sumw2();   // must be called before AddBinContent
  if (fSumw2.fN)  fSumw2.fArray[bin] += w*w;
  AddBinContent(bin, w);
  if (bin == 0 || bin > fXaxis.GetNbins())
    {
      //if (!TH1::GetStatOverflowsBehaviour())
      //      {return -1;}
    }
  Double_t z= w;
  fTsumw   += z;
  fTsumw2  += z*z;
  fTsumwx  += z*x;
  fTsumwx2 += z*x*x;
  return bin;
}

void TH1Set::DoFillN(Int_t ntimes, const Double_t *x, const Double_t *w, Int_t stride)
{
  ntimes *= stride;
  for (Int_t i=0;i<ntimes;i+=stride)
  {
    Fill(x[i], w[i]);
  }
}

Int_t TH1Set::AddNewBin(long long int x)
{
  Int_t nBinsNew = fXaxis.GetNbins()+1;
  fXaxis.Set(nBinsNew, fXaxis.GetXmin(), fXaxis.GetXmax()+1);
  abscissaToBinIndex[x] = nBinsNew;
  fNcells += 1;
  return nBinsNew;
}

Bool_t TH1Set::Add(const TH1 *h1, Double_t c1)
{
  Bool_t result = TH1D::Add(h1, c1);
  
  auto casted = dynamic_cast<const TH1Set*>(h1);
  if (!casted)
    {return kFALSE;}
  else
  {
    auto& d = casted->data;
    for (const auto& bin : d)
      {data[bin.first] += bin.second;}
  }
  
  return result;
}

Double_t TH1Set::GetBinContentByAbscissa(long long int x) const
{
  return data.HasAbscissa(x) ? GetBinContent(abscissaToBinIndex.at(x)) : 0;
}

Double_t TH1Set::GetBinErrorByAbscissa(long long int x) const
{
  return data.HasAbscissa(x) ? GetBinError(abscissaToBinIndex.at(x)) : 0;
}

void TH1Set::SetBinContentByAbscissa(long long int x, Double_t newValue)
{
  if (data.HasAbscissa(x))
    {SetBinContent(abscissaToBinIndex.at(x), newValue);}
  else
    {SetBinContent(AddNewBin(x), newValue);}
}

void TH1Set::SetBinErrorByAbscissa(long long int x, Double_t newError)
{
  if (data.HasAbscissa(x))
    {SetBinError(abscissaToBinIndex.at(x), newError);}
  else
    {SetBinError(AddNewBin(x), newError);}
}
