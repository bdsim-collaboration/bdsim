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
#include "scorermesh.h"

using namespace GMAD;

ScorerMesh::ScorerMesh()
{
  clear();
  PublishMembers();
}

void ScorerMesh::clear()
{
  name          = "";
  scoreQuantity = "";
  geometryType  = "box";

  nx    = 0;
  ny    = 0;
  nz    = 0;
  nr    = 0;
  nphi  = 0;
  ne    = 1;
  xsize = 0;
  ysize = 0;
  zsize = 0;
  rInner = 0;
  rOuter = 0;
  eLow  = 1e-12;
  eHigh = 1e4;
  eScale = "linear";
  eBinsEdgesFilenamePath = "";
  sequence         = "";
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
}

void ScorerMesh::PublishMembers()
{
  publish("name",          &ScorerMesh::name);
  publish("scoreQuantity", &ScorerMesh::scoreQuantity);
  publish("geometryType",  &ScorerMesh::geometryType);
  publish("nx",            &ScorerMesh::nx);
  publish("ny",            &ScorerMesh::ny);
  publish("nz",            &ScorerMesh::nz);
  publish("nr",            &ScorerMesh::nr);
  publish("nphi",          &ScorerMesh::nphi);
  publish("ne",            &ScorerMesh::ne);
  publish("xsize",         &ScorerMesh::xsize);
  publish("ysize",         &ScorerMesh::ysize);
  publish("zsize",         &ScorerMesh::zsize);
  publish("rInner",        &ScorerMesh::rInner);
  publish("rOuter",        &ScorerMesh::rOuter);
  publish("rsize",         &ScorerMesh::rOuter);
  publish("eLow",          &ScorerMesh::eLow);
  publish("eHigh",         &ScorerMesh::eHigh);
  publish("eScale",        &ScorerMesh::eScale);
  publish("eBinsEdgesFilenamePath", &ScorerMesh::eBinsEdgesFilenamePath);
  publish("sequence",      &ScorerMesh::sequence);
  publish("referenceElement", &ScorerMesh::referenceElement);
  publish("referenceElementNumber", &ScorerMesh::referenceElementNumber);
  publish("s",             &ScorerMesh::s);
  publish("x",             &ScorerMesh::x);
  publish("y",             &ScorerMesh::y);
  publish("z",             &ScorerMesh::z);
  publish("phi",           &ScorerMesh::phi);
  publish("theta",         &ScorerMesh::theta);
  publish("psi",           &ScorerMesh::psi);
  publish("axisX",         &ScorerMesh::axisX);
  publish("axisY",         &ScorerMesh::axisY);
  publish("axisZ",         &ScorerMesh::axisZ);
  publish("angle",         &ScorerMesh::angle);
  publish("axisAngle",     &ScorerMesh::axisAngle);
}

void ScorerMesh::print()const
{
  std::cout << "ScorerMesh: "   << std::endl
            << "name "          << name          << std::endl
            << "scoreQuantity " << scoreQuantity << std::endl
            << "nx "            << nx            << std::endl
            << "ny "            << ny            << std::endl
            << "nz "            << nz            << std::endl
            << "ne "            << ne            << std::endl
            << "xsize "         << xsize         << std::endl
            << "ysize "         << ysize         << std::endl
            << "zsize "         << zsize         << std::endl
            << "rInner "        << rInner        << std::endl
            << "rOuter "        << rOuter        << std::endl
            << "eLow "          << eLow          << std::endl
            << "eHigh "         << eHigh         << std::endl
            << "eScale "        << eScale        << std::endl
            << "sequence "      << sequence      << std::endl
            << "referenceElement " << referenceElement << std::endl
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
