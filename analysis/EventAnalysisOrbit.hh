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
#ifndef EVENTANALYSISORBIT_H
#define EVENTANALYSISORBIT_H

#include "EventAnalysis.hh"

#include "Rtypes.h" // for classdef

#include <string>
#include <vector>

class Event;
class TChain;
class TFile;

/**
 * @brief Simple analysis to pull out first hit in each sampler.
 *
 * @author L. Nevay
 */

class EventAnalysisOrbit: public EventAnalysis
{
public:
  /// The default constructor is not intended for use and will not
  /// work for the purpose of analysis. It is required by the ROOT
  /// C++ reflection system in case you wish to save the object.
  EventAnalysisOrbit();

  /// Constructor intended for use to construct an event analysis object.
  EventAnalysisOrbit(Event*   eventIn,
                     TChain*  chain,
                     bool     perEntryAnalysis    = true,
                     bool     processSamplersIn   = true,
                     bool     debugIn             = false,
                     bool     printOutIn          = true,
                     double   printModuloFraction = 0.01,
                     bool     emittanceOnTheFlyIn = false,
                     long int eventStartIn        = 0,
                     long int eventEndIn          = -1);

  virtual ~EventAnalysisOrbit(){;}

  void Clear();                 ///< Empty the member vectors of their data.
  void ExtractOrbit(int index); ///< Extract an orbit from the data.
  void WriteOrbit(TFile* f);    ///< Write orbit to a ROOT file.

  /// @{ Temporary storage for orbit.
  std::vector<double> ss;
  std::vector<double> x;
  std::vector<double> xp;
  std::vector<double> y;
  std::vector<double> yp;
  std::vector<std::string> elementName;
  /// @}

  ClassDef(EventAnalysisOrbit,1);
};

#endif
