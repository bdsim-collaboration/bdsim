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
#ifndef BDSDICOMFILECT_H
#define BDSDICOMFILECT_H
#include "BDSDebug.hh"
#include "BDSDicomVFile.hh"
#include "BDSWarning.hh"

#include "G4String.hh"
#include "G4ThreeVector.hh"
#include "G4Types.hh"

#include <ostream>
#include <vector>

class DcmDataset;
class BDSDicomFileMgr;

class BDSDicomFileCT: public BDSDicomVFile
{
public:
  BDSDicomFileCT();
  explicit BDSDicomFileCT(DcmDataset* dset);
  virtual ~BDSDicomFileCT(){};

  virtual void ReadData();
  void BuildMaterials();
  void DumpMateIDsToTextFile(std::ofstream& fout);
  void DumpDensitiesToTextFile(std::ofstream& fout);

  void operator+=(const BDSDicomFileCT& rhs);

  /// Add two slices that have the same dimensions, merging them in Z.
  BDSDicomFileCT operator+(const BDSDicomFileCT& rhs);

  void DumpHeaderToTextFile(std::ofstream& fout);

  // Get and set methods
  G4int GetNoVoxelX() const {return fNoVoxelX;};
  G4int GetNoVoxelY() const {return fNoVoxelY;};
  G4int GetNoVoxelZ() const {return fNoVoxelZ;};
  G4int GetNoVoxels() const {return fNoVoxelX * fNoVoxelY * fNoVoxelZ; };

  G4double GetMinX() const {return fMinX; };
  G4double GetMinY() const {return fMinY; };
  G4double GetMinZ() const {return fMinZ; };
  G4double GetMaxX() const {return fMaxX; };
  G4double GetMaxY() const {return fMaxY; };
  G4double GetMaxZ() const {return fMaxZ; };

  std::vector<G4double> GetDensities() const {return fDensities;}
  std::vector<size_t> GetMateIDs() const {return fMateIDs;}

  void SetNoVoxelX(const G4int& val) {fNoVoxelX = val;}
  void SetNoVoxelY(const G4int& val) {fNoVoxelY = val;}
  void SetNoVoxelZ(const G4int& val) {fNoVoxelZ = val;}

  void SetMinX(const G4double& val) {fMinX = val;}
  void SetMaxX(const G4double& val) {fMaxX = val;}
  void SetMinY(const G4double& val) {fMinY = val;}
  void SetMaxY(const G4double& val) {fMaxY = val;}
  void SetMinZ(const G4double& val) {fMinZ = val;}
  void SetMaxZ(const G4double& val) {fMaxZ = val;}

  const G4double& GetLocation() const {return fLocation;}
  
  G4ThreeVector GetOrientationRows()    const {return fOrientationRows;}
  G4ThreeVector GetOrientationColumns() const {return fOrientationColumns;}

protected:
  /// Initialisation of variables.
  void InitialiseVariables();
  
  G4double fLocation;
  G4double fBitAllocated;
  G4double fRescaleSlope;
  G4double fRescaleIntercept;

  G4int fNoVoxelX, fNoVoxelY, fNoVoxelZ;       // number of voxels in each dimensions
  G4double fMinX, fMinY, fMinZ;                // minimum extension of voxels (position of wall)
  G4double fMaxX, fMaxY, fMaxZ;                // maximum extension of voxels (position of wall)
  G4double fVoxelDimX, fVoxelDimY, fVoxelDimZ; // maximum extension of voxels (position of wall)

  G4ThreeVector fOrientationRows;
  G4ThreeVector fOrientationColumns;

  std::vector<int> fHounsfieldV;

  BDSDicomFileMgr* theFileMgr;

private:
  void ReadPixelData();
  void Print(std::ostream& out);

  std::vector<size_t> fMateIDs;
  std::vector<G4double> fDensities;
};


#endif
