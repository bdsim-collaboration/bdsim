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
#ifndef BDSFIELDMAG_H
#define BDSFIELDMAG_H

#include "globals.hh" // geant4 types / globals
#include "G4MagneticField.hh"
#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"

class BDSModulator;

/**
 * @brief Interface for static magnetic fields that may or may not be local.
 * 
 * Defines abstract interface for all derived magnetic fields. More common
 * to use G4ThreeVectors than arrays so provides common functionality.
 * 
 * @author Laurie Nevay
 */

class BDSFieldMag: public G4MagneticField
{
public:
  /// Constructor takes transform in, which is inverted and applied to positions
  /// before querying the 'pure' derived field object. This allows local offset,
  /// translations, reflections and scalings to be applied if required - ie for
  /// misalignment from the local coordinates.
  BDSFieldMag();
  explicit BDSFieldMag(G4Transform3D transformIn);
  virtual ~BDSFieldMag(){;}

  /// Get the magnetic field vector in local coordinates. The derived class does
  /// not need to apply the transform.
  virtual G4ThreeVector GetField(const G4ThreeVector& position,
				 const G4double       t = 0) const = 0;
  
  /// Each derived class should override this if needs be. Used to warn about
  /// time modulation with a time-varying field.
  virtual G4bool TimeVarying() const {return false;}
  
  /// Implement interface to this class's GetField to fulfill G4MagneticField
  /// inheritance and allow a BDSFieldMag instance to be passed around in the field
  /// factory even if it's not wrapped in a BDSFieldGlobal instance and is in fact
  /// in local coordinates. This uses GetFieldTransformed.
  virtual void GetFieldValue(const G4double point[4],
			     G4double* field) const;
  
  /// Get the field value after applying transform for local offset.
  virtual G4ThreeVector GetFieldTransformed(const G4ThreeVector& position,
					    const G4double       t) const;

  /// Set the transform applied before evaluating the field. This can be used
  /// to account for any difference between the field coordinate system and
  /// the coordinate system of the geometry.  Ie an offset aperture.  This is
  /// a public interface to allow the transform to be set after construction so
  /// that derived classes don't need modified constructors.
  virtual void SetTransform(const G4Transform3D& transformIn);
  
  /// Set the optional modulator.
  void SetModulator(BDSModulator* modulatorIn) {modulator = modulatorIn;}

  /// Accessor.
  inline G4bool FiniteStrength() const {return finiteStrength;}

protected:
  G4bool finiteStrength; ///< Flag to cache whether finite nor not.
  
  /// Transform to apply for the field relative to the local coordinates of the geometry.
  G4Transform3D transform;
  
  G4bool transformIsNotIdentity; ///< Cache of whether to use transform at all.
  BDSModulator* modulator; ///< Optional modulator;

private:
  /// The complimentary transform used to initially rotate the point of query.
  G4Transform3D inverseTransform;
};

#endif
