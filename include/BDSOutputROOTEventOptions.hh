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
#ifndef BDSOUTPUTROOTEVENTOPTIONS_H
#define BDSOUTPUTROOTEVENTOPTIONS_H

#include "TObject.h"

// relative path to suppress ROOT error message (issue #199)
#include "../parser/optionsBase.h"

/** 
 * @brief Class to store all options for a BDSIM run.
 *
 * @author Stewart Boogert
 */

class BDSOutputROOTEventOptions: public TObject, public GMAD::OptionsBase
{
public:
  BDSOutputROOTEventOptions();
  explicit BDSOutputROOTEventOptions(const GMAD::OptionsBase* ob);
  virtual ~BDSOutputROOTEventOptions();
  
#ifndef __ROOTBUILD__   
  void Fill();
#endif
  ClassDef(BDSOutputROOTEventOptions,9);
};

#endif
