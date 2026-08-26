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
#ifndef BDSLINKREGISTRY_H
#define BDSLINKREGISTRY_H
#include "G4Transform3D.hh"
#include "G4Types.hh"

#include <map>
#include <string>

class BDSLinkOpaqueBox;

/**
 * @brief Registry / map of components for tracker linkage.
 *  
 * @author Laurie Nevay
 */

class BDSLinkRegistry
{
public:
  BDSLinkRegistry();
  ~BDSLinkRegistry();

  G4int Register(BDSLinkOpaqueBox*    componentIn,
		             const G4Transform3D& inputToGlobalIn,
		             const G4Transform3D& outputToGlobalIn,
                     G4int               samplerIDIn);

  const G4Transform3D& Transform(const std::string& name) const;
  const G4Transform3D& Transform(G4int ID) const;

  const G4Transform3D& TransformInverse(const std::string& name) const;
  const G4Transform3D& TransformInverse(G4int ID) const;

  G4bool NoRotation(const std::string& name) const;
  G4bool NoRotation(G4int ID) const;

  G4double InputClearance(G4int ID) const;

private:

  struct LinkEntry
  {
    G4bool            noRotation;  // true if component straight and no rotation is required
    BDSLinkOpaqueBox* component;
    G4Transform3D     transform;
    G4Transform3D     transformInverse; ///< Global to output reference frame.
    G4double          inputClearance;   ///< Artificial upstream distance to compensate.
    G4int             id;
  };

  std::map<std::string, LinkEntry> byName;
  std::map<G4int,       LinkEntry> byID;
};

#endif
