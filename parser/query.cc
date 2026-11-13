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
#include "query.h"

using namespace GMAD;

Query::Query()
{
  clear();
  PublishMembers();
}

void Query::clear()
{
  name        = "";
  fieldObject = "";
  nx   = 1;
  ny   = 1;
  nz   = 1;
  nt   = 1;
  xmin = 0;
  xmax = 0;
  ymin = 0;
  ymax = 0;
  zmin = 0;
  zmax = 0;
  tmin = 0;
  tmax = 0;
  outfileMagnetic = "";
  outfileElectric = "";

  queryMagneticField = false;
  queryElectricField = false;
  
  overwriteExistingFiles = true;
  printTransform = true;
  
  drawArrows = true;
  drawZeroValuePoints = true;
  drawBoxes = true;
  boxAlpha = 0.2;

  pointsFile = "";

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
  
  checkParameters = true;
}

void Query::PublishMembers()
{
  publish("name",        &Query::name);
  publish("fieldObject", &Query::fieldObject);
  publish("nx",          &Query::nx);
  publish("ny",          &Query::ny);
  publish("nz",          &Query::nz);
  publish("nt",          &Query::nt);
  publish("xmin",        &Query::xmin);
  publish("ymin",        &Query::ymin);
  publish("zmin",        &Query::zmin);
  publish("tmin",        &Query::tmin);
  publish("xmax",        &Query::xmax);
  publish("ymax",        &Query::ymax);
  publish("zmax",        &Query::zmax);
  publish("tmax",        &Query::tmax);
  publish("outfileMagnetic", &Query::outfileMagnetic);
  publish("outfileElectric", &Query::outfileElectric);

  publish("queryMagneticField", &Query::queryMagneticField);
  publish("queryElectricField", &Query::queryElectricField);
  
  publish("overwriteExistingFiles", &Query::overwriteExistingFiles);
  publish("printTransform",         &Query::printTransform);
  
  publish("drawArrows",             &Query::drawArrows);
  publish("drawZeroValuePoints",    &Query::drawZeroValuePoints);
  publish("drawBoxes",              &Query::drawBoxes);
  publish("boxAlpha",               &Query::boxAlpha);
  
  publish("pointsFile", &Query::pointsFile);

  publish("referenceElement", &Query::referenceElement);
  publish("referenceElementNumber", &Query::referenceElementNumber);
  publish("s",             &Query::s);
  publish("x",             &Query::x);
  publish("y",             &Query::y);
  publish("z",             &Query::z);
  publish("phi",           &Query::phi);
  publish("theta",         &Query::theta);
  publish("psi",           &Query::psi);
  publish("axisX",         &Query::axisX);
  publish("axisY",         &Query::axisY);
  publish("axisZ",         &Query::axisZ);
  publish("angle",         &Query::angle);
  publish("axisAngle",     &Query::axisAngle);
  
  publish("checkParameters", &Query::checkParameters);
}

void Query::print()const
{
  std::cout << "query: "
	    << "name "        << name        << std::endl
	    << "nx "  << nx   << std::endl
    	    << "ny "  << ny   << std::endl
    	    << "nz "  << nz   << std::endl
    	    << "nt "  << ny   << std::endl
	    << "x: (" << xmin << ", " << xmax << ")" << std::endl
    	    << "y: (" << ymin << ", " << ymax << ")" << std::endl
    	    << "z: (" << zmin << ", " << zmax << ")" << std::endl
    	    << "t: (" << tmin << ", " << tmax << ")" << std::endl
	    << "outfileMagnetic: " << outfileMagnetic << std::endl
	    << "outfileElectric: " << outfileElectric << std::endl
    	    << "fieldObject "            << fieldObject << std::endl
	    << "queryMagneticField: "    << queryMagneticField     << std::endl
	    << "queryElectricField: "    << queryElectricField     << std::endl
	    << "overwriteExistingFiles " << overwriteExistingFiles << std::endl
	    << "printTransform "         << printTransform         << std::endl
      << "drawArrows "             << drawArrows             << std::endl
      << "drawZeroValuePoints "    << drawZeroValuePoints    << std::endl
      << "drawBoxes "              << drawBoxes              << std::endl
      << "boxAlpha "               << boxAlpha               << std::endl
	    << "pointsFile "             << pointsFile             << std::endl
	    << "referenceElement "       << referenceElement       << std::endl
	    << "referenceElementNumber " << referenceElementNumber << std::endl
	    << "s "             << s             << std::endl
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
	    << "axisAngle "     << axisAngle     << std::endl;
}
