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
#include "blmplacement.h"

using namespace GMAD;

BLMPlacement::BLMPlacement()
{
  clear();
  PublishMembers();
}

void BLMPlacement::clear()
{
  name         = "";
  referenceElement = "";
  referenceElementNumber = 0;
  s     = 0;
  x     = 0;
  y     = 0;
  z     = 0;
  phi   = 0;
  theta = 0;
  psi   = 0;
  axisX = 0;
  axisY = 0;
  axisZ = 0;
  angle = 0;
  axisAngle = false;
  side = "";
  sideOffset = 0;

  geometryFile = "";
  geometryType = "";
  blmMaterial  = "";
  blm1 = 0;
  blm2 = 0;
  blm3 = 0;
  blm4 = 0;
  scoreQuantity = "";
  bias = "";
}

void BLMPlacement::PublishMembers()
{
  publish("name",          &BLMPlacement::name);
  publish("referenceElement", &BLMPlacement::referenceElement);
  publish("referenceElementNumber", &BLMPlacement::referenceElementNumber);
  publish("s",             &BLMPlacement::s);
  publish("x",             &BLMPlacement::x);
  publish("y",             &BLMPlacement::y);
  publish("z",             &BLMPlacement::z);
  publish("phi",           &BLMPlacement::phi);
  publish("theta",         &BLMPlacement::theta);
  publish("psi",           &BLMPlacement::psi);
  publish("axisX",         &BLMPlacement::axisX);
  publish("axisY",         &BLMPlacement::axisY);
  publish("axisZ",         &BLMPlacement::axisZ);
  publish("angle",         &BLMPlacement::angle);
  publish("axisAngle",     &BLMPlacement::axisAngle);
  publish("side",          &BLMPlacement::side);
  publish("sideOffset",    &BLMPlacement::sideOffset);

  publish("geometryFile",  &BLMPlacement::geometryFile);
  publish("geometryType",  &BLMPlacement::geometryType);
  publish("blmMaterial",   &BLMPlacement::blmMaterial);
  publish("blm1",          &BLMPlacement::blm1);
  publish("blm2",          &BLMPlacement::blm2);
  publish("blm3",          &BLMPlacement::blm3);
  publish("blm4",          &BLMPlacement::blm4);
  publish("scoreQuantity", &BLMPlacement::scoreQuantity);
  publish("bias",          &BLMPlacement::bias);
}

void BLMPlacement::print()const
{
  std::cout << "BLMPlacement: "
	    << "name "          << name          << std::endl
	    << "referenceElement" << referenceElement << std::endl
	    << "referenceElementNumber" << referenceElementNumber << std::endl
	    << "s"              << s             << std::endl
	    << "x "             << x             << std::endl
    	<< "y "             << y             << std::endl
    	<< "z "             << z             << std::endl
    	<< "phi "           << phi           << std::endl
    	<< "theta "         << theta         << std::endl
    	<< "psi "           << psi           << std::endl
    	<< "axisX "         << axisX         << std::endl
    	<< "axisY "         << axisY         << std::endl
    	<< "axisZ "         << axisZ         << std::endl
    	<< "angle "         << angle         << std::endl
	    << "axisAngle "     << axisAngle     << std::endl
	    << "side "          << side          << std::endl
	    << "sideOffset "    << sideOffset    << std::endl
	    << "geometryFile "  << geometryFile  << std::endl
	    << "geometryType "  << geometryType  << std::endl
	    << "blmMaterial "   << blmMaterial   << std::endl
	    << "blm1 "          << blm1          << std::endl
    	<< "blm2 "          << blm2          << std::endl
    	<< "blm3 "          << blm3          << std::endl
    	<< "blm4 "          << blm4          << std::endl
	    << "scoreQuantity " << scoreQuantity << std::endl
	    << "bias "          << bias          << std::endl;
}
