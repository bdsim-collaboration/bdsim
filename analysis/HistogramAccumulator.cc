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

#include "TH1.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"
#include "BDSBH4DBase.hh"

#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>

ClassImp(HistogramAccumulator)

HistogramAccumulator::HistogramAccumulator():
  nDimensions(1),
  n(0),
  terminated(false),
  mean(nullptr),
  variance(nullptr),
  result(nullptr)
{;}

HistogramAccumulator::HistogramAccumulator(TH1* baseHistogram,
                                           const std::string& resultHistNameIn,
                                           const std::string& resultHistTitleIn):
  HistogramAccumulator(baseHistogram, (int)baseHistogram->GetDimension(), resultHistNameIn, resultHistTitleIn)
{;}

HistogramAccumulator::HistogramAccumulator(TH1*               baseHistogram,
                                           int                nDimensionsIn,
                                           const std::string& resultHistNameIn,
                                           const std::string& resultHistTitleIn):
  nDimensions(nDimensionsIn),
  n(0),
  terminated(false),
  resultHistName(resultHistNameIn),
  resultHistTitle(resultHistTitleIn),
  mean(nullptr),
  variance(nullptr),
  result(nullptr)
{
  std::string meanName = resultHistName + "_Mean";
  std::string variName = resultHistName + "_Vari";
  switch (nDimensions)
    {
    case 1:
      {
        mean     = dynamic_cast<TH1D*>(baseHistogram->Clone(meanName.c_str()));
        variance = dynamic_cast<TH1D*>(baseHistogram->Clone(variName.c_str()));
        result   = dynamic_cast<TH1D*>(baseHistogram->Clone(resultHistName.c_str()));
        break;
      }
    case 2:
      {
        mean     = dynamic_cast<TH2D*>(baseHistogram->Clone(meanName.c_str()));
        variance = dynamic_cast<TH2D*>(baseHistogram->Clone(variName.c_str()));
        result   = dynamic_cast<TH2D*>(baseHistogram->Clone(resultHistName.c_str()));
        break;
      }
    case 3:
      {
        mean     = dynamic_cast<TH3D*>(baseHistogram->Clone(meanName.c_str()));
        variance = dynamic_cast<TH3D*>(baseHistogram->Clone(variName.c_str()));
        result   = dynamic_cast<TH3D*>(baseHistogram->Clone(resultHistName.c_str()));
        break;
      }
    case 4:
      {
#ifdef USE_BOOST
        mean     = dynamic_cast<BDSBH4DBase*>(baseHistogram)->Clone(meanName.c_str());
        variance = dynamic_cast<BDSBH4DBase*>(baseHistogram)->Clone(variName.c_str());
        result   = dynamic_cast<BDSBH4DBase*>(baseHistogram)->Clone(resultHistName.c_str());
        break;
#endif
      }
    default:
      {throw std::domain_error("Invalid number of dimensions"); break;}
    }
  if (mean && variance && result)
    {// technically these could be nullptr
      if (nDimensions != 4)
        {
          // empty contents
          mean->Reset();
          variance->Reset();
          result->Reset();
          // set title
          result->SetTitle(resultHistTitle.c_str());
          mean->SetTitle(meanName.c_str());
          variance->SetTitle(variName.c_str());
        }
      else
        {
#ifdef USE_BOOST
          // empty contents
          static_cast<BDSBH4DBase*>(mean)->Reset_BDSBH4D();
          static_cast<BDSBH4DBase*>(variance)->Reset_BDSBH4D();
          static_cast<BDSBH4DBase*>(result)->Reset_BDSBH4D();
          // set title
          static_cast<BDSBH4DBase*>(result)->SetTitle(resultHistTitle.c_str());
          static_cast<BDSBH4DBase*>(mean)->SetTitle(meanName.c_str());
          static_cast<BDSBH4DBase*>(variance)->SetTitle(variName.c_str());
#endif
        }
    }
}

HistogramAccumulator::~HistogramAccumulator()
{
  // deleting histograms removes them from currently open output file
  delete mean;
  delete variance;
  // leak result here as ROOT annoyingly requires this to be left
}

void HistogramAccumulator::Accumulate(TH1* newValue)
{
  // temporary variables
  double newMean = 0;
  double newVari = 0;
  const double error   = 0; // needed to pass reference to unused parameter
  const unsigned long nEntriesToAccumulate = 1;

  n++; // must always count even if nothing to add up

  switch (nDimensions)
    {
    case 1:
      {
        TH1D* h1  = dynamic_cast<TH1D*>(mean);
        TH1D* h1e = dynamic_cast<TH1D*>(variance);
        TH1D* ht  = dynamic_cast<TH1D*>(newValue);
        for (int j = 0; j <= h1->GetNbinsX() + 1; ++j)
          {
            AccumulateSingleValue(h1->GetBinContent(j),
                                  h1e->GetBinContent(j),
                                  ht->GetBinContent(j),
                                  error, n, nEntriesToAccumulate,
                                  newMean, newVari);
            h1->SetBinContent(j, newMean);
            h1e->SetBinContent(j, newVari);
          }
        break;
      }
    case 2:
      {
        TH2D* h1  = dynamic_cast<TH2D*>(mean);
        TH2D* h1e = dynamic_cast<TH2D*>(variance);
        TH2D* ht  = dynamic_cast<TH2D*>(newValue);
        for (int j = 0; j <= h1->GetNbinsX() + 1; ++j)
          {
            for (int k = 0; k <= h1->GetNbinsY() + 1; ++k)
              {
                AccumulateSingleValue(h1->GetBinContent(j,k),
                                      h1e->GetBinContent(j,k),
                                      ht->GetBinContent(j,k),
                                      error, n, nEntriesToAccumulate,
                                      newMean, newVari);
                h1->SetBinContent(j, k, newMean);
                h1e->SetBinContent(j, k, newVari);
              }
          }
        break;
      }
    case 3:
      {
        TH3D* h1  = dynamic_cast<TH3D*>(mean);
        TH3D* h1e = dynamic_cast<TH3D*>(variance);
        TH3D* ht  = dynamic_cast<TH3D*>(newValue);
        for (int j = 0; j <= h1->GetNbinsX() + 1; ++j)
          {
            for (int k = 0; k <= h1->GetNbinsY() + 1; ++k)
              {
                for (int l = 0; l <= h1->GetNbinsZ() + 1; ++l)
                  {
                    AccumulateSingleValue(h1->GetBinContent(j,k,l),
                                          h1e->GetBinContent(j,k,l),
                                          ht->GetBinContent(j,k,l),
                                          error, n, nEntriesToAccumulate,
                                          newMean, newVari);
                    h1->SetBinContent(j, k, l, newMean);
                    h1e->SetBinContent(j, k, l, newVari);
                  }
              }
          }
        break;
      }
    case 4:
      {
#ifdef USE_BOOST
        BDSBH4DBase* h1  = dynamic_cast<BDSBH4DBase*>(mean);
        BDSBH4DBase* h1e = dynamic_cast<BDSBH4DBase*>(variance);
        BDSBH4DBase* ht  = dynamic_cast<BDSBH4DBase*>(newValue);
        for (int j = -1; j <= h1->GetNbinsX(); ++j)
          {
            for (int k = -1; k <= h1->GetNbinsY(); ++k)
              {
                for (int l = -1; l <= h1->GetNbinsZ(); ++l)
                  {
                    for (int e = -1; e <= h1->GetNbinsE(); ++e)
                      {
                        AccumulateSingleValue(h1->At(j,k,l,e),
                                              h1e->At(j,k,l,e),
                                              ht->At(j,k,l,e),
                                              error, n, nEntriesToAccumulate,
                                              newMean, newVari);
                        h1->Set_BDSBH4D(j,k,l,e, newMean);
                        h1e->Set_BDSBH4D(j,k,l,e, newVari);
                      }
                    
                  }
              }
          }
        break;
#endif
      }
    default:
      {break;}
    }
}

TH1* HistogramAccumulator::Terminate()
{
  // error on mean is sqrt(1/n) * std = sqrt(1/n) * sqrt(1/(n-1)) * sqrt(variance)
  // the only variable is the variance, so take the rest out as a factor.
  const double nD = (double)n; // cast only once
  const double factor = std::sqrt(1./(nD * (nD - 1))); // nan if n = 1 -> won't be used
  double mn     = 0; // temporary variable for mean
  double err    = 0; // temporary variable for standard error on mean
  double var    = 0; // temporary variable for variance
  
  // note here we set the std to 0 if there's only one entry (ie n = 1) to avoid
  // division by zero and nans
  switch (nDimensions)
    {
    case 1:
      {
        for (int j = 0; j <= result->GetNbinsX() + 1; ++j)
          {
            mn  = mean->GetBinContent(j);
            var = variance->GetBinContent(j);
            err = n > 1 ? factor*std::sqrt(var) : 0;
            result->SetBinContent(j, mn);
            result->SetBinError(j,   err);
          }
        break;
      }
    case 2:
      {
        for (int j = 0; j <= result->GetNbinsX() + 1; ++j)
          {
            for (int k = 0; k <= result->GetNbinsY() + 1; ++k)
              {
                mn  = mean->GetBinContent(j,k);
                var = variance->GetBinContent(j, k);
                err = n > 1 ? factor*std::sqrt(var) : 0;
                result->SetBinContent(j, k, mn);
                result->SetBinError(j, k,   err);
              }
          }
        break;
      }
    case 3:
      {
        for (int j = 0; j <= result->GetNbinsX() + 1; ++j)
          {
            for (int k = 0; k <= result->GetNbinsY() + 1; ++k)
              {
                for (int l = 0; l <= result->GetNbinsZ() + 1; ++l)
                  {
                    mn  = mean->GetBinContent(j,k,l);
                    var = variance->GetBinContent(j, k, l);
                    err = n > 1 ? factor*std::sqrt(var) : 0;
                    result->SetBinContent(j,k,l, mn);
                    result->SetBinError(j,k,l,   err);
                  }
              }
          }
        break;
      }
    case 4:
      {
#ifdef USE_BOOST
        auto histCast = dynamic_cast<BDSBH4DBase*>(result);
        auto mnCast   = dynamic_cast<BDSBH4DBase*>(mean);
        auto varCast  = dynamic_cast<BDSBH4DBase*>(variance);
        auto resCast  = dynamic_cast<BDSBH4DBase*>(result);
        int nBinsX = histCast->GetNbinsX();
        int nBinsY = histCast->GetNbinsY();
        int nBinsZ = histCast->GetNbinsZ();
        int nBinsE = histCast->GetNbinsE();
        for (int j = -1; j <= nBinsX; ++j)
          {
            for (int k = -1; k <= nBinsY; ++k)
              {
                for (int l = -1; l <= nBinsZ; ++l)
                  {
                    for (int e = -1; e <= nBinsE; ++e)
                      {
                        mn  = mnCast->At(j, k, l, e);
                        var = varCast->At(j, k, l, e);
                        err = n > 1 ? factor*std::sqrt(var) : 0;
                        resCast->Set_BDSBH4D(j, k, l, e, mn);
                        resCast->SetError_BDSBH4D(j, k, l, e, err);
                      }
                  }
              }
          }
        break;
#endif
      }
    default:
      {break;}
    }
  if(nDimensions==4)
    {dynamic_cast<BDSBH4DBase*>(result)->SetEntries_BDSBH4D((double)n);}
  else
    {result->SetEntries((double)n);}

  return result;
}

void HistogramAccumulator::AccumulateSingleValue(double         oldMean,
                                                 double         oldVari,
                                                 double         x,
                                                 double       /*xVari*/,
                                                 unsigned long  nEntriesAccumulated,
                                                 unsigned long/*nEntriesToAccumulate*/,
                                                 double&        newMean,
                                                 double&        newVari) const
{
  newMean = oldMean + ((x - oldMean) / (double)nEntriesAccumulated);
  newVari = oldVari + ((x - oldMean) * (x - newMean));
}

void HistogramAccumulator::Flush()
{
  mean->Reset();
  variance->Reset();
  result->Reset();
  n = 0;
  terminated = false;
}