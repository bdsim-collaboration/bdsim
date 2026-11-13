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
  BDSGen5Shell(G4double InnerRad,G4double OuterRad);
  ~BDSGen5Shell();


  G4double* GenPoint();


private:

  G4double GenSin2();
  G4double GenSin3();
  G4double GenRadius();

  G4double itsVec[5];

  G4double Theta0,Theta1,Theta2,Theta3;

  G4double area2,area3,areaR;
  G4double Gen2Lower,Gen2Upper,Gen3Lower,Gen3Upper;
  G4double GenRLower;
  G4double ShellInnerRadius,ShellOuterRadius;

};


#endif
