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
#ifndef BDSOUTPUTROOTEVENTBEAM_H
#define BDSOUTPUTROOTEVENTBEAM_H

#include "TObject.h"

// relative path to suppress ROOT error message (issue #199)
#include "../parser/beamBase.h"

/** 
 * @brief Class to store all beam options for a BDSIM run.
 *
 * @author Laurie Nevay
 */

class BDSOutputROOTEventBeam: public TObject, public GMAD::BeamBase
{
public:
  BDSOutputROOTEventBeam();
  explicit BDSOutputROOTEventBeam(const GMAD::BeamBase* ob);
  virtual ~BDSOutputROOTEventBeam();
  
#ifndef __ROOTBUILD__   
  void Fill();
#endif
  ClassDef(BDSOutputROOTEventBeam,7);
};

#endif
