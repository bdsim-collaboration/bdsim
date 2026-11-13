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
#ifndef BDSANALYSISMANAGER_H
#define BDSANALYSISMANAGER_H

#include "globals.hh"         // geant4 globals / types
#include <vector>

class BDSHistogram1D;

/**
 * @brief Analysis interface class. Create, store and access histograms.
 * 
 * Singleton pattern. This is conceptually based on the Geant4 AnalysisManager classes,
 * however this does not inherit it nor adhere strictly to it. The
 * purpose of this class is only to deal with histograms. The Geant4
 * AnalysisManagers are based on an output format.  Here this will 
 * be a communal histogram class and storage system and the different
 * output formats will write them appropriately.
 */

class BDSAnalysisManager
{
public:
  /// singleton accessor
  static BDSAnalysisManager* Instance();
  ~BDSAnalysisManager();

  /// Create a new histogram
  G4int Create1DHistogram(G4String name,
			  G4String title,
			  G4int    nbins,
			  G4double xmin,
			  G4double xmax,
			  G4String xlabel="",
			  G4String ylabel="");

  G4int Create1DHistogram(G4String name,
			  G4String title,
			  std::vector<double>& edges,
			  G4String xlabel="",
			  G4String ylabel="");

  /// Access a histogram
  BDSHistogram1D* GetHistogram(G4int index);

  /// Fill a histogram
  void Fill1DHistogram(G4int histoIndex, G4double value, G4double weight=1.0);
  /// Fill a histogram with a range
  void Fill1DHistogram(G4int histoIndex, std::pair<G4double,G4double> range, G4double weight=1.0);
  
  /// Return number of histograms
  G4int NumberOfHistograms()const;

private:
  /// private default constructor for singleton pattern
  BDSAnalysisManager();

  /// function to check whether histogram index is valid
  void CheckHistogramIndex(G4int index);
  
  static BDSAnalysisManager* _instance;

  std::vector<BDSHistogram1D*> histograms1d;
  
};

#endif

  
