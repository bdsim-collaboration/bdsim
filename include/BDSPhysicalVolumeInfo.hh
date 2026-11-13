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
#ifndef BDSPHYSICALVOLUMEINFO_H
#define BDSPHYSICALVOLUMEINFO_H

#include "G4String.hh"
#include "globals.hh" // geant4 types / globals

class BDSBeamline;

/**
 * @brief A class holding any information pertaining to a particular physical
 * volume in a BDSIM geant4 model.  
 * 
 * Currently, this is used for the name and
 * s position coordinate, but can be extended to store any other pertinent 
 * information.
 * 
 * @author Laurie Nevay
 */

class BDSPhysicalVolumeInfo
{
public:
  explicit BDSPhysicalVolumeInfo(G4double SPosAtMiddleOfElement);
  BDSPhysicalVolumeInfo(G4String nameIn,
			G4String placementNameIn,
			G4double sPosIn,
			G4int    beamlineIndexIn = -1,
			BDSBeamline* beamlineIn = nullptr);
  ~BDSPhysicalVolumeInfo();

  /// output stream
  friend std::ostream& operator<< (std::ostream &out, BDSPhysicalVolumeInfo const &info);

  /// Get the name of the logical volume
  inline G4String GetName() const {return name;}

  /// Set the name of the logical volume
  inline void SetName(G4String NameIn) {name = NameIn;}

  /// Get the name of the logical volume
  inline G4String GetPlacementName() const {return placementName;}

  /// Set the name of the logical volume
  inline void SetPlacementName(G4String placementNameIn)
  {placementName = placementNameIn;}

  /// Get the s position coordinate of the logical volume
  inline G4double GetSPos() const {return spos;}

  /// Set the s position coordinate of the logical volume
  void SetSPos(G4double SPosIn) {spos = SPosIn;}

  /// @{ Accessor.
  inline G4int  GetBeamlineIndex() const {return beamlineIndex;}
  inline BDSBeamline* GetBeamline() const {return beamline;}
  inline BDSBeamline* GetBeamlineMassWorld() const {return beamlineMassWorld;}
  inline G4int        GetBeamlineMassWorldIndex() const {return beamlineMassWorldIndex;}
  /// @}
  
private:
  BDSPhysicalVolumeInfo();

  G4String name;
  G4String placementName;
  G4double spos;
  G4int    beamlineIndex;
  BDSBeamline* beamline;

  /// Corresponding mass world beam line - this may in fact be the same as the beam line.
  BDSBeamline* beamlineMassWorld;
  /// Corresponding mass world beam line index - also may be the same as beamlineIndex.
  G4int        beamlineMassWorldIndex;
};


#endif
