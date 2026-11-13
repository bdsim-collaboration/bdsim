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
#include "modulator.h"

using namespace GMAD;

Modulator::Modulator()
{
  clear();
  PublishMembers();
}

void Modulator::clear()
{
  name = "";
  type = "";
  frequency = 0;
  phase = 0;
  tOffset = 0;
  amplitudeOffset = 0;
  amplitudeScale = 1.0;
  T0 = 0;
  T1 = 0;
}

void Modulator::PublishMembers()
{
  publish("name",            &Modulator::name);
  publish("type",            &Modulator::type);
  publish("frequency",       &Modulator::frequency);
  publish("phase",           &Modulator::phase);
  publish("tOffset",         &Modulator::tOffset);
  publish("amplitudeOffset", &Modulator::amplitudeOffset);
  publish("amplitudeScale",  &Modulator::amplitudeScale);
  publish("T0",              &Modulator::T0);
  publish("T1",              &Modulator::T1);
}

void Modulator::print()const
{
  std::cout << "modulator: "
	    << "name "            << name            << std::endl
	    << "type "            << type            << std::endl
	    << "frequency "       << frequency       << std::endl
	    << "phase "           << phase           << std::endl
	    << "tOffset "         << tOffset         << std::endl
	    << "amplitudeOffset " << amplitudeOffset << std::endl
	    << "amplitudeScale "  << amplitudeScale  << std::endl
	    << "T0 "              << T0              << std::endl
    	    << "T1 "              << T1              << std::endl;
}
