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
#include "BDSFieldBuilder.hh"
#include "BDSFieldFactory.hh"
#include "BDSFieldInfo.hh"
#include "BDSFieldObjects.hh"

#include "G4LogicalVolume.hh"

#include <set>
#include <vector>

BDSFieldBuilder* BDSFieldBuilder::instance = nullptr;

BDSFieldBuilder* BDSFieldBuilder::Instance()
{
  if (!instance)
    {instance = new BDSFieldBuilder();}
  return instance;
}

BDSFieldBuilder::~BDSFieldBuilder()
{
  instance = nullptr;
}

BDSFieldBuilder::BDSFieldBuilder()
{
  size_t defaultSize = 30;
  infos.reserve(defaultSize);
  lvs.reserve(defaultSize);
  propagators.reserve(defaultSize);
}

void BDSFieldBuilder::RegisterFieldForConstruction(const BDSFieldInfo*      info,
                                                   const std::vector<G4LogicalVolume*>& logicalVolumes,
                                                   const G4bool             propagateToDaughters,
                                                   const BDSMagnetStrength* magnetStrengthForScaling,
                                                   const G4String&          scalingKey)
{
  if (info)
    {
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "Registering info: " << info
             << " to volume(s): ";
      for (auto vol : logicalVolumes)
        {G4cout << vol->GetName() << " ";}
      G4cout << G4endl;
#endif
      infos.push_back(info);
      lvs.push_back(logicalVolumes);
      propagators.push_back(propagateToDaughters);
      if (info->AutoScale())
        {// only store if we're going to use for autoscaling
          G4int index = (G4int)infos.size() - 1;
          scalingStrengths[index] = magnetStrengthForScaling;
          scalingKeys[index]      = scalingKey;
        }
    }
}
  
void BDSFieldBuilder::RegisterFieldForConstruction(const BDSFieldInfo*      info,
                                                   G4LogicalVolume*         logicalVolume,
                                                   const G4bool             propagateToDaughters,
                                                   const BDSMagnetStrength* magnetStrengthForScaling,
                                                   const G4String&          scalingKey)
{
  std::vector<G4LogicalVolume*> lvsForThisInfo = {logicalVolume};
  RegisterFieldForConstruction(info,
                               lvsForThisInfo,
                               propagateToDaughters,
                               magnetStrengthForScaling,
                               scalingKey);
}

void BDSFieldBuilder::RegisterFieldForConstruction(const BDSFieldInfo*      info,
                                                   const std::set<G4LogicalVolume*>& logicalVolumes,
                                                   const G4bool             propagateToDaughters,
                                                   const BDSMagnetStrength* magnetStrengthForScaling,
                                                   const G4String&          scalingKey)
{
  // copy into vector for this interface
  std::vector<G4LogicalVolume*> lvsForThisInfo;
  lvsForThisInfo.reserve(logicalVolumes.size());
  for (auto lv : logicalVolumes)
    {lvsForThisInfo.push_back(lv);}
  RegisterFieldForConstruction(info,
                               lvsForThisInfo,
                               propagateToDaughters,
                               magnetStrengthForScaling,
                               scalingKey);
} 

std::vector<BDSFieldObjects*> BDSFieldBuilder::CreateAndAttachAll()
{
  std::vector<BDSFieldObjects*> fields;
  fields.reserve(infos.size());
  for (G4int i = 0; i < (G4int)infos.size(); i++)
    {
      BDSFieldObjects* field = nullptr;
      const BDSFieldInfo* currentInf = infos[i];
      try
      {
        if (currentInf->AutoScale())
          {
            field = BDSFieldFactory::Instance()->CreateField(*(infos[i]),
                                                             scalingStrengths[i],
                                                             scalingKeys[i]);
          }
        else
          {field = BDSFieldFactory::Instance()->CreateField(*(infos[i]));}
      }
      catch (BDSException& e)
        {
          e.AppendToMessage("\nField would be attached to logical volume named \"" + lvs[i][0]->GetName() + "\"");
          throw e;
        }
      if (field)
        {
          fields.push_back(field);
          field->AttachToVolume(lvs[i], propagators[i]); // works with vector of LVs*
        }
    }
  return fields;
}
