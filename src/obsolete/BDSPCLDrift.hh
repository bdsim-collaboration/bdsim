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
class G4VPhysicalVolume;

class BDSPCLDrift :public BDSMultipole
{
public:
  BDSPCLDrift(G4String aName, G4double aLength,
           std::list<G4double> blmLocZ, std::list<G4double> blmLocTheta, 
	      G4double aperX, G4double aperyUp, G4double aperYDown, G4double aperDy,  G4String aTunnelMaterial="", G4double aper=0, G4double tunnelRadius=0., G4double tunnelOffsetX=BDSGlobalConstants::Instance()->GetTunnelOffsetX(), G4String Material="");
  ~BDSPCLDrift();

protected:

private:
  virtual void SetVisAttributes();
  virtual void BuildBLMs();
  virtual void BuildBeampipe();
  /// field related objects
  virtual void BuildBPFieldAndStepper();
  virtual void BuildOuterVolume();

  G4double itsYAperUp, itsYAperDown, itsDyAper;

  G4VSolid* outer_solid;
  G4VSolid* inner_solid;

  G4LogicalVolume* itsOuterBeamPipeLogicalVolume;
  G4LogicalVolume* itsInnerBeamPipeLogicalVolume;
  
  G4VPhysicalVolume* itsPhysiInner;
  G4VPhysicalVolume* itsPhysiOuter;

  G4VisAttributes* itsBeampipeVisAtt;
  G4VisAttributes* itsInnerBeampipeVisAtt;

  //  G4UserLimits* itsBeampipeUserLimits;
  //  G4UserLimits* itsInnerBeampipeUserLimits;
};

#endif
