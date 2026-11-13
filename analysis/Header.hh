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
#ifndef ANALYSISHEADER_H
#define ANALYSISHEADER_H

#include "Rtypes.h" // for classdef

class BDSOutputROOTEventHeader;
class TTree;

/**
 * @brief Header loader.
 *
 * @author Laurie Nevay.
 */

class Header
{
public:
  Header();
  Header(bool debugIn);
  virtual ~Header();

  /// Set the branch addresses to address the contents of the file.
  void SetBranchAddress(TTree* t);

  /// Member that ROOT can map file data to locally.
  BDSOutputROOTEventHeader* header;

private:
  bool debug;
  
  ClassDef(Header,1);
};

#endif
