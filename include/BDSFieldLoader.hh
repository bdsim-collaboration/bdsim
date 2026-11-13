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
#ifndef BDSFIELDLOADER_H
#define BDSFIELDLOADER_H

#include "BDSArrayReflectionType.hh"
#include "BDSInterpolatorType.hh"
#include "G4String.hh"
#include "G4Transform3D.hh"

#include <array>
#include <set>

class BDSArray1DCoords;
class BDSArray2DCoords;
class BDSArray3DCoords;
class BDSArray4DCoords;
class BDSArrayInfo;
class BDSArrayOperatorIndex;
class BDSArrayOperatorValue;
class BDSFieldInfo;
class BDSFieldMag;
class BDSFieldMagInterpolated;
class BDSFieldEInterpolated;
class BDSFieldEMInterpolated;
class BDSInterpolator1D;
class BDSInterpolator2D;
class BDSInterpolator3D;
class BDSInterpolator4D;
class BDSMagnetStrength;

/**
 * @brief A loader for various field map formats.
 *
 * A factory to use the appropriate file loader for different field map formats
 * and construct into required field including the correct interpolator and possible
 * reflections.
 * 
 * This is a singleton as the field loader owns the loaded data arrays and reuses them
 * wrapping them in interpolators multiple times if needed. For this reason there should
 * be only one field loader.
 * 
 * @author Laurie Nevay
 */

class BDSFieldLoader
{
public:
  /// Singleton accessor.
  static BDSFieldLoader* Instance();

  ~BDSFieldLoader();

  void DeleteArrays();

  /// Main interface to load a magnetic field.
  BDSFieldMagInterpolated* LoadMagField(const BDSFieldInfo& info,
					const BDSMagnetStrength* scalingStrength = nullptr,
					const G4String&          scalingKey      = "none");

  /// Main interface to load an electric field.
  BDSFieldEInterpolated*   LoadEField(const BDSFieldInfo& info);

  /// Main interface to load an electro-magnetic field.
  BDSFieldEMInterpolated*  LoadEMField(const BDSFieldInfo& info);

private:
  /// Private default constructor as singleton
  BDSFieldLoader();

  /// Singleton instance
  static BDSFieldLoader* instance;
  
  /// @{ Check file path isn't empty and throw exception if it is.
  static void BFilePathOK(const BDSFieldInfo& info);
  static void EFilePathOK(const BDSFieldInfo& info);
  /// @}

  /// @{ Return the cached array if there is one - may return nullptr.
  BDSArray1DCoords* Get1DCached(const G4String& filePath);
  BDSArray2DCoords* Get2DCached(const G4String& filePath);
  BDSArray3DCoords* Get3DCached(const G4String& filePath);
  BDSArray4DCoords* Get4DCached(const G4String& filePath);
  /// @}

  /// @{ Utility function to use the right templated loader class (gz or normal).
  BDSArray2DCoords* LoadPoissonMag2D(const G4String& filePath);
  BDSArray1DCoords* LoadBDSIM1D(const G4String& filePath);
  BDSArray2DCoords* LoadBDSIM2D(const G4String& filePath);
  BDSArray3DCoords* LoadBDSIM3D(const G4String& filePath);
  BDSArray4DCoords* LoadBDSIM4D(const G4String& filePath);
  /// @}

  /// Create the appropriate array operators (index and value) and assign to the pointers
  /// given by reference. Assumes valid pointer for reflectionTypes argument.
  void CreateOperators(const BDSArrayReflectionTypeSet* reflectionTypes,
		       const BDSArray4DCoords* existingArray,
		       BDSArrayOperatorIndex*& indexOperator,
		       BDSArrayOperatorValue*& valueOperator) const;
  
  /// Print a warning if the array info says the zero-point in array coordinates is
  /// not within the tolerance value. Only do this for dimensions where index operating
  /// is applied as given by operatesOnXYZT.
  void ReportIfProblemWithReflection(const BDSArrayInfo& info,
                                     const std::array<G4bool, 4>& operatesOnXYZT,
                                     G4double tolerance=0.05) const;

  /// Small utility to check the pointer is valid and if it is that it's also not empty.
  /// Returns true only if it's value and not empty.
  G4bool NeedToProvideTransform(const BDSArrayReflectionTypeSet* reflectionTypes) const;
  
  BDSArray1DCoords* CreateArrayReflected(BDSArray1DCoords* existingArray,
                                         const BDSArrayReflectionTypeSet* reflectionType) const;
  BDSArray2DCoords* CreateArrayReflected(BDSArray2DCoords* existingArray,
                                         const BDSArrayReflectionTypeSet* reflectionType) const;
  BDSArray3DCoords* CreateArrayReflected(BDSArray3DCoords* existingArray,
                                         const BDSArrayReflectionTypeSet* reflectionType) const;
  BDSArray4DCoords* CreateArrayReflected(BDSArray4DCoords* existingArray,
                                         const BDSArrayReflectionTypeSet* reflectionType) const;

  /// Create the appropriate 1D interpolator for an array.
  BDSInterpolator1D* CreateInterpolator1D(BDSArray1DCoords*   array,
  					  BDSInterpolatorType interpolatorType) const;
  
  /// Create the appropriate 2D interpolator for an array.
  BDSInterpolator2D* CreateInterpolator2D(BDSArray2DCoords*   array,
  					  BDSInterpolatorType interpolatorType) const;

  /// Create the appropriate 3D interpolator for an array.
  BDSInterpolator3D* CreateInterpolator3D(BDSArray3DCoords*   array,
  					  BDSInterpolatorType interpolatorType) const;

  /// Create the appropriate 4D interpolator for an array.
  BDSInterpolator4D* CreateInterpolator4D(BDSArray4DCoords*   array,
  					  BDSInterpolatorType interpolatorType) const;

  /// Load a 1D BDSIM format magnetic field.
  BDSFieldMagInterpolated* LoadBDSIM1DB(const G4String&      filePath,
					BDSInterpolatorType  interpolatorType,
					const G4Transform3D& transform,
					G4double             bScaling,
					const BDSArrayReflectionTypeSet* reflection = nullptr);
  
  /// Load a 2D BDSIM format magnetic field.
  BDSFieldMagInterpolated* LoadBDSIM2DB(const G4String&      filePath,
					BDSInterpolatorType  interpolatorType,
					const G4Transform3D& transform,
					G4double             bScaling,
                                        const BDSArrayReflectionTypeSet* reflection = nullptr);
  
  /// Load a 3D BDSIM format magnetic field.
  BDSFieldMagInterpolated* LoadBDSIM3DB(const G4String&      filePath,
					BDSInterpolatorType  interpolatorType,
					const G4Transform3D& transform,
					G4double             bScaling,
                                        const BDSArrayReflectionTypeSet* reflection = nullptr);
  
  /// Load a 4D BDSIM format magnetic field.
  BDSFieldMagInterpolated* LoadBDSIM4DB(const G4String&      filePath,
					BDSInterpolatorType  interpolatorType,
					const G4Transform3D& transform,
					G4double             bScaling,
                                        const BDSArrayReflectionTypeSet* reflection = nullptr);
  
  /// Load a 2D poisson superfish B field map.
  BDSFieldMagInterpolated* LoadPoissonSuperFishB(const G4String&      filePath,
						 BDSInterpolatorType  interpolatorType,
						 const G4Transform3D& transform,
						 G4double             bScaling,
                                                 const BDSArrayReflectionTypeSet* reflection = nullptr);
  
  /// Similar to LoadPoissonSuperFishB() but the data below y = x is reflected
  /// and the data relfected from one quadrant to all four at the array level.
  BDSFieldMagInterpolated* LoadPoissonSuperFishBQuad(const G4String&      filePath,
						     BDSInterpolatorType  interpolatorType,
						     const G4Transform3D& transform,
						     G4double             bScaling,
                                                     const BDSArrayReflectionTypeSet* reflection = nullptr);
  
  /// Similar to LoadPoissonSuperFishB() but with appropriate reflections for
  /// a map for the positive quadrant reflected to all quadrants.
  BDSFieldMagInterpolated* LoadPoissonSuperFishBDipole(const G4String&      filePath,
						       BDSInterpolatorType  interpolatorType,
						       const G4Transform3D& transform,
						       G4double             bScaling,
                                                       const BDSArrayReflectionTypeSet* reflection = nullptr);
  
  /// Load a 1D BDSIM format electric field.
  BDSFieldEInterpolated* LoadBDSIM1DE(const G4String&      filePath,
				      BDSInterpolatorType  interpolatorType,
				      const G4Transform3D& transform,
				      G4double             eScaling,
                                      const BDSArrayReflectionTypeSet* reflection = nullptr);
  
  /// Load a 2D BDSIM format electric field.
  BDSFieldEInterpolated* LoadBDSIM2DE(const G4String&      filePath,
				      BDSInterpolatorType  interpolatorType,
				      const G4Transform3D& transform,
				      G4double             eScaling,
                                      const BDSArrayReflectionTypeSet* reflection = nullptr);
  
  /// Load a 3D BDSIM format electric field.
  BDSFieldEInterpolated* LoadBDSIM3DE(const G4String&      filePath,
				      BDSInterpolatorType  interpolatorType,
				      const G4Transform3D& transform,
				      G4double             eScaling,
                                      const BDSArrayReflectionTypeSet* reflection = nullptr);

  /// Load a 4D BDSIM format electric field.
  BDSFieldEInterpolated* LoadBDSIM4DE(const G4String&      filePath,
				      BDSInterpolatorType  interpolatorType,
				      const G4Transform3D& transform,
				      G4double             eScaling,
                                      const BDSArrayReflectionTypeSet* reflection = nullptr);

  /// Load a 1D BDSIM format electro-magnetic field.
  BDSFieldEMInterpolated* LoadBDSIM1DEM(const G4String&      eFilePath,
					const G4String&      bFilePath,
					BDSInterpolatorType  eInterpolatorType,
					BDSInterpolatorType  bInterpolatorType,
					const G4Transform3D& transform,
					G4double             eScaling,
					G4double             bScaling,
                                        const BDSArrayReflectionTypeSet* eReflection = nullptr,
                                        const BDSArrayReflectionTypeSet* bReflection = nullptr);

  /// Load a 2D BDSIM format electro-magnetic field.
  BDSFieldEMInterpolated* LoadBDSIM2DEM(const G4String&      eFilePath,
					const G4String&      bFilePath,
					BDSInterpolatorType  eInterpolatorType,
					BDSInterpolatorType  bInterpolatorType,
					const G4Transform3D& transform,
					G4double             eScaling,
					G4double             bScaling,
                                        const BDSArrayReflectionTypeSet* eReflection = nullptr,
                                        const BDSArrayReflectionTypeSet* bReflection = nullptr);
  
  /// Load a 3D BDSIM format electro-magnetic field.
  BDSFieldEMInterpolated* LoadBDSIM3DEM(const G4String&      eFilePath,
					const G4String&      bFilePath,
					BDSInterpolatorType  eInterpolatorType,
					BDSInterpolatorType  bInterpolatorType,
					const G4Transform3D& transform,
					G4double             eScaling,
					G4double             bScaling,
                                        const BDSArrayReflectionTypeSet* eReflection = nullptr,
                                        const BDSArrayReflectionTypeSet* bReflection = nullptr);

  /// Load a 4D BDSIM format electro-magnetic field.
  BDSFieldEMInterpolated* LoadBDSIM4DEM(const G4String&      eFilePath,
					const G4String&      bFilePath,
					BDSInterpolatorType  eInterpolatorType,
					BDSInterpolatorType  bInterpolatorType,
					const G4Transform3D& transform,
					G4double             eScaling,
					G4double             bScaling,
                                        const BDSArrayReflectionTypeSet* eReflection = nullptr,
                                        const BDSArrayReflectionTypeSet* bReflection = nullptr);

  /// @{ Map of cached field map array.
  std::map<G4String, BDSArray1DCoords*> arrays1d;
  std::map<G4String, BDSArray2DCoords*> arrays2d;
  std::map<G4String, BDSArray3DCoords*> arrays3d;
  std::map<G4String, BDSArray4DCoords*> arrays4d;
  /// @}
};

#endif
