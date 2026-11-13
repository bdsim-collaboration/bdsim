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
#include "BDSShowerModel.hh"
#include "G4ThreeVector.hh"
#include "GVFlashShowerParameterisation.hh"

BDSShowerModel::BDSShowerModel(G4String modelName, G4Region* envelope):
  GFlashShowerModel(modelName, envelope)
{;}

BDSShowerModel::BDSShowerModel(G4String modelName):
  GFlashShowerModel(modelName)
{;}

G4bool BDSShowerModel::CheckContainment(const G4FastTrack& fastTrack)
{
  G4bool filter=false;
  // track informations
  G4ThreeVector DirectionShower=fastTrack.GetPrimaryTrackLocalDirection();
  G4ThreeVector InitialPositionShower=fastTrack.GetPrimaryTrackLocalPosition();
  
  G4ThreeVector OrthoShower, CrossShower; 
  // Returns orthogonal vector 
  OrthoShower = DirectionShower.orthogonal();
  // Shower in direction perpendicular to OrthoShower and DirectionShower
  CrossShower = DirectionShower.cross(OrthoShower);
  
  G4double  R     = Parameterisation->GetAveR99(); //The shower model is changed here for BDSIM - 99% of the shower shape is considered instead of 90%
  G4double  Z     = Parameterisation->GetAveT99(); //i.e. GetAveR99 etc. instead of GetAveR90 etc. when deciding if the shower is contained or not
  G4int CosPhi[4] = {1,0,-1,0};
  G4int SinPhi[4] = {0,1,0,-1};
  
  G4ThreeVector Position;
  G4int NlateralInside=0;
  // pointer to solid we're in
  G4VSolid *SolidCalo = fastTrack.GetEnvelopeSolid();
  for(int i=0; i<4 ;i++)
    {
      // polar coordinates
      Position = InitialPositionShower + 
	Z*DirectionShower           +
	R*CosPhi[i]*OrthoShower     +
	R*SinPhi[i]*CrossShower     ;
      
      if(SolidCalo->Inside(Position) != kOutside) 
	{NlateralInside++;}
    }
  
  // choose to parameterise or flag when all inetc...
  if(NlateralInside==4)
    {filter=true;}
  // G4cout << " points =   " <<NlateralInside << G4endl;
  return filter;
}
