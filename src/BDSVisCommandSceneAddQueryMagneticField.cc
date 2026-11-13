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
#include "BDSDetectorConstruction.hh"
#include "BDSFieldQueryInfo.hh"
#include "BDSUtilities.hh"
#include "BDSVisCommandSceneAddQueryMagneticField.hh"
#include "BDSVisFieldModel.hh"

#include "globals.hh"
#include "G4String.hh"
#include "G4Types.hh"
#include "G4UIcommand.hh"
#include "G4UIparameter.hh"
#include "G4Version.hh"
#include "G4VisManager.hh"

#if G4VERSION_NUMBER < 1060
#include "G4UImanager.hh"
#include "G4VSceneHandler.hh"
#endif

#include <vector>

BDSVisCommandSceneAddQueryMagneticField::BDSVisCommandSceneAddQueryMagneticField(const BDSDetectorConstruction* realWorldIn):
  realWorld(realWorldIn),
  command(nullptr)
{
  command = new G4UIcommand("/bds/field/drawQuery", this);
  command->SetGuidance("Adds field representation from a query to the current scene. The key \"all\" can be used to draw all defined queries.");

  G4UIparameter* parameter;
  parameter = new G4UIparameter("queryName", 's', false);
  parameter->SetDefaultValue("");
  command->SetParameter(parameter);
}

BDSVisCommandSceneAddQueryMagneticField::~BDSVisCommandSceneAddQueryMagneticField()
{
  delete command;
}

G4String BDSVisCommandSceneAddQueryMagneticField::GetCurrentValue(G4UIcommand*)
{
  return "";
}

void BDSVisCommandSceneAddQueryMagneticField::SetNewValue(G4UIcommand*, G4String newValue)
{
  G4VisManager::Verbosity verbosity = G4VisManager::GetVerbosity();
  G4bool warn = verbosity >= G4VisManager::warnings;
  G4Scene* scene = fpVisManager->GetCurrentScene();
  if (!scene)
    {
      if (verbosity >= G4VisManager::errors)
	{G4cerr <<  "ERROR: No current scene.  Please create one." << G4endl;}
      return;
    }
  if (newValue.empty())
    {G4cerr << "No query name specified." << G4endl; return;}
  std::vector<G4String> queryNames = BDS::SplitOnWhiteSpace(newValue);
  
  std::vector<BDSFieldQueryInfo*> queries;
  
  // Build a map of available query object names
  const std::vector<BDSFieldQueryInfo*>& allQueries = realWorld->FieldQueries();
  std::map<G4String, BDSFieldQueryInfo*> nameToObject;
  for (const auto& query : allQueries)
    {nameToObject[query->name] = query;}
  
  // Build a vector of the queries by name
  for (const auto& name : queryNames)
    {
      if (name == "all")
	{
	  queries = allQueries;
	  break;
	}
      
      auto search = nameToObject.find(name);
      if (search != nameToObject.end())
        {queries.push_back(search->second);}
      else
        {G4cerr << "No such query name \"" << name << "\"" << G4endl;}
    }
  
  if (queries.empty())
    {G4cerr << "No queries found to draw" << G4endl; return;}
  
  G4VModel* model = new BDSVisFieldModel(queries);
  G4bool successful = scene->AddRunDurationModel(model, warn);
  
  if (successful && verbosity >= G4VisManager::confirmations)
    {G4cout << "Magnetic field, if any, will be drawn in scene \"" << scene->GetName() << "\"" << G4endl;}
  else
    {
      if (!successful && verbosity >= G4VisManager::warnings)
        {G4cerr << "WARNING: not possible to add to the scene" << G4endl;}
    }

  CheckSceneAndNotifyHandlers(scene);
}

#if G4VERSION_NUMBER < 1060
void BDSVisCommandSceneAddQueryMagneticField::CheckSceneAndNotifyHandlers(G4Scene* scene)
{
  if (!scene)
    {return;}
  
  G4VSceneHandler* sceneHandler = fpVisManager->GetCurrentSceneHandler();
  if (!sceneHandler)
    {return;}
  
  if (scene == sceneHandler->GetScene())
    {G4UImanager::GetUIpointer()->ApplyCommand("/vis/scene/notifyHandlers");}
}
#endif
