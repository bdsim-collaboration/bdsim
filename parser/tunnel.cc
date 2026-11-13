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
#include "tunnel.h"

using namespace GMAD;

Tunnel::Tunnel():
  aper1(0.0),aper2(0.0),
  offsetX(0.0),offsetY(0.0),
  thickness(0.0),soilThickness(0.0),
  floorOffset(0.0),visible(true)
{
  PublishMembers();
}

void Tunnel::clear()
{
  name = "";
  type = "";
  
  aper1         = 0.0;
  aper2         = 0.0;
  offsetX       = 0.0;
  offsetY       = 0.0;
  thickness     = 0.0;
  soilThickness = 0.0;
  floorOffset   = 0.0;
  visible       = true;

  material     = "";
  soilMaterial = "";
  startElement = "";
  endElement   = "";
}

void Tunnel::PublishMembers()
{
  publish("name",&Tunnel::name);
  publish("type",&Tunnel::type);
  publish("radius",&Tunnel::aper1);
  publish("aper1",&Tunnel::aper1);
  publish("aper2",&Tunnel::aper2);
  publish("offsetX",&Tunnel::offsetX);
  publish("offsetY",&Tunnel::offsetY);
  publish("thickness",&Tunnel::thickness);
  publish("soilThickness",&Tunnel::soilThickness);
  publish("floorOffset",&Tunnel::floorOffset);
  publish("visible",&Tunnel::visible);
  publish("material",&Tunnel::material);
  publish("soilMaterial",&Tunnel::soilMaterial);
  publish("startElement",&Tunnel::startElement);
  publish("endElement",&Tunnel::endElement);
}

void Tunnel::print()const
{
  std::cout << "tunnel: "
	    << name          << " "
	    << type          << " "
	    << aper1         << " "
	    << aper2         << " "
	    << offsetX       << " "
	    << offsetY       << " "
	    << thickness     << " "
	    << soilThickness << " "
	    << floorOffset   << " "
	    << material      << " "
	    << soilMaterial  << " "
	    << startElement  << " "
	    << endElement
	    << std::endl;
}
