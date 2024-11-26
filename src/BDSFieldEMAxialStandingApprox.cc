//
// Created by Boogert Stewart on 25/11/2024.
//

#include "CLHEP/Units/PhysicalConstants.h"
#include "BDSFieldEMAxialStandingApprox.hh"
#include "BDSMagnetStrength.hh"

BDSFieldEMAxialStandingApprox::BDSFieldEMAxialStandingApprox(BDSMagnetStrength const* strength):
    BDSFieldEMAxialStandingApprox((*strength)["cavity_voltage"],
                                  (*strength)["cavity_phase"],
                                  (*strength)["cavity_ncell"],
                                  (*strength)["cavity_cellphase"],
                                  (*strength)["cavity_celllength"],
                                  (*strength)["synchronousT0"])
{;}

BDSFieldEMAxialStandingApprox::BDSFieldEMAxialStandingApprox(G4double voltageIn,
                                                             G4double cavityPhaseIn,
                                                             G4int nCellIn,
                                                             G4double cellPhaseAdvanceIn,
                                                             G4double cellLengthIn,
                                                             G4double synchronousTIn) :
                                                             voltage(voltageIn),
                                                             cavityPhase(cavityPhaseIn),
                                                             nCell(nCellIn),
                                                             cellPhaseAdvance(cellPhaseAdvanceIn),
                                                             cellLength(cellLengthIn),
                                                             synchronousT(synchronousTIn) {
  G4int nStep = 200;
  G4double beta = 1;
  cellLength = cellLength*CLHEP::m;
  totalFieldLength = nCell*cellLength;
  frequency = cellPhaseAdvance/(cellLength/CLHEP::c_light)/2./M_PI;
  eFieldAmplitude = 1.0;
  eFieldAmplitude = voltage/this->Voltage(cellLength,nStep);
  transitTime = this->TransitTimeFactor(2*M_PI*frequency,cellLength,beta,nStep);
  zeroes = Zeroes(totalFieldLength,nStep);
  std::cout << "frequency       =" << frequency << std::endl;
  std::cout << "totalFieldLength=" << totalFieldLength << std::endl;
  std::cout << "transitTime     =" << transitTime << std::endl;
  std::cout << "eFieldAmplitude =" << eFieldAmplitude << std::endl;
  std::cout << "Zeroes = ";
  for (const auto& val : zeroes) {
    std::cout << val << " ";
  }
  std::cout << std::endl;
}

G4double BDSFieldEMAxialStandingApprox::GetEz(G4double z, G4double t) const {
  // compute spatial phase
  G4double zphase = z/cellLength*cellPhaseAdvance;

  // compute time phase
  G4double tphase = 2*M_PI*frequency*(t-synchronousT);

  // compute Ez
  G4double Ez = eFieldAmplitude*cos(zphase+tphase);
  return Ez;
}

G4double BDSFieldEMAxialStandingApprox::GetEz_tderiv(G4double z, G4double t) const {
  // compute spatial phase
  G4double zphase = z/cellLength*cellPhaseAdvance;

  // compute time phase
  G4double tphase = 2*M_PI*frequency*(t-synchronousT);

  // compute Ez
  G4double Ez_tderiv = -eFieldAmplitude*sin(zphase+tphase);
  return Ez_tderiv;
}

std::pair<G4ThreeVector, G4ThreeVector> BDSFieldEMAxialStandingApprox::GetField(const G4ThreeVector& position,
                                                                                const G4double       t) const {
  // Converting from Local Cartesian to Local Cylindrical
  G4double phi = std::atan2(position.y(), position.x());
  G4double r = std::hypot(position.x(), position.y());
  G4double z = position.z();

  // compute Ez
  G4double Ez = GetEz(z,t);
  G4double Ez_tderiv = GetEz_tderiv(z,t);

  //std::cout << z << " " << zphase << " " << t << " " << tphase << " " << Ez << std::endl;

  // compute derivatives of Ez for Br and Er
  G4double Er = -1/2. * r * (GetEz(z+0.1,t) - GetEz(z-0.1,t))/0.2;
  G4double Bt = -1/pow(CLHEP::c_light,2) * r/2.0 * (2*M_PI)*frequency * Ez_tderiv;

  G4double Bz = 0;

  // E transform to cartestian
  G4double Ex = Er * cos(phi);
  G4double Ey = Er * sin(phi);

  // B transform to cartestian
  G4double Bx = - Bt * sin(phi);
  G4double By = + Bt * cos(phi);

  // Local B and E field vectors
  G4ThreeVector LocalB = G4ThreeVector(Bx, By, Bz);
  G4ThreeVector LocalE = G4ThreeVector(Ex, Ey, Ez);

  auto result = std::make_pair(LocalB, LocalE);
  return result;
}