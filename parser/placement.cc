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
#include "placement.h"
#include "query.h"
#include "samplerplacement.h"
#include "scorermesh.h"

using namespace GMAD;

Placement::Placement()
{
  clear();
  PublishMembers();
}

void Placement::clear()
{
  name         = "";
  geometryFile = "";
  bdsimElement = "";
  sequence     = "";
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
  sensitive  = true;
  axisAngle  = false;
  side       = "";
  sideOffset = 0;
  autoColour = true;
  stripOuterVolume = false;
  fieldAll   = "";
  dontReloadGeometry = false;
}

void Placement::PublishMembers()
{
  publish("name",          &Placement::name);
  publish("geometryFile",  &Placement::geometryFile);
  publish("bdsimElement",  &Placement::bdsimElement);
  publish("sequence",      &Placement::sequence);
  publish("referenceElement", &Placement::referenceElement);
  publish("referenceElementNumber", &Placement::referenceElementNumber);
  publish("s",             &Placement::s);
  publish("x",             &Placement::x);
  publish("y",             &Placement::y);
  publish("z",             &Placement::z);
  publish("phi",           &Placement::phi);
  publish("theta",         &Placement::theta);
  publish("psi",           &Placement::psi);
  publish("axisX",         &Placement::axisX);
  publish("axisY",         &Placement::axisY);
  publish("axisZ",         &Placement::axisZ);
  publish("angle",         &Placement::angle);
  publish("sensitive",     &Placement::sensitive);
  publish("axisAngle",     &Placement::axisAngle);
  publish("side",          &Placement::side);
  publish("sideOffset",    &Placement::sideOffset);
  publish("autoColour",    &Placement::autoColour);
  publish("stripOuterVolume", &Placement::stripOuterVolume);
  publish("fieldAll",      &Placement::fieldAll);
  publish("dontReloadGeometry", &Placement::dontReloadGeometry);
}

void Placement::print()const
{
  std::cout << "Placement: \n"
	    << "name \""           << name             << "\"\n"
	    << "geometryFile \""   << geometryFile     << "\"\n"
	    << "bdsimElement \""   << bdsimElement     << "\"\n"
	    << "sequence \""       << sequence         << "\"\n"
	    << "referenceElement " << referenceElement << "\"\n"
	    << "referenceElementNumber " << referenceElementNumber << "\n"
	    << "s "             << s             << "\n"
	    << "x "             << x             << "\n"
      << "y "             << y             << "\n"
    	<< "z "             << z             << "\n"
      << "phi "           << phi           << "\n"
      << "theta "         << theta         << "\n"
      << "psi "           << psi           << "\n"
      << "axisX "         << axisX         << "\n"
      << "axisY "         << axisY         << "\n"
      << "axisZ "         << axisZ         << "\n"
      << "angle "         << angle         << "\n"
	    << "sensitive "     << sensitive     << "\n"
	    << "axisAngle "     << axisAngle     << "\n"
	    << "side \""        << side          << "\"\n"
      << "sideOffset "    << sideOffset    << "\n"
	    << "axisAngle "     << axisAngle     << "\n"
      << "autoColour "    << autoColour    << "\n"
      << "stripOuterVolume "    << stripOuterVolume << "\n"
	    << "fieldAll \""    << fieldAll      << "\"" << "\n"
      << "dontReloadGeometry " << dontReloadGeometry << std::endl;
}

Placement::Placement(const SamplerPlacement& sp):
  sensitive(false),
  sideOffset(0)
{
  name      = sp.name;
  referenceElement       = sp.referenceElement;
  referenceElementNumber = sp.referenceElementNumber;
  s         = sp.s;
  x         = sp.x;
  y         = sp.y;
  z         = sp.z;
  phi       = sp.phi;
  theta     = sp.theta;
  psi       = sp.psi;
  axisX     = sp.axisX;
  axisY     = sp.axisY;
  axisZ     = sp.axisZ;
  angle     = sp.angle;
  axisAngle = sp.axisAngle;
  autoColour = false;
  stripOuterVolume = false;
  dontReloadGeometry = false;
}

Placement::Placement(const ScorerMesh& sm):
  sensitive(false),
  sideOffset(0)
{
  name      = sm.name;
  sequence  = sm.sequence;
  referenceElement       = sm.referenceElement;
  referenceElementNumber = sm.referenceElementNumber;
  s         = sm.s;
  x         = sm.x;
  y         = sm.y;
  z         = sm.z;
  phi       = sm.phi;
  theta     = sm.theta;
  psi       = sm.psi;
  axisX     = sm.axisX;
  axisY     = sm.axisY;
  axisZ     = sm.axisZ;
  angle     = sm.angle;
  axisAngle = sm.axisAngle;
  autoColour = false;
  stripOuterVolume = false;
  dontReloadGeometry = false;
}

Placement::Placement(const BLMPlacement& bp):
  sensitive(false)
{
  name      = bp.name;
  referenceElement       = bp.referenceElement;
  referenceElementNumber = bp.referenceElementNumber;
  s          = bp.s;
  x          = bp.x;
  y          = bp.y;
  z          = bp.z;
  phi        = bp.phi;
  theta      = bp.theta;
  psi        = bp.psi;
  axisX      = bp.axisX;
  axisY      = bp.axisY;
  axisZ      = bp.axisZ;
  angle      = bp.angle;
  axisAngle  = bp.axisAngle;
  side       = bp.side;
  sideOffset = bp.sideOffset;
  autoColour = false;
  stripOuterVolume = false;
  dontReloadGeometry = false;
}

Placement::Placement(const Query& qu):
  sensitive(false),
  sideOffset(0),
  autoColour(false),
  stripOuterVolume(false)
{
  name      = qu.name;
  referenceElement       = qu.referenceElement;
  referenceElementNumber = qu.referenceElementNumber;
  s          = qu.s;
  x          = qu.x;
  y          = qu.y;
  z          = qu.z;
  phi        = qu.phi;
  theta      = qu.theta;
  psi        = qu.psi;
  axisX      = qu.axisX;
  axisY      = qu.axisY;
  axisZ      = qu.axisZ;
  angle      = qu.angle;
  axisAngle  = qu.axisAngle;
  dontReloadGeometry = false;
}