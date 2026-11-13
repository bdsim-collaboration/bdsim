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
#include "BDSArray1DCoords.hh"
#include "BDSArray1DCoordsTransformed.hh"
#include "BDSArray2DCoords.hh"
#include "BDSArray2DCoordsTransformed.hh"
#include "BDSArray3DCoords.hh"
#include "BDSArray3DCoordsTransformed.hh"
#include "BDSArray4DCoords.hh"
#include "BDSArray4DCoordsTransformed.hh"
#include "BDSArray2DCoordsRDipole.hh"
#include "BDSArray2DCoordsRQuad.hh"
#include "BDSArrayInfo.hh"
#include "BDSArrayOperatorIndex.hh"
#include "BDSArrayOperatorIndexFlip.hh"
#include "BDSArrayOperatorIndexReflect.hh"
#include "BDSArrayOperatorIndexV.hh"
#include "BDSArrayOperatorValue.hh"
#include "BDSArrayOperatorValueFlip.hh"
#include "BDSArrayOperatorValueReflect.hh"
#include "BDSArrayOperatorValueReflectDipoleXY.hh"
#include "BDSArrayOperatorValueReflectDipoleY.hh"
#include "BDSArrayOperatorValueReflectQuadrupoleXY.hh"
#include "BDSArrayOperatorValueReflectSolenoidZ.hh"
#include "BDSArrayOperatorValueV.hh"
#include "BDSArrayReflectionType.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSFieldEInterpolated.hh"
#include "BDSFieldEInterpolated1D.hh"
#include "BDSFieldEInterpolated2D.hh"
#include "BDSFieldEInterpolated3D.hh"
#include "BDSFieldEInterpolated4D.hh"
#include "BDSFieldEMInterpolated.hh"
#include "BDSFieldEMInterpolated1D.hh"
#include "BDSFieldEMInterpolated2D.hh"
#include "BDSFieldEMInterpolated3D.hh"
#include "BDSFieldEMInterpolated4D.hh"
#include "BDSFieldFormat.hh"
#include "BDSFieldInfo.hh"
#include "BDSFieldLoader.hh"
#include "BDSFieldLoaderBDSIM.hh"
#include "BDSFieldLoaderPoisson.hh"
#include "BDSFieldMagInterpolated.hh"
#include "BDSFieldMagInterpolated1D.hh"
#include "BDSFieldMagInterpolated2D.hh"
#include "BDSFieldMagInterpolated3D.hh"
#include "BDSFieldMagInterpolated4D.hh"
#include "BDSFieldValue.hh"
#include "BDSInterpolator1D.hh"
#include "BDSInterpolator1DCubic.hh"
#include "BDSInterpolator1DLinear.hh"
#include "BDSInterpolator1DLinearMag.hh"
#include "BDSInterpolator1DNearest.hh"
#include "BDSInterpolator2D.hh"
#include "BDSInterpolator2DCubic.hh"
#include "BDSInterpolator2DLinear.hh"
#include "BDSInterpolator2DLinearMag.hh"
#include "BDSInterpolator2DNearest.hh"
#include "BDSInterpolator3D.hh"
#include "BDSInterpolator3DCubic.hh"
#include "BDSInterpolator3DLinear.hh"
#include "BDSInterpolator3DLinearMag.hh"
#include "BDSInterpolator3DNearest.hh"
#include "BDSInterpolator4D.hh"
#include "BDSInterpolator4DCubic.hh"
#include "BDSInterpolator4DLinear.hh"
#include "BDSInterpolator4DLinearMag.hh"
#include "BDSInterpolator4DNearest.hh"
#include "BDSInterpolatorType.hh"
#include "BDSFieldMagGradient.hh"
#include "BDSMagnetStrength.hh"
#include "BDSWarning.hh"

#include "globals.hh" // geant4 types / globals
#include "G4String.hh"

#include <algorithm>
#include <array>
#include <cmath>
#include <fstream>
#include <set>

#ifdef USE_GZSTREAM
#include "src-external/gzstream/gzstream.h"
#endif

BDSFieldLoader* BDSFieldLoader::instance = nullptr;

BDSFieldLoader* BDSFieldLoader::Instance()
{
  if (!instance)
    {instance = new BDSFieldLoader();}
  return instance;
}

BDSFieldLoader::BDSFieldLoader()
{;}

BDSFieldLoader::~BDSFieldLoader()
{
  DeleteArrays();
  instance = nullptr;
}

void BDSFieldLoader::DeleteArrays()
{
  for (auto& a : arrays1d)
    {delete a.second;}
  for (auto& a : arrays2d)
    {delete a.second;}
  for (auto& a : arrays3d)
    {delete a.second;}
  for (auto& a : arrays4d)
    {delete a.second;}
}

BDSFieldMagInterpolated* BDSFieldLoader::LoadMagField(const BDSFieldInfo&      info,
                                                      const BDSMagnetStrength* scalingStrength,
                                                      const G4String&          scalingKey)
{
  BFilePathOK(info);
  G4String                    filePath = info.MagneticFile();
  BDSFieldFormat                format = info.MagneticFormat();
  BDSInterpolatorType interpolatorType = info.MagneticInterpolatorType();
  G4Transform3D              transform = info.Transform();
  G4double                    bScaling = info.BScaling();
  BDSArrayReflectionTypeSet reflection = info.MagneticArrayReflectionType();
  BDSArrayReflectionTypeSet* reflectionPointer = reflection.empty() ? nullptr : &reflection;
  
  BDSFieldMagInterpolated* result = nullptr;
  try
  {
  switch (format.underlying())
    {
    case BDSFieldFormat::bdsim1d:
      {result = LoadBDSIM1DB(filePath, interpolatorType, transform, bScaling, reflectionPointer); break;}
    case BDSFieldFormat::bdsim2d:
      {result = LoadBDSIM2DB(filePath, interpolatorType, transform, bScaling, reflectionPointer); break;}
    case BDSFieldFormat::bdsim3d:
      {result = LoadBDSIM3DB(filePath, interpolatorType, transform, bScaling, reflectionPointer); break;}
    case BDSFieldFormat::bdsim4d:
      {result = LoadBDSIM4DB(filePath, interpolatorType, transform, bScaling, reflectionPointer); break;}
    case BDSFieldFormat::poisson2d:
      {result = LoadPoissonSuperFishB(filePath, interpolatorType, transform, bScaling, reflectionPointer); break;}
    case BDSFieldFormat::poisson2dquad:
      {result = LoadPoissonSuperFishBQuad(filePath, interpolatorType, transform, bScaling, reflectionPointer); break;}
    case BDSFieldFormat::poisson2ddipole:
      {result = LoadPoissonSuperFishBDipole(filePath, interpolatorType, transform, bScaling, reflectionPointer); break;}
    default:
      {break;}
    }
  }
  catch (BDSException& e)
  {
    e.AppendToMessage("\nError in field definition \"" + info.NameOfParserDefinition() + "\".");
    throw e;
  }

  if (result && info.AutoScale() && scalingStrength)
    {
      // prepare temporary recipe for field with cubic interpolation and no scaling
      // other than units
      BDSFieldInfo temporaryRecipe = BDSFieldInfo(info);
      temporaryRecipe.SetAutoScale(false); // prevent recursion
      temporaryRecipe.SetBScaling(1);      // don't affect result with inadvertent scaling

      // enforce cubic interpolation for continuous higher differentials
      G4int nDimFF = BDS::NDimensionsOfFieldFormat(format);
      auto magIntType = BDS::InterpolatorTypeSpecificFromAuto(nDimFF, BDSInterpolatorType::cubicauto);
      temporaryRecipe.SetMagneticInterpolatorType(magIntType);

      // build temporary field object
      BDSFieldMagInterpolated* tempField = LoadMagField(temporaryRecipe);

      // calculate field gradients and therefore associated strengths for a given rigidity
      BDSFieldMagGradient calculator;
      BDSMagnetStrength* calculatedStrengths = calculator.CalculateMultipoles(tempField,
                                                                              5,/*up to 5th order*/
                                                                              info.BRho());
      
      delete tempField; // clear up

      G4double calculatedNumber = (*calculatedStrengths)[scalingKey];
      G4double ratio = (*scalingStrength)[scalingKey] / calculatedNumber;
      if (!std::isnormal(ratio))
        {
          G4cout << __METHOD_NAME__ << "invalid ratio detected (" << ratio << ") setting to 1.0" << G4endl;
          ratio = 1;
        }
      G4double newScale = result->Scaling() * ratio;
      G4cout << "autoScale> Calculated " << scalingKey << " = " << calculatedNumber << G4endl;
      G4cout << "autoScale> Ratio of supplied strength to calculated map strength: " << ratio << G4endl;
      G4cout << "autoScale> New overall scaling factor: " << bScaling*ratio << G4endl;
      result->SetScaling(newScale);
      delete calculatedStrengths;
    }
  
  return result;
}

BDSFieldEInterpolated* BDSFieldLoader::LoadEField(const BDSFieldInfo& info)
{
  EFilePathOK(info);
  G4String                    filePath = info.ElectricFile();
  BDSFieldFormat                format = info.ElectricFormat();
  BDSInterpolatorType interpolatorType = info.ElectricInterpolatorType();
  G4Transform3D              transform = info.Transform();
  G4double                    eScaling = info.EScaling();
  BDSArrayReflectionTypeSet reflection = info.ElectricArrayReflectionType();
  BDSArrayReflectionTypeSet* reflectionPointer = reflection.empty() ? nullptr : &reflection;
  
  BDSFieldEInterpolated* result = nullptr;
  try
  {
  switch (format.underlying())
    {
    case BDSFieldFormat::bdsim1d:
      {result = LoadBDSIM1DE(filePath, interpolatorType, transform, eScaling, reflectionPointer); break;}
    case BDSFieldFormat::bdsim2d:
      {result = LoadBDSIM2DE(filePath, interpolatorType, transform, eScaling, reflectionPointer); break;}
    case BDSFieldFormat::bdsim3d:
      {result = LoadBDSIM3DE(filePath, interpolatorType, transform, eScaling, reflectionPointer); break;}
    case BDSFieldFormat::bdsim4d:
      {result = LoadBDSIM4DE(filePath, interpolatorType, transform, eScaling, reflectionPointer); break;}
    default:
      {break;}
    }
  }
  catch (BDSException& e)
  {
    e.AppendToMessage(" error in field definition \"" + info.NameOfParserDefinition() + "\"");
    throw e;
  }
  return result;
}

BDSFieldEMInterpolated* BDSFieldLoader::LoadEMField(const BDSFieldInfo& info)
{
  BFilePathOK(info);
  EFilePathOK(info);
  G4String           eFilePath = info.ElectricFile();
  G4String           bFilePath = info.MagneticFile();
  BDSFieldFormat     eFormat   = info.ElectricFormat();
  BDSFieldFormat     bFormat   = info.MagneticFormat();
  BDSInterpolatorType eIntType = info.ElectricInterpolatorType();
  BDSInterpolatorType bIntType = info.MagneticInterpolatorType();
  G4Transform3D      transform = info.Transform();
  G4double            eScaling = info.EScaling();
  G4double            bScaling = info.BScaling();
  BDSArrayReflectionTypeSet bReflection = info.MagneticArrayReflectionType();
  BDSArrayReflectionTypeSet* bReflectionPointer = bReflection.empty() ? nullptr : &bReflection;
  BDSArrayReflectionTypeSet eReflection = info.ElectricArrayReflectionType();
  BDSArrayReflectionTypeSet* eReflectionPointer = eReflection.empty() ? nullptr : &eReflection;

  // As the different dimension interpolators don't inherit each other, it's very
  // very hard to make a compact polymorphic construction routine here.  In future,
  // if we use delayed construction with setters, we could piece together the BDSFieldEM
  // one bit at a time. This is more an issue with the number of dimensions than anything.
  if (bFormat != eFormat)
    {throw BDSException(__METHOD_NAME__, "different formats for E and B fields are not currently supported for an EM field");}
  
  BDSFieldEMInterpolated* result = nullptr;
  try
  {
  switch (eFormat.underlying())
    {
    case BDSFieldFormat::bdsim1d:
      {
        result = LoadBDSIM1DEM(eFilePath, bFilePath, eIntType, bIntType, transform,
                               eScaling, bScaling, eReflectionPointer, bReflectionPointer);
        break;
      }
    case BDSFieldFormat::bdsim2d:
      {
        result = LoadBDSIM2DEM(eFilePath, bFilePath, eIntType, bIntType, transform,
                               eScaling, bScaling, eReflectionPointer, bReflectionPointer);
        break;
      }
    case BDSFieldFormat::bdsim3d:
      {
        result = LoadBDSIM3DEM(eFilePath, bFilePath, eIntType, bIntType, transform,
                               eScaling, bScaling, eReflectionPointer, bReflectionPointer);
        break;
      }
    case BDSFieldFormat::bdsim4d:
      {
        result = LoadBDSIM4DEM(eFilePath, bFilePath, eIntType, bIntType, transform,
                               eScaling, bScaling, eReflectionPointer, bReflectionPointer);
        break;
      }
    default:
      {break;}
    }
  }
  catch (BDSException& e)
  {
    e.AppendToMessage(" error in field definition \"" + info.NameOfParserDefinition() + "\"");
    throw e;
  }
  return result;  
}

void BDSFieldLoader::BFilePathOK(const BDSFieldInfo& info)
{
  G4String filePath = info.MagneticFile();
  if (filePath.empty())
    {
      G4String msg = "no magneticFile specified in field definition \"" + info.NameOfParserDefinition() + "\"";
      throw BDSException(__METHOD_NAME__, msg);
    }
}

void BDSFieldLoader::EFilePathOK(const BDSFieldInfo& info)
{
  G4String filePath = info.ElectricFile();
  if (filePath.empty())
    {
      G4String msg = "no electricFile specified in field definition \"" + info.NameOfParserDefinition() + "\"";
      throw BDSException(__METHOD_NAME__, msg);
    }
}

BDSArray1DCoords* BDSFieldLoader::Get1DCached(const G4String& filePath)
{
  auto result = arrays1d.find(filePath);
  if (result != arrays1d.end())
    {return result->second;}
  else
    {return nullptr;}
}

BDSArray2DCoords* BDSFieldLoader::Get2DCached(const G4String& filePath)
{
  auto result = arrays2d.find(filePath);
  if (result != arrays2d.end())
    {return result->second;}
  else
    {return nullptr;}
}

BDSArray3DCoords* BDSFieldLoader::Get3DCached(const G4String& filePath)
{
  auto result = arrays3d.find(filePath);
  if (result != arrays3d.end())
    {return result->second;}
  else
    {return nullptr;}
}

BDSArray4DCoords* BDSFieldLoader::Get4DCached(const G4String& filePath)
{
  auto result = arrays4d.find(filePath);
  if (result != arrays4d.end())
    {return result->second;}
  else
    {return nullptr;}
}

BDSArray2DCoords* BDSFieldLoader::LoadPoissonMag2D(const G4String& filePath)
{
  BDSArray2DCoords* cached = Get2DCached(filePath);
  if (cached)
    {return cached;}

  BDSArray2DCoords* result = nullptr;
  if (filePath.rfind("gz") != std::string::npos)
    {
#ifdef USE_GZSTREAM
      BDSFieldLoaderPoisson<igzstream> loader;
      result = loader.LoadMag2D(filePath);
#else
      throw BDSException(__METHOD_NAME__, "Compressed file loading - but BDSIM not compiled with ZLIB.");
#endif
    }
  else
    {
      BDSFieldLoaderPoisson<std::ifstream> loader;
      result = loader.LoadMag2D(filePath);
    }
  arrays2d[filePath] = result;
  return result;  
}

BDSArray1DCoords* BDSFieldLoader::LoadBDSIM1D(const G4String& filePath)
{
  BDSArray1DCoords* cached = Get1DCached(filePath);
  if (cached)
    {return cached;}

  // Don't want to template this class and there's no base class pointer
  // for BDSFieldLoader so unfortunately, there's a wee bit of repetition.
  BDSArray1DCoords* result = nullptr;
  if (filePath.rfind("gz") != std::string::npos)
    {
#ifdef USE_GZSTREAM
      BDSFieldLoaderBDSIM<igzstream> loader;
      result = loader.Load1D(filePath);
#else
      throw BDSException(__METHOD_NAME__, "Compressed file loading - but BDSIM not compiled with ZLIB.");
#endif
    }
  else
    {
      BDSFieldLoaderBDSIM<std::ifstream> loader;
      result = loader.Load1D(filePath);
    }
  arrays1d[filePath] = result;
  return result;
}

BDSArray2DCoords* BDSFieldLoader::LoadBDSIM2D(const G4String& filePath)
{
  BDSArray2DCoords* cached = Get2DCached(filePath);
  if (cached)
    {return cached;}
  
  BDSArray2DCoords* result = nullptr;
  if (filePath.rfind("gz") != std::string::npos)
    {
#ifdef USE_GZSTREAM
      BDSFieldLoaderBDSIM<igzstream> loader;
      result = loader.Load2D(filePath);
#else
      throw BDSException(__METHOD_NAME__, "Compressed file loading - but BDSIM not compiled with ZLIB.");
#endif
    }
  else
    {
      BDSFieldLoaderBDSIM<std::ifstream> loader;
      result = loader.Load2D(filePath);
    }
  arrays2d[filePath] = result;
  return result;
}

BDSArray3DCoords* BDSFieldLoader::LoadBDSIM3D(const G4String& filePath)
{
  BDSArray3DCoords* cached = Get3DCached(filePath);
  if (cached)
    {return cached;}

  BDSArray3DCoords* result = nullptr;
  if (filePath.rfind("gz") != std::string::npos )
    {
#ifdef USE_GZSTREAM
      BDSFieldLoaderBDSIM<igzstream> loader;
      result = loader.Load3D(filePath);
#else
      throw BDSException(__METHOD_NAME__, "Compressed file loading - but BDSIM not compiled with ZLIB.");
#endif
    }
  else
    {
      BDSFieldLoaderBDSIM<std::ifstream> loader;
      result = loader.Load3D(filePath);
}
  arrays3d[filePath] = result;
  return result;
}

BDSArray4DCoords* BDSFieldLoader::LoadBDSIM4D(const G4String& filePath)
{
  BDSArray4DCoords* cached = Get4DCached(filePath);
  if (cached)
    {return cached;}

  BDSArray4DCoords* result = nullptr;
  if (filePath.rfind("gz") != std::string::npos)
    {
#ifdef USE_GZSTREAM
      BDSFieldLoaderBDSIM<igzstream> loader;
      result = loader.Load4D(filePath);
#else
      throw BDSException(__METHOD_NAME__, "Compressed file loading - but BDSIM not compiled with ZLIB.");
#endif
    }
  else
    {
      BDSFieldLoaderBDSIM<std::ifstream> loader;
      result = loader.Load4D(filePath);
    }
  arrays4d[filePath] = result;
  return result;
}

BDSInterpolator1D* BDSFieldLoader::CreateInterpolator1D(BDSArray1DCoords*   array,
                                                        BDSInterpolatorType interpolatorType) const
{
  BDSInterpolator1D* result = nullptr;
  switch (interpolatorType.underlying())
    {
    case BDSInterpolatorType::nearest1d:
      {result = new BDSInterpolator1DNearest(array); break;}
    case BDSInterpolatorType::linear1d:
      {result = new BDSInterpolator1DLinear(array); break;}
    case BDSInterpolatorType::linearmag1d:
      {result = new BDSInterpolator1DLinearMag(array); break;}
    case BDSInterpolatorType::cubic1d:
      {result = new BDSInterpolator1DCubic(array); break;}
    default:
      {throw BDSException(__METHOD_NAME__, "Invalid interpolator type for 1D field: " + interpolatorType.ToString()); break;}
    }
  return result;
}

BDSInterpolator2D* BDSFieldLoader::CreateInterpolator2D(BDSArray2DCoords*   array,
                                                        BDSInterpolatorType interpolatorType) const
{
  BDSInterpolator2D* result = nullptr;
  switch (interpolatorType.underlying())
    {
    case BDSInterpolatorType::nearest2d:
      {result = new BDSInterpolator2DNearest(array); break;}
    case BDSInterpolatorType::linear2d:
      {result = new BDSInterpolator2DLinear(array); break;}
    case BDSInterpolatorType::linearmag2d:
      {result = new BDSInterpolator2DLinearMag(array); break;}
    case BDSInterpolatorType::cubic2d:
      {result = new BDSInterpolator2DCubic(array); break;}
    default:
      {throw BDSException(__METHOD_NAME__, "Invalid interpolator type for 2D field: " + interpolatorType.ToString()); break;}
    }
  return result;
}

BDSInterpolator3D* BDSFieldLoader::CreateInterpolator3D(BDSArray3DCoords*   array,
                                                        BDSInterpolatorType interpolatorType) const
{
  BDSInterpolator3D* result = nullptr;
  switch (interpolatorType.underlying())
    {
    case BDSInterpolatorType::nearest3d:
      {result = new BDSInterpolator3DNearest(array); break;}
    case BDSInterpolatorType::linear3d:
      {result = new BDSInterpolator3DLinear(array); break;}
    case BDSInterpolatorType::linearmag3d:
      {result = new BDSInterpolator3DLinearMag(array); break;}
    case BDSInterpolatorType::cubic3d:
      {result = new BDSInterpolator3DCubic(array); break;}
    default:
      {throw BDSException(__METHOD_NAME__, "Invalid interpolator type for 3D field: " + interpolatorType.ToString()); break;}
    }
  return result;
}

BDSInterpolator4D* BDSFieldLoader::CreateInterpolator4D(BDSArray4DCoords*   array,
                                                        BDSInterpolatorType interpolatorType) const
{
  BDSInterpolator4D* result = nullptr;
  switch (interpolatorType.underlying())
    {
    case BDSInterpolatorType::nearest4d:
      {result = new BDSInterpolator4DNearest(array); break;}
    case BDSInterpolatorType::linear4d:
      {result = new BDSInterpolator4DLinear(array); break;}
    case BDSInterpolatorType::linearmag4d:
      {result = new BDSInterpolator4DLinearMag(array); break;}
    case BDSInterpolatorType::cubic4d:
      {result = new BDSInterpolator4DCubic(array); break;}
    default:
      {throw BDSException(__METHOD_NAME__, "Invalid interpolator type for 4D field: " + interpolatorType.ToString()); break;}
    }
  return result;        
}

void BDSFieldLoader::CreateOperators(const BDSArrayReflectionTypeSet* reflectionTypes,
                                     const BDSArray4DCoords* existingArray,
                                     BDSArrayOperatorIndex*& indexOperator,
                                     BDSArrayOperatorValue*& valueOperator) const
{
  G4String details;
  G4bool problem = BDS::ProblemWithArrayReflectionCombination(*reflectionTypes, &details);
  if (problem)
    {
      G4String msg = "Invalid combination of array transforms.\n";
      msg += details;
      throw BDSException(__METHOD_NAME__, msg); // caught at a higher level to append name of definition
    }
  
  auto arrayInfo = BDSArrayInfo(existingArray);
  
  std::vector<BDSArrayOperatorIndex*> indexOperators;
  std::vector<BDSArrayOperatorValue*> valueOperators;
  
  // input xyzt are w.r.t. spatial dimensions - we want to translate that into
  // whether to operate on the 'x', 'y', 'z', 't' of the array, which may
  // not truly be xyzt (this is a hangover from the initial design). Think of
  // it as xyzt (spatial) -> ijkl (array). e.g. 1D array of spatial z -> only
  // 'x' in the array is filled.
  // .underlying() gives us an int from the enum
  
  // flips are combined into 1 operator
  std::array<G4bool,4> flipIndexOperators = {false, false, false, false};
  for (auto& reflectionType : *reflectionTypes)
    {
      switch (reflectionType.underlying())
        {
        case BDSArrayReflectionType::flipx:
          {flipIndexOperators[existingArray->DimensionIndex(BDSDimensionType::x)] = true; break;}
        case BDSArrayReflectionType::flipy:
          {flipIndexOperators[existingArray->DimensionIndex(BDSDimensionType::y)] = true; break;}
        case BDSArrayReflectionType::flipz:
          {flipIndexOperators[existingArray->DimensionIndex(BDSDimensionType::z)] = true; break;}
        case BDSArrayReflectionType::flipt:
          {flipIndexOperators[existingArray->DimensionIndex(BDSDimensionType::t)] = true; break;}
        default:
          {break;}
        }
    }
  G4bool anyFlipIndexOperators = std::any_of(std::begin(flipIndexOperators), std::end(flipIndexOperators), [](bool i){return i;});
  if (anyFlipIndexOperators)
    {//nIndexOperatorsRequired
      indexOperators.emplace_back(new BDSArrayOperatorIndexFlip(flipIndexOperators));
      valueOperators.emplace_back(new BDSArrayOperatorValueFlip(flipIndexOperators)); // no operation
    }
  
  // basic reflections are combined into 1 operator
  std::array<G4bool,4> reflectIndexOperators = {false, false, false, false};
  for (auto& reflectionType : *reflectionTypes)
    {
      switch (reflectionType.underlying())
        {
        case BDSArrayReflectionType::reflectx:
          {reflectIndexOperators[existingArray->DimensionIndex(BDSDimensionType::x)] = true; break;}
        case BDSArrayReflectionType::reflecty:
          {reflectIndexOperators[existingArray->DimensionIndex(BDSDimensionType::y)] = true; break;}
        case BDSArrayReflectionType::reflectz:
          {reflectIndexOperators[existingArray->DimensionIndex(BDSDimensionType::z)] = true; break;}
        case BDSArrayReflectionType::reflectt:
          {reflectIndexOperators[existingArray->DimensionIndex(BDSDimensionType::t)] = true; break;}
        default:
          {break;}
        }
    }
  G4bool anyReflectIndexOperators = std::any_of(std::begin(reflectIndexOperators), std::end(reflectIndexOperators), [](bool i){return i;});
  if (anyReflectIndexOperators)
    {//nIndexOperatorsRequired
      indexOperators.emplace_back(new BDSArrayOperatorIndexReflect(reflectIndexOperators, arrayInfo));
      valueOperators.emplace_back(new BDSArrayOperatorValueReflect(reflectIndexOperators, arrayInfo));
    }
  
  // special reflections
  for (auto& reflectionType : *reflectionTypes)
    {
      switch (reflectionType.underlying())
        {
        case BDSArrayReflectionType::reflectxydipole:
          {
            indexOperators.emplace_back(new BDSArrayOperatorIndexReflect({true, true, false, false}, arrayInfo));
            valueOperators.emplace_back(new BDSArrayOperatorValueReflectDipoleXY());
            break;
          }
        case BDSArrayReflectionType::reflectxzdipole:
          {
            indexOperators.emplace_back(new BDSArrayOperatorIndexReflect({false, true, false, false}, arrayInfo));
            valueOperators.emplace_back(new BDSArrayOperatorValueReflectDipoleY());
            break;
          }
        case BDSArrayReflectionType::reflectyzdipole:
          {
            indexOperators.emplace_back(new BDSArrayOperatorIndexReflect({true, false, false, false}, arrayInfo));
            valueOperators.emplace_back(new BDSArrayOperatorValueReflect({true, false,  false, false}, arrayInfo));
            break;
          }
        case BDSArrayReflectionType::reflectzsolenoid:
          {
            indexOperators.emplace_back(new BDSArrayOperatorIndexReflect({false, false, true,  false}, arrayInfo));
            valueOperators.emplace_back(new BDSArrayOperatorValueReflectSolenoidZ());
            break;
          }
        case BDSArrayReflectionType::reflectxyquadrupole:
          {
            indexOperators.emplace_back(new BDSArrayOperatorIndexReflect({true, true, false, false}, arrayInfo));
            valueOperators.emplace_back(new BDSArrayOperatorValueReflectQuadrupoleXY());
            break;
          }
        default:
          {break;}
        }
    }
  
  if (indexOperators.size() > 1)
    {
      auto indexResult = new BDSArrayOperatorIndexV();
      for (auto io : indexOperators)
        {indexResult->push_back(io);}
      auto valueResult = new BDSArrayOperatorValueV();
      for (auto vo : valueOperators)
        {valueResult->push_back(vo);}
      indexOperator = indexResult;
      valueOperator = valueResult;
    }
  else
    {
      indexOperator = indexOperators[0];
      valueOperator = valueOperators[0];
    }
  
  ReportIfProblemWithReflection(arrayInfo, indexOperator->OperatesOnXYZT());
  
  G4cout << "Array ( index | value ) operator: (" << indexOperator->Name() << " | " << valueOperator->Name() << ")" << G4endl;
}

void BDSFieldLoader::ReportIfProblemWithReflection(const BDSArrayInfo& info,
                                                   const std::array<G4bool, 4>& operatesOnXYZT,
                                                   G4double tolerance) const
{
  G4String suffix[4] = {"st", "nd", "rd", "th"};
  for (G4int i = 0; i < 4; i++)
    {
      G4double integerPart = 0;
      if ( (std::modf(std::abs(info.zeroPoint[i]), &integerPart) > tolerance) && operatesOnXYZT[i])
        {
          G4String msg = "Array reflection will not work as intended as the axis zero point is not an integer number of \n";
          msg += "array steps from 0 in the " + std::to_string(i + 1) + suffix[i];
          msg += " dimension of the array (tolerance 5% of array step size)";
          BDS::Warning(msg);
        }
    }
}

G4bool BDSFieldLoader::NeedToProvideTransform(const BDSArrayReflectionTypeSet* reflectionTypes) const
{
  if (!reflectionTypes)
    {return false;}
  else
    {
      if (reflectionTypes->empty())
        {return false;}
      else
        {return true;}
    }
}

BDSArray1DCoords* BDSFieldLoader::CreateArrayReflected(BDSArray1DCoords* existingArray,
                                                       const BDSArrayReflectionTypeSet* reflectionTypes) const
{
  if (!NeedToProvideTransform(reflectionTypes))
    {return existingArray;}
  
  BDSArrayOperatorIndex* indexOperator = nullptr;
  BDSArrayOperatorValue* valueOperator = nullptr;
  CreateOperators(reflectionTypes, existingArray, indexOperator, valueOperator);
  BDSArray1DCoords* result = new BDSArray1DCoordsTransformed(existingArray, indexOperator, valueOperator);
  return result;
}

BDSArray2DCoords* BDSFieldLoader::CreateArrayReflected(BDSArray2DCoords* existingArray,
                                                       const BDSArrayReflectionTypeSet* reflectionTypes) const
{
  if (!NeedToProvideTransform(reflectionTypes))
    {return existingArray;}

  BDSArrayOperatorIndex* indexOperator = nullptr;
  BDSArrayOperatorValue* valueOperator = nullptr;
  CreateOperators(reflectionTypes, existingArray, indexOperator, valueOperator);
  BDSArray2DCoords* result = new BDSArray2DCoordsTransformed(existingArray, indexOperator, valueOperator);
  return result;
}

BDSArray3DCoords* BDSFieldLoader::CreateArrayReflected(BDSArray3DCoords* existingArray,
                                                       const BDSArrayReflectionTypeSet* reflectionTypes) const
{
  if (!NeedToProvideTransform(reflectionTypes))
    {return existingArray;}

  BDSArrayOperatorIndex* indexOperator = nullptr;
  BDSArrayOperatorValue* valueOperator = nullptr;
  CreateOperators(reflectionTypes, existingArray, indexOperator, valueOperator);
  BDSArray3DCoords* result = new BDSArray3DCoordsTransformed(existingArray, indexOperator, valueOperator);
  return result;
}

BDSArray4DCoords* BDSFieldLoader::CreateArrayReflected(BDSArray4DCoords* existingArray,
                                                       const BDSArrayReflectionTypeSet* reflectionTypes) const
{
  if (!NeedToProvideTransform(reflectionTypes))
    {return existingArray;}

  BDSArrayOperatorIndex* indexOperator = nullptr;
  BDSArrayOperatorValue* valueOperator = nullptr;
  CreateOperators(reflectionTypes, existingArray, indexOperator, valueOperator);
  BDSArray4DCoords* result = new BDSArray4DCoordsTransformed(existingArray, indexOperator, valueOperator);
  return result;
}

BDSFieldMagInterpolated* BDSFieldLoader::LoadBDSIM1DB(const G4String&      filePath,
                                                      BDSInterpolatorType  interpolatorType,
                                                      const G4Transform3D& transform,
                                                      G4double             bScaling,
                                                      const BDSArrayReflectionTypeSet* reflection)

{
  G4double   bScalingUnits = bScaling * CLHEP::tesla;
  BDSArray1DCoords*  array = LoadBDSIM1D(filePath);
  BDSArray1DCoords* arrayR = CreateArrayReflected(array, reflection);
  BDSInterpolator1D*    ar = CreateInterpolator1D(arrayR, interpolatorType);
  BDSFieldMagInterpolated* result = new BDSFieldMagInterpolated1D(ar, transform, bScalingUnits);
  return result;
}

BDSFieldMagInterpolated* BDSFieldLoader::LoadBDSIM2DB(const G4String&      filePath,
                                                      BDSInterpolatorType  interpolatorType,
                                                      const G4Transform3D& transform,
                                                      G4double             bScaling,
                                                      const BDSArrayReflectionTypeSet* reflection)
{
  G4double   bScalingUnits = bScaling * CLHEP::tesla;
  BDSArray2DCoords*  array = LoadBDSIM2D(filePath);
  BDSArray2DCoords* arrayR = CreateArrayReflected(array, reflection);
  BDSInterpolator2D*    ar = CreateInterpolator2D(arrayR, interpolatorType);
  BDSFieldMagInterpolated* result = new BDSFieldMagInterpolated2D(ar, transform, bScalingUnits);
  return result;
}

BDSFieldMagInterpolated* BDSFieldLoader::LoadBDSIM3DB(const G4String&      filePath,
                                                      BDSInterpolatorType  interpolatorType,
                                                      const G4Transform3D& transform,
                                                      G4double             bScaling,
                                                      const BDSArrayReflectionTypeSet* reflection)
{
  G4double   bScalingUnits = bScaling * CLHEP::tesla;
  BDSArray3DCoords*  array = LoadBDSIM3D(filePath);
  BDSArray3DCoords* arrayR = CreateArrayReflected(array, reflection);
  BDSInterpolator3D*    ar = CreateInterpolator3D(arrayR, interpolatorType);
  BDSFieldMagInterpolated* result = new BDSFieldMagInterpolated3D(ar, transform, bScalingUnits);
  return result;
}

BDSFieldMagInterpolated* BDSFieldLoader::LoadBDSIM4DB(const G4String&      filePath,
                                                      BDSInterpolatorType  interpolatorType,
                                                      const G4Transform3D& transform,
                                                      G4double             bScaling,
                                                      const BDSArrayReflectionTypeSet* reflection)
{
  G4double   bScalingUnits = bScaling * CLHEP::tesla;
  BDSArray4DCoords*  array = LoadBDSIM4D(filePath);
  BDSArray4DCoords* arrayR = CreateArrayReflected(array, reflection);
  BDSInterpolator4D*    ar = CreateInterpolator4D(arrayR, interpolatorType);
  BDSFieldMagInterpolated* result = new BDSFieldMagInterpolated4D(ar, transform, bScalingUnits);
  return result;
}

BDSFieldMagInterpolated* BDSFieldLoader::LoadPoissonSuperFishB(const G4String&      filePath,
                                                               BDSInterpolatorType  interpolatorType,
                                                               const G4Transform3D& transform,
                                                               G4double             bScaling,
                                                               const BDSArrayReflectionTypeSet* reflection)
{
  G4double   bScalingUnits = bScaling * CLHEP::gauss;
  BDSArray2DCoords*  array = LoadPoissonMag2D(filePath);
  BDSArray2DCoords* arrayR = CreateArrayReflected(array, reflection);
  BDSInterpolator2D*    ar = CreateInterpolator2D(arrayR, interpolatorType);
  BDSFieldMagInterpolated* result = new BDSFieldMagInterpolated2D(ar, transform, bScalingUnits);
  return result;
}

BDSFieldMagInterpolated* BDSFieldLoader::LoadPoissonSuperFishBQuad(const G4String&      filePath,
                                                                   BDSInterpolatorType  interpolatorType,
                                                                   const G4Transform3D& transform,
                                                                   G4double             bScaling,
                                                                   const BDSArrayReflectionTypeSet* /*reflection*/)
{
  G4double  bScalingUnits = bScaling * CLHEP::gauss;
  BDSArray2DCoords* array = LoadPoissonMag2D(filePath);
  //BDSArray2DCoords* arrayR = CreateArrayReflected(array, reflection);
  if (std::abs(array->XStep() - array->YStep()) > 1e-9)
    {throw BDSException(__METHOD_NAME__, "asymmetric grid spacing for reflected quadrupole will result in a distorted field map - please regenerate the map with even spatial samples.");}
  BDSArray2DCoordsRQuad* rArray = new BDSArray2DCoordsRQuad(array);
  BDSInterpolator2D*         ar = CreateInterpolator2D(rArray, interpolatorType);
  BDSFieldMagInterpolated* result = new BDSFieldMagInterpolated2D(ar, transform, bScalingUnits);
  return result;
}

BDSFieldMagInterpolated* BDSFieldLoader::LoadPoissonSuperFishBDipole(const G4String&      filePath,
                                                                     BDSInterpolatorType  interpolatorType,
                                                                     const G4Transform3D& transform,
                                                                     G4double             bScaling,
                                                                     const BDSArrayReflectionTypeSet* /*reflection*/)
{
  G4double  bScalingUnits = bScaling * CLHEP::gauss;
  BDSArray2DCoords* array = LoadPoissonMag2D(filePath);
  //BDSArray2DCoords* arrayR = CreateArrayReflected(array, reflection);
  BDSArray2DCoordsRDipole* rArray = new BDSArray2DCoordsRDipole(array);
  BDSInterpolator2D*           ar = CreateInterpolator2D(rArray, interpolatorType);
  BDSFieldMagInterpolated* result = new BDSFieldMagInterpolated2D(ar, transform, bScalingUnits);
  return result;
}

BDSFieldEInterpolated* BDSFieldLoader::LoadBDSIM1DE(const G4String&      filePath,
                                                    BDSInterpolatorType  interpolatorType,
                                                    const G4Transform3D& transform,
                                                    G4double             eScaling,
                                                    const BDSArrayReflectionTypeSet* reflection)
{
  G4double   eScalingUnits = eScaling * CLHEP::volt/CLHEP::m;
  BDSArray1DCoords*  array = LoadBDSIM1D(filePath);
  BDSArray1DCoords* arrayR = CreateArrayReflected(array, reflection);
  BDSInterpolator1D*    ar = CreateInterpolator1D(arrayR, interpolatorType);
  BDSFieldEInterpolated* result = new BDSFieldEInterpolated1D(ar, transform, eScalingUnits);
  return result;
}

BDSFieldEInterpolated* BDSFieldLoader::LoadBDSIM2DE(const G4String&      filePath,
                                                    BDSInterpolatorType  interpolatorType,
                                                    const G4Transform3D& transform,
                                                    G4double             eScaling,
                                                    const BDSArrayReflectionTypeSet* reflection)
{
  G4double   eScalingUnits = eScaling * CLHEP::volt/CLHEP::m;
  BDSArray2DCoords*  array = LoadBDSIM2D(filePath);
  BDSArray2DCoords* arrayR = CreateArrayReflected(array, reflection);
  BDSInterpolator2D*    ar = CreateInterpolator2D(arrayR, interpolatorType);
  BDSFieldEInterpolated* result = new BDSFieldEInterpolated2D(ar, transform, eScalingUnits);
  return result;
}

BDSFieldEInterpolated* BDSFieldLoader::LoadBDSIM3DE(const G4String&      filePath,
                                                    BDSInterpolatorType  interpolatorType,
                                                    const G4Transform3D& transform,
                                                    G4double             eScaling,
                                                    const BDSArrayReflectionTypeSet* reflection)
{
  G4double   eScalingUnits = eScaling * CLHEP::volt/CLHEP::m;
  BDSArray3DCoords*  array = LoadBDSIM3D(filePath);
  BDSArray3DCoords* arrayR = CreateArrayReflected(array, reflection);
  BDSInterpolator3D*    ar = CreateInterpolator3D(arrayR, interpolatorType);
  BDSFieldEInterpolated* result = new BDSFieldEInterpolated3D(ar, transform, eScalingUnits);
  return result;
}

BDSFieldEInterpolated* BDSFieldLoader::LoadBDSIM4DE(const G4String&      filePath,
                                                    BDSInterpolatorType  interpolatorType,
                                                    const G4Transform3D& transform,
                                                    G4double             eScaling,
                                                    const BDSArrayReflectionTypeSet* reflection)
{
  G4double   eScalingUnits = eScaling * CLHEP::volt/CLHEP::m;
  BDSArray4DCoords*  array = LoadBDSIM4D(filePath);
  BDSArray4DCoords* arrayR = CreateArrayReflected(array, reflection);
  BDSInterpolator4D*    ar = CreateInterpolator4D(arrayR, interpolatorType);
  BDSFieldEInterpolated* result = new BDSFieldEInterpolated4D(ar, transform, eScalingUnits);
  return result;
}

BDSFieldEMInterpolated* BDSFieldLoader::LoadBDSIM1DEM(const G4String&      eFilePath,
                                                      const G4String&      bFilePath,
                                                      BDSInterpolatorType  eInterpolatorType,
                                                      BDSInterpolatorType  bInterpolatorType,
                                                      const G4Transform3D& transform,
                                                      G4double             eScaling,
                                                      G4double             bScaling,
                                                      const BDSArrayReflectionTypeSet* eReflection,
                                                      const BDSArrayReflectionTypeSet* bReflection)
{
  G4double    eScalingUnits = eScaling * CLHEP::volt / CLHEP::m;
  G4double    bScalingUnits = bScaling * CLHEP::tesla;
  BDSArray1DCoords* eArray  = LoadBDSIM1D(eFilePath);
  BDSArray1DCoords* bArray  = LoadBDSIM1D(bFilePath);
  BDSArray1DCoords* eArrayR = CreateArrayReflected(eArray, eReflection);
  BDSArray1DCoords* bArrayR = CreateArrayReflected(bArray, bReflection);
  BDSInterpolator1D*   eInt = CreateInterpolator1D(eArrayR, eInterpolatorType);
  BDSInterpolator1D*   bInt = CreateInterpolator1D(bArrayR, bInterpolatorType);
  BDSFieldEMInterpolated* result = new BDSFieldEMInterpolated1D(eInt, bInt, transform,
                                                                eScalingUnits, bScalingUnits);
  return result;
}

BDSFieldEMInterpolated* BDSFieldLoader::LoadBDSIM2DEM(const G4String&      eFilePath,
                                                      const G4String&      bFilePath,
                                                      BDSInterpolatorType  eInterpolatorType,
                                                      BDSInterpolatorType  bInterpolatorType,
                                                      const G4Transform3D& transform,
                                                      G4double             eScaling,
                                                      G4double             bScaling,
                                                      const BDSArrayReflectionTypeSet* eReflection,
                                                      const BDSArrayReflectionTypeSet* bReflection)
{
  G4double    eScalingUnits = eScaling * CLHEP::volt / CLHEP::m;
  G4double    bScalingUnits = bScaling * CLHEP::tesla;
  BDSArray2DCoords*  eArray = LoadBDSIM2D(eFilePath);
  BDSArray2DCoords*  bArray = LoadBDSIM2D(bFilePath);
  BDSArray2DCoords* eArrayR = CreateArrayReflected(eArray, eReflection);
  BDSArray2DCoords* bArrayR = CreateArrayReflected(bArray, bReflection);
  BDSInterpolator2D*   eInt = CreateInterpolator2D(eArrayR, eInterpolatorType);
  BDSInterpolator2D*   bInt = CreateInterpolator2D(bArrayR, bInterpolatorType);
  BDSFieldEMInterpolated* result = new BDSFieldEMInterpolated2D(eInt, bInt, transform,
                                                                eScalingUnits, bScalingUnits);
  return result;
}

BDSFieldEMInterpolated* BDSFieldLoader::LoadBDSIM3DEM(const G4String&      eFilePath,
                                                      const G4String&      bFilePath,
                                                      BDSInterpolatorType  eInterpolatorType,
                                                      BDSInterpolatorType  bInterpolatorType,
                                                      const G4Transform3D& transform,
                                                      G4double             eScaling,
                                                      G4double             bScaling,
                                                      const BDSArrayReflectionTypeSet* eReflection,
                                                      const BDSArrayReflectionTypeSet* bReflection)
{
  G4double    eScalingUnits = eScaling * CLHEP::volt / CLHEP::m;
  G4double    bScalingUnits = bScaling * CLHEP::tesla;
  BDSArray3DCoords*  eArray = LoadBDSIM3D(eFilePath);
  BDSArray3DCoords*  bArray = LoadBDSIM3D(bFilePath);
  BDSArray3DCoords* eArrayR = CreateArrayReflected(eArray, eReflection);
  BDSArray3DCoords* bArrayR = CreateArrayReflected(bArray, bReflection);
  BDSInterpolator3D*   eInt = CreateInterpolator3D(eArrayR, eInterpolatorType);
  BDSInterpolator3D*   bInt = CreateInterpolator3D(bArrayR, bInterpolatorType);
  BDSFieldEMInterpolated* result = new BDSFieldEMInterpolated3D(eInt, bInt, transform,
                                                                eScalingUnits, bScalingUnits);
  return result;
}

BDSFieldEMInterpolated* BDSFieldLoader::LoadBDSIM4DEM(const G4String&      eFilePath,
                                                      const G4String&      bFilePath,
                                                      BDSInterpolatorType  eInterpolatorType,
                                                      BDSInterpolatorType  bInterpolatorType,
                                                      const G4Transform3D& transform,
                                                      G4double             eScaling,
                                                      G4double             bScaling,
                                                      const BDSArrayReflectionTypeSet* eReflection,
                                                      const BDSArrayReflectionTypeSet* bReflection)
{
  G4double    eScalingUnits = eScaling * CLHEP::volt / CLHEP::m;
  G4double    bScalingUnits = bScaling * CLHEP::tesla;
  BDSArray4DCoords*  eArray = LoadBDSIM4D(eFilePath);
  BDSArray4DCoords*  bArray = LoadBDSIM4D(bFilePath);
  BDSArray4DCoords* eArrayR = CreateArrayReflected(eArray, eReflection);
  BDSArray4DCoords* bArrayR = CreateArrayReflected(bArray, bReflection);
  BDSInterpolator4D*   eInt = CreateInterpolator4D(eArrayR, eInterpolatorType);
  BDSInterpolator4D*   bInt = CreateInterpolator4D(bArrayR, bInterpolatorType);
  BDSFieldEMInterpolated* result = new BDSFieldEMInterpolated4D(eInt, bInt, transform,
                                                                eScalingUnits, bScalingUnits);
  return result;
}
