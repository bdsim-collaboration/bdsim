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
#ifndef BDSOUTPUTROOTEVENTCOLLIMATORINFO_H
#define BDSOUTPUTROOTEVENTCOLLIMATORINFO_H

#ifndef __ROOTBUILD__
class BDSBeamlineElement;
#endif

#include "TObject.h"

#include <vector>

/**
 * @brief Data stored for each collimator in the model.
 * 
 * @author Laurie Nevay
 */

class BDSOutputROOTEventCollimatorInfo: public TObject
{
public:
  /// Default constructor for ROOT
  BDSOutputROOTEventCollimatorInfo();

  std::string componentName;
  std::string componentType;
  double      length;
  double      tilt;
  double      offsetX;
  double      offsetY;
  std::string material;
  double      xSizeIn;
  double      ySizeIn;
  double      xSizeOut;
  double      ySizeOut;
  double      jawTiltLeft;
  double      jawTiltRight;
  
  virtual ~BDSOutputROOTEventCollimatorInfo();
#ifndef __ROOTBUILD__
  void Fill(const BDSBeamlineElement* element);
#endif
  virtual void Flush();

  ClassDef(BDSOutputROOTEventCollimatorInfo, 2);
};


#endif
