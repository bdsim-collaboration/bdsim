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
#include "BDSBunch.hh"
#include "BDSBunchFactory.hh"
#include "BDSBunch.hh"
#include "BDSDebug.hh"
#include "parser/options.h"

BDSBunch::BDSBunch()
{
#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  // Instantiate default reference distribution.
  bdsBunch = new BDSBunch();
}

BDSBunch::~BDSBunch()
{
  delete bdsBunch;
}

void BDSBunch::SetOptions(const GMAD::Options& opt)
{
#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  // check options and construct correct bdsBunchInterface instance replacing reference one
  delete bdsBunch;

  bdsBunch = BDSBunchFactory::createBunch(opt.distribType,opt.distribFile);
  bdsBunch->SetOptions(opt);
  return;
}

void BDSBunch::GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
			       G4double& xp, G4double& yp, G4double& zp,
			       G4double& t , G4double&  E, G4double& weight)
{
#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  bdsBunch->GetNextParticle(x0,y0,z0,xp,yp,zp,t,E,weight);
  return;
}
