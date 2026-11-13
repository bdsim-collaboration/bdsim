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
#ifndef RUNANALYSIS_H
#define RUNANALYSIS_H

#include "Rtypes.h" // for classdef

#include "Analysis.hh"

class Run;
class TChain;

/**
 * @brief Analysis of a run.
 *
 * @author Stuart Walker.
 */

class RunAnalysis: public Analysis
{
public:
  /// The default constructor is not intended for use and will not
  /// work for the purpose of analysis. It is required by the ROOT
  /// C++ reflection system in case you wish to save the object.
  RunAnalysis();

  /// Constructor intended for use to construct a run analysis object.
  RunAnalysis(Run*    runIn,
	      TChain* chainIn,
	      bool    perEntryAnalysis = true,
	      bool    debugIn          = false);

  virtual ~RunAnalysis() noexcept;

  /// Operate on each entry in the run tree.
  virtual void Process();

protected:
  Run* run; ///< Run object that data loaded from the file will be loaded into.

  ClassDef(RunAnalysis,1);
};

#endif
