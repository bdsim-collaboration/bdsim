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

#ifndef BDSDICOMVFILE_H
#define BDSDICOMVFILE_H

#include "G4String.hh"
#include "G4Types.hh"

#include <vector>

#include "dcmtk/dcmdata/dcfilefo.h"

class DcmDataset;

class BDSDicomVFile
{
public:
  BDSDicomVFile();
  BDSDicomVFile(DcmDataset* dset);
  virtual ~BDSDicomVFile(){};

  inline void SetFileName(const G4String& fileNameIn) {fFileName = fileNameIn;}

protected:
  /// Pure virtual funciton to be implemented in derived class.
  virtual void ReadData() = 0;
  
  virtual std::vector<G4double> Read1Data(DcmDataset* dset,
					  DcmTagKey tagKey,
					  G4int nData);

  DcmDataset* theDataset;
  G4String fFileName;
};

#endif
