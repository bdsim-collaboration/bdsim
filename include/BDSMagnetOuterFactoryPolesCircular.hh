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
#ifndef BDSMAGNETOUTERFACTORYPOLESCIRCULAR_H
#define BDSMAGNETOUTERFACTORYPOLESCIRCULAR_H

#include "BDSMagnetOuterFactoryPolesBase.hh"

/**
 * @brief Factory class for outer volume of magnets. Produces magnets
 * with 2N-poles around the beampipe with a circular yoke.
 * 
 * Most magnets are 2N poles, but sector- and r-bends as well as 
 * muon spoilers, and h/v kickers are unique.
 *
 * @author Laurie Nevay
 */


class BDSMagnetOuterFactoryPolesCircular: public BDSMagnetOuterFactoryPolesBase
{
public:
  BDSMagnetOuterFactoryPolesCircular();
  virtual ~BDSMagnetOuterFactoryPolesCircular(){;}
};

#endif
