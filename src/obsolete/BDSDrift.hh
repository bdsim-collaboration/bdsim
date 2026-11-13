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
	   G4double aLength,
           std::list<G4double> blmLocZ, 
	   std::list<G4double> blmLocTheta, 
	   G4double startAper, 
	   G4double endAper = BDSGlobalConstants::Instance()->GetBeampipeRadius(), 
	   G4String aTunnelMaterial = "", 
	   G4bool aperset = false, 
	   G4double aper = 0, 
	   G4double tunnelOffsetX = BDSGlobalConstants::Instance()->GetTunnelOffsetX(), 
	   G4double phiAngleIn = 0, 
	   G4double phiAngleOut = 0);
  
  BDSDrift(G4String     name,
	   G4double     length,
	   beamPipeInfo beamPipeInfoIn,
	   G4double     boxSize,
	   G4String     outerMaterial="",
	   G4String     tunnelMaterial="",
	   G4double     tunnelRadius=0,
	   G4double     tunnelOffsetX=0);
  ~BDSDrift();

protected:
  virtual void Build();

private:
  virtual void BuildBPFieldAndStepper();
  virtual void BuildBLMs();
  G4double itsStartOuterR;
  G4double itsEndOuterR;
  G4bool itsAperset;
};

#endif
