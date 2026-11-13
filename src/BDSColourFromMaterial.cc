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
#include "BDSColours.hh"
#include "BDSColourFromMaterial.hh"
#include "BDSUtilities.hh"

#include "G4Colour.hh"
#include "G4DataVector.hh"
#include "G4Material.hh"
#include "G4PhysicsFreeVector.hh"
#include "G4String.hh"
#include "G4Types.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include <map>
#include <vector>

BDSColourFromMaterial* BDSColourFromMaterial::instance = nullptr;

BDSColourFromMaterial* BDSColourFromMaterial::Instance()
{
  if (!instance)
    {instance = new BDSColourFromMaterial();}
  return instance;
}

BDSColourFromMaterial::~BDSColourFromMaterial()
{
  instance = nullptr;
  delete generalDensity;
}

BDSColourFromMaterial::BDSColourFromMaterial()
{
  BDSColours* c = BDSColours::Instance();
  defines["air"]         = c->GetColour("air:240 240 240 0.05");
  defines["airbdsim"]    = defines["air"];
  defines["boron"]       = c->GetColour("reallyreallydarkgrey");
  defines["brass"]       = c->GetColour("LHCcoil");
  defines["carbon"]      = c->GetColour("reallyreallydarkgrey");
  defines["c"]           = defines["carbon"];
  defines["graphite"]    = defines["carbon"];
  defines["chlorine"]    = c->GetColour("yellow");
  defines["cl"]          = defines["chlorine"];
  defines["concrete"]    = c->GetColour("tunnel");
  defines["lhcconcrete"] = defines["concrete"];
  defines["copper"]      = c->GetColour("coil");
  defines["cu"]          = defines["copper"];
  defines["cuh2o"]       = defines["copper"];
  defines["iron"]        = c->GetColour("iron");
  defines["fe"]          = defines["iron"];
  defines["gold"]        = c->GetColour("gold:220 176 71");
  defines["au"]          = defines["gold"];
  defines["kapton"]      = c->GetColour("kapton");
  defines["lead"]        = c->GetColour("lead");
  defines["pb"]          = defines["lead"];
  defines["lyso"]        = c->GetColour("lyso: 230 210 235 0.3");
  defines["lso"]         = defines["lyso"];
  defines["yso"]         = defines["lyso"];
  defines["lysoce"]      = defines["lyso"];
  defines["marble"]      = c->GetColour("marble:228 228 228 1.0");
  defines["stainlesssteel"] = c->GetColour("beampipe");
  defines["stainless-steel"] = defines["stainlesssteel"];
  defines["sulphur"]     = c->GetColour("yellow");
  defines["s"]           = defines["sulphur"];
  defines["vacuum"]      = defines["air"];
  defines["cucrzr"]      = defines["copper"];
  
  // for older versions of Geant4 < V11 we have to use G4DataVector which
  // can't use list initialisation. In V11 onwards, G4PhysicsFreeVector
  // changed to take std::vector<G4double> as arguments but G4DataVector
  // inherits this so it's ok. Order must be ascending.
  G4DataVector densities(5);
  for (auto v : {1e-4, 0.01, 0.1, 1.0, 1e2})
    {densities.emplace_back(v * CLHEP::g / CLHEP::cm3);}
  G4DataVector values(5);
  for (auto v : {210, 180, 150, 120, 100})
    {values.emplace_back(v);}
  generalDensity = new G4PhysicsFreeVector(densities, values);
}

G4Colour* BDSColourFromMaterial::GetColour(const G4Material* material,
                                           const G4String& prefixToStripFromName)
{
  G4String materialName = material->GetName();
  materialName = BDS::LowerCase(materialName);

  // strip off g4 so we don't have to define duplicates of everything
  std::string toErase = "g4_";
  size_t pos = materialName.find(toErase);
  if (pos != std::string::npos)
    {materialName.erase(pos, toErase.length());}
  
  // note, not all material are prefixed by preprocessor and might not contain that prefix string
  G4String prefixToStripFromNameLower = BDS::LowerCase(prefixToStripFromName);
  if (!prefixToStripFromName.empty() && BDS::StrContains(materialName, prefixToStripFromNameLower))
    {materialName.erase(0, prefixToStripFromName.size());}

  auto search = defines.find(materialName);
  if (search != defines.end())
    {return search->second;}
  else
    {
      G4double alpha = 1;
      G4State state = material->GetState();
      switch (state)
	{
	case G4State::kStateGas:
	  {alpha = 0.05; break;}
	case G4State::kStateSolid:
	  {alpha = 1.0; break;}
	case G4State::kStateLiquid:
	  {alpha = 0.5; break;}
	default:
	  {alpha = 1.0; break;}
	}
      G4double density = material->GetDensity();
      G4double value   = generalDensity->Value(density);
      G4String vs      = G4String(std::to_string(value));
      G4Colour* result = BDSColours::Instance()->GetColour(materialName + ":" +
							   vs + " " +
							   vs + " " +
							   vs + " " +
							   std::to_string(alpha));
      defines[materialName] = result; // cache it as we'd always produce the same result
      return result;
    }
}

G4Colour* BDSColourFromMaterial::GetColourWithDefault(const G4Material* material,
                                                      G4Colour* defaultIn) const
{
  G4String materialName = material->GetName();
  materialName = BDS::LowerCase(materialName);
  
  // strip off g4 so we don't have to define duplicates of everything
  std::string toErase = "g4_";
  size_t pos = materialName.find(toErase);
  if (pos != std::string::npos)
    {materialName.erase(pos, toErase.length());}
  
  auto search = defines.find(materialName);
  return search != defines.end() ? search->second : defaultIn;
}
