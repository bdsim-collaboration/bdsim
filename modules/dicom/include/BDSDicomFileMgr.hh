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

//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//

#ifndef BDSDICOMFILEMGR_H
#define BDSDICOMFILEMGR_H

#include "globals.hh"
#include "G4PhysicsOrderedFreeVector.hh"
#include "G4PhysicsVector.hh"

#include <map>
#include <vector>

#include "dcmtk/dcmdata/dcfilefo.h"

class BDSDicomVFile;
class BDSDicomFileCT;

//typedef std::multimap<OFString,DicomVFile*> msd;
typedef std::map<G4double, BDSDicomFileCT*> mdct;
enum VerbLevel
  {
   silentVerb  = -1,
   errorVerb   = 0,
   warningVerb = 1,
   infoVerb    = 2,
   debugVerb   = 3,
   testVerb    = 4
  };

/**
 * @brief DICOM File Manager.
 * 
 */

class BDSDicomFileMgr
{
public:
  static BDSDicomFileMgr* GetInstance();
  ~BDSDicomFileMgr(){};
  
  void SetCompression(G4String fComp);
  void AddFile(G4String fComp);
  void AddMaterial(std::vector<G4String> data);
  void AddMaterialDensity(std::vector<G4String> data);
  void AddCT2Density(std::vector<G4String> data);
  
  void Convert(G4String filePath, G4String fFileName);
  void CheckNColumns(std::vector<G4String> wl, size_t vsizeTh);
  void ProcessFiles();
  void CheckCTSlices();
  G4double Hounsfield2density(G4double Hval);
  size_t GetMaterialIndex(G4double Hval);
  size_t GetMaterialIndexByDensity(G4double density);
  void BuildCTMaterials();
  void MergeCTFiles();
  void DumpToTextFile();

  inline G4int    GetCompression()     const {return fCompression;}
  inline G4String GetFileOutName()     const {return theFileOutName;}
  inline G4bool   IsMaterialsDensity() const {return bMaterialsDensity;}

protected:
    G4int fCompression;

private:
  BDSDicomFileMgr();
  
  static BDSDicomFileMgr* theInstance; ///< Singleton instance.

  G4String theFileOutName;
  mdct theCTFiles;
  std::map<G4double, G4String> theMaterials;
  std::map<G4double, G4String> theMaterialsDensity;
  std::map<G4int, G4double> theCT2Density;
  
  G4PhysicsOrderedFreeVector* results;
  
  BDSDicomFileCT* theCTFileAll;
  
public:
  static int verbose;
  G4bool bMaterialsDensity;
};

#endif
