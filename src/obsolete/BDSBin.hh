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
#ifndef BDSBIN_H
#define BDSBIN_H

#include "globals.hh" // geant4 globals / types

#include <cmath>
#include <utility>
#include <ostream>

/**
 * @brief A histogram bin.
 * 
 * @author Laurie Nevay
 */

class BDSBin
{
public:
  BDSBin(G4double inXmin, G4double inXmax);
  G4double GetValue()    const {return total;}
  G4double GetError()    const {return std::sqrt(sumWeightsSquared);}
  G4double GetLowerEdge()const {return xmin;}
  G4double GetUpperEdge()const {return xmax;}
  G4double GetLength()   const {return xmax - xmin;}
  std::pair<G4double, G4double> GetXMeanAndTotal() const;
  BDSBin operator+= (const G4double& weight);
  G4bool operator== (const G4double& rhs   ) {return (total == rhs);}
  G4bool operator!= (const G4double& rhs   ) {return !operator == (rhs);}
  void   Empty(){total = 0; sumWeightsSquared = 0;}
  G4bool InRange(G4double x) const;

  /// output stream
  friend std::ostream& operator<< (std::ostream &out, BDSBin const &bin);

private:
  /// Private default constructor to force use of provided one.
  BDSBin();
  
  G4double xmin;
  G4double xmax;
  G4double total;
  G4double sumWeightsSquared;
};


#endif
