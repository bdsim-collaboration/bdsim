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
  kT=CLHEP::k_Boltzmann*Temperature;

  a=1.266;
  b=-0.6;
  c=0.648;

  x1=c;
  x2=(log(c)-a)/b;

  area1 = x1*x1/2;
  area2 = area1 + c*(x2-x1);
  area3=(- exp(a+b*x2)/b);
  TotalArea = area2 + area3;

} 
 
 
BDSPlanckEngine::~BDSPlanckEngine(){}


G4LorentzVector BDSPlanckEngine::PerformPlanck()
{

  // Generate Planck Spectrum photon; using method described by
  // H.Burkardt, SL/Note 93-73

  G4double phi=CLHEP::twopi * G4UniformRand() ;
  G4double sinphi=sin(phi);
  G4double cosphi=cos(phi);

  G4double costh=2.*G4UniformRand()-1.;
  G4double sinth=sqrt(1-costh*costh);

  G4int ntry=0;



 G4double x;
 G4bool repeat=true;

 do {ntry++;
      G4double area=TotalArea*G4UniformRand();
      if(area<=area1)
	{x=sqrt(2.*area);
	if(x*G4UniformRand()<=PlanckSpectrum(x))repeat=false;    
	}
      else
	{ if(area<=area2)
	   {x=x1+ (area-area1)/c ;
	    if(c*G4UniformRand()<=PlanckSpectrum(x))repeat=false;    
	   }
	  else
	   {x= (log((area-TotalArea)*b) - a)/b; 
	    if(exp(a+b*x)*G4UniformRand()<=PlanckSpectrum(x))repeat=false;    
	   }
	}
  }while(ntry<ntryMax && repeat);


 if(ntry==ntryMax)G4Exception("BDSPlanckEngine:Max number of loops exceeded", "-1", FatalException, "");

 G4double Egam=kT*x;

 itsFourMom.setPx(Egam*sinth*cosphi);
 itsFourMom.setPy(Egam*sinth*sinphi);
 itsFourMom.setPz(Egam*costh);
 itsFourMom.setE(Egam);
 
 return itsFourMom;
}
