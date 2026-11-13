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
#ifndef BDSGEOMETRYGMAD
#define BDSGEOMETRYGMAD

#include "BDSGeometry.hh"

#include "globals.hh"
#include "G4LogicalVolume.hh"

#include <fstream>

/**
 * @brief GMad geometry parser that dynamically creates Geant4 geometry.
 *
 **/

class BDSGeometryGMAD: public BDSGeometry
{
public:
  BDSGeometryGMAD(G4String);
  virtual void Construct(G4LogicalVolume *marker);

private:
  std::ifstream inputf;
  G4String GetWord();
  void GetParameter(G4double& x, G4String name, G4String lastToken);
  void GetParameter(G4String& lval, G4String name, G4String lastToken);
  void Error();
};

#endif
