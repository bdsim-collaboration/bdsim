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
#ifndef BDSNAVIGATORPLACEMENTS_H
#define BDSNAVIGATORPLACEMENTS_H

#include "G4AffineTransform.hh"
#include "G4Navigator.hh"
#include "G4ThreeVector.hh"
#include "G4Types.hh"

#include <utility>

class G4VPhysicalVolume;

/**
 * @brief Extra G4Navigator to get coordinate transforms for placement world.
 * 
 * This class provides a simple transform to the frame of a volume found at
 * a point in the provided world as long as it is not the world volume. No
 * special advancing or boundary checking is done - simple yes or no.
 * 
 * This class provides a static auxiliary navigator that each derived 
 * class can use. Making the auxiliary navigator static is not done
 * to reduce memory usage but because navigating from an unknown place 
 * to anywhere in the geometry is much more costly than a relative move
 * in the geometry.
 *
 * See InitialiseTransform() documentation for why we have mutable variables.
 * 
 * @author Laurie Nevay
 */

class BDSNavigatorPlacements
{
public:
  BDSNavigatorPlacements();
  ~BDSNavigatorPlacements();

  /// Setup the navigator w.r.t. to a world volume - typically real world.
  static void AttachWorldVolumeToNavigator(G4VPhysicalVolume* worldPVIn)
  {navigator->SetWorldVolume(worldPVIn); worldPV = worldPVIn;}

  static void ResetNavigatorStates();
  
  /// Locate the point and setup transforms. If the point is not in a volume that's
  /// not the world volume (i.e. a placement volume) then the bool reference variable
  /// will be set to false and a 0,0,0 3 vector returned.
  G4ThreeVector ConvertToLocal(const G4ThreeVector& globalPosition,
                               G4bool& foundAPlacementVolume) const;

  /// Similar to above function but does NOT initialise the transforms.
  G4ThreeVector ConvertToLocalNoSetup(const G4ThreeVector& globalPosition) const;

  /// Convert a vector (axis) from local to global coordinates. NOTE this
  /// function must only be used once the instance of this class has been initialised,
  /// setting up the transforms.  It is up to the developer to ensure this,
  /// otherwise you'll find a bad access.
  G4ThreeVector ConvertAxisToGlobal(const G4ThreeVector& localAxis) const;
  
  /// Convert a vector (axis) from local to global coordinates. Note this function
  /// must only be used once the instance of this class has been initialised, setting
  /// up the transforms. It is up to the developer to ensure this. This utility function
  /// operates on two threevectors in a pair.
  std::pair<G4ThreeVector, G4ThreeVector> ConvertAxisToGlobal(const std::pair<G4ThreeVector, G4ThreeVector>& localAxis) const;

protected:
  mutable G4AffineTransform globalToLocal;
  mutable G4AffineTransform localToGlobal;
  
  /// Navigator object for safe navigation in the real (mass) world without
  /// affecting tracking of the particle.
  static G4Navigator* navigator;

private:
  /// @{ Utility function to select appropriate transform.
  inline const G4AffineTransform& GlobalToLocal() const {return globalToLocal;}
  inline const G4AffineTransform& LocalToGlobal() const {return localToGlobal;}
  /// @}
  
  /// Locate the supplied point the in the geometry and get and store
  /// the transform to that volume in the member variable. This function
  /// has to be const as it's called the first time in GetField which is
  /// a pure virtual const function from G4MagneticField that we have to
  /// implement and have to keep const. This function doesn't change the
  /// const pointer but does change the contents of what it points to.
  G4bool InitialiseTransform(const G4ThreeVector& globalPosition) const;
  
  /// Counter to keep track of when the last instance of the class is deleted
  /// and therefore when the navigators can be safely deleted without affecting
  static G4int numberOfInstances;
  
  /// Cache of world PV to test if we're getting the wrong volume for the transform.
  static G4VPhysicalVolume* worldPV;
};


#endif
