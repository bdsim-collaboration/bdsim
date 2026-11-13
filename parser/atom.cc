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
#include "atom.h"

using namespace GMAD;

Atom::Atom()
{
  clear();
  PublishMembers();
}

void Atom::clear()
{
  name = "";
  A = 0.0;
  Z = 0.0;
  symbol = "";
}

void Atom::PublishMembers()
{
  publish("name",   &Atom::name);
  publish("A",      &Atom::A);
  publish("Z",      &Atom::Z);
  publish("symbol", &Atom::symbol);
}

void Atom::print()const
{
  std::cout << "atom: "   << name   << " "
	    << "A= "      << A      << "g/mole "
	    << "Z= "      << Z      << " "
	    << "symbol= " << symbol << " "
	    << std::endl;
}
