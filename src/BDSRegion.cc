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
#include "BDSGlobalConstants.hh"
#include "BDSRegion.hh"
#include "BDSUtilities.hh"

#include "parser/region.h"

#include "globals.hh"
#include "G4ProductionCuts.hh"
#include "G4Region.hh"

#include <ostream>

BDSRegion::BDSRegion(G4String nameIn):
  name(nameIn),
  g4cuts(nullptr),
  g4region(nullptr)
{
  // these are the defaults according to BDSIM, but no necessarily the defaults
  // according to the physics list used - ie if a Geant4 reference physics list
  // is used with default
  BDSGlobalConstants* g = BDSGlobalConstants::Instance();
  defaultRangeCut   = g->DefaultRangeCut();
  if (g->DefaultRangeCutsSet())
    {
      rangeCutElectrons = g->ProdCutElectronsSet() ? g->ProdCutElectrons() : defaultRangeCut;
      rangeCutPositrons = g->ProdCutPositronsSet() ? g->ProdCutPositrons() : defaultRangeCut;
      rangeCutProtons   = g->ProdCutProtonsSet()   ? g->ProdCutProtons()   : defaultRangeCut;
      rangeCutPhotons   = g->ProdCutPhotonsSet()   ? g->ProdCutPhotons()   : defaultRangeCut;
    }
  else
    {
      rangeCutElectrons = g->ProdCutElectrons();
      rangeCutPositrons = g->ProdCutPositrons();
      rangeCutProtons   = g->ProdCutProtons();
      rangeCutPhotons   = g->ProdCutPhotons();
    }
  ProduceG4Region();
}

BDSRegion::BDSRegion(G4String nameIn,
		     const BDSRegion* defaultValue,
		     G4double defaultRangeCutIn,
		     G4double rangeCutElectronsIn,
		     G4double rangeCutPositronsIn,
		     G4double rangeCutProtonsIn,
		     G4double rangeCutPhotonsIn):
  name(nameIn),
  g4cuts(nullptr),
  g4region(nullptr)
{
  // if the G4dobule defaultRangeCutIn is supplied, then this takes precedence over
  // using any values from const BDSRegion* defaultValue
  G4bool useSuppliedDefaultNumber = BDS::IsFinite(defaultRangeCutIn);
  defaultRangeCut   = BDS::IsFinite(defaultRangeCutIn)   ? defaultRangeCutIn   : defaultValue->defaultRangeCut;
  rangeCutElectrons = BDS::IsFinite(rangeCutElectronsIn) ? rangeCutElectronsIn : (useSuppliedDefaultNumber ? defaultRangeCutIn : defaultValue->rangeCutElectrons);
  rangeCutPositrons = BDS::IsFinite(rangeCutPositronsIn) ? rangeCutPositronsIn : (useSuppliedDefaultNumber ? defaultRangeCutIn : defaultValue->rangeCutPositrons);
  rangeCutProtons   = BDS::IsFinite(rangeCutProtonsIn)   ? rangeCutProtonsIn   : (useSuppliedDefaultNumber ? defaultRangeCutIn : defaultValue->rangeCutProtons);
  rangeCutPhotons   = BDS::IsFinite(rangeCutPhotonsIn)   ? rangeCutPhotonsIn   : (useSuppliedDefaultNumber ? defaultRangeCutIn : defaultValue->rangeCutPhotons);
  ProduceG4Region();
}

BDSRegion::BDSRegion(const GMAD::Region& parserRegion,
		     const BDSRegion*    defaultRegion):
  BDSRegion(G4String(parserRegion.name),
	    defaultRegion,
	    parserRegion.defaultRangeCut  * CLHEP::m,
	    parserRegion.prodCutElectrons * CLHEP::m,
	    parserRegion.prodCutPositrons * CLHEP::m,
	    parserRegion.prodCutProtons   * CLHEP::m,
	    parserRegion.prodCutPhotons   * CLHEP::m)
{;}

BDSRegion::~BDSRegion()
{
  delete g4cuts;
  delete g4region;
}

std::ostream& operator<< (std::ostream& out, BDSRegion const &r)
{
  out << "Region \"" << r.name << "\"" << G4endl;
  out << "default range cut " << r.defaultRangeCut   << " mm" << G4endl;
  out << "e- range cut      " << r.rangeCutElectrons << " mm" << G4endl;
  out << "e+ range cut      " << r.rangeCutPositrons << " mm" << G4endl;
  out << "proton range cut  " << r.rangeCutProtons   << " mm" << G4endl;
  out << "photon range cut  " << r.rangeCutPhotons   << " mm" << G4endl;
  return out;
}

void BDSRegion::ProduceG4Region()
{
  g4cuts = new G4ProductionCuts();
  g4cuts->SetProductionCut(rangeCutElectrons, "e-");
  g4cuts->SetProductionCut(rangeCutPositrons, "e+");
  g4cuts->SetProductionCut(rangeCutProtons,   "proton");
  g4cuts->SetProductionCut(rangeCutPhotons,   "gamma");
  
  g4region = new G4Region(name);
  g4region->SetProductionCuts(g4cuts);
}

