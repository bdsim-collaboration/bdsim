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
#include "FileMapper.hh"
#include "Header.hh"
#include "HistogramAccumulator.hh"
#include "HistogramAccumulatorMerge.hh"
#include "HistogramAccumulatorSum.hh"

#include "BDSOutputROOTEventHeader.hh"

#include "TDirectory.h"
#include "TFile.h"
#include "TH1.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"
#include "TList.h"
#include "TKey.h"
#include "TObject.h"
#include "TTree.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>


bool RBDS::GetFileType(TFile*       file,
                       std::string& fileType,
                       int*         dataVersion)
{
  // check if valid file at all
  if (file->IsZombie())
    {return false;}

  // load header to get which type of file it is
  TTree* headerTree = dynamic_cast<TTree*>(file->Get("Header"));
  if (!headerTree)
    {return false;} // no header -> definitely not a bdsim file
  Header* headerLocal = new Header();
  headerLocal->SetBranchAddress(headerTree);
  headerTree->GetEntry(0);
  fileType = headerLocal->header->fileType;
  if (dataVersion) // optional
    {(*dataVersion) = headerLocal->header->dataVersion;}
  delete headerLocal;
  return true;
}

bool RBDS::IsBDSIMOutputFile(TFile* file,
                             int* dataVersion)
{
  if (!file)
    {return false;}
  // check if valid file at all
  if (file->IsZombie())
    {return false;}
  
  std::string fileType;
  bool success = GetFileType(file, fileType, dataVersion);
  if (!success)
    {return false;}

  return fileType == "BDSIM";
}

bool RBDS::IsBDSIMOutputFile(const std::string& filePath,
                             int* dataVersion)
{
  TFile* f = new TFile(filePath.c_str());
  bool result = IsBDSIMOutputFile(f, dataVersion);
  f->Close();
  delete f;
  return result;
}

bool RBDS::IsREBDSIMOutputFile(TFile* file)
{
  // check if valid file at all
  if (file->IsZombie())
    {return false;}

  std::string fileType;
  bool success = GetFileType(file, fileType);
  if (!success)
    {return false;}
  
  return fileType == "REBDSIM";
}

bool RBDS::IsREBDSIMOutputFile(const std::string& filePath)
{
  TFile* f = new TFile(filePath.c_str());
  bool result = IsREBDSIMOutputFile(f);
  f->Close();
  delete f;
  return result;
}

bool RBDS::IsREBDSIMOrCombineOutputFile(TFile* file)
{
  // check if valid file at all
  if (file->IsZombie())
    {return false;}

  std::string fileType;
  bool success = GetFileType(file, fileType);
  if (!success)
    {return false;}

  // check if 'REBDSIM' is in the name at all
  // 'REBDSIM' or 'REBDSIMCOMBINE' are possible options
  std::size_t found = fileType.find("REBDSIM");
  return found != std::string::npos;
}

bool RBDS::IsREBDSIMOrCombineOutputFile(const std::string& filePath)
{
  TFile* f = new TFile(filePath.c_str());
  bool result = IsREBDSIMOrCombineOutputFile(f);
  f->Close();
  delete f;
  return result;
}

int RBDS::DetermineDimensionality(TH1* h)
{
  return (int)h->GetDimension();
}

void RBDS::WarningMissingHistogram(const std::string& histName,
                                   const std::string& fileName)
{
  std::cout << "No histogram \"" << histName << "\" in file " << fileName << std::endl;
}

HistogramMap::HistogramMap(TFile* file,
                           TFile* output,
                           bool   debugIn):
  debug(debugIn)
{
  TDirectory* rootDir = static_cast<TDirectory*>(file);

  std::string rootDirName = "";
  MapDirectory(rootDir, output, rootDirName);
}

void HistogramMap::MapDirectory(TDirectory* dir,
                                TFile*      output,
                                const std::string& dirPath)
{
  // to safely return to where we were and not affect other code
  // cache the current directory
  TDirectory* originalDir = TDirectory::CurrentDirectory();
  if (debug)
    {
      gDirectory->pwd();
      std::cout << "Original directory: " << originalDir->GetName() << std::endl;
      std::cout << "Directory: " << dir->GetName() << std::endl;
    }
  dir->cd(); // change into dir

  if (debug)
    {gDirectory->pwd();}

  TList* dirk = dir->GetListOfKeys();
  for (int i = 0; i < dirk->GetEntries(); ++i)
    {
      TObject* keyObject = dirk->At(i); // key object in list of keys
      TObject* dirObject = dir->Get(keyObject->GetName()); // object in file

      std::string objectName = std::string(keyObject->GetName());
      std::string className  = std::string(dirObject->ClassName());

      if (className == "TDirectory" || className == "TDirectoryFile")
        {
          TDirectory* subDir = dynamic_cast<TDirectory*>(dirObject);
          if (!subDir)
            {continue;} // shouldn't happen but protect against bad dynamic cast access
          MapDirectory(subDir, output, dirPath + "/" + objectName); // recursion!
        }
      else if (dirObject->InheritsFrom("TH1") || dirObject->InheritsFrom("BDSBH4DBase"))
        {
          TH1* h = dynamic_cast<TH1*>(dirObject);
          if (!h)
            {continue;} // shouldn't happen but protect against bad dynamic cast access
          int nDim;
          bool BDSBH4Dtype = false;
          
          if (dirObject->InheritsFrom("BDSBH4DBase"))
            {
              nDim = 4;
              BDSBH4Dtype = true;
            }
          else
            {nDim = RBDS::DetermineDimensionality(h);}
          
          if (debug)
            {gDirectory->pwd();}
          
          std::string histPath = dirPath + "/";
          histPath.erase(0,1); // erase leading '/'
          std::string histName  = std::string(h->GetName());
          std::string histTitle = std::string(h->GetTitle());
          TDirectory* outDir = output->GetDirectory(histPath.c_str());
          if (!outDir)
            {output->mkdir(histPath.c_str());} // this returns the parent dir for some stupid reason
          // instead get the directory from the output, knowing it now exists
          outDir = output->GetDirectory(histPath.c_str());
          output->cd(histPath.c_str()); // change into it so new histograms are added to it
          
          // create appropriate type of merge
          HistogramAccumulator* acc = nullptr;
          RBDS::MergeType mergeType = RBDS::DetermineMergeType(dir->GetName());
          switch (mergeType)
            {
            case RBDS::MergeType::meanmerge:
              {
                acc = new HistogramAccumulatorMerge(h, nDim, histName, histTitle);
                break;
              }
            case RBDS::MergeType::sum:
              {
                acc = new HistogramAccumulatorSum(h, nDim, histName, histTitle);
                break;
              }
            case RBDS::MergeType::none:
            default:
              {continue; break;}
            }
          
          RBDS::HistogramPath path = {histPath, histName, acc, outDir, BDSBH4Dtype};
          histograms.push_back(path);
          std::cout << "Found histogram> " << histPath << histName << std::endl;
        }
      else
        {continue;} // don't care about other objects
    }
  originalDir->cd();
}

RBDS::MergeType RBDS::DetermineMergeType(const std::string& parentDir)
{
  if (parentDir == "PerEntryHistograms")
    {return RBDS::MergeType::meanmerge;}
  else if (parentDir == "PerEntryHistogramSets")
    {return RBDS::MergeType::meanmerge;}
  else if (parentDir == "MergedHistograms")
    {return RBDS::MergeType::meanmerge;}
  else if (parentDir == "SimpleHistograms")
    {return RBDS::MergeType::sum;}
  else if (parentDir == "SimpleHistogramSets")
    {return RBDS::MergeType::sum;}
  else
    {return RBDS::MergeType::none;}
}
