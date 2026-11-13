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
#ifdef USE_ROOT

#include "BDSScoreWriter.hh"

#include "G4MultiFunctionalDetector.hh"
#include "G4VScoringMesh.hh"

#include <map>
#include <fstream>

#ifdef G4ANALYSIS_USE
#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"
#include "TFile.h"
#include "TTree.h"
#endif


BDSScoreWriter::BDSScoreWriter()
	: G4VScoreWriter() {
		;
}

BDSScoreWriter::~BDSScoreWriter() {
	;
}

void BDSScoreWriter::DumpQuantityToFile(const G4String & psName, const G4String & /*fileName*/, const G4String & option)
{
  time_t rawtime;
  struct tm * timeinfo;
  time ( &rawtime );
  timeinfo = localtime ( &rawtime );
  
  strftime (fStartTime,50,"%Y-%m-%d-%H-%M-%S",timeinfo);
  std::ostringstream s;
  s << fFireAngle;
  // use own file name
  G4String fName = fMaterial + "_" + s.str() + "_score_"+ fStartTime + ".root";
  
  TFile* rootFile = new TFile(fName,"RECREATE");
  if(!rootFile) {
    G4cout << " HistoManager::book :" 
	   << " problem creating the ROOT TFile "
	   << G4endl;
    return;
  }
  DumpQuantityToFile(psName, rootFile, option);
  return;
}




void BDSScoreWriter::DumpQuantityToFile(const G4String & psName, TFile* tFile, const G4String & option) {

	//
  if(verboseLevel > 0) {
    G4cout << "User-defined DumpQuantityToFile() method is invoked."
	   << G4endl;
    G4cout << "  -- to obtain a projection of the quantity <"
	   << psName
	   << "> onto the x-y plane --" << G4endl;
  }
  
  // change the option string into lowercase to the case-insensitive.
  G4String opt = option;
  std::transform(opt.begin(), opt.end(), opt.begin(), (int (*)(int))(tolower));
  
  // confirm the option
  if(opt.size() == 0) opt = "";
  TH2D* histo2D[4]; 
  
  // retrieve the map
  MeshScoreMap fSMap = fScoringMesh->GetScoreMap();
  
  
  MeshScoreMap::const_iterator msMapItr = fSMap.find(psName);
  if(msMapItr == fSMap.end()) {
    G4cerr << "ERROR : DumpToFile : Unknown quantity, \""
	   << psName << "\"." << G4endl;
    return;
  }
  std::map<G4int, G4double*> * score = msMapItr->second->GetMap();
  
  G4RotationMatrix nullRotationMatrix;
  
  if(fScoringMesh->GetRotationMatrix() != nullRotationMatrix){
    G4cerr << "BDSScoreWriter::DumpQuantityToFile : Error - cannot handle rotation of scaring mesh. Exiting." << G4endl;
    exit(0);
  }
  
  G4double xMin=fScoringMesh->GetTranslation().x()-fScoringMesh->GetSize().x();
  G4double xMax=fScoringMesh->GetTranslation().x()+fScoringMesh->GetSize().x();

  G4double yMin=fScoringMesh->GetTranslation().y()-fScoringMesh->GetSize().y();
  G4double yMax=fScoringMesh->GetTranslation().y()+fScoringMesh->GetSize().y();

  G4double zMin=fScoringMesh->GetTranslation().z()-fScoringMesh->GetSize().z();
  G4double zMax=fScoringMesh->GetTranslation().z()+fScoringMesh->GetSize().z();

  histo2D[1] = new TH2D("projection_xy",msMapItr->first, fNMeshSegments[0], xMin, xMax, fNMeshSegments[1], yMin, yMax);
  if (!histo2D[1]) G4cout << "\n can't create histo2D" << G4endl;
  for(int x = 0; x < fNMeshSegments[0]; x++) {
    for(int y = 0; y < fNMeshSegments[1]; y++) {
      for(int z = 0; z < fNMeshSegments[2]; z++) {
	G4int idx = GetIndex(x, y, z);
	std::map<G4int, G4double*>::iterator value = score->find(idx);
	if(value != score->end()) 
	  {
	    double val = histo2D[1]->GetBinContent(x+1,y+1);
	    val += (*(value->second));
	    histo2D[1]->SetBinContent(x+1,y+1,val);
	  }
      }
    } 
  } 
  
  histo2D[2] = new TH2D("projection_zy",msMapItr->first, fNMeshSegments[2], zMin, zMax, fNMeshSegments[1], yMin, yMax);
  if (!histo2D[2]) G4cout << "\n can't create histo2D" << G4endl;
  for(int z = 0; z < fNMeshSegments[2]; z++) {
    for(int y = 0; y < fNMeshSegments[1]; y++) {
      for(int x = 0; x < fNMeshSegments[0]; x++) {
	G4int idx = GetIndex(x, y, z);
	std::map<G4int, G4double*>::iterator value = score->find(idx);
	if(value != score->end()) 
	  {
	    double val = histo2D[2]->GetBinContent(z+1,y+1);
	    val += (*(value->second));
	    histo2D[2]->SetBinContent(z+1,y+1,val);
	  }
      }
    } 
  } 
  
  histo2D[3] = new TH2D("projection_zx",msMapItr->first, fNMeshSegments[2], zMin, zMax, fNMeshSegments[0], xMin, xMax);
  if (!histo2D[3]) G4cout << "\n can't create histo2D" << G4endl;
  for(int z = 0; z < fNMeshSegments[2]; z++) {
    for(int x = 0; x < fNMeshSegments[0]; x++) {
      for(int y = 0; y < fNMeshSegments[1]; y++) {
	G4int idx = GetIndex(x, y, z);
	std::map<G4int, G4double*>::iterator value = score->find(idx);
	if(value != score->end()) 
	  {
	    double val = histo2D[3]->GetBinContent(z+1,x+1);
	    val += (*(value->second));
	    histo2D[3]->SetBinContent(z+1,x+1,val);
	  }
      }
    } 
  } 


  TH3D* histo3D  = new TH3D("projection_xyz",msMapItr->first, fNMeshSegments[0], xMin, xMax, fNMeshSegments[1], yMin, yMax, fNMeshSegments[2], zMin, zMax);
  if (!histo3D) G4cout << "\n can't create histo3D" << G4endl;
  for(int x = 0; x < fNMeshSegments[0]; x++) {
    for(int y = 0; y < fNMeshSegments[1]; y++) {
      for(int z = 0; z < fNMeshSegments[2]; z++) {
	G4int idx = GetIndex(x, y, z);
	std::map<G4int, G4double*>::iterator value = score->find(idx);
	if(value != score->end()) 
	  {
	    double val = histo3D->GetBinContent(x+1,y+1,z+1);
	    val += (*(value->second));
	    histo3D->SetBinContent(x+1,y+1,z+1,val);
	  }
      }
    } 
  } 
  
  if (tFile) {
    tFile->Write();       // Writing the histograms to the file
    tFile->Close();        // and closing the tree (and the file)
    G4cout << "\n----> Score File is saved \n" << G4endl;
  }
}

#endif // USE_ROOT
