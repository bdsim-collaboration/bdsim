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
#include "BDSBin.hh"
#include "BDSDebug.hh"
#include "BDSHistogram.hh"

#include "globals.hh"

#include <ostream>
#include <utility>
#include <vector>

BDSHistogram1D::BDSHistogram1D(G4double xminIn,
			       G4double xmaxIn,
			       G4int    nbinsIn,
			       G4String nameIn,
			       G4String titleIn,
			       G4String xlabelIn,
			       G4String ylabelIn):
  overflow(BDSBin(xmaxIn,DBL_MAX)),
  underflow(BDSBin(DBL_MIN,xminIn)),
  name(nameIn),title(titleIn),
  xlabel(xlabelIn),ylabel(ylabelIn),
  entries(0),equidistantBins(true),
  xmin(xminIn),xmax(xmaxIn),nbins(nbinsIn)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "name: " << nameIn << ", title: " << titleIn << G4endl;
  G4cout << __METHOD_NAME__ << "xmin: " << xmin << ", xmax: " << xmax << ", nbins: " << nbins << G4endl;
#endif
  // test to see we have at least 1 bin
  if (nbins < 1)
    {
      G4cerr << __METHOD_NAME__ << "must have at least 1 bin - nbins: " << nbins << G4endl;
      exit(1);
    }
  if (xmax <= xmin)
    {
      G4cerr << __METHOD_NAME__ << "xmax must be greater than xmin: xmax = "
	     << xmax << ", xmin = " << xmin << G4endl;
      exit(1);
    }
  
  // reserve size for speed optimisation
  bins.reserve(nbins);

  // calculate binwidth
  binwidth = (xmax - xmin) / (G4double)nbins;
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ 
	 << " S min : "    << xmin 
	 << " m, S max : " << xmax 
	 << " m, nbins : " << nbins 
	 << " Bin width: " << binwidth 
	 << " m" << G4endl;
#endif
  G4double localmin, localmax;
  localmin = xmin;
  localmax = xmin + binwidth;
  for (G4int i = 0; i < nbins; ++i)
    {
      bins.push_back(BDSBin(localmin,localmax));
      localmin += binwidth;
      localmax += binwidth;
    }
  // overflow bin - reassign to encapsulate last bin edge
  overflow = BDSBin(xmax,DBL_MAX);
}

BDSHistogram1D::BDSHistogram1D(std::vector<G4double> binEdges,
			       G4String nameIn,
			       G4String titleIn,
			       G4String xlabelIn,
			       G4String ylabelIn):
  overflow(BDSBin(binEdges.back(), DBL_MAX)),
  underflow(BDSBin(DBL_MIN, binEdges[0])),
  name(nameIn),title(titleIn),
  xlabel(xlabelIn),ylabel(ylabelIn),entries(0),
  equidistantBins(false),xmin(binEdges[0]),
  xmax(binEdges.back())
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "name: " << nameIn << ", title: " << titleIn << G4endl;
  G4cout << __METHOD_NAME__ << "xmin: " << binEdges.front() << ", xmax: " << binEdges.back() << ", nbins: " << binEdges.size() << G4endl;
#endif
  nbins = binEdges.size()-1; // -1 (for extra edge)
  // reserve size for speed optimisation
  bins.reserve(nbins);
  
  // prepare iterators
  std::vector<double>::iterator iter, end;
  iter = binEdges.begin();
  end = binEdges.end();

  if (binEdges.size() >= 2)
    {
      G4double binstart = binEdges.front();
      for (iter = binEdges.begin(); iter != (end-1); ++iter)
	{
	  G4double binfinish = *(iter+1);
	  if ((binfinish - binstart) > 1e-6)
	    { //only add a bin if it's a finite width
	      bins.push_back(BDSBin(binstart,binfinish));
	      binstart = binfinish;
	    }
	  // else do not update binstart to prevent empty space in histogram
	}
    }
  else if (binEdges.size() == 1)
    {
      // default 1x 1m bin
      bins.push_back(BDSBin(binEdges.front(),binEdges.front()+1.0));
    }

  // set number of bins
  nbins = bins.size();
  // calculate average binwidth
  binwidth = (xmax - xmin) / (G4double)nbins;

  //cache bin edges
  lowerBinEdges = GetBinLowerEdges();
}

void BDSHistogram1D::Empty()
{
  for (auto bin : bins)
    {bin.Empty();}
}

std::vector<G4double> BDSHistogram1D::GetBinValues() const
{
  std::vector<G4double> result;
  if (bins.size() < 1)
    {return result;}
  for (auto bin : bins)
    {result.push_back(bin.GetValue());}
  return result;
}

std::vector<std::pair<G4double, G4double> > BDSHistogram1D::GetBinXMeansAndTotals() const
{
  std::vector<std::pair<G4double ,G4double> > result;
  if (bins.size() < 1)
    {return result;}
  for (const auto& bin : bins)
    {result.push_back(bin.GetXMeanAndTotal());}
  return result;
}

std::vector<G4double> BDSHistogram1D::GetBinLowerEdges() const
{
  std::vector<G4double> result;
  if (bins.size() < 1)
    {return result;}
  for (const auto& bin : bins)
    {result.push_back(bin.GetLowerEdge());}
  return result;
}

void BDSHistogram1D::PrintBins()const
{
  G4cout << "Name: "      << name      << G4endl;
  G4cout << "Underflow: " << underflow << G4endl;
  G4cout << "Overflow:  " << overflow  << G4endl;
  for (const auto& bin : bins)
    {G4cout << bin << G4endl;}
}

std::pair<BDSBin, BDSBin> BDSHistogram1D::GetUnderOverFlowBins() const
{return std::make_pair(underflow,overflow);}

std::pair<G4double,G4double> BDSHistogram1D::GetUnderOverFlowBinValues() const
{
  return std::make_pair(bins.front().GetValue(),bins.back().GetValue());
}

G4int BDSHistogram1D::GetBinNumber(G4double x) const
{
  G4int binNumber = nbins; // overflow bin
  if (equidistantBins)
    {
      // floor since vector position starts at 0
      binNumber = std::floor( (x - xmin) / binwidth );
      // set maxima
      if (binNumber < 0) binNumber = -1;
      if (binNumber > nbins) binNumber = nbins;
    }
  else
    {
      if (underflow.InRange(x))
	{binNumber = -1;}
      else if (overflow.InRange(x))
	{binNumber = nbins;}
      else
	{
	  auto bin = std::upper_bound(lowerBinEdges.begin(), lowerBinEdges.end(), x);
	  binNumber = (bin - lowerBinEdges.begin())-1;
	}
    }
  return binNumber;
}

BDSBin& BDSHistogram1D::GetBin(G4int binNumber)
{
  if (binNumber < 0)
    {return underflow;}
  else if (binNumber >= nbins)
    {return overflow;}
  else
    {return bins[binNumber];}
}

BDSBin& BDSHistogram1D::GetBin(G4double x)
{
  G4int binNr = GetBinNumber(x);
  return GetBin(binNr);
}
  
void BDSHistogram1D::Fill(G4double x, G4double weight)
{
  BDSBin& bin = GetBin(x);
  bin+=weight;
  entries++;
}

void BDSHistogram1D::Fill(std::pair<G4double,G4double> range, G4double weight)
{
  G4double lower = range.first;
  G4double upper = range.second;

  // swap if in reverse order
  if (upper < lower)
    {std::swap(lower,upper);}

  G4int binNrLower = GetBinNumber(lower);
  BDSBin& binLower = GetBin(binNrLower);
  // check if upper end in same bin and if so fill
  // this is done instead of directly looking up upperbin and comparing bins
  // since presumably faster than bin lookup so only a little wasteful if not true and faster if true
  if (binLower.InRange(upper))
    {binLower += weight;}
  else
    {
      G4int binNrUpper = GetBinNumber(upper);
      BDSBin& binUpper = GetBin(binNrUpper);
      // fill each bin with the weight of bin-distance / distance
      G4double distance = upper - lower;
      // fill lower and upper bin first
      G4double binLength = (binLower.GetUpperEdge() - lower);
      binLower += weight * binLength / distance;

      binLength = (upper - binUpper.GetLowerEdge());
      binUpper += weight * binLength / distance;

      // rest of bins: always in bins vector and fill over full width
      for (G4int i = binNrLower+1; i<binNrUpper; i++) {
	bins[i] += weight * bins[i].GetLength() / distance;
      }
    }
  entries++;
}

std::ostream& operator<< (std::ostream &out, BDSHistogram1D const &hist)
{
  return out << "Name: \"" << hist.name << "\", Title: \"" << hist.title << "\", "
	     << "X: \"" << hist.xlabel << "\", Y: \"" << hist.ylabel << "\"" << G4endl
	     << "FirstBinLeft = " << hist.bins[0].GetLowerEdge() 
	     << ", LastBinLeft = " << hist.bins.back().GetLowerEdge()
	     << ", NBins = "       << hist.bins.size()
	     << ", NEntries = "    << hist.entries;
}
