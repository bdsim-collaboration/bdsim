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
//
/// \file medical/DICOM/src/DicomPhantomZSliceMerged.cc
/// \brief Implementation of the DicomPhantomZSliceMerged class
//
//
// The code was written by :
//      * Jonathan Madsen : jonathan.madsen@cern.ch (12/18/2012)
//
//  Texas A&M University
//  3133 TAMU, Zachry Building
//  College Station, TX 77843, USA
//
//*******************************************************

#include "BDSDicomPhantomParameterisationColour.hh"

#include "globals.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4VisAttributes.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VVisManager.hh"

#include <istream>

BDSDicomPhantomParameterisationColour::BDSDicomPhantomParameterisationColour(const G4String& colourFile):
  G4PhantomParameterisation()
{
  ReadColourData(colourFile);
  SetSkipEqualMaterials(false);
}

BDSDicomPhantomParameterisationColour::~BDSDicomPhantomParameterisationColour()
{;}

void BDSDicomPhantomParameterisationColour::ReadColourData(const G4String& colourFile)
{
  //----- Add a G4VisAttributes for materials not defined in file;
  auto* blankAtt = new G4VisAttributes();
  blankAtt->SetVisibility(false);
  fColours["Default"] = blankAtt;

  //----- Read file
  std::ifstream fin(colourFile.c_str());
  G4int nMate;
  G4String mateName;
  G4double cred, cgreen, cblue, copacity;
  fin >> nMate;
  for (G4int ii = 0; ii < nMate; ii++)
    {
      fin >> mateName;
      if (fin.eof())
	{break;}
      fin >> cred >> cgreen >> cblue >> copacity;
      G4Colour colour(cred, cgreen, cblue, copacity);
      auto* visAtt = new G4VisAttributes(colour);
      visAtt->SetVisibility(true);
      fColours[mateName] = visAtt;
      mColours[ii] = new G4VisAttributes(*visAtt);
    }
}

G4Material* BDSDicomPhantomParameterisationColour::ComputeMaterial(const G4int copyNo,
								   G4VPhysicalVolume* physVol,
								   const G4VTouchable*)
{
  G4Material* mate = G4PhantomParameterisation::ComputeMaterial(copyNo, physVol, nullptr);
  if (G4VVisManager::GetConcreteInstance() && physVol)
    {
      G4String mateName = mate->GetName();
      std::string::size_type iuu = mateName.find("__");
      if (iuu != std::string::npos)
	mateName = mateName.substr(0, iuu);
      
      if (0 < fColours.count(mateName))
	physVol->GetLogicalVolume()->SetVisAttributes(fColours.find(mateName)->second);
      else
        {
	  bool found = false;
	  for (const auto& itr : fColours)
            {
	      G4String mat_color = itr.first;
	      auto len = mat_color.length();
	      if (mateName.find(mat_color) == 0 && mateName.length() > len && mateName[len] == '_')
                {
		  physVol->GetLogicalVolume()->SetVisAttributes(fColours.find(mat_color)->second);
		  found = true;
                }
	      if (found)
		break;
            }
	  if (!found)
            {
	      auto matIndex = G4int(GetMaterialIndex(copyNo));
	      static uintmax_t n = 0;
	      if (n++ < 100)
		G4cout << "Unknown material name " << mateName
		       << " for index " << matIndex << G4endl;
	      if (mColours.find(matIndex) != mColours.end())
		{physVol->GetLogicalVolume()->SetVisAttributes(mColours.find(matIndex)->second);}
	      else
		{physVol->GetLogicalVolume()->SetVisAttributes(fColours.begin()->second);}
            }
        }
    }
  return mate;
}
