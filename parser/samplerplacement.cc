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
#include "samplerplacement.h"

using namespace GMAD;

SamplerPlacement::SamplerPlacement()
{
  clear();
  PublishMembers();
}

void SamplerPlacement::clear()
{
  name = "";
  samplerType = "plane";
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
  apertureModel = "";
  shape = "circular";
  aper1 = 0;
  aper2 = 0;
  aper3 = 0;
  aper4 = 0;
  startAnglePhi = 0;
  sweepAnglePhi = -1;   // -1 to flag we should use 2pi later when we have units
  startAngleTheta = 0;
  sweepAngleTheta = -1; // -1 to flag we should use 2pi later when we have units
  partID.clear();
  partIDSetID = -1;
}

void SamplerPlacement::PublishMembers()
{
  publish("name",          &SamplerPlacement::name);
  publish("samplerType",   &SamplerPlacement::samplerType);
  publish("referenceElement", &SamplerPlacement::referenceElement);
  publish("referenceElementNumber", &SamplerPlacement::referenceElementNumber);
  publish("s",             &SamplerPlacement::s);
  publish("x",             &SamplerPlacement::x);
  publish("y",             &SamplerPlacement::y);
  publish("z",             &SamplerPlacement::z);
  publish("phi",           &SamplerPlacement::phi);
  publish("theta",         &SamplerPlacement::theta);
  publish("psi",           &SamplerPlacement::psi);
  publish("axisX",         &SamplerPlacement::axisX);
  publish("axisY",         &SamplerPlacement::axisY);
  publish("axisZ",         &SamplerPlacement::axisZ);
  publish("angle",         &SamplerPlacement::angle);
  publish("axisAngle",     &SamplerPlacement::axisAngle);
  publish("apertureModel", &SamplerPlacement::apertureModel);
  publish("shape",         &SamplerPlacement::shape);
  publish("apertureType",  &SamplerPlacement::shape);
  publish("aper1",         &SamplerPlacement::aper1);
  publish("aper2",         &SamplerPlacement::aper2);
  publish("aper3",         &SamplerPlacement::aper3);
  publish("aper4",         &SamplerPlacement::aper4);
  publish("startAnglePhi", &SamplerPlacement::startAnglePhi);
  publish("sweepAnglePhi", &SamplerPlacement::sweepAnglePhi);
  publish("startAngleTheta", &SamplerPlacement::startAngleTheta);
  publish("sweepAngleTheta", &SamplerPlacement::sweepAngleTheta);
  publish("partID",        &SamplerPlacement::partID);
  // partIDSetID is for internal use only - not published
}

void SamplerPlacement::print()const
{
  std::cout << "SamplerPlacement: "
	    << "name "          << name          << std::endl
	    << "samplerType "   << samplerType   << std::endl
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
    	    << "apertureMode "  << apertureModel << std::endl
	    << "shape "         << shape         << std::endl
	    << "aper1 "         << aper1         << std::endl
	    << "aper2 "         << aper2         << std::endl
	    << "aper3 "         << aper3         << std::endl
	    << "aper4 "         << aper4         << std::endl
	    << "startAnglePhi " << startAnglePhi << std::endl
	    << "sweepAnglePhi " << sweepAnglePhi << std::endl
	    << "startAngleTheta " << startAngleTheta << std::endl
	    << "sweepAngleTheta " << sweepAngleTheta << std::endl
	    << "partID list {";
  for (auto v : partID)
    {std::cout << v << ", ";}
  std::cout << "}" << std::endl
  << "partIDSet ID " << partIDSetID << std::endl;
}

void SamplerPlacement::set_value(const std::string& property, Array* value, bool bExit)
{
#ifdef BDSDEBUG
  std::cout << "parser> Setting value " << std::setw(25) << std::left << property << std::endl;
#endif
  if(property=="partID")
  {
    value->set_vector(partID);
  }
  else
  {
    std::cerr << "Error: parser> unknown material option \"" << property << "\", or doesn't expect vector type" << std::endl;
    if(bExit)
      {exit(1);}
    else
      {std::rethrow_exception(std::current_exception());} // to be caught by python
  }
}

#if __cplusplus >= 201703L
std::list<std::variant<bool, int, double, std::string>> SamplerPlacement::get_value_array(const std::string & property) {
  std::list<std::variant<bool, int, double, std::string>> retval;

  if(property=="partID")
  {
    retval.resize(partID.size());
    std::copy(partID.begin(),partID.end(), retval.begin());
  }

  else
  {
    std::cerr << "Error: parser> unknown material option \"" << property << "\", or doesn't expect vector type" << std::endl;
  }

  return retval;
}
#endif