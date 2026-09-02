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

#include "BDSGDMLPreprocessor.hh"

#include "G4String.hh"

#include <iostream>

int main(int argc, char** argv)
{
  if (argc != 2)
    {
      std::cerr << "Expected the BDSIM data directory as one argument" << std::endl;
      return 1;
    }

  const G4String expected = G4String(argv[1]) + "/gdml/schema/gdml.xsd";
  const G4String actual = BDS::GDMLSchemaLocation();
  if (actual != expected)
    {
      std::cerr << "Expected GDML schema at \"" << expected
                << "\", but found \"" << actual << "\"" << std::endl;
      return 1;
    }

  return 0;
}
