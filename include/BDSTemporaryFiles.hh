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
#ifndef BDSTEMPORARYFILES_H
#define BDSTEMPORARYFILES_H

#include "G4String.hh"

#include <vector>

/**
 * @brief Holder for all temporary files created by BDSIM.
 * 
 * A single place where all temporary file names are held. The
 * class determines a directory to put them when required and 
 * cleans up all temporary files upon deletion.
 *
 * Has a separate method to initialise the temporary directory
 * so that it's possible to delete the singleton at the end of the
 * program without having to construct the temporary directory
 * and possibly encountering an error.
 * 
 * @author Laurie Nevay
 */

class BDSTemporaryFiles
{
public:
  ~BDSTemporaryFiles();

  /// Singleton accessor.
  static BDSTemporaryFiles* Instance();

  /// Create a temporary file for use in BDSIM. A unique file name will be returned.
  /// It's the caller's responsibility to open this file.
  G4String CreateTemporaryFileUnnamed(const G4String& extension);

  /// Create a temporary file for use in BDSIM based on a currently existing one. A file
  /// with the same name but with the file name prefix and suffix in the temporary
  /// directory will be returned. It's the caller's responsibility to open this file.
  G4String CreateTemporaryFile(const G4String& originalFilePath,
			       G4String        fileNamePrefix = "",
			       G4String        fileNameSuffix = "");

private:
  /// Private default constructor as singleton.
  BDSTemporaryFiles();

  /// Separate function to initialise temporary directory. This way it can
  /// be optionally called in case the temp dir isn't used at all and the singleton
  /// is deleted at the end of the program. No point getting error about temp dir
  /// upon cleaning up something that wasn't used.
  void InitialiseTempDir();

  /// User feedback that new file has been created.
  void WarnOfNewFile(const G4String& newFileName);

  /// Singleton instance.
  static BDSTemporaryFiles* instance;

  G4String              userSpecifiedTemporaryDirectory; ///< Optional user-specified path to try.
  G4String              temporaryDirectory;    ///< Directory all files will be placed in.
  G4bool                temporaryDirectorySet; ///< Whether directory has been set and made.
  std::vector<G4String> allocatedFiles;        ///< Record of of all files allocated.
  G4int                 unNamedFileCount;      ///< Count of unnamed files created.
  G4bool                removeTemporaryFiles;  ///< Whether to clean up.
};

#endif
