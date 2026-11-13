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
#ifndef BDSIQUERY_H
#define BDSIQUERY_H

#include "BDSFieldClassType.hh"
#include "BDSFieldType.hh"

#include "globals.hh" // geant4 types / globals
#include "G4String.hh"

class G4Field;

namespace GMAD
{
  class Query;
}

namespace BDSI
{
  void Query(G4Field* field, const GMAD::Query& params, const BDSFieldType fieldType);
  
  void Query1D(G4Field* field, const GMAD::Query& params, const BDSFieldClassType type);
  void Query2D(G4Field* field, const GMAD::Query& params, const BDSFieldClassType type);
  void Query3D(G4Field* field, const GMAD::Query& params, const BDSFieldClassType type);
  void Query4D(G4Field* field, const GMAD::Query& params, const BDSFieldClassType type);

  void WriteOut(std::ostream* out,
		const G4int    nDim,
		const G4double coords[4],
		const G4double result[6],
		const BDSFieldClassType type);

  const G4double margin = 1e-6;
}

#endif
