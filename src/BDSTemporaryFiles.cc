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
#include "BDSGlobalConstants.hh"
#include "BDSTemporaryFiles.hh"
#include "BDSUtilities.hh"

#include "globals.hh"

#include <cstdlib>
#include <cstdio>
#include <string>
#include <sys/stat.h>
#include <unistd.h>

BDSTemporaryFiles* BDSTemporaryFiles::instance = nullptr;

BDSTemporaryFiles::BDSTemporaryFiles():
  userSpecifiedTemporaryDirectory(""),
  temporaryDirectory(""),
  temporaryDirectorySet(false),
  unNamedFileCount(0)
{
  userSpecifiedTemporaryDirectory = BDSGlobalConstants::Instance()->TemporaryDirectory();
  removeTemporaryFiles = BDSGlobalConstants::Instance()->RemoveTemporaryFiles();
}

void BDSTemporaryFiles::InitialiseTempDir()
{
  // determine temp directory
  std::string bdsimExecDir   = BDS::GetBDSIMExecPath();
  std::string workingDirTemp = bdsimExecDir + "temp/";
  std::vector<G4String> dirsToTry = {"/tmp/", "/temp/", workingDirTemp};

  // replace set of trial paths if one is specified explicitly in the options
  if (!userSpecifiedTemporaryDirectory.empty())
    {
      if (userSpecifiedTemporaryDirectory.back() != '/')
	{userSpecifiedTemporaryDirectory += "/";}
      dirsToTry = {userSpecifiedTemporaryDirectory};
    }
  
  for (const auto& dir : dirsToTry)
    {
      if (BDS::DirectoryExists(dir))
        {// found a dir we can use - create bdsim-specific dir in it
	  // use mkdtemp, which takes template char* (non-const)
	  // use a vector for automatic clearing up with scope
	  G4String templateDir = dir + "bdsim_XXXXXX";
	  std::vector<char> v(templateDir.length() + 1);
	  std::strcpy(&v[0], templateDir.c_str());
	  char* pc = &v[0];
	  
	  char* newTempDirC = mkdtemp(pc);
	  if (!newTempDirC)
            {throw BDSException(__METHOD_NAME__, "Unable to create directory: " + templateDir);}
	  else
            {
	      G4String newTempDir = G4String(newTempDirC);
	      temporaryDirectory = newTempDir;
	      if (temporaryDirectory.back() != '/')
                {temporaryDirectory += "/";}
	      temporaryDirectorySet = true;
            }
	  break;
        }
    }
  
  if (!temporaryDirectorySet) // check it's ok
    {
      G4cerr << __METHOD_NAME__ << "Could not create temporary dir in one of:" << G4endl;
      for (const auto& dir : dirsToTry)
	{G4cerr << dir << G4endl;}
      throw BDSException(__METHOD_NAME__, "Required for operation.");
    }
  else
    {G4cout << __METHOD_NAME__ << "Created temporary directory in: " << temporaryDirectory << G4endl;}
}

BDSTemporaryFiles::~BDSTemporaryFiles()
{
  if (removeTemporaryFiles == false || (allocatedFiles.empty()))
    {
      // no need to warn user about deleting no files.
      instance = nullptr;
      return;
    }

  G4cout << __METHOD_NAME__<< "Removing temporary files" << G4endl;
    
  for (const auto& fn : allocatedFiles)
    {
      if (BDS::FileExists(fn))
        {
#ifdef BDSDEBUG
          G4cout << "Removing \"" << fn << "\"" << G4endl;
#endif
          int result = remove(fn.c_str()); // delete file
          if (result != 0)
#ifdef BDSDEBUG
            {G4cout << "Error deleting file: \"" << fn << "\"" << G4endl;}
#else
          {continue;}
#endif
        }
    }
  // remove directory which should be empty (if not won't be deleted)
  rmdir(temporaryDirectory);

  G4cout << __METHOD_NAME__ << "Temporary files removed" << G4endl;
}

BDSTemporaryFiles* BDSTemporaryFiles::Instance()
{
  if (!instance)
    {instance = new BDSTemporaryFiles();}
  return instance;
}

G4String BDSTemporaryFiles::CreateTemporaryFileUnnamed(const G4String& extension)
{
  if (!temporaryDirectorySet)
    {InitialiseTempDir();}

  G4String newFileName = temporaryDirectory + "bdsTemp_" + std::to_string(unNamedFileCount) + "." + extension;
  unNamedFileCount += 1;

  allocatedFiles.push_back(newFileName);
  WarnOfNewFile(newFileName);
  return newFileName;
}

G4String BDSTemporaryFiles::CreateTemporaryFile(const G4String& originalFilePath,
						G4String        fileNamePrefix,
						G4String        fileNameSuffix)
{
  if (!temporaryDirectorySet)
    {InitialiseTempDir();}
    
  G4String path     = "";
  G4String fileName = "";
  BDS::SplitPathAndFileName(originalFilePath, path, fileName);
  G4String name      = "";
  G4String extension = "";
  BDS::SplitFileAndExtension(fileName, name, extension);

  if (!fileNamePrefix.empty() && (fileNamePrefix.back() != '_'))
    {fileNamePrefix += "_";}       // ensure ends with "_" for padding
  if (!fileNameSuffix.empty() && (fileNameSuffix.front() != '_'))
  {fileNameSuffix = "_" + fileNameSuffix;}// ensure starts with "_" for padding

  G4String newFileName = temporaryDirectory + fileNamePrefix + name + fileNameSuffix + extension;
  
  allocatedFiles.push_back(newFileName);
  WarnOfNewFile(newFileName);
  return newFileName;
}
   
void BDSTemporaryFiles::WarnOfNewFile(const G4String& newFileName)
{
  G4cout << "New temporary file created: " << newFileName << G4endl;
}
