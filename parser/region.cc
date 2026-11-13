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
#include "region.h"

using namespace GMAD;

Region::Region()
{
  clear();
  PublishMembers();
}

void Region::clear()
{
  // In case these aren't explictly set, these are set to a sensible
  // default of 1mm (SI units here) - the Geant4 default.  All are copied
  // to a region definition in BDSDetectorConstruction.
  defaultRangeCut  = 0.0;
  prodCutPhotons   = 0.0;
  prodCutElectrons = 0.0;
  prodCutPositrons = 0.0;
  prodCutProtons   = 0.0;
}

void Region::PublishMembers()
{
  publish("name",            &Region::name);
  publish("defaultRangeCut", &Region::defaultRangeCut);
  publish("prodCutPhotons",  &Region::prodCutPhotons);
  publish("prodCutElectrons",&Region::prodCutElectrons);
  publish("prodCutPositrons",&Region::prodCutPositrons);
  publish("prodCutProtons",  &Region::prodCutProtons);
}

void Region::print()const
{
  std::cout << "region: "
	    << name             << " "
	    << defaultRangeCut  << " "
	    << prodCutPhotons   << " "
	    << prodCutElectrons << " "
	    << prodCutPositrons << " "
	    << prodCutProtons
	    << std::endl;
}
