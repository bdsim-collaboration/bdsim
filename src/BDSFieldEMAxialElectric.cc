#include  "BDSCavityInfo.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSFieldEMAxialElectric.hh"
#include "BDSMagnetStrength.hh"
#include "BDSUtilities.hh"

#include "CLHEP/Units/PhysicalConstants.h"
#include "globals.hh"
#include "G4ThreeVector.hh"

#include <fstream>
#include "TMath.h"

#include <cmath>
#include <utility>


BDSFieldEMAxialElectric::BDSFieldEMAxialElectric(BDSMagnetStrength const* strength):
BDSFieldEMAxialElectric((*strength)["cavity_efield"],
                       (*strength)["cavity_radius"],
                       (*strength)["cavity_length"],
                       (*strength)["cavity_tphase"],
                       (*strength)["cavity_zphase"],
                       (*strength)["cavity_travelling"],
                       (*strength)["frequency"],
                       (*strength)["synchronousT0"])
{;}

BDSFieldEMAxialElectric::BDSFieldEMAxialElectric(G4double eFieldMaxIn,
                                           G4double radiusIn,
                                           G4double lengthIn,
                                           G4double tphaseIn,
                                           G4double zphaseIn,
                                           G4bool   travellingIn,
                                           G4double frequencyIn,
                                           G4double synchronousTIn):
                                                                      eFieldMax(eFieldMaxIn),
                                                                      radius(radiusIn),
                                                                      length(lengthIn),
                                                                      tphase(tphaseIn),
                                                                      zphase(zphaseIn),
                                                                      travelling(travellingIn),
                                                                      frequency(frequencyIn),
                                                                      synchronousT(synchronousTIn)
{

  std::ifstream ifstr("/Users/lode/Documents/Work/BDSIM/fields/Ez_field_PERLE_5_cell.txt",std::ios::in);
  // make arrays x, Ez

  G4double Ez, z;

  while(ifstr)
  {
    ifstr >> z >> Ez;
    G4cout << "z: " << z << " Ez: " << Ez << G4endl;
    Ez_vector.push_back(Ez);
    z_vector.push_back(z/1000);
  }

  //voltage = Voltage();
  //ttFactor = TransitTimeFactor();
}

std::pair<G4ThreeVector, G4ThreeVector> BDSFieldEMAxialElectric::GetField(const G4ThreeVector& position,
                                                                       const G4double       t) const
{

  G4double Ez_temp = 0;

  if(position.z() < z_vector[0])
  {
    Ez_temp =  Ez_vector[0];
  }

  if(position.z() > z_vector[z_vector.size()-1])
  {
    Ez_temp =  Ez_vector[z_vector.size()-1];
  }

  for (int i = 1; i < z_vector.size(); i++)
  {
    if (position.z() > z_vector[i-1] && position.z() < z_vector[i])
    { // Local B and E field vectors
      // G4cout << "i: " << i << " " << z_vector[i-1] << " " << z_vector[i] << " " <<  position.z() << G4endl;
      Ez_temp = std::max(Ez_vector[i], Ez_vector[i-1]);
      break;
    }
  }

  G4ThreeVector LocalB = G4ThreeVector(0, 0, 0);
  G4ThreeVector LocalE = G4ThreeVector(0, 0, Ez_temp);

  auto result = std::make_pair(LocalB, LocalE);

  return result;
}

void BDSFieldEMAxialElectric::SetMaxEField(G4double eFieldMaxIn) {
  eFieldMax = eFieldMaxIn;
}