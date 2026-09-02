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
#include "BDSColours.hh"
#include "BDSCrystal.hh"
#include "BDSCrystalFactory.hh"
#include "BDSCrystalInfo.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSGlobalConstants.hh"
#include "BDSMaterials.hh"
#include "BDSUtilities.hh"

#include "G4Box.hh"
#include "G4DisplacedSolid.hh"
#include "G4ExtrudedSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4RotationMatrix.hh"
#include "G4String.hh"
#include "G4ThreeVector.hh"
#include "G4Torus.hh"
#include "G4Tubs.hh"
#include "G4TwoVector.hh"
#include "G4Types.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"
#include "G4Version.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include <algorithm>
#include <cmath>
#include <memory>
#include <map>
#include <set>
#include <vector>

// only use the crystal extensions if using 10.4.p00 upwards
#if G4VERSION_NUMBER > 1039
#include "G4ExtendedMaterial.hh"
#include "G4CrystalExtension.hh"
#include "G4CrystalUnitCell.hh"
#include "G4ChannelingMaterialData.hh"
#include "G4LogicalCrystalVolume.hh"
#endif

#if G4VERSION_NUMBER >= 1120
#include "G4BaierKatkov.hh"
#include "G4ChannelingFastSimModel.hh"
#include "G4FastSimulationManager.hh"
#include "G4Region.hh"
#include "G4RegionStore.hh"
#endif

namespace
{
#if G4VERSION_NUMBER >= 1120
  struct FastSimCrystalGroup
  {
    explicit FastSimCrystalGroup(const BDSCrystalInfo& recipeIn, G4Region* regionIn):
      recipe(recipeIn), region(regionIn) {}

    BDSCrystalInfo recipe;
    G4Region* region;
    std::vector<std::pair<G4LogicalVolume*, BDSCrystalInfo>> logicalVolumes;
  };

  std::map<G4String, std::unique_ptr<FastSimCrystalGroup>> fastSimCrystalGroups;
#endif
}

BDSCrystalFactory::BDSCrystalFactory():
  maxStepFactor(1.1),
  nPoints(30)
{
  CleanUp();
}

void BDSCrystalFactory::CleanUp()
{
  FactoryBaseCleanUp();
  crystalSolid      = nullptr;
  crystalLV         = nullptr;
  placementOffset   = G4ThreeVector();
  placementRotation = nullptr;
}

BDSCrystal* BDSCrystalFactory::CreateCrystal(const G4String& name,
					     const BDSCrystalInfo* recipe)
{
  CleanUp();
  
  BDSCrystal* result = nullptr;
  switch(recipe->shape.underlying())
    {
    case BDSCrystalType::box:
      {result = CreateCrystalBox(name, recipe); break;}
    case BDSCrystalType::cylinder:
      {result = CreateCrystalCylinder(name, recipe); break;}
    case BDSCrystalType::torus:
      {result = CreateCrystalTorus(name, recipe); break;}
    default:
      {break;}
    }
  return result;
}
  
void BDSCrystalFactory::CommonConstruction(const G4String&       nameIn,
					   const BDSCrystalInfo* recipe)
{
  allSolids.insert(crystalSolid);

  // only in g4.10.4 onwards do we build the crystal extensions - otherwise regular LV
#if G4VERSION_NUMBER > 1039
  if (recipe->UseFastSim())
    {
#if G4VERSION_NUMBER >= 1120
      crystalLV = new G4LogicalVolume(crystalSolid,
                                      recipe->material,
                                      nameIn + "_crystal_lv");
      RegisterFastSim(recipe);
#else
      throw BDSException(__METHOD_NAME__, "FastSim crystals require Geant4 11.2 or newer");
#endif
    }
  else
    {
      G4ExtendedMaterial* crystalMat = new G4ExtendedMaterial(nameIn+"_crystal.material", recipe->material);
  
      crystalMat->RegisterExtension(std::unique_ptr<G4CrystalExtension>(new G4CrystalExtension(crystalMat)));
      G4CrystalExtension* crystalExtension = dynamic_cast<G4CrystalExtension*>(crystalMat->RetrieveExtension("crystal"));

      G4CrystalUnitCell* uCell = new G4CrystalUnitCell(recipe->sizeA,
                                                       recipe->sizeB,
                                                       recipe->sizeC,
                                                       recipe->alpha,
                                                       recipe->beta,
                                                       recipe->gamma,
                                                       recipe->spaceGroup);
      crystalExtension->SetUnitCell(uCell);

      crystalMat->RegisterExtension(std::unique_ptr<G4ChannelingMaterialData>(new G4ChannelingMaterialData("channeling")));

      G4ChannelingMaterialData* crystalChannelingData = (G4ChannelingMaterialData*)crystalMat->RetrieveExtension("channeling");
      G4String fileName = BDS::GetFullPath(recipe->data);
      if (!BDS::FileExists(fileName + "_pot.txt"))
        {throw BDSException(__METHOD_NAME__, "No such crystal data files beginning with: \"" + fileName + "\"");}
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "Raw data path: " << recipe->data << G4endl;
      G4cout << __METHOD_NAME__ << "Using crystal data: " << fileName << G4endl;
#endif
      crystalChannelingData->SetFilename(fileName);
  
      // -ve here due to right handed coordinate system + convention of +ve bending angle bends
      // away from beam axis. This convention is also implied by when we do a -ve x axis rotation
      // for cylindrical crystals (including the start and sweep angle).
      G4double bendingRadius = -recipe->BendingRadiusHorizontal();
      crystalChannelingData->SetBR(bendingRadius);
  
      crystalLV = new G4LogicalCrystalVolume(crystalSolid,
                                             crystalMat,
                                             nameIn + "_crystal_lv",
                                             nullptr, nullptr, nullptr,
                                             true, 0, 0, 0, recipe->miscutAngleY);
    }

  BDSAcceleratorModel::Instance()->VolumeSet("crystals")->insert(crystalLV);
#else
  // build logical volumes
  crystalLV = new G4LogicalVolume(crystalSolid,
				  recipe->material,
				  nameIn + "_crystal_lv");
#endif
  allLogicalVolumes.insert(crystalLV);
  
  SetVisAttributes();
  SetUserLimits(recipe->lengthZ);
}

void BDSCrystalFactory::RegisterFastSim(const BDSCrystalInfo* recipe)
{
#if G4VERSION_NUMBER >= 1120
  G4RegionStore* regionStore = G4RegionStore::GetInstance();
  G4Region* region = regionStore->GetRegion(recipe->FastSimRegionName(), false);
  if (!region)
    {region = new G4Region(recipe->FastSimRegionName());}
  region->AddRootLogicalVolume(crystalLV);

  auto group = fastSimCrystalGroups.find(recipe->name);
  if (group == fastSimCrystalGroups.end())
    {
      auto newGroup = std::make_unique<FastSimCrystalGroup>(*recipe, region);
      group = fastSimCrystalGroups.insert(std::make_pair(recipe->name, std::move(newGroup))).first;
    }
  group->second->logicalVolumes.emplace_back(crystalLV, *recipe);
#else
  (void)recipe;
#endif
}

void BDSCrystalFactory::ConstructFastSimModels()
{
#if G4VERSION_NUMBER >= 1120
  for (const auto& groupEntry : fastSimCrystalGroups)
    {
      const BDSCrystalInfo* recipe = &groupEntry.second->recipe;
      G4Region* region = groupEntry.second->region;
      G4ChannelingFastSimModel* model =
        new G4ChannelingFastSimModel(recipe->FastSimModelName(), region);
#if G4VERSION_NUMBER < 1130
      if (recipe->fastSimDefaultHighAngleLimit != 0 ||
          !recipe->fastSimHighAngleParticles.empty())
        {throw BDSException(__METHOD_NAME__, "absolute FastSim high-angle controls require Geant4 11.3 or newer");}
#endif
#if G4VERSION_NUMBER < 1140
      if (recipe->coherentPairProduction)
        {throw BDSException(__METHOD_NAME__, "coherent pair production requires Geant4 11.4 or newer");}
#endif
#if G4VERSION_NUMBER >= 1130
      model->Input(recipe->material, recipe->lattice, recipe->fastSimDataPath);
#else
      if (!recipe->fastSimDataPath.empty())
        {throw BDSException(__METHOD_NAME__, "a custom FastSim data path requires Geant4 11.3 or newer");}
      model->Input(recipe->material, recipe->lattice);
#endif

      model->SetDefaultLowKineticEnergyLimit(recipe->fastSimDefaultLowKineticEnergy);
      model->SetDefaultLindhardAngleNumberHighLimit(recipe->fastSimDefaultLindhardAngleHighLimit);
#if G4VERSION_NUMBER >= 1130
      model->SetDefaultHighAngleLimit(recipe->fastSimDefaultHighAngleLimit);
#endif
      model->SetMaxPhotonsProducedPerStep(recipe->fastSimMaxPhotonsPerStep);

      for (std::size_t i = 0; i < recipe->fastSimLowEnergyParticles.size(); ++i)
        {model->SetLowKineticEnergyLimit(recipe->fastSimLowEnergyLimits[i],
                                         recipe->fastSimLowEnergyParticles[i]);}
      for (std::size_t i = 0; i < recipe->fastSimLindhardAngleParticles.size(); ++i)
        {model->SetLindhardAngleNumberHighLimit(recipe->fastSimLindhardAngleLimits[i],
                                                recipe->fastSimLindhardAngleParticles[i]);}
#if G4VERSION_NUMBER >= 1130
      for (std::size_t i = 0; i < recipe->fastSimHighAngleParticles.size(); ++i)
        {model->SetHighAngleLimit(recipe->fastSimHighAngleLimits[i],
                                  recipe->fastSimHighAngleParticles[i]);}
#endif

      if (recipe->radiation)
        {
          model->RadiationModelActivate();
          G4BaierKatkov* radiation = model->GetRadiationModel();
          radiation->SetSinglePhotonRadiationProbabilityLimit(
            recipe->radiationSinglePhotonProbabilityLimit);
          radiation->SetNSmallTrajectorySteps(recipe->radiationSmallTrajectorySteps);
          radiation->SetSamplingPhotonsNumber(recipe->radiationSamplingPhotons);
          radiation->SetRadiationAngleFactor(recipe->radiationAngleFactor);
          radiation->SetSpectrumEnergyRange(recipe->radiationMinPhotonEnergy,
                                            recipe->radiationMaxPhotonEnergy,
                                            recipe->radiationSpectrumBins);
          for (std::size_t i = 0; i < recipe->radiationStatisticsMinEnergy.size(); ++i)
            {
              radiation->AddStatisticsInPhotonEnergyRegion(
                recipe->radiationStatisticsMinEnergy[i],
                recipe->radiationStatisticsMaxEnergy[i],
                recipe->radiationStatisticsMultiple[i]);
            }

#if G4VERSION_NUMBER >= 1140
          if (recipe->radiationVirtualCollimator == "round")
            {
              radiation->SetRoundVirtualCollimator(recipe->radiationCollimatorHalfWidthX,
                                                    recipe->radiationCollimatorCentreX,
                                                    recipe->radiationCollimatorCentreY);
            }
          else if (recipe->radiationVirtualCollimator == "elliptic" ||
                   recipe->radiationVirtualCollimator == "elliptical")
            {
              radiation->SetEllipticVirtualCollimator(recipe->radiationCollimatorHalfWidthX,
                                                       recipe->radiationCollimatorHalfWidthY,
                                                       recipe->radiationCollimatorCentreX,
                                                       recipe->radiationCollimatorCentreY);
            }
          else if (recipe->radiationVirtualCollimator == "rectangular")
            {
              radiation->SetRectangularVirtualCollimator(recipe->radiationCollimatorHalfWidthX,
                                                          recipe->radiationCollimatorHalfWidthY,
                                                          recipe->radiationCollimatorCentreX,
                                                          recipe->radiationCollimatorCentreY);
            }
          else if (recipe->radiationVirtualCollimator != "none")
            {
              throw BDSException(__METHOD_NAME__, "unknown radiation virtual collimator \"" +
                                  recipe->radiationVirtualCollimator + "\"");
            }
#else
          if (recipe->radiationVirtualCollimator == "round")
            {
              if (recipe->radiationCollimatorCentreX != 0 ||
                  recipe->radiationCollimatorCentreY != 0)
                {throw BDSException(__METHOD_NAME__, "an offset radiation virtual collimator requires Geant4 11.4 or newer");}
              // Geant4 11.2 and 11.3 expose only a centred circular
              // collimator whose input is the full angular diameter.
              radiation->SetVirtualCollimator(2 * recipe->radiationCollimatorHalfWidthX);
            }
          else if (recipe->radiationVirtualCollimator != "none")
            {throw BDSException(__METHOD_NAME__, "elliptic and rectangular radiation virtual collimators require Geant4 11.4 or newer");}
#endif
        }

      auto crystalData = model->GetCrystalData();
      for (const auto& volumeEntry : groupEntry.second->logicalVolumes)
        {
          G4LogicalVolume* logicalVolume = volumeEntry.first;
          const BDSCrystalInfo& volumeRecipe = volumeEntry.second;
          crystalData->SetBendingAngle(volumeRecipe.bendingAngleYAxis, logicalVolume);
          crystalData->SetMiscutAngle(volumeRecipe.miscutAngleY, logicalVolume);
          if (!volumeRecipe.fastSimCUGeometryFile.empty())
            {
#if G4VERSION_NUMBER >= 1140
              crystalData->SetCrystallineUndulatorParameters(logicalVolume,
                                                              volumeRecipe.fastSimCUGeometryFile);
#else
              throw BDSException(__METHOD_NAME__, "an imported crystalline-undulator geometry requires Geant4 11.4 or newer");
#endif
            }
          else if (volumeRecipe.fastSimCUAmplitude > 0 && volumeRecipe.fastSimCUPeriod > 0)
            {
              crystalData->SetCrystallineUndulatorParameters(volumeRecipe.fastSimCUAmplitude,
                                                              volumeRecipe.fastSimCUPeriod,
                                                              volumeRecipe.fastSimCUPhase,
                                                              logicalVolume);
            }
        }
    }
#endif
}

void BDSCrystalFactory::SetVisAttributes()
{
  G4VisAttributes* crysVisAttr = new G4VisAttributes(*BDSColours::Instance()->GetColour("crystal"));
  crysVisAttr->SetVisibility(true);
  crysVisAttr->SetForceLineSegmentsPerCircle(200);
  allVisAttributes.insert(crysVisAttr);
  crystalLV->SetVisAttributes(crysVisAttr);
}

void BDSCrystalFactory::SetUserLimits(G4double length)
{
  auto defaultUL = BDSGlobalConstants::Instance()->DefaultUserLimits();
  //copy the default and update with the length of the object rather than the default 1m
  G4UserLimits* ul = BDS::CreateUserLimits(defaultUL, length*maxStepFactor);

  if (ul != defaultUL) // if it's not the default register it
    {allUserLimits.insert(ul);}
  crystalLV->SetUserLimits(ul);
}

BDSCrystal* BDSCrystalFactory::BuildCrystalObject(const BDSCrystalInfo* recipe,
						  const BDSExtent& extent)
{  
  // build the BDSCrystal instance and return it
  BDSCrystal* aCrystal = new BDSCrystal(recipe, crystalSolid, crystalLV,
					extent, placementOffset, placementRotation);

  // register objects
  aCrystal->RegisterSolid(allSolids);
  aCrystal->RegisterSensitiveVolume(crystalLV, BDSSDType::collimatorcomplete);
  aCrystal->RegisterUserLimits(allUserLimits);
  aCrystal->RegisterVisAttributes(allVisAttributes);
  aCrystal->ExcludeLogicalVolumeFromBiasing(crystalLV); // can't double bias one LV ie with generic biasing
  
  return aCrystal;
}

BDSCrystal* BDSCrystalFactory::CreateCrystalBox(const G4String&       nameIn,
						const BDSCrystalInfo* recipe)
{
  crystalSolid = new G4Box(nameIn + "_solid",
			   recipe->lengthX * 0.5,
			   recipe->lengthY * 0.5,
			   recipe->lengthZ * 0.5);

  CommonConstruction(nameIn, recipe);
  
  placementOffset = G4ThreeVector(0, 0, 0);
  BDSExtent ext = BDSExtent(recipe->lengthX * 0.5,
			    recipe->lengthY * 0.5,
			    recipe->lengthZ * 0.5);
  
  return BuildCrystalObject(recipe, ext); // no placement offset - leave as default
}

void BDSCrystalFactory::CalculateSolidAngles(G4double bendingAngle,
					     G4double& startAngle,
					     G4double& sweepAngle) const
{
  if (bendingAngle >= 0)
    {
      startAngle = CLHEP::twopi - 0.5 * bendingAngle;
      sweepAngle = bendingAngle;
    }
  else
    {
      startAngle = CLHEP::pi - 0.5*std::abs(bendingAngle);
      sweepAngle = std::abs(bendingAngle);
    }
}

BDSExtent BDSCrystalFactory::CalculateExtents(G4double xBendingAngle,
					      G4double xBendingRadius,
					      G4double xThickness,
					      const BDSCrystalInfo* recipe) const
{
  // calculate horizontal extents - do in +ve version and flip for -ve
  G4double xHi  = xBendingRadius - (xBendingRadius - xThickness*0.5)*std::cos(std::abs(xBendingAngle)*0.5);
  G4double xLow = -(xThickness * 0.5);
  if (xBendingAngle > 0)
    {
      std::swap(xHi, xLow);
      xHi  *= -1;
      xLow *= -1;
    }
  G4double dz = (xBendingRadius + xThickness*0.5) * std::sin(std::abs(xBendingAngle)*0.5);

  // note this is the extent the partial cylinder would have with its intended placement.
  // convenient for now but doesn't strictly match the solid which is built along the z axis.
  BDSExtent ext = BDSExtent(xLow, xHi,
			    recipe->lengthY * 0.5, recipe->lengthY * 0.5,
			    -dz, dz);
  return ext;
}

BDSCrystal* BDSCrystalFactory::CreateCrystalCylinder(const G4String&       nameIn,
						     const BDSCrystalInfo* recipe)
{
  G4double ba = recipe->bendingAngleYAxis; // bending angle

  // if no bending angle, create a box as that's all we can create
  if (!BDS::IsFinite(ba))
    {return CreateCrystalBox(nameIn, recipe);}

  G4double xBR = std::abs(recipe->BendingRadiusHorizontal());
  G4double thickness = recipe->lengthX;
  
  // calculate start angle and sweep angle
  G4double startAngle;
  G4double sweepAngle;
  CalculateSolidAngles(ba, startAngle, sweepAngle);
  
  G4Tubs* rawShape = new G4Tubs(nameIn + "_solid",
				xBR - 0.5*thickness,
				xBR + 0.5*thickness,
				recipe->lengthY*0.5,
				startAngle,
				sweepAngle);

  // crystal channelling only works in local unitX of a given solid which
  // makes it impossible to use a cylinder. we cheat by using a G4DisplacedSolid
  // that's a class used internally by geant4's boolean solids to rotate and translate
  // the frame of a solid. another option was an intersection with a big box, but
  // geant4 can't handle this.
  G4RotationMatrix* relativeRotation = new G4RotationMatrix();
  relativeRotation->rotateX(-CLHEP::halfpi);
  G4double bendingRadiusH = recipe->BendingRadiusHorizontal(); // includes sign
  G4ThreeVector offset(-bendingRadiusH,0,0);
  crystalSolid = new G4DisplacedSolid(nameIn + "_shifted_solid",
				      rawShape,
				      relativeRotation,
				      offset);
  
  CommonConstruction(nameIn, recipe);
  placementOffset = G4ThreeVector();
  BDSExtent ext = CalculateExtents(ba, xBR, thickness, recipe);
  return BuildCrystalObject(recipe, ext);
}

BDSCrystal* BDSCrystalFactory::CreateCrystalTorus(const G4String&       nameIn,
						  const BDSCrystalInfo* recipe)
{
  G4double xBA = recipe->bendingAngleYAxis; // bending angle horizontal
  G4double xBR = std::abs(recipe->BendingRadiusHorizontal());
  G4double zBA = recipe->bendingAngleZAxis;
  G4double zBR = std::abs(recipe->BendingRadiusVertical());

  // if no bending angle, create a box as that's all we can create
  if (!BDS::IsFinite(xBA))
    {return CreateCrystalBox(nameIn, recipe);}
  
  G4double thickness = recipe->lengthX;
  
  // calculate start angle and sweep angle
  G4double startAngle;
  G4double sweepAngle;
  CalculateSolidAngles(xBA, startAngle, sweepAngle);
  
  G4double angFraction = sweepAngle / (G4double)nPoints;
  std::vector<G4TwoVector> points;

  G4double innerRadius = xBR - thickness*0.5;
  for (G4int i = 0; i < nPoints; i++)
    {
      G4double ang = startAngle + (G4double)i * angFraction;
      G4double x = innerRadius * std::cos(ang);
      G4double y = innerRadius * std::sin(ang);
      points.emplace_back(G4TwoVector(x,y));
    }
  G4double outerRadius = xBR + thickness*0.5;
  for (G4int i = nPoints; i > 0; i--)
    {
      G4double ang = startAngle + (G4double)i * angFraction;
      G4double x = outerRadius * std::cos(ang);
      G4double y = outerRadius * std::sin(ang);
      points.emplace_back(G4TwoVector(x,y));
    }

  G4double zStartAngle = CLHEP::twopi - 0.5 * zBA;
  G4double zSweepAngle = zBA;

  // create vertical z sections, accumulate max/min shift at same time
  G4double xmin = 0;
  G4double xmax = 0;
  G4double zAngFraction = zSweepAngle / (G4double)nPoints;
  std::vector<G4ExtrudedSolid::ZSection> zSections;
  for (G4int i = 0; i < nPoints; i++)
    {
      G4double zAng = zStartAngle + (G4double)i * zAngFraction;
      G4double z = zBR * std::sin(zAng);
      G4double x = zBR - (zBR * std::cos(zAng));
      if (zBA < 0)
	{
	  z *= -1;
	  x *= -1;
	}
      xmin = std::min(xmin, x);
      xmax = std::max(xmax, x);
      zSections.emplace_back(G4ExtrudedSolid::ZSection(z, G4TwoVector(x, 0), 1));
    }

  G4ExtrudedSolid* rawShape = new G4ExtrudedSolid(nameIn + "_solid",
						  points,
						  zSections);
  
  // crystal channelling only works in local unitX of a given solid which
  // makes it impossible to use a cylinder. we cheat by using a G4DisplacedSolid
  // that's a class used internally by geant4's boolean solids to rotate and translate
  // the frame of a solid. another option was an intersection with a big box, but
  // geant4 can't handle this.
  G4RotationMatrix* relativeRotation = new G4RotationMatrix();
  relativeRotation->rotateX(-CLHEP::halfpi);
  G4ThreeVector offset(0,0,0);
  crystalSolid = new G4DisplacedSolid(nameIn + "_shifted_solid",
				      rawShape,
				      relativeRotation,
				      offset);

  CommonConstruction(nameIn, recipe);

  // placement offset - no rotation as we've rotated the solid internally
  placementOffset = G4ThreeVector(-1*recipe->BendingRadiusHorizontal(), 0, 0);
  
  BDSExtent ext = CalculateExtents(xBA, xBR, thickness, recipe);
  G4double xLow = ext.XNeg() + xmin;
  G4double xHi  = ext.XPos() + xmax;
  BDSExtent extTorus = BDSExtent(xLow, xHi, ext.YNeg(), ext.YPos(), ext.ZNeg(), ext.ZPos());
  
  return BuildCrystalObject(recipe, extTorus);
}
