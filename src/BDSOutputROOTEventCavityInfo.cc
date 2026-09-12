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
#include "BDSOutputROOTEventCavityInfo.hh"

#ifndef __ROOTBUILD__
#include "BDSAcceleratorComponent.hh"
#include "BDSBeamlineElement.hh"
#include "BDSCavityElement.hh"
#include "BDSCavityInfo.hh"
#include "BDSCavityType.hh"
#include "BDSFieldInfo.hh"
#include "BDSMagnetStrength.hh"
#include "BDSTiltOffset.hh"

#include "CLHEP/Units/SystemOfUnits.h"
#endif

ClassImp(BDSOutputROOTEventCavityInfo)

BDSOutputROOTEventCavityInfo::BDSOutputROOTEventCavityInfo():
  componentName(""),
  componentType(""),
  length(0),
  tilt(0),
  efield(0),
  gradient(0),
  frequency(0),
  phase(0),
  material(""),
  cavityType(""),
  irisRadius(0),
  thickness(0),
  equatorRadius(0),
  halfCellLength(0),
  numberOfPoints(0),
  numberOfCells(0),
  equatorHorizontalAxis(0),
  equatorVerticalAxis(0),
  irisHorizontalAxis(0),
  irisVerticalAxis(0),
  tangentLineAngle(0)
{;}

BDSOutputROOTEventCavityInfo::~BDSOutputROOTEventCavityInfo()
{;}

void BDSOutputROOTEventCavityInfo::Flush()
{
  componentName = "";
  componentType = "";
  length        = 0;
  tilt          = 0;
  efield        = 0;
  gradient      = 0;
  frequency     = 0;
  phase         = 0;
  material      = "";
  cavityType    = "";
  irisRadius    = 0;
  thickness     = 0;
  equatorRadius = 0;
  halfCellLength = 0;
  numberOfPoints = 0;
  numberOfCells = 0;
  equatorHorizontalAxis = 0;
  equatorVerticalAxis = 0;
  irisHorizontalAxis = 0;
  irisVerticalAxis = 0;
  tangentLineAngle = 0;
}

#ifndef __ROOTBUILD__
void BDSOutputROOTEventCavityInfo::Fill(const BDSBeamlineElement* element)
{
  componentName = element->GetName();
  componentType = element->GetType();
  length        = element->GetChordLength() / CLHEP::m;
  const BDSTiltOffset* to = element->GetTiltOffset();
  if (to)
    {tilt = to->GetTilt() / CLHEP::rad;}
  else
    {tilt    = 0;}
  material = element->GetMaterial();

  BDSAcceleratorComponent* comp = element->GetAcceleratorComponent();
  BDSCavityElement* cav = dynamic_cast<BDSCavityElement*>(comp);
  const BDSFieldInfo* fieldInfo = cav->GetFieldInfo();
  const BDSCavityInfo* cavInfo = cav->GetCavityInfo();
  if (fieldInfo)
    {
      const BDSMagnetStrength* ms = fieldInfo->MagnetStrength();
      efield = (*ms)["efield"]/BDSMagnetStrength::Unit("efield");
      frequency = (*ms)["frequency"]/BDSMagnetStrength::Unit("frequency");
      phase = (*ms)["phase"]/BDSMagnetStrength::Unit("phase");
    }
  if (cavInfo)
    {
      cavityType = cavInfo->cavityType.ToString(); //BDS::DetermineCavityType(cavInfo->cavityType);
      irisRadius = cavInfo->irisRadius;
      thickness = cavInfo->thickness;
      equatorRadius = cavInfo->equatorRadius;
      halfCellLength = cavInfo->halfCellLength;
      numberOfPoints = cavInfo->numberOfPoints;
      numberOfCells = cavInfo->numberOfCells;
      equatorHorizontalAxis = cavInfo->equatorHorizontalAxis;
      equatorVerticalAxis = cavInfo->equatorVerticalAxis;
      irisHorizontalAxis = cavInfo->irisHorizontalAxis;
      irisVerticalAxis = cavInfo->irisVerticalAxis;
      tangentLineAngle = cavInfo->tangentLineAngle;
    }
}
#endif
