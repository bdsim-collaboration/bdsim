//
// Created by Boogert Stewart on 16/08/2024.
//

#ifndef BDSIM_BDSFIELDEMACCELERATION_HH
#define BDSIM_BDSFIELDEMACCELERATION_HH

#include "BDSFieldEM.hh"

class BDSFieldEMAcceleration : public BDSFieldEM
{
public:
  G4double Voltage(G4double length, G4int nSteps = 200);
  G4double TransitTimeFactor(G4double length, G4double omega, G4double beta = 1 , G4int nSteps = 200);

  static const G4double Z0; ///< Impedance of free space.

};

#endif // BDSIM_BDSFIELDEMACCELERATION_HH
