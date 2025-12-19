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
#include "aperture.h"

using namespace GMAD;

Aperture::Aperture()
{
  clear();
  PublishMembers();
}

void Aperture::clear()
{
  name         = "";
  apertureType = "";
  aper1        = 0;
  aper2        = 0;
  aper3        = 0;
  aper4        = 0;
}

void Aperture::PublishMembers()
{
  publish("name",          &Aperture::name);
  publish("apertureType",  &Aperture::apertureType);
  publish("aper1",         &Aperture::aper1);
  publish("aperture1",     &Aperture::aper1); // alternative
  publish("aper",          &Aperture::aper1); // alternative - backwards compatibility
  publish("beampipeRadius",&Aperture::aper1); // alternative - backwards compatibility
  publish("aper2",         &Aperture::aper2);
  publish("aperture2",     &Aperture::aper2); // alternative
  publish("aper3",         &Aperture::aper3);
  publish("aperture3",     &Aperture::aper3); // alternative
  publish("aper4",         &Aperture::aper4);
  publish("aperture4",     &Aperture::aper4); // alternative
}

void Aperture::print()const
{
  std::cout << "Aperture: "
	    << "name "          << name          << std::endl
	    << "apertureType "  << apertureType  << std::endl
	    << "aper1 "         << aper1         << std::endl
	    << "aper2 "         << aper2         << std::endl
      << "aper3 "         << aper3         << std::endl
      << "aper4 "         << aper4         << std::endl;
}
