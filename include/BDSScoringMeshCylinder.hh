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
#ifndef BDSSCORINGMESHCYLINDER_H
#define BDSSCORINGMESHCYLINDER_H

#include "G4ScoringCylinder.hh"
#include "G4String.hh"
#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"

class BDSHistBinMapper;
class BDSScorerMeshInfo;

/**
 * @brief Wrapper for G4ScoringCylinder to allow full access to placement.
 * 
 * The G4ScoringCylinder class provides an incomplete interface to the
 * underlying translation 3 vector and rotation matrix. Here, we
 * wrap the class to assign these protected members directly.
 * 
 * We have several constructors to retain compatibility with Geant4's
 * delayed construction model for use interactively.
 * 
 * @author Eliott Ramoisiaux
 */

class BDSScoringMeshCylinder: public G4ScoringCylinder
{
public:
  /// Construct with size and divisions from recipe class. Retain full control
  /// over name of mesh.
  BDSScoringMeshCylinder(const G4String&          name,
                    const BDSScorerMeshInfo& recipe,
                    const G4Transform3D&     placementTransform);

  /// Separate constructor with objects that are assigned directly to
  /// protected base class members.
  BDSScoringMeshCylinder(const G4String&         name,
                    const G4ThreeVector&    translation,
                    const G4RotationMatrix& rotation);
  
  const BDSHistBinMapper* Mapper() const;

  virtual ~BDSScoringMeshCylinder();

protected:
  mutable BDSHistBinMapper* mapper;

  G4double scorerSize[3];
  G4int nSegment[3];
  G4int nEnergySegments;
};

#endif
