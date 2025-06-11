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
#ifndef BDSPARTICLECOORDSFULL_H
#define BDSPARTICLECOORDSFULL_H 

#include "BDSParticleCoords.hh"

#include "G4Types.hh"

#include <ostream>

/**
 * @brief A set of particle coordinates including energy and weight.
 * 
 * @author Laurie Nevay
 */

class BDSParticleCoordsFull: public BDSParticleCoords
{
public:
  BDSParticleCoordsFull();
  BDSParticleCoordsFull(G4double xIn,
                        G4double yIn,
                        G4double zIn,
                        G4double xpIn,
                        G4double ypIn,
                        G4double zpIn,
                        G4double tIn,
                        G4double sIn,
                        G4double totalEnergyIn,
                        G4double weightIn);
  BDSParticleCoordsFull(const BDSParticleCoords& localIn,
                        G4double                 sIn,
                        G4double                 totalEnergyIn,
                        G4double                 weightIn);

  virtual ~BDSParticleCoordsFull(){;}
  
  /// Output stream
  friend std::ostream& operator<< (std::ostream& out, BDSParticleCoordsFull const& p);

  /// Actual print out method so it can be called from a derived class.
  virtual void Print(std::ostream& out) const;

  G4double          s;   ///< TODO - remove this. Unused. S (global) is calculated from S0 + z.
  G4double          totalEnergy;
  G4double          weight;
};

#endif
