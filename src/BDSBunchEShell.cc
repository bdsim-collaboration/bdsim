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
#include "BDSBunchEShell.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSParticleCoordsFull.hh"

#include "parser/beam.h"

#include "Randomize.hh"
#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>

BDSBunchEShell::BDSBunchEShell(): 
  BDSBunch("eshell"),
  shellX(0.0),
  shellXp(0.0),
  shellY(0.0),
  shellYp(0.0),
  shellXWidth(0.0),
  shellXpWidth(0.0),
  shellYWidth(0.0),
  shellYpWidth(0.0) 
{;}

BDSBunchEShell::~BDSBunchEShell() 
{;}

void BDSBunchEShell::SetOptions(const BDSParticleDefinition* beamParticle,
                                const GMAD::Beam& beam,
                                const BDSBunchType& distrType,
                                G4Transform3D beamlineTransformIn,
                                const G4double beamlineSIn)
{
  BDSBunch::SetOptions(beamParticle, beam, distrType, beamlineTransformIn, beamlineSIn);
  shellX  = beam.shellX  * CLHEP::m;
  shellY  = beam.shellY  * CLHEP::m;
  shellXp = beam.shellXp * CLHEP::rad;
  shellYp = beam.shellYp * CLHEP::rad;
  shellXWidth  = beam.shellXWidth  * CLHEP::m;
  shellXpWidth = beam.shellXpWidth * CLHEP::rad;
  shellYWidth  = beam.shellYWidth  * CLHEP::m;
  shellYpWidth = beam.shellYpWidth * CLHEP::rad;
}

void BDSBunchEShell::CheckParameters()
{
  BDSBunch::CheckParameters();
  if (shellX <= 0)
    {throw BDSException(__METHOD_NAME__, "shellX <= 0");}
  if (shellY <= 0)
    {throw BDSException(__METHOD_NAME__, "shellY <= 0");}
  if (shellXp <= 0)
    {throw BDSException(__METHOD_NAME__, "shellXp <= 0");}
  if (shellYp <= 0)
    {throw BDSException(__METHOD_NAME__, "shellYp <= 0");}

  // widths can be zero but can't be negative
  if (shellXWidth <  0)
    {throw BDSException(__METHOD_NAME__, "shellXWidth < 0");}
  if (shellYWidth <  0)
    {throw BDSException(__METHOD_NAME__, "shellYWidth < 0");}
  if (shellXpWidth <  0)
    {throw BDSException(__METHOD_NAME__, "shellXpWidth < 0");}
  if (shellYpWidth <  0)
    {throw BDSException(__METHOD_NAME__, "shellYpWidth < 0");}
}

BDSParticleCoordsFull BDSBunchEShell::GetNextParticleLocal()
{
  G4double phi   = 2 * CLHEP::pi * G4RandFlat::shoot();
  G4double xamp  = 0.5 - G4RandFlat::shoot();
  G4double yamp  = 0.5 - G4RandFlat::shoot();
  G4double xpamp = 0.5 - G4RandFlat::shoot();
  G4double ypamp = 0.5 - G4RandFlat::shoot();
  
  G4double x  = X0  + (std::sin(phi) * shellX)  + xamp  * shellXWidth;
  G4double xp = Xp0 + (std::cos(phi) * shellXp) + xpamp * shellXpWidth;
  phi = 2 * CLHEP::pi * G4RandFlat::shoot();
  G4double y  = Y0  + (std::sin(phi) * shellY)  + yamp  * shellYWidth;
  G4double yp = Yp0 + (std::cos(phi) * shellYp) + ypamp * shellYpWidth;
  G4double zp = CalculateZp(xp,yp,Zp0);
  G4double E  = E0 * (1 + sigmaE/2. * (1. -2. * G4RandFlat::shoot()));

  return BDSParticleCoordsFull(x,y,Z0,xp,yp,zp,T0,S0,E,/*weight=*/1.0);
}
