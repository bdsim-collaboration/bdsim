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
#include "BDSOutputROOTEventCollimatorInfo.hh"

#ifndef __ROOTBUILD__
#include "BDSAcceleratorComponent.hh"
#include "BDSBeamlineElement.hh"
#include "BDSCollimator.hh"
#include "BDSCollimatorJaw.hh"
#include "BDSTiltOffset.hh"

#include "CLHEP/Units/SystemOfUnits.h"
#endif

ClassImp(BDSOutputROOTEventCollimatorInfo)

BDSOutputROOTEventCollimatorInfo::BDSOutputROOTEventCollimatorInfo():
  componentName(""),
  componentType(""),
  length(0),
  tilt(0),
  offsetX(0),
  offsetY(0),
  material(""),
  xSizeIn(0),
  ySizeIn(0),
  xSizeOut(0),
  ySizeOut(0),
  jawTiltLeft(0),
  jawTiltRight(0)
{;}

BDSOutputROOTEventCollimatorInfo::~BDSOutputROOTEventCollimatorInfo()
{;}

void BDSOutputROOTEventCollimatorInfo::Flush()
{
  componentName = "";
  componentType = "";
  length        = 0;
  tilt          = 0;
  offsetX       = 0;
  offsetY       = 0;
  material      = "";
  xSizeIn       = 0;
  ySizeIn       = 0;
  xSizeOut      = 0;
  ySizeOut      = 0;
  jawTiltLeft   = 0;
  jawTiltRight  = 0;
}

#ifndef __ROOTBUILD__
void BDSOutputROOTEventCollimatorInfo::Fill(const BDSBeamlineElement* element)
{
  componentName = element->GetName();
  componentType = element->GetType();
  length        = element->GetChordLength() / CLHEP::m;
  const BDSTiltOffset* to = element->GetTiltOffset();
  if (to)
    {
      tilt = to->GetTilt() / CLHEP::rad;
      offsetX = to->GetXOffset() / CLHEP::m;
      offsetY = to->GetYOffset() / CLHEP::m;
    }
  else
    {
      tilt    = 0;
      offsetX = 0;
      offsetY = 0;
    }
  material = element->GetMaterial();

  BDSAcceleratorComponent* comp = element->GetAcceleratorComponent();
  BDSCollimator* coll = dynamic_cast<BDSCollimator*>(comp);
  if (coll)
    {
      xSizeIn  = coll->XApertureIn() / CLHEP::m;
      ySizeIn  = coll->YApertureIn() / CLHEP::m;
      xSizeOut = coll->XApertureOut() / CLHEP::m;
      ySizeOut = coll->YApertureOut() / CLHEP::m;
      if (xSizeOut < 0) // fix default -1 value to be the same as input
        {xSizeOut = xSizeIn;}
      if (ySizeOut < 0)
        {ySizeOut = ySizeIn;}
    }

  BDSCollimatorJaw* jcoll = dynamic_cast<BDSCollimatorJaw*>(comp);
  if (jcoll)
    {
      jawTiltLeft = jcoll->GetJawTiltLeft() / CLHEP::rad;
      jawTiltRight = jcoll->GetJawTiltRight() / CLHEP::rad;
    } 
}
#endif
