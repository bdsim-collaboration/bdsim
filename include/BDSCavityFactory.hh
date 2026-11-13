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
#ifndef BDSCAVITYFACTORY_H
#define BDSCAVITYFACTORY_H

#include "BDSCavityType.hh"

#include "globals.hh"
#include "G4String.hh"

class BDSCavity;
class BDSCavityFactoryBase;
class BDSCavityFactoryElliptical;
class BDSCavityFactoryRectangular;
class BDSCavityFactoryPillBox;
class BDSCavityInfo;

class G4Material;

/**
 * @brief Interface to create any RF cavity geometry.
 *
 * @author Laurie Nevay
 */

class BDSCavityFactory
{
public:
  static BDSCavityFactory* Instance(); ///< Singleton accessor.

  /// Reset static singleton instance to nullptr and delete the singleton
  /// factories for different beam pipe styles. Delete them here as they
  /// should only be accessed through this class.
  ~BDSCavityFactory();

  /// Main function to create a piece of cavity geometry.
  BDSCavity* CreateCavity(const G4String&      name,
                          G4double             totalChordLength,
                          const BDSCavityInfo* info,
                          G4Material*          vacuumMaterial);

private:
  BDSCavityFactory(); ///< Private constructor as singleton pattern.

  static BDSCavityFactory* instance; ///< Singleton instance.

  ///@{ Unused default constructors
  BDSCavityFactory(const BDSCavityFactory&) = delete;
  BDSCavityFactory& operator=(const BDSCavityFactory&) = delete;
  ///@}

  BDSCavityFactoryBase* GetAppropriateFactory(const BDSCavityType cavityType);

  /// @{ Point to factory this main interface owns.
  BDSCavityFactoryElliptical*  elliptical;
  BDSCavityFactoryRectangular* rectangular;
  BDSCavityFactoryPillBox*     pillBox;
  /// @}
};

#endif
