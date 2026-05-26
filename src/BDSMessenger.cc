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
#include "BDSAcceleratorModel.hh"
#include "BDSBeamline.hh"
#include "BDSBeamlineElement.hh"
#include "BDSMessenger.hh"
#include "BDSParser.hh"
#include "BDSSamplerRegistry.hh"
#include "BDSUIcmdStringInt.hh"
#include "BDSUtilities.hh"

#include "parser/query.h"

#include "globals.hh"
#include "G4UImanager.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIcmdWithAString.hh"

#include <iomanip>
#include <iostream>
#include <ostream>
#include <string>


BDSMessenger::BDSMessenger()
{
  bdsDirectory = new G4UIdirectory("/bds/");
  bdsDirectory->SetGuidance("BDSIM commands");

  bdsBeamlineDirectory = new G4UIdirectory("/bds/beamline/");

  beamlineListCmd = new G4UIcmdWithoutParameter("/bds/beamline/list",this);
  beamlineListCmd->SetGuidance("List beamline components");

  elementNameSearchCmd = new G4UIcmdWithAString("/bds/beamline/namesearch",this);
  elementNameSearchCmd->SetGuidance("Search beamline components for element");
  
  elementGoToCmd = new BDSUIcmdStringInt("/bds/beamline/goto", this);
  G4String elementGoToCmdGuidance = "Move the viewpoint to a particular element's location. ";
  elementGoToCmdGuidance += "Arguments are the name of an element in the beam line and optionally,";
  elementGoToCmdGuidance += " the instance number (zero-counting).";
  elementGoToCmd->SetGuidance(elementGoToCmdGuidance);
  
  bdsSamplersDirectory = new G4UIdirectory("/bds/samplers/");
  samplerListCmd = new G4UIcmdWithoutParameter("/bds/samplers/list",this);
  samplerListCmd->SetGuidance("List samplers");
  
  samplerViewCmd = new G4UIcmdWithoutParameter("/bds/samplers/view",this);
  samplerViewCmd->SetGuidance("View sampler paralle world");
  
  queryListCmd = new G4UIcmdWithoutParameter("/bds/field/listQueries", this);
  queryListCmd->SetGuidance("List all queries defined in input");

  // G4UImanager* UIManager = G4UImanager::GetUIpointer();
  // UIManager->ApplyCommand("/control/execute " + visMacroFilename);
}

BDSMessenger::~BDSMessenger()
{
  delete bdsDirectory;
  delete bdsBeamlineDirectory;
  delete beamlineListCmd;
  delete elementNameSearchCmd;
  delete elementGoToCmd;
  delete bdsSamplersDirectory;
  delete samplerListCmd;
  delete samplerViewCmd;
}

void BDSMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
  if(command == beamlineListCmd)
    {BeamLineList();}
  else if(command == elementNameSearchCmd)
    {ElementNameSearch(newValue);}
  else if(command == elementGoToCmd)
    {GoToElement(newValue);}
  else if(command == samplerListCmd)
    {SamplerList();}
  else if (command == samplerViewCmd)
    {ViewSamplers();}
  else if (command == queryListCmd)
    {ListQueries();}
}

void BDSMessenger::BeamLineList()
{
  const BDSBeamline *beamline = BDSAcceleratorModel::Instance()->BeamlineMain();

  int j = 0;
  auto flagsCache(G4cout.flags());
  G4cout << std::right
         << std::setw(4)  << "index" << std::setfill(' ')
         << std::setw(25) << "name"
         << std::setw(25) << "placement name"
         << std::setw(20) << "type"
         << std::setw(20) << "S-middle[m]" << G4endl;
  for (auto i = beamline->begin(); i != beamline->end(); ++i, ++j)
    {G4cout << BDSBeamlineElementToString(j) << G4endl;}
  G4cout.flags(flagsCache);
}

std::string BDSMessenger::BDSBeamlineElementToString(G4int iElement)
{
  std::stringstream ss;

  const BDSBeamline* beamline = BDSAcceleratorModel::Instance()->BeamlineMain();
  const BDSBeamlineElement* e = beamline->at(iElement);
  
  ss << std::setfill('0') << std::right << std::setw(4)  << iElement << " " << std::setfill(' ')
     << std::setw(25) << e->GetName()
     << std::setw(25) << e->GetPlacementName()
     << std::setw(20) << e->GetType()
     << std::setw(20) << std::setprecision(4) << std::fixed << e->GetSPositionMiddle()/CLHEP::m;
  
  return ss.str();
}

void BDSMessenger::ElementNameSearch(std::string name)
{
  const BDSBeamline* beamline = BDSAcceleratorModel::Instance()->BeamlineMain();
  for (auto i = beamline->begin(); i != beamline->end(); ++i)
    {
      if(BDS::StrContains((*i)->GetName(), name))
        {G4cout << (*i)->GetName() << G4endl;}
    }
}

void BDSMessenger::GoToElement(const std::string& value)
{
  if (value.empty())
    {G4cerr << "empty string given - can't search" << G4endl; return;}
  std::vector<G4String> words = BDS::SplitOnWhiteSpace(value);
  G4String name = words[0];
  G4int instance = 0;
  if (words.size() > 1)
    {instance = std::stoi(words[1]);}
  
  const BDSBeamline* beamline = BDSAcceleratorModel::Instance()->BeamlineMain();

  if (!beamline)
    {G4cout << "No beam line in this model so not possible to search it."; return;}
  
  // search for the name exactly
  const BDSBeamlineElement* e = beamline->GetElement(name, instance);
 
  G4int count = -1;
  if (!e)
    {// search the beam line for any element containing the name at all
      for (const auto& el : *beamline)
        {
          if (BDS::StrContains(el->GetName(), name))
            {
              count++;
              if (count == instance)
                {
                  e = el;
                  break;
                }
            }
        }
      if (!e)
        {
          G4cout << "No component found by that name" << G4endl;
          if (count > -1)
            {G4cout << "only " << count << " instances found." << G4endl;}
          return;
        }
    }
  
  G4ThreeVector pos = e->GetReferencePositionMiddle();
  G4cout << "goto> " << name << " at global position> " << pos/CLHEP::m << " m" << G4endl;
  G4UImanager* UIManager = G4UImanager::GetUIpointer();
  G4String posStr = std::to_string(pos.x()) + " " + std::to_string(pos.y()) + " " + std::to_string(pos.z());
  UIManager->ApplyCommand("/vis/viewer/set/targetPoint " + posStr + " mm");
}

void BDSMessenger::ElementTypeSearch(std::string /*type*/)
{;}

void BDSMessenger::SamplerList()
{
  if (BDSSamplerRegistry::Instance()->size() == 0)
    {
      G4cout << "No samplers defined" << G4endl;
      return;
    }
  for (const auto& name : BDSSamplerRegistry::Instance()->GetUniqueNames())
    {G4cout << name << G4endl;}
}

void BDSMessenger::ViewSamplers()
{
  // we can't use drawVolume because that clears the scene and only draws one volume
  // inside it really applied add/volume as we do here
  // somehow viewing worlds undoes these trajectory actions in our default vis.mac
  G4UImanager* UIManager = G4UImanager::GetUIpointer();
  UIManager->ApplyCommand("/vis/scene/add/volume SamplerWorld_main");
  UIManager->ApplyCommand("/vis/sceneHandler/attach");
  UIManager->ApplyCommand("/vis/scene/endOfEventAction accumulate");
  UIManager->ApplyCommand("/vis/scene/add/trajectories");
  UIManager->ApplyCommand("/vis/ogl/flushAt endOfEvent");
}

std::string BDSMessenger::BDSSamplerToString(std::string /*samplerName*/)
{
  return std::string();
}

std::string BDSMessenger::BDSSamplerToString(int iSampler)
{
  std::stringstream ss;

  BDSSamplerPlacementRecord sInfo = BDSSamplerRegistry::Instance()->GetInfo(iSampler);

  ss << std::setfill('0') << std::right << std::setw(4) << iSampler << " " << std::setfill(' ')
     << std::right << std::setw(20) << sInfo.Name() << " "
     << std::right << std::setw(20) << sInfo.SPosition();
  return ss.str();
}

void BDSMessenger::ListQueries()
{
  auto queries = BDSParser::Instance()->GetQueries();
  for (const auto& qu : queries)
    {G4cout << qu.name << G4endl;}
}
