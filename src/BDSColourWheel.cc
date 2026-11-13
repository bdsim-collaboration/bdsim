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
#include "BDSColourWheel.hh"

#include "G4Colour.hh"

BDSColourWheel::BDSColourWheel()
{
  palette.push_back(G4Colour(1.0, 0.0, 0.0, 0.4));
  palette.push_back(G4Colour(0.0, 1.0, 0.0, 0.4));
  palette.push_back(G4Colour(0.0, 0.0, 1.0, 0.4));
  Reset();
}

BDSColourWheel::~BDSColourWheel()
{;}

void BDSColourWheel::Spin()
{
  ++colour_it;
  if(colour_it == palette.end())
    {Reset();}
}

void BDSColourWheel::Reset()
{
  colour_it = palette.begin();
}

G4Colour BDSColourWheel::Colour()
{
  return (*colour_it);
}
