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
#ifndef BEAMANALYSIS_H
#define BEAMANALYSIS_H

#include "Rtypes.h" // for classdef

#include "Analysis.hh"

class Beam;
class TChain;

/**
 * @brief Analysis of the options tree.
 * 
 * @author Stuart Walker.
 */

class BeamAnalysis: public Analysis
{
public:
  /// The default constructor is not intended for use and will not
  /// work for the purpose of analysis. It is required by the ROOT
  /// C++ reflection system in case you wish to save the object.
  BeamAnalysis();

  /// Constructor intended for use to construct a beam analysis object.
  BeamAnalysis(Beam*   beamIn,
	       TChain* chain,
	       bool    perEntryAnalysis = true,
	       bool    debugIn          = false);

  virtual ~BeamAnalysis() noexcept;

  /// Operate on each entry in the run tree.
  virtual void Process();

protected:
  Beam* beam; ///< Beam object that data loaded from the file will be loaded into.

  ClassDef(BeamAnalysis,1);
};

#endif
