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

  // generate area of approximate function for sin^2 theta
  area2=1./3. +   2.*(pi/2-1)     + 1./3;

  // generate area of approximate function for sin^3 theta
  area3=1./4. +   2.*(pi/2-1)     + 1./4;

  Gen2Lower=1./3.;
  Gen2Upper=area2-1./3.;
  Gen3Lower=0.25;
  Gen3Upper=area3-1./4.;

  // radial area
  areaR=0;
  GenRLower=0;
  if(ShellInnerRadius<ShellOuterRadius)
    {
      GenRLower=pow(ShellInnerRadius,5)/5.;
      areaR=pow(ShellOuterRadius,5)/5.-GenRLower;
    }   
}

BDSGen5Shell::~BDSGen5Shell()
{
  // Not allocated cannot be deleted SB
  //  delete[] itsVec;
  
}


G4double* BDSGen5Shell::GenPoint()
{

  Theta0=GenSin3();

  Theta1=GenSin2();

  Theta2=acos(2*G4UniformRand()-1);

  Theta3=G4UniformRand()*twopi;

  G4double s0=sin(Theta0);
  G4double c0=cos(Theta0);
  G4double s1=sin(Theta1);
  G4double c1=cos(Theta1);
  G4double s2=sin(Theta2);
  G4double c2=cos(Theta2);
  G4double s3=sin(Theta3);
  G4double c3=cos(Theta3);

  itsVec[0]=c0;
  itsVec[1]=s0*c1;
  itsVec[2]=s0*s1*c2;
  itsVec[3]=s0*s1*s2*c3;
  itsVec[4]=s0*s1*s2*s3;

  G4double R;
  if(ShellInnerRadius==ShellOuterRadius)
    R=ShellInnerRadius;
  else
    R=GenRadius();

  for(G4int i=0;i<5;i++) itsVec[i]*=R;

  return itsVec;
}

G4double BDSGen5Shell::GenSin2()
{
  // generate theta according to a sin^2 distribution
  G4double theta,trial;
  G4bool lok=false;

  //split up the region into three, for more efficient MC:
  do
    {
      trial=area2*G4UniformRand();
      
      if(trial>=Gen2Upper)
	{
	  theta=pi-pow(1-3*(area2-trial),1./3.);
	  if(G4UniformRand()*pow(pi-theta,2)<=pow(sin(theta),2))lok=true;
	}
      else 
	{
	  if (trial>Gen2Lower)
	    {
	      theta=1+trial-1./3.;
	      if(G4UniformRand()<pow(sin(theta),2))lok=true;
	    }
	  else
	    {
	      theta=pow(3*trial,1./3.);
	      if(G4UniformRand()*pow(theta,2)<=pow(sin(theta),2))lok=true;
	    }
	}
    }while(lok==false);

  return theta;
}

G4double BDSGen5Shell::GenSin3()
{
  // generate theta according to a sin^2 distribution
  G4double theta,trial;
  G4bool lok=false;

  //split up the region into three, for more efficient MC:
  do
    {
      trial=area3*G4UniformRand();
      
      if(trial>=Gen3Upper)
	{
	  theta=pi-pow(1-4*(area3-trial),1./4.);
	  if(G4UniformRand()*pow(pi-theta,3)<=pow(sin(theta),3))lok=true;
	}
      else 
	{
	  if (trial>Gen3Lower)
	    {
	      theta=1+trial-1/4.;
	      if(G4UniformRand()<pow(sin(theta),3))lok=true;
	    }
	  else
	    {
	      theta=pow(4*trial,1./4.);
	      if(G4UniformRand()*pow(theta,3)<=pow(sin(theta),3))lok=true;
	    }
	}
    }while(lok==false);

  return theta;
}

G4double BDSGen5Shell::GenRadius()
{
  // generate r^4 dr distribution
  G4double rad;
  G4double trial=areaR*G4UniformRand();
  rad=pow(5.*(GenRLower+trial),1./5.);

  return rad;
}
