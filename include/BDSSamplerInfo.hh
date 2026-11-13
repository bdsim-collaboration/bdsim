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
#ifndef BDSSAMPLERINFO_H
#define BDSSAMPLERINFO_H
#include "BDSSamplerType.hh"

#include "G4String.hh"
#include "G4Types.hh"

class BDSBeamlineElement;

/**
 * @brief All info required to build a sampler but not place it.
 *
 * @author Laurie Nevay
 */

class BDSSamplerInfo
{
public:
  BDSSamplerInfo() = delete;
  BDSSamplerInfo(const G4String& nameIn,
		 BDSSamplerType  samplerTypeIn,
		 G4int           pdgSetIDIn = -1);
  ~BDSSamplerInfo();

  /// Copy constructor.
  BDSSamplerInfo(const BDSSamplerInfo& other) = default;
  /// Assignment operator not used.
  BDSSamplerInfo& operator=(const BDSSamplerInfo&) = delete;

  G4String       name;
  BDSSamplerType samplerType;
  G4int          pdgSetID;
  const BDSBeamlineElement* startElement;
  const BDSBeamlineElement* finishElement;
};

#endif
