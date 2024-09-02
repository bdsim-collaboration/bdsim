#include "BDSCavityInfo.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSFieldEMAxialElectric.hh"
#include "BDSMagnetStrength.hh"
#include "BDSUtilities.hh"

#include "CLHEP/Units/PhysicalConstants.h"
#include "globals.hh"
#include "G4ThreeVector.hh"

#include "TMath.h"

#include <cmath>
#include <utility>


BDSFieldEMAxialElectric::BDSFieldEMAxialElectric(BDSMagnetStrength const* strength):
  BDSFieldEMAxialElectric((*strength)["cavity_efield"],
                       (*strength)["cavity_radius"],
                       (*strength)["cavity_length"],
                       (*strength)["cavity_m"],
                       (*strength)["cavity_n"],
                       (*strength)["cavity_p"],
                       (*strength)["cavity_tphase"],
                       (*strength)["cavity_zphase"],
                       (*strength)["cavity_travelling"],
                       (*strength)["frequency"],
                       (*strength)["synchronousT0"])
{;}

BDSFieldEMAxialElectric::BDSFieldEMAxialElectric(G4double eFieldMaxIn,
                                           G4double radiusIn,
                                           G4double lengthIn,
                                           G4int    mIn,
                                           G4int    nIn,
                                           G4int    pIn,
                                           G4double tphaseIn,
                                           G4double zphaseIn,
                                           G4bool   travellingIn,
                                           G4double frequencyIn,
                                           G4double synchronousTIn):
                                                                      eFieldMax(eFieldMaxIn),
                                                                      radius(radiusIn),
                                                                      length(lengthIn),
                                                                      m(mIn),
                                                                      n(nIn),
                                                                      p(pIn),
                                                                      tphase(tphaseIn),
                                                                      zphase(zphaseIn),
                                                                      travelling(travellingIn),
                                                                      frequency(frequencyIn),
                                                                      synchronousT(synchronousTIn)
{
  //voltage = Voltage();
  //ttFactor = TransitTimeFactor();
}

std::pair<G4ThreeVector, G4ThreeVector> BDSFieldEMAxialElectric::GetField(const G4ThreeVector& position,
                                                                       const G4double       t) const
{
  // Converting from Local Cartesian to Local Cylindrical
  G4double phi = std::atan2(position.y(),position.x());
  G4double r   = std::hypot(position.x(),position.y());
  G4double z   = position.z();

  // divergences @ r=0, so avoid a little
  if(r == 0) {
    r = 1e-15;
  }

  // t phase
  //G4double tmodE = cos(omega*(t - synchronousT) + tphase);
  //G4double tmodB = sin(omega*(t - synchronousT) + tphase);

  G4double Ez = 0;
  G4double Er = 0;
  G4double Et = 0;

  G4double Bz = 0;
  G4double Br = 0;
  G4double Bt = 0;

  // E transform to cartestian
  G4double Ex = Er * cos(phi) - Et * sin(phi);
  G4double Ey = Er * sin(phi) + Et * cos(phi);

  // B transform to cartestian
  G4double Bx = Br * cos(phi) - Bt * sin(phi);
  G4double By = Br * sin(phi) + Bt * cos(phi);

  // Local B and E field vectors
  G4ThreeVector LocalB = G4ThreeVector(Bx, By, Bz);
  G4ThreeVector LocalE = G4ThreeVector(Ex, Ey, Ez);

  // G4cout << "field> " << LocalB << " " << LocalE << G4endl;
  auto result = std::make_pair(LocalB, LocalE);
  return result;
}

void BDSFieldEMAxialElectric::SetMaxEField(G4double eFieldMaxIn) {
  eFieldMax = eFieldMaxIn;
}