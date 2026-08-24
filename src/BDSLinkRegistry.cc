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
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSLinkOpaqueBox.hh"
#include "BDSLinkRegistry.hh"

#include "G4Transform3D.hh"
#include "G4Types.hh"

#include <map>
#include <string>

BDSLinkRegistry::BDSLinkRegistry()
{;}

BDSLinkRegistry::~BDSLinkRegistry()
{;}

G4int BDSLinkRegistry::Register(BDSLinkOpaqueBox*    componentIn,
			       const G4Transform3D& inputToGlobalIn,
			       const G4Transform3D& outputToGlobalIn,
                               G4int               samplerIDIn)
{
  G4cout << "New ID " << samplerIDIn << " for " << componentIn->LinkName() << G4endl;
  G4bool noRotation = !(componentIn->Angled());
  BDSLinkRegistry::LinkEntry le = {noRotation,
                                   componentIn,
                                   inputToGlobalIn,
                                   outputToGlobalIn.inverse(),
                                   componentIn->InputClearance(),
                                   samplerIDIn};
  byName[componentIn->LinkName()] = le;
  byID[samplerIDIn] = le;
  return samplerIDIn;
}

const G4Transform3D& BDSLinkRegistry::Transform(const std::string& name) const
{
  auto search = byName.find(name);
  if (search != byName.end())
    {return search->second.transform;}
  else
    {throw BDSException(__METHOD_NAME__, "unknown link element name \"" + name + "\"");}
}

const G4Transform3D& BDSLinkRegistry::Transform(const G4int ID) const
{
  auto search = byID.find(ID);
  if (search != byID.end())
    {return search->second.transform;}
  else
    {throw BDSException(__METHOD_NAME__, "unknown link element ID " + std::to_string(ID));}
}

const G4Transform3D& BDSLinkRegistry::TransformInverse(const std::string& name) const
{
  auto search = byName.find(name);
  if (search != byName.end())
    {return search->second.transformInverse;}
  else
    {throw BDSException(__METHOD_NAME__, "unknown link element name \"" + name + "\"");}
}

const G4Transform3D& BDSLinkRegistry::TransformInverse(const G4int ID) const
{
  auto search = byID.find(ID);
  if (search != byID.end())
    {return search->second.transformInverse;}
  else
    {throw BDSException(__METHOD_NAME__, "unknown link element ID " + std::to_string(ID));}
}

G4bool BDSLinkRegistry::NoRotation(const std::string& name) const
{
  auto search = byName.find(name);
  if (search != byName.end())
    {return search->second.noRotation;}
  else
    {throw BDSException(__METHOD_NAME__, "unknown link element name \"" + name + "\"");}
}

G4bool BDSLinkRegistry::NoRotation(G4int ID) const
{
  auto search = byID.find(ID);
  if (search != byID.end())
    {return search->second.noRotation;}
  else
    {throw BDSException(__METHOD_NAME__, "unknown link element ID " + std::to_string(ID));}
}

G4double BDSLinkRegistry::InputClearance(G4int ID) const
{
  auto search = byID.find(ID);
  if (search != byID.end())
    {return search->second.inputClearance;}
  else
    {throw BDSException(__METHOD_NAME__, "unknown link element ID " + std::to_string(ID));}
}
