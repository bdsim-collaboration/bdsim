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
#include "HistogramAccumulatorMerge.hh"

#include "TH1.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"
#include "BDSBH4DBase.hh"

#include <cmath>
#include <iostream>
#include <string>

ClassImp(HistogramAccumulatorMerge)

HistogramAccumulatorMerge::HistogramAccumulatorMerge():
  HistogramAccumulator(nullptr, 0, "", "")
{;}

HistogramAccumulatorMerge::HistogramAccumulatorMerge(TH1*               baseHistogram,
						     int                nDimensionsIn,
						     const std::string& resultHistNameIn,
						     const std::string& resultHistTitleIn):
  HistogramAccumulator(baseHistogram,
		       nDimensionsIn,
		       resultHistNameIn,
		       resultHistTitleIn)
{;}

void HistogramAccumulatorMerge::Accumulate(TH1* newValue)
{
  // temporary variables
  double newMean = 0;
  double newVari = 0;
  double var     = 0;

  // Want the number of events accumulated so far. We purposively set the entries
  // in the mean histogram as the number of events accumulated, not the number of
  // histograms (ie files here).
  unsigned long oldEntries;
  unsigned long newEntries;

  if (nDimensions == 4)
    {
      oldEntries = (unsigned long)static_cast<BDSBH4DBase*>(mean)->GetEntries_BDSBH4D();
      newEntries = (unsigned long)static_cast<BDSBH4DBase*>(newValue)->GetEntries_BDSBH4D();
    }
  else
    {
      oldEntries = (unsigned long)mean->GetEntries();   // works for base class*
      newEntries = (unsigned long)newValue->GetEntries();   // works for base class*
    }
  
  unsigned long newTotalEntries = oldEntries + newEntries;
  const double nD     = (double)newEntries;
  const double factor = nD * (nD - 1);
  
  switch (nDimensions)
    {
    case 1:
      {
	TH1D* h1  = dynamic_cast<TH1D*>(mean);
	TH1D* h1e = dynamic_cast<TH1D*>(variance);
	TH1D* ht  = dynamic_cast<TH1D*>(newValue);
	for (int j = 0; j <= h1->GetNbinsX() + 1; ++j)
	  {
	    var = std::pow(ht->GetBinError(j), 2) * factor;
	    AccumulateSingleValue(h1->GetBinContent(j),
				  h1e->GetBinContent(j),
				  ht->GetBinContent(j),
				  var,
				  oldEntries, newEntries,
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
		var = std::pow(ht->GetBinError(j,k), 2) * factor;
		AccumulateSingleValue(h1->GetBinContent(j,k),
				      h1e->GetBinContent(j,k),
				      ht->GetBinContent(j,k),
				      var,
				      oldEntries, newEntries,
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
		    var = std::pow(ht->GetBinError(j,k,l), 2) * factor;
		    AccumulateSingleValue(h1->GetBinContent(j,k,l),
					  h1e->GetBinContent(j,k,l),
					  ht->GetBinContent(j,k,l),
					  var,
					  oldEntries, newEntries,
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
			var = std::pow(ht->AtError(j,k,l,e), 2) * factor;
			AccumulateSingleValue(h1->At(j,k,l,e),
					      h1e->At(j,k,l,e),
					      ht->At(j,k,l,e),
					      var,
					      oldEntries, newEntries,
					      newMean, newVari);
			h1->Set_BDSBH4D(j, k, l, e, newMean);
			h1e->Set_BDSBH4D(j, k, l, e, newVari);
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
    {
      dynamic_cast<BDSBH4DBase*>(mean)->SetEntries_BDSBH4D(newTotalEntries);
      dynamic_cast<BDSBH4DBase*>(variance)->SetEntries_BDSBH4D(newTotalEntries);
    }
  else
    {
      mean->SetEntries(newTotalEntries);
      variance->SetEntries(newTotalEntries);
    }
  n = newTotalEntries; // updated to Terminate() works correctly
}

void HistogramAccumulatorMerge::AccumulateSingleValue(double        oldMean,
						      double        oldVari,
						      double        x,
						      double        xVari,
						      unsigned long nEntriesAccumulated,
						      unsigned long nEntriesToAccumulate,
						      double&       newMean,
						      double&       newVari) const
{
  double dMean  = x - oldMean;
  double dMean2 = std::pow(dMean, 2);
  double nA = (double)nEntriesAccumulated;
  double nB = (double)nEntriesToAccumulate;
  double nT = nA + nB;

  newMean = oldMean + nB * (dMean / nT);

  double q = (nA * nB) * (dMean2 / nT);
  newVari = oldVari + xVari + q;
}
