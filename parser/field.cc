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
#include "field.h"

using namespace GMAD;

Field::Field()
{
  clear();
  PublishMembers();
}

void Field::clear()
{
  name                 = "";
  type                 = "";
  eScaling             = 1.0;
  bScaling             = 1.0;
  integrator           = "g4classicalrk4";
  globalTransform      = true;
  magneticFile         = "";
  magneticInterpolator = "cubic";
  electricFile         = "";
  electricInterpolator = "cubic";
  fieldModulator       = "";
  x         = 0;
  y         = 0;
  z         = 0;
  t         = 0;
  phi       = 0;
  theta     = 0;
  psi       = 0;
  axisX     = 0;
  axisY     = 0;
  axisZ     = 0;
  angle     = 0;
  axisAngle = false;
  autoScale = false;
  maximumStepLength = -1;
  maximumStepLengthOverride = -1;
  magneticSubField = "";
  electricSubField = "";
  magneticReflection = "";
  electricReflection = "";
  fieldParameters = "";
}

void Field::PublishMembers()
{
  publish("name",                 &Field::name);
  publish("type",                 &Field::type);
  publish("eScaling",             &Field::eScaling);
  publish("bScaling",             &Field::bScaling);
  publish("integrator",           &Field::integrator);
  publish("globalTransform",      &Field::globalTransform);
  publish("magneticFile",         &Field::magneticFile);
  publish("magneticInterpolator", &Field::magneticInterpolator);
  publish("electricFile",         &Field::electricFile);
  publish("electricInterpolator", &Field::electricInterpolator);
  publish("fieldModulator",       &Field::fieldModulator);
  publish("x",                    &Field::x);
  publish("y",                    &Field::y);
  publish("z",                    &Field::z);
  publish("t",                    &Field::t);
  publish("phi",                  &Field::phi);
  publish("theta",                &Field::theta);
  publish("psi",                  &Field::psi);
  publish("axisX",                &Field::axisX);
  publish("axisY",                &Field::axisY);
  publish("axisZ",                &Field::axisZ);
  publish("angle",                &Field::angle);
  publish("axisAngle",            &Field::axisAngle);
  publish("autoScale",            &Field::autoScale);
  publish("maximumStepLength",    &Field::maximumStepLength);
  publish("maximumStepLengthOverride", &Field::maximumStepLengthOverride);
  publish("magneticSubField",     &Field::magneticSubField);
  publish("electricSubField",     &Field::electricSubField);
  publish("magneticReflection",   &Field::magneticReflection);
  publish("electricReflection",   &Field::electricReflection);
  publish("fieldParameters",      &Field::fieldParameters);
}

void Field::print()const
{
  std::cout << "field: "
            << "name "                 << name                 << std::endl
            << "type "                 << type                 << std::endl
            << "eScaling "             << eScaling             << std::endl
            << "bScaling "             << bScaling             << std::endl
            << "integrator "           << integrator           << std::endl
            << "magneticFile "         << magneticFile         << std::endl
            << "magneticInterpolator " << magneticInterpolator << std::endl
            << "electricFile "         << electricFile         << std::endl
            << "electricInterpolator " << electricInterpolator << std::endl
            << "fieldModulator "       << fieldModulator       << std::endl
            << "x, y, z,t "            << x << " " << y << " " << z << " " << t << std::endl
            << "phi, theta, psi "      << phi   << " " << theta << " " << psi   << std::endl
            << "axisX, Y, Z "          << axisX << " " << axisY << " " << axisZ << std::endl
            << "angle "                << angle                << std::endl
            << "axisAngle "            << axisAngle            << std::endl
            << "autoScale "            << autoScale            << std::endl
            << "maximumStepLength "    << maximumStepLength    << std::endl
            << "maximumStepLengthOverride " << maximumStepLengthOverride << std::endl
            << "electricSubField "     << electricSubField     << std::endl
            << "magneticSubField "     << magneticSubField     << std::endl
            << "magneticReflection "   << magneticReflection   << std::endl
            << "electricReflection "   << electricReflection   << std::endl
            << "fieldParameters "      << fieldParameters      << std::endl;
}
