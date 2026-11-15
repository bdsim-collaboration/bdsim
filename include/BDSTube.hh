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
#ifndef BDSTUBE_H
#define BDSTUBE_H

#include "G4String.hh"
#include "G4TessellatedSolid.hh"
#include "G4TwoVector.hh"
#include "G4Types.hh"

#include <vector>

class BDSPolygon;

/**
 * @brief 
 *
 * @author Laurie Nevay
 */

class BDSTube: public G4TessellatedSolid
{
public:
  BDSTube(const G4String&   nameIn,
          G4double          fullLengthIn,
          const BDSPolygon& startingShape);

  BDSTube(const G4String&   nameIn,
          G4double          fullLengthIn,
          const BDSPolygon& startingShape,
          const BDSPolygon& finishingShape,
          unsigned int      numberOfSuggestedZSections = 2);
  
  BDSTube(const G4String& nameIn,
          G4double        fullLengthIn,
          const std::vector<G4TwoVector>& startingPoints);

  BDSTube(const G4String&                 nameIn,
          G4double                        fullLengthIn,
          const std::vector<G4TwoVector>& startingPoints,
          const std::vector<G4TwoVector>& finishingPoints,
          unsigned int numberOfSuggestedZSections = 2);

  BDSTube(const G4String& nameIn,
          G4double        dZNegative,
          G4double        dZPositive,
          const std::vector<G4TwoVector>& startingPoints,
          const std::vector<G4TwoVector>& finishingPoints,
          unsigned int    numberOfSuggestedZSections = 2);

  virtual ~BDSTube(){;}

protected:

  /// Construct 2x triangles between each pair of points from start to finish.
  /// Points are linearly interpolated between z positions (minimum 2).
  void RegularConstruction(const std::vector<G4double>&    z,
                           const std::vector<G4TwoVector>& startingPoints,
                           const std::vector<G4TwoVector>& finishingPoints);

  /// No z interpolation but construct with N to M points where M is a*N where
  /// a is an integer. Can be either way around.  E.g. 4 to 8; 12 to 4.
  void SubMultipleConstruction(G4double dZNegative,
                               G4double dZPositive,
                               const std::vector<G4TwoVector>& startingPoints,
                               const std::vector<G4TwoVector>& finishingPoints);
};

#endif
