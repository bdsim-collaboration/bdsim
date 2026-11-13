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
#include "cavitymodel.h"

using namespace GMAD;

CavityModel::CavityModel()
{
  clear();
  PublishMembers();
}

void CavityModel::clear()
{
  name                  = "";
  type                  = "";
  material              = "";
  irisRadius            = 0.0;
  equatorRadius         = 0.0;
  halfCellLength        = 0.0;
  equatorHorizontalAxis = 0.0;
  equatorVerticalAxis   = 0.0;
  irisHorizontalAxis    = 0.0;
  irisVerticalAxis      = 0.0;
  tangentLineAngle      = 0.0;
  thickness             = 0.0;
  numberOfPoints        = 24;
  numberOfCells         = 1;
}

void CavityModel::PublishMembers()
{
  publish("name",                  &CavityModel::name);
  publish("type",                  &CavityModel::type);
  publish("material",              &CavityModel::material);
  publish("irisRadius",            &CavityModel::irisRadius);
  publish("halfCellLength",        &CavityModel::halfCellLength);
  publish("equatorHorizontalAxis", &CavityModel::equatorHorizontalAxis);
  publish("equatorVerticalAxis",   &CavityModel::equatorVerticalAxis);
  publish("equatorRadius",         &CavityModel::equatorRadius);
  publish("irisHorizontalAxis",    &CavityModel::irisHorizontalAxis);
  publish("irisVerticalAxis",      &CavityModel::irisVerticalAxis);
  publish("tangentLineAngle",      &CavityModel::tangentLineAngle);
  publish("thickness",             &CavityModel::thickness);
  publish("numberOfPoints",        &CavityModel::numberOfPoints);
  publish("numberOfCells",         &CavityModel::numberOfCells);
}

void CavityModel::print()const
{
  std::cout << "cavitymodel: "
	    << "name "                   << name << " " << std::endl
	    << "type "                   << type << " " << std::endl
	    << "material "               << material << " " << std::endl
	    << "irisRadius "             << irisRadius << " " << std::endl
	    << "equatorRadius "          << equatorRadius << " " << std::endl
	    << "halfCellLength "         << halfCellLength << " " << std::endl
	    << "equatorHorizontalAxis "  << equatorHorizontalAxis << " " << std::endl
	    << "equatorVerticalAxis "    << equatorVerticalAxis << " " << std::endl
	    << "irisHorizontalAxis "     << irisHorizontalAxis << " " << std::endl
	    << "irisVerticalAxis "       << irisVerticalAxis << " " << std::endl
	    << "tangentLineAngle "       << tangentLineAngle << " " << std::endl
	    << "thickness "              << thickness << " " << std::endl
	    << "numberOfPoints "         << numberOfPoints << " " << std::endl
	    << "numberOfCells "          << numberOfCells << " " << std::endl
	    << std::endl;
}
