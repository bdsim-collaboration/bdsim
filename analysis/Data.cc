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
#include "Data.hh"
#include "FileMapper.hh"
#include "Header.hh"

#include "Rtypes.h"
#include "TDirectory.h"
#include "TFile.h"
#include "TTree.h"

#include "BDSOutputROOTEventHeader.hh"

#include <iostream>
#include <map>
#include <string>
#include <vector>

ClassImp(DataDummyClass)

DataDummyClass::DataDummyClass()
{;}

DataDummyClass::~DataDummyClass()
{;}

TFile* DataDummyClass::CreateEmptyRebdsimFile(const std::string& fileName,
					      unsigned long long int nOriginalEventsIn)
{
  return RBDS::CreateEmptyRebdsimFile(fileName, nOriginalEventsIn);
}

TFile* DataDummyClass::CreateEmptyBdskimFile(const std::string& originalFileName,
                                             const std::string& newOutputFileName)
{
  return RBDS::CreateEmptyBdskimFile(originalFileName, newOutputFileName);
}

std::map<std::string, TDirectory*> DataDummyClass::CreateDirectories(TFile* outputFile,
								     std::string treeName)
{
  return RBDS::CreateDirectories(outputFile, treeName);
}

TFile* RBDS::CreateEmptyRebdsimFile(const std::string& fileName,
                                    unsigned long long int nOriginalEventsIn)
{
  TFile* outputFile = new TFile(fileName.c_str(), "RECREATE");
  
  outputFile->cd();
  BDSOutputROOTEventHeader* headerOut = new BDSOutputROOTEventHeader();
  headerOut->Fill(); // updates time stamp
  headerOut->SetFileType("REBDSIM");
  headerOut->nOriginalEvents = nOriginalEventsIn;
  
  TTree* headerTree = new TTree("Header", "REBDSIM Header");
  headerTree->Branch("Header.", "BDSOutputROOTEventHeader", headerOut);
  headerTree->Fill();
  headerTree->Write("", TObject::kOverwrite);
  
  std::vector<std::string> expectedTrees = {"Beam.",
                                            "Event.",
                                            "Run.",
                                            "Options.",
                                            "Model."};
  for (const auto& treeName : expectedTrees)
    {RBDS::CreateDirectories(outputFile, treeName);}
  
  return outputFile;
}

TFile* RBDS::CreateEmptyBdskimFile(const std::string& originalFileName,
                                   const std::string& newOutputFileName)
{
  TFile* original = new TFile(originalFileName.c_str(), "READ");
  if (!RBDS::IsBDSIMOutputFile(originalFileName))
    {
      std::cerr << originalFileName << " is not a BDSIM output file" << std::endl;
      delete original;
      return nullptr;
    }
  return CreateEmptyBdskimFile(original, newOutputFileName);
}

TFile* RBDS::CreateEmptyBdskimFile(TFile* originalFile,
                                   const std::string& newOutputFileName)
{
  TTree* headerTree = dynamic_cast<TTree*>(originalFile->Get("Header")); // should be safe given check we've just done
  if (!headerTree)
    {std::cerr << "Error with header" << std::endl; return nullptr;}

  auto headerLocal = new Header();
  headerLocal->SetBranchAddress(headerTree);
  Long64_t nEntriesHeader = headerTree->GetEntries();
  headerTree->GetEntry(nEntriesHeader - 1); // get the last entry (2nd is more up to date if it exists)
  // We also want to explicitly copy the skim variables that might only be known in the 2nd instance.
  BDSOutputROOTEventHeader* headerOut = new BDSOutputROOTEventHeader(*(headerLocal->header));
  headerOut->skimmedFile = true;

  TFile* output = new TFile(newOutputFileName.c_str(), "RECREATE");
  if (output->IsZombie())
    {std::cerr << "Couldn't open output file " << newOutputFileName << std::endl; return nullptr;}
  output->cd();
  TTree* outputHeaderTree = new TTree("Header", "BDSIM Header");
  outputHeaderTree->Branch("Header.", "BDSOutputROOTEventHeader", headerOut);
  outputHeaderTree->Fill();

  // Setup clones of other trees in the new output file
  std::vector<std::string> treeNames = {"ParticleData", "Beam", "Options", "Model", "Run"};
  for (const auto& tn : treeNames)
  {
    TTree* original = dynamic_cast<TTree*>(originalFile->Get(tn.c_str()));
    if (!original)
      {
        std::cerr << "Failed to load Tree named " << tn << std::endl;
        delete output;
        delete originalFile;
        return nullptr;
      }
    auto clone = original->CloneTree();
    clone->AutoSave();
  }

  return output;
}

std::map<std::string, TDirectory*> RBDS::CreateDirectories(TFile* outputFile,
                                                           std::string treeName)
{
  // treeName typically has a "." at the end, deleting it here:
  std::string cleanedName     = treeName.erase(treeName.size() - 1);
  std::string perEntryDirName = "PerEntryHistograms";
  std::string simpleDirName   = "SimpleHistograms";
  std::string mergedDirName   = "MergedHistograms";
  // We have to pedantically check the existance of directories in the
  // case we're overwriting a file, ROOT will segfault classically if
  // the directory already exists ignoring the overwriting mode.
  // Always more code to compensate for ROOT.
  TDirectory* rebdsimDir = outputFile->GetDirectory(cleanedName.c_str());
  if (!rebdsimDir)
    {rebdsimDir  = outputFile->mkdir(cleanedName.c_str());}
  rebdsimDir->cd();
  
  TDirectory* perEntryDir = rebdsimDir->GetDirectory(perEntryDirName.c_str());
  if (!perEntryDir)
    {rebdsimDir->mkdir(perEntryDirName.c_str());}
  
  TDirectory* simpleDir = rebdsimDir->GetDirectory(simpleDirName.c_str());
  if (!simpleDir)
    {rebdsimDir->mkdir(simpleDirName.c_str());}
  
  TDirectory* mergedDir = rebdsimDir->GetDirectory(mergedDirName.c_str());
  if (!mergedDir)
    {rebdsimDir->mkdir(mergedDirName.c_str());}
  
  std::map<std::string, TDirectory*> result = {{cleanedName, rebdsimDir},
                                               {perEntryDirName, perEntryDir},
                                               {simpleDirName,   simpleDir},
                                               {mergedDirName,   mergedDir}};
  return result;
}
