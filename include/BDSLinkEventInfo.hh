/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2024.

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
#ifndef BDSLINKEVENTINFO_H
#define BDSLINKEVENTINFO_H

#include "BDSEventInfo.hh"

#include "globals.hh" // geant4 types / globals

/**
 * @brief Simple extension to cache extra variables through an event.
 * 
 * @author Laurie Nevay
 */

class BDSLinkEventInfo: public BDSEventInfo
{
public:
  BDSLinkEventInfo():
    externalParticleIDofPrimary(0),
    externalParentIDofPrimary(0),
    linkSamplerID(-1)
  {;}
  virtual ~BDSLinkEventInfo(){;}

  void Flush() override {info->Flush(); externalParticleIDofPrimary=0; externalParentIDofPrimary=0; linkSamplerID=-1;}

  G4int externalParticleIDofPrimary;
  G4int externalParentIDofPrimary;
  G4int linkSamplerID; ///< Only this output sampler belongs to the selected link element.
};

#endif
