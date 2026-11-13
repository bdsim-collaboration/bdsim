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
#ifndef BDSOUTPUTROOTEVENTCAVITYINFO_H
#define BDSOUTPUTROOTEVENTCAVITYINFO_H

#ifndef __ROOTBUILD__
class BDSBeamlineElement;
#endif

#include "TObject.h"

#include <vector>

/**
 * @brief Data stored for each cavity in the model.
 * 
 * @author William Shields
 */

class BDSOutputROOTEventCavityInfo: public TObject
{
public:
  /// Default constructor for ROOT
  BDSOutputROOTEventCavityInfo();

  std::string componentName;
  std::string componentType;
  double      length;
  double      tilt;
  double      efield;
  double      gradient;
  double      frequency;
  double      phase;
  std::string material;
  std::string cavityType;
  double      irisRadius;
  double      thickness;
  double      equatorRadius;
  double      halfCellLength;
  int         numberOfPoints;
  int         numberOfCells;
  double      equatorHorizontalAxis;
  double      equatorVerticalAxis;
  double      irisHorizontalAxis;
  double      irisVerticalAxis;
  double      tangentLineAngle;

  virtual ~BDSOutputROOTEventCavityInfo();
#ifndef __ROOTBUILD__
  void Fill(const BDSBeamlineElement* element);
#endif
  virtual void Flush();

  ClassDef(BDSOutputROOTEventCavityInfo, 1);
};


#endif
