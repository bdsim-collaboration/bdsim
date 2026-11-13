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
#ifndef BDSHISTOGRAM_H
#define BDSHISTOGRAM_H

#include "globals.hh"       // geant4 globals / types
#include <vector>
#include <utility>
#include <ostream>

#include "BDSBin.hh"

class BDSHistogram1D;

typedef BDSHistogram1D BDSHistogram;

/**
 * @brief a 1D histogram class
 * 
 * @author Laurie Nevay
 */

class BDSHistogram1D
{
private:
  /// Typedefs up first so we can declare public iterators.
  typedef std::vector<BDSBin> BinVector;

   /// Vector of Bin objects - the data.
  BinVector bins;
  
public:
  /// constructor for equidistant bins
  BDSHistogram1D(G4double xmin,
		 G4double xmax,
		 G4int    nbins,
		 G4String nameIn="histogram",
		 G4String titleIn="histogram",
		 G4String xlabelIn="",
		 G4String ylabelIn="");
  /// constructor for user-defined bin widths
  BDSHistogram1D(std::vector<G4double> binEdges,
		 G4String name="histogram",
		 G4String titleIn="histogram",
		 G4String xlabelIn="",
		 G4String ylabelIn="");
  ~BDSHistogram1D(){;}

  /// Empty the contents of the histogram bins.
  void Empty();
  
  /// Create entry at point x in histogram with weight.
  void Fill(G4double x, G4double weight=1.0);
  
  /// Fill over a range, weights get equally distributed over bin lengths
  void Fill(std::pair<G4double,G4double> range, G4double weight=1.0);

  /// Get all bins
  const std::vector<BDSBin>& GetBins() const {return bins;}

  /// Get the value from all bins.
  std::vector<G4double> GetBinValues() const;

  /// Get the centre of each bin paired with its value.
  std::vector<std::pair<G4double, G4double> > GetBinXMeansAndTotals() const;

  /// Get a vector of the lower edges of each bin.
  std::vector<G4double> GetBinLowerEdges() const;

  /// print value of all bins to cout
  void PrintBins() const;

  /// Access the underflow and overflow bins.
  std::pair<BDSBin,BDSBin> GetUnderOverFlowBins() const;

  /// Acesss the values of the underflow and overflow bins
  std::pair<G4double,G4double> GetUnderOverFlowBinValues() const;

  /// @{Property accessor
  inline const BDSBin& GetUnderflowBin() const {return underflow;}
  inline const BDSBin& GetOverflowBin()  const {return overflow;}
  inline const BDSBin& GetFirstBin()     const {return bins.front();}
  inline const BDSBin& GetLastBin()      const {return bins.back();}
  inline G4String GetName()            const {return name;}
  inline G4String GetTitle()           const {return title;}
  inline G4String GetXLabel()          const {return xlabel;}
  inline G4String GetYLabel()          const {return ylabel;}
  inline size_t   GetNBins()           const {return bins.size();}
  inline G4int    GetNEntries()        const {return entries;}
  inline G4bool   GetEquidistantBins() const {return equidistantBins;}
  inline G4double GetXMin()            const {return xmin;}
  inline G4double GetXMax()            const {return xmax;}
  /// @}

  ///@{ iterator mechanics
  typedef BinVector::iterator       iterator;
  typedef BinVector::const_iterator const_iterator;
  iterator       begin()       {return bins.begin();}
  iterator       end()         {return bins.end();}
  const_iterator begin() const {return bins.begin();}
  const_iterator end()   const {return bins.end();}
  G4bool         empty() const {return bins.empty();}
  ///@}
  
  /// output stream
  friend std::ostream& operator<< (std::ostream &out, BDSHistogram1D const &hist);
  
private:
  BDSHistogram1D();
  /// Returns bin number in vector for value x
  /// smaller than 0 means underflow, larger or equal than nbins means overflow
  /// only accurate if equidistantBins = true, otherwise indication
  G4int GetBinNumber(G4double value)const;
  /// Get Bin corresponding to value
  BDSBin& GetBin(G4double value);
  /// Get Bin corresponding to binnumber
  BDSBin& GetBin(G4int binNumber);
  
  /// overflow bin
  BDSBin overflow;
  
  /// underflow bin
  BDSBin underflow;
  
  G4String name;
  G4String title;
  G4String xlabel;
  G4String ylabel;
  
  /// number of entries  
  G4int    entries;
  
  /// equidistant binning
  G4bool   equidistantBins;

  /// helper variables for cpu speedup
  G4double xmin;
  G4double xmax;
  G4double nbins;
  G4double binwidth;

  /// A cache of lower bin edges in the case of uneven bins for fast searching
  std::vector<G4double> lowerBinEdges;
};

#endif
