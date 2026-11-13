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
		       G4double *Bfield ) const
{
  G4Navigator* SkewSextNavigator=
    G4TransportationManager::GetTransportationManager()->
    GetNavigatorForTracking();

 // gab_dec03>>
  G4ThreeVector LocalR, GlobalR;
  GlobalR.setX(Point[0]);
  GlobalR.setY(Point[1]);
  GlobalR.setZ(Point[2]);
  // LocalR = SkewSextNavigator->GetCurrentLocalCoordinate();
  G4AffineTransform GlobalAffine=SkewSextNavigator->
    GetGlobalToLocalTransform();
  LocalR=GlobalAffine.TransformPoint(GlobalR); 
  // gab_dec03<<


  Bfield[0]=(LocalR.x()*LocalR.x()-LocalR.y()*LocalR.y())*itsBDblePrime/2;
  Bfield[1]=-2*LocalR.x()*LocalR.y()*itsBDblePrime/2;
  Bfield[2]=0;

  // factor of 2 is actually 2-factorial.
}



