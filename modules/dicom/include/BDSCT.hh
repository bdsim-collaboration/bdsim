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
#ifndef BDSCT_H
#define BDSCT_H
#include "BDSAcceleratorComponent.hh"

#include "G4String.hh"
#include "G4Types.hh"

#include <fstream>
#include <map>
#include <set>
#include <vector>

class BDSDicomFileMgr;
class G4Box;
class G4LogicalVolume;
class G4Material;
class G4VPhysicalVolume;

/**
 *  @brief Data structure containing information about a material used in a DICOM scan.
 */
struct matInfo
{
  G4double sumOfDensities; ///< The sum of all densities of all the voxels containing this material
  G4int nVoxels;           ///< The number of voxels that contain this material
  G4int materialID;        ///< This ID associated with this material
};

/**
 * @brief An object based on a DICOM X-ray Computed Tomograph (CT) scan.
 *
 * @author Nathan Farber
 */

class BDSCT: public BDSAcceleratorComponent
{
public:
  BDSCT() = delete; ///< No default constructor.
  BDSCT(const G4String& nameIn,
        const G4String& dicomDataPath,
        const G4String& dicomDataFile);

  virtual ~BDSCT();

  /// Override base class version.
  virtual G4String Material() const override { return "infiniteabsorber"; }

protected:
  /// Call default build then override visualisation attributes.
  virtual void Build() override;

  /// Build a simple box or cylinder.
  virtual void BuildContainerLogicalVolume() override;

  /// Customised user limits.
  virtual void BuildUserLimits() override;

private:
  /// @{ Assignment and copy constructor not implemented nor used
  BDSCT &operator=(const BDSCT &) = delete;
  BDSCT(BDSCT &) = delete;
  /// @}
  
  /// Create the original materials.
  void InitialisationOfMaterials();
  
  /// Read the DICOM files describing the phantom
  void ReadPhantomData();
  
  void ReadVoxelDensities(std::ifstream &fin);
  
  /// Build a new material if the density of the voxel is different to the other voxels.
  G4Material* BuildMaterialWithChangingDensity(const G4Material* origMaterial,
                                               G4float density,
                                               G4String newMaterialName);
  
  
  /// Construct the phantom volumes. This method should be implemented for each of the derived classes.
  void BuildPhantom();

  void SetScorer(G4LogicalVolume* voxel_logic);

  G4Material* fAir;

  // World ...
  G4Box* fWorld_solid;
  G4LogicalVolume* fWorld_logic;
  G4VPhysicalVolume* fWorld_phys;

  G4Box* fContainer_solid;
  G4VPhysicalVolume* fContainer_phys;

  std::vector<G4Material*> fOriginalMaterials; ///< Record of original materials.
  /// Record of new materials created to distinguish different density
  ///  voxels that have the same original materials.
  std::vector<G4Material*> fMaterials;

  size_t* fMateIDs; ///< Index of material of each voxel.
  
  /// Density difference to distinguish material for each original material (by index).
  std::map<G4int, G4double> fDensityDiffs;

  G4int fNVoxelX, fNVoxelY, fNVoxelZ;
  G4double fVoxelHalfDimX, fVoxelHalfDimY, fVoxelHalfDimZ;
  G4double fMinX, fMinY, fMinZ; // minimum extension of voxels (position wall)
  G4double fMaxX, fMaxY, fMaxZ; // maximum extension of voxels (position wall)
  
  /// Map numberOfMaterial to G4Material. They are the list of materials as built from .geom file
  std::map<G4int, G4Material*> thePhantomMaterialsOriginal;

  std::set<G4LogicalVolume*> fScorers;
  G4bool fConstructed;
  BDSDicomFileMgr* theFileMgr;
  G4String dicomDataPath;
  G4String dicomDataFile;
};

#endif
