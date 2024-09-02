#include "BDSFieldEMAcceleration.hh"

const G4double BDSFieldEMAcceleration::Z0 = CLHEP::mu0 * CLHEP::c_light;

G4double BDSFieldEMAcceleration::Voltage(G4double length, G4int nSteps)
{
  G4cout << "BDSFieldEMAcceleration::Voltage" << G4endl;

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

  G4cout << "BDSFieldEMAcceleration::Voltage> " << simpIntegral << G4endl;

  return simpIntegral;
}

G4double BDSFieldEMAcceleration::TransitTimeFactor(G4double omega, G4double length, G4double beta, G4int nSteps)
{
  G4cout << "BDSFieldEMCircularTM::TransitTimeFactor>" << G4endl;

  auto voltage = Voltage(length, nSteps);


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

  G4cout << "BDSFieldEMCircularTM::TransitTimeFactor> " << simpIntegral/voltage << G4endl;

  return simpIntegral/voltage;
}