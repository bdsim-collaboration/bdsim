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
#ifndef BDSBUNCH_H
#define BDSBUNCH_H

#include "globals.hh"

class BDSBunch;

namespace GMAD {
  class Options;
}

// can become a singleton? - JS

/**
 * @brief A delegator class for the BDSIM particle distribution generator.
 * 
 * The user should instantiate this class only and set the options from the parser.
 * This class generates new particle coordinates for each event based on the 
 * GMAD::Options structure passed from the parser. 
 *
 * @author Stewart Boogert
 */

// can become a singleton? - JS

class BDSBunch
{
protected:
  /// BDSBunch holds the distribution information and provides the next particle
  BDSBunch* bdsBunch;
  
public:
  BDSBunch(); 
  ~BDSBunch();
  /// Copy bunch parameters from parser options.
  void SetOptions(const GMAD::Options& opt);

  /// Interface to supply coordinates for a single particle.
  void GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
		       G4double& xp, G4double& yp, G4double& zp,
		       G4double& t , G4double&  E, G4double& weight);
};

#endif
