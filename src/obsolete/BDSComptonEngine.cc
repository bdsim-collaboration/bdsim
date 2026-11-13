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
G4double BDSComptonEngine::ComptonDifferentialCrossSection(G4double costh, G4double lgamma){ //lgamma is the lorentz gamma, costheta is the scattering angle
  G4double f1=(1+costh*costh)/(1+lgamma*(1-costh)*(1-costh));
  G4double f2=1+(lgamma*lgamma*(1-costh)*(1-costh))/((1+costh*costh)*(1+lgamma*(1-costh)));
  G4double f=f1*f2;
  return f;
}
 
G4double BDSComptonEngine::PeakAmplitudeOfComptonDifferentialCrossSection(G4double lgamma){
  G4double ndiv=1e6;
  G4double divsize=2/ndiv;
  G4double costh=-1;
  G4double fmax=0;
  G4double f=0;
  for(G4int i=0; i<ndiv; i++){
    f=ComptonDifferentialCrossSection(costh,lgamma);
    if(f>fmax) fmax=f;
    costh+=divsize;
  }
  return fmax;
}


void BDSComptonEngine::PerformHighEnergyCompton2()
{

  // Generate compton event; using method described in
  // H.Burkardt, SL/Note 93-73
  
  G4double phi=CLHEP::twopi * G4UniformRand() ;
  G4double sinphi=sin(phi);
  G4double cosphi=cos(phi);
  
  G4LorentzVector GamInLab, ElInLab;
  G4double x;        
  G4ThreeVector Boost;
  G4double costh, costh2,sinth,sinth2;
  
  Boost = itsIncomingEl.boostVector();
   //   BoostToLab.boost(-Boost);
  G4LorentzRotation BoostToLab(-Boost);
  GamInLab=BoostToLab*(itsIncomingGam);
  ElInLab=BoostToLab*(itsIncomingEl);
  G4ThreeVector LabGammaDir= GamInLab.vect().unit();
  
  G4double beta = (GamInLab.e()-ElInLab.e())/(GamInLab.e()+ElInLab.e());
  G4double lgamma = 1/(sqrt(1-beta*beta));

  G4double crsSecNormFact= PeakAmplitudeOfComptonDifferentialCrossSection(lgamma); //Normalisation factor
  
  G4double r1, r2, result; //Random variable

  do { //Rejection method
    r1=G4UniformRand()*2-1; //Random costh
    r2=G4UniformRand(); //Random number betweeon 0 and 1  
    result = ComptonDifferentialCrossSection(r1,lgamma)/crsSecNormFact; //Result between 0 and 1
  } while (result<r2); //Rejection
  
  costh = r1;
  costh2=costh*costh;
  sinth2=1.-costh2;
  sinth=sqrt(sinth2);

  // x is ratio of scattered to unscattered photon energy:
  x = 1/(1+ GamInLab.e()*(1-costh)/CLHEP::electron_mass_c2);

  G4double Egam = x * GamInLab.e();
  
  // Generate final momenta:
  sinth=sqrt(sinth2);
  itsScatteredGam.setPx(Egam*sinth*cosphi);
  itsScatteredGam.setPy(Egam*sinth*sinphi);
  itsScatteredGam.setPz(Egam*costh);
  itsScatteredGam.setE(Egam);
  
  itsScatteredEl.setPx(-itsScatteredGam.px());
  itsScatteredEl.setPy(-itsScatteredGam.py());
  itsScatteredEl.setPz(GamInLab.e()-itsScatteredGam.pz());
  itsScatteredEl.setE( sqrt( pow(CLHEP::electron_mass_c2,2) +
			     pow(itsScatteredEl.px(),2)+
			     pow(itsScatteredEl.py(),2)+
			     pow(itsScatteredEl.pz(),2) ) );
  
  // Rotates the reference frame from Uz to newUz (unit vector).
  itsScatteredGam.rotateUz(LabGammaDir);
  itsScatteredEl.rotateUz(LabGammaDir);
  
  // now boost back to the original frame:
  G4LorentzRotation BoostFromLab(Boost);
  itsScatteredGam *= BoostFromLab;
  itsScatteredEl *= BoostFromLab;
}




void BDSComptonEngine::PerformHighEnergyCompton()
{

  // Generate high energy compton event
  G4double phi=CLHEP::twopi * G4UniformRand() ;
  G4double sinphi=sin(phi);
  G4double cosphi=cos(phi);
  
  G4LorentzVector GamInCM;
  G4LorentzVector ElInCM;
  G4double costh, costh2,sinth,sinth2;
  
  //Boost the electron and gamma to the cm frame
  G4ThreeVector Boost=(itsIncomingEl+itsIncomingGam)/(itsIncomingEl.e()+itsIncomingGam.e());
  //  G4ThreeVector Boost=itsIncomingEl.boostVector();

  G4LorentzRotation BoostToCM(-Boost);
  GamInCM=BoostToCM*(itsIncomingGam);
  ElInCM=BoostToCM*(itsIncomingEl);
  G4ThreeVector CMGammaDir= GamInCM.vect().unit();
  
  G4double e_cms=GamInCM.e()+ElInCM.e();
  G4double s=pow(e_cms,2); // overrides CLHEP::s
  G4double eps= s*(1e-6 );
  G4double t_max=0;
  G4double t_min=-s+eps;
  G4double w_max = 0.5*(pow((s+t_max)/s,2)+1);
  G4double w=0;
  G4double t=0;
  G4double r1=0;
  G4double r2=0;

#ifdef BDSDEBUG
  G4cout << "e_cms = " << e_cms << ", r2 = " << r2 << G4endl;
  
  G4cout << "Gamma CM E = " << GamInCM.e() << G4endl;
  G4cout << "Gamma CM px = " << GamInCM.px() << G4endl;
  G4cout << "Gamma CM py = " << GamInCM.py() << G4endl;
  G4cout << "Gamma CM pz = " << GamInCM.pz() << G4endl;
  
  G4cout << "e- CM E = " << ElInCM.e() << G4endl;
  G4cout << "e- CM px = " << ElInCM.px() << G4endl;
  G4cout << "e- CM py = " << ElInCM.py() << G4endl;
  G4cout << "e- CM pz = " << ElInCM.pz() << G4endl;
#endif


  do{
    r1=G4UniformRand();
    r2=G4UniformRand();
    t = pow(s+t_max,r1)*pow(s+t_min,1.-r1)-s;
    w = 0.5*(pow((s+t)/s,2)+1);
#ifdef BDSDEBUG
    G4cout << "r1 = " << r1 << ", r2 = " << r2 << G4endl;
    G4cout << "s= " << s << ", t = " << t << ", w = " << w << ", w_max = " << w_max << G4endl;
#endif
  }
  while(r2>(w/w_max));
#ifdef BDSDEBUG
  G4cout << "Finished: t = " << t << ", w = " << w << ", w_max = " << w_max << G4endl;
#endif

  G4double theta_cms = acos(2.*t/s + 1.);

  costh=cos(theta_cms);
  costh2=costh*costh;
  sinth2=1.-costh2;
  sinth=sqrt(sinth2);

  G4double p_trans_x_cms = (e_cms/2)*sinth*cosphi;
  G4double p_trans_y_cms = (e_cms/2)*sinth*sinphi;
  G4double p_long_cms = (e_cms/2)*costh;
  
  //generate final momenta
  itsScatteredGam.setPx(p_trans_x_cms);
  itsScatteredGam.setPy(p_trans_y_cms);
  itsScatteredGam.setPz(p_long_cms);
  itsScatteredGam.setE(e_cms/2);
  //  itsScatteredGam.setE(e_cms/2);

  itsScatteredEl.setPx(-p_trans_x_cms);
  itsScatteredEl.setPy(-p_trans_y_cms);
  itsScatteredEl.setPz(-p_long_cms);
  itsScatteredEl.setE(e_cms/2);
  //  itsScatteredEl.setE(sqrt( pow(CLHEP::electron_mass_c2,2)+pow(its);

  /*
  itsScatteredGam.setPx(p_trans_x_cms);
  itsScatteredGam.setPy(p_trans_y_cms);
  itsScatteredGam.setPz(p_long_cms);
  itsScatteredGam.setE(e_cms);

  itsScatteredEl.setPx(-itsScatteredGam.px());
  itsScatteredEl.setPy(-itsScatteredGam.py());
  itsScatteredEl.setPz(GamInCM.e()-itsScatteredGam.pz());
  itsScatteredEl.setE( sqrt( pow(CLHEP::electron_mass_c2,2) +
			     pow(itsScatteredEl.px(),2)+
			     pow(itsScatteredEl.py(),2)+
			     pow(itsScatteredEl.pz(),2) ) );
  */
  // Rotates the reference frame from Uz to newUz (unit vector).
  itsScatteredGam.rotateUz(CMGammaDir);
  itsScatteredEl.rotateUz(CMGammaDir);
  
  // now boost back to the original frame:
  G4LorentzRotation BoostFromCM(Boost);
  itsScatteredGam *= BoostFromCM;
  itsScatteredEl *= BoostFromCM;
}
