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
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSGlobalConstants.hh"
#include "BDSRandom.hh"
#include "BDSUtilities.hh"

#include "globals.hh"
#include "G4String.hh"
#include "G4Types.hh"

#include "CLHEP/Random/Random.h"
#include "CLHEP/Random/JamesRandom.h"
#ifdef CLHEPHASMIXMAX
#include "CLHEP/Random/MixMaxRng.h"
#else
#include "CLHEP/ClhepVersion.h"
#endif

#include <ctime>
#include <map>
#include <string>
#include <sstream>

template<>
std::map<BDSRandomEngineType, std::string>* BDSRandomEngineType::dictionary =
  new std::map<BDSRandomEngineType, std::string> ({
						   {BDSRandomEngineType::hepjames,  "hepjames"},
						   {BDSRandomEngineType::mixmax,    "mixmax"}
    });

BDSRandomEngineType BDSRandom::DetermineRandomEngineType(G4String engineType)
{
  std::map<G4String, BDSRandomEngineType> types;
  types["hepjames"] = BDSRandomEngineType::hepjames;
  types["mixmax"]   = BDSRandomEngineType::mixmax;

  engineType = BDS::LowerCase(engineType);
  
  auto result = types.find(engineType);
  if (result == types.end())
    {// it's not a valid key
      G4String msg = "\"" + engineType + "\" is not a valid random engine\n";
      msg += "Available random engines are:\n";
      for (auto& it : types)
	{msg += "\"" + it.first + "\"\n";}
      throw BDSException(__METHOD_NAME__, msg);
    }
  return result->second;
}

void BDSRandom::CreateRandomNumberGenerator(const G4String& engineName)
{
  auto et = BDSRandom::DetermineRandomEngineType(engineName);
  G4cout << __METHOD_NAME__ << "Engine name: " << et.ToString() << G4endl;
  switch (et.underlying())
    {
    case BDSRandomEngineType::hepjames:
      {CLHEP::HepRandom::setTheEngine(new CLHEP::HepJamesRandom()); break;}
    case BDSRandomEngineType::mixmax:
#ifdef CLHEPHASMIXMAX
      {CLHEP::HepRandom::setTheEngine(new CLHEP::MixMaxRng()); break;}
#else
      {
	G4String msg = G4String(et.ToString()) + " is not available in CLHEP version: " + G4String(CLHEP::Version::String());
	throw BDSException(__METHOD_NAME__, msg);
	break;
      }
#endif
    default:
      {throw BDSException(__METHOD_NAME__, "engine \"" + engineName + "\" not implemented"); break;}
    }
}

void BDSRandom::SetSeed()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "set the seed" << G4endl;
  G4cout << __METHOD_NAME__ << "seed from BDSGlobalConstants = " 
         << BDSGlobalConstants::Instance()->Seed() << G4endl
         << __METHOD_NAME__ << "seed set in GMAD options: "
	 << BDSGlobalConstants::Instance()->SeedSet() << G4endl;
#endif
  // if seed positive set it, else use the time
  long seed = 0;

  if(BDSGlobalConstants::Instance()->Seed() < 0)
    {seed = time(nullptr);}
  else
    {seed = BDSGlobalConstants::Instance()->Seed();}

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "selected seed = " << seed << G4endl;
#endif

  CLHEP::HepRandom::setTheSeed(seed);

  // feedback - get the seed from the generator itself (ensures set correctly)
  G4cout << __METHOD_NAME__ << "Random number generator's seed = "
	 << CLHEP::HepRandom::getTheSeed() << G4endl;
#ifdef BDSDEBUG
  BDSRandom::PrintFullSeedState();
#endif
}

void BDSRandom::PrintFullSeedState()
{
  G4cout << __METHOD_NAME__ << "Random number generator's state: " << G4endl << G4endl;
  CLHEP::HepRandom::saveFullState(G4cout);
  G4cout << G4endl;
}

void BDSRandom::WriteSeedState(const G4String& suffix)
{
  G4String baseFileName = BDSGlobalConstants::Instance()->OutputFileName();
  G4String seedstatefilename = baseFileName + suffix + ".seedstate.txt";
  std::ofstream ofseedstate;
  ofseedstate.open(seedstatefilename);
  CLHEP::HepRandom::saveFullState(ofseedstate);
  ofseedstate.close();
}

G4String BDSRandom::GetSeedState()
{
  std::stringstream currentState;
  CLHEP::HepRandom::saveFullState(currentState);
  return G4String(currentState.str());
}

void BDSRandom::LoadSeedState(const G4String& inSeedFilename)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "loading file: " << inSeedFilename << G4endl;
#endif
  std::ifstream ifseedstate;
  ifseedstate.open(inSeedFilename);
  if (ifseedstate.is_open())
    {CLHEP::HepRandom::restoreFullState(ifseedstate);}
  else
    {throw BDSException(__METHOD_NAME__, "cannot open file : " + inSeedFilename);}
  ifseedstate.close();
#ifdef BDSDEBUG
  BDSRandom::PrintFullSeedState();
#endif
}

void BDSRandom::SetSeedState(const G4String& seedState)
{
  if (seedState.empty())
    {G4cout << __METHOD_NAME__ << "empty seed state supplied - no seed state set" << G4endl; return;}
  std::stringstream ss;
  ss.str(seedState); // set contents of string stream as input string
  SetSeedState(ss);
}

void BDSRandom::SetSeedState(std::stringstream& seedState)
{
  CLHEP::HepRandom::restoreFullState(seedState);
#ifdef BDSEBUG
  BDSRandom::PrintFullSeedState();
#endif
}
