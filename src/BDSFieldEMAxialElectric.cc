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

const G4double BDSFieldEMAxialElectric::Z0 = CLHEP::mu0 * CLHEP::c_light;

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
  if (! travelling) {
    G4cout << "BDSFieldEMAxialElectric::BDSFieldEMAxialElectric> set frequency=" << frequency << G4endl;
    //kmn = JNsZeros[m][n - 1]/ radius;
    kmn = 0.0;
    kz = p * CLHEP::pi / length;
    omega = sqrt(pow(CLHEP::c_light,2) * (pow(kmn,2) + pow(kz,2)));
    if(frequency ==0) { // if there is a set frequency use that
      frequency = omega/CLHEP::twopi;
      G4cout << "BDSFieldEMAxialElectric::BDSFieldEMAxialElectric> frequency is zero, calculating frequency=" << frequency << G4endl;
    }
  }
  else {
    omega = CLHEP::twopi * frequency;
  }

  voltage = Voltage();
  ttFactor = TransitTimeFactor();
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
  G4double tmodE = cos(omega*(t - synchronousT) + tphase);
  G4double tmodB = sin(omega*(t - synchronousT) + tphase);

  //G4cout << "position,time> " << position << " " << t << " " << tphase << " " << tmodE << " " << tmodB << G4endl;
  //G4cout << t << " " << position << G4endl;
  //G4cout << "time> " << t << " " << synchronousT << " " << tphase << " " << tmodE << " " << tmodB << G4endl;

  //G4double tmod = 1;

  // electric field
  //G4double Ez = tmodE * eFieldMax * BesselJ(m, kmn*r) * cos(m*phi) * cos(p*CLHEP::pi*z/length + zphase);
  //G4double Er = -1.22 *tmodE * p*CLHEP::pi/length * radius/JNsZeros[m][n-1] * eFieldMax * BesselJDeriv(m, kmn*r) * cos(m*phi) * sin(p*CLHEP::pi*z/length + zphase);
  //G4double Et = -1.22 *tmodE * p*CLHEP::pi/length*m*pow(radius,2)/pow(JNsZeros[m][n-1],2)/r * eFieldMax * BesselJ(m, kmn*r) * sin(m*phi) * sin(p*CLHEP::pi*z/length + zphase)  ;

  G4double Ez = 0;
  G4double Er = 0;
  G4double Et = 0;

  // magnetic field
  //G4double Bz = CLHEP::tesla * tmodB * 0;
  //G4double Br = CLHEP::tesla * tmodB * omega*m*pow(radius,2)/pow(JNsZeros[m][n-1],2)/r/pow(CLHEP::c_light,2) * eFieldMax * BesselJ(m,kmn*r) * sin(m*phi) * cos(p*CLHEP::pi*z/length + zphase);
  //G4double Bt = CLHEP::tesla * tmodB * omega*radius/JNsZeros[m][n-1]/pow(CLHEP::c_light,2) * eFieldMax * BesselJDeriv(m, kmn*r) * cos(m*phi) * cos(p*CLHEP::pi*z/length + zphase);

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

G4double BDSFieldEMAxialElectric::GetFrequency() {
  return frequency;
}

void BDSFieldEMAxialElectric::SetMaxEField(G4double eFieldMaxIn) {
  eFieldMax = eFieldMaxIn;
}


G4double BDSFieldEMAxialElectric::Voltage(G4int nSteps)
{
  G4cout << "BDSFieldEMAxialElectric::Gain" << G4endl;

  // save variables for reinstating later
  G4double tphase_old = tphase;
  G4double synchronousT_old = synchronousT;

  // set to approriate values
  tphase = 0.0;
  synchronousT = 0.0;

  std::vector<G4double> zV;
  std::vector<G4double> EzV;

  // compute fields
  for(int i=0;i<=nSteps;i++) {
    G4double z = (i*length)/nSteps - length/2.0;
    auto field = GetField(G4ThreeVector(0,0,z),0);
    zV.push_back(z);
    EzV.push_back(field.second.getZ());
  }

  // simpson's rule
  G4double simpIntegral = 0;
  for(int i=0;i<=nSteps-2;i+=2) {
    simpIntegral += (zV[i+2]-zV[i])/6*(EzV[i] + 4*EzV[i+1] + EzV[i+2]);
  }

  G4cout << "Gain> " << simpIntegral << G4endl;

  // reset variables
  tphase = tphase_old;
  synchronousT = synchronousT_old;

  return simpIntegral;
}

G4double BDSFieldEMAxialElectric::TransitTimeFactor(G4double beta, G4int nSteps)
{
  G4cout << "BDSFieldEMAxialElectric::TransitTimeFactor" << G4endl;

  voltage = Voltage(nSteps);
  // save variables for reinstating later
  G4double tphase_old = tphase;
  G4double synchronousT_old = synchronousT;

  // set to approriate values
  tphase = 0.0;
  synchronousT = 0.0;

  std::vector<G4double> zV;
  std::vector<G4double> EzV;

  // compute fields
  for(int i=0;i<=nSteps;i++) {
    G4double z = (i*length)/nSteps - length/2.0;
    auto field = GetField(G4ThreeVector(0,0,z),0);
    zV.push_back(z);
    EzV.push_back(field.second.getZ()*cos(omega*z/beta/CLHEP::c_light));
  }

  // simpson's rule
  G4double simpIntegral = 0;
  for(int i=0;i<=nSteps-2;i+=2) {
    simpIntegral += (zV[i+2]-zV[i])/6*(EzV[i] + 4*EzV[i+1] + EzV[i+2]);
  }

  G4cout << "TransitTimeFactor> " << simpIntegral/voltage << G4endl;

  // reset variables
  tphase = tphase_old;
  synchronousT = synchronousT_old;

  return simpIntegral/voltage;
}