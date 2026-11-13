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

#ifndef BDSDICOMPHANTOMPARAMETERISATIONCOLOUR_H
#define BDSDICOMPHANTOMPARAMETERISATIONCOLOUR_H

#include <map>

#include "G4PhantomParameterisation.hh"

class G4VisAttributes;

/**
 * @brief DICOM phantom parameterisation with colour.
 * 
 */

class BDSDicomPhantomParameterisationColour: public G4PhantomParameterisation
{
public:
  typedef std::map<G4String, G4VisAttributes*> ColourMap_t;

  static G4String defaultColorFile;

  explicit BDSDicomPhantomParameterisationColour(const G4String& colorFile = defaultColorFile);
  virtual ~BDSDicomPhantomParameterisationColour();

  virtual G4Material* ComputeMaterial(const G4int repNo,
				      G4VPhysicalVolume* currentVol,
				      const G4VTouchable* parentTouch = nullptr);

  const ColourMap_t& GetColourMap() const {return fColours;}
  ColourMap_t&       GetColourMap() {return fColours;}
  
private:
    void ReadColourData(const G4String& colourFile);
    ColourMap_t fColours;
    std::map<G4int, G4VisAttributes*> mColours;
};

#endif
