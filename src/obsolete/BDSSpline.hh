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
typedef std::vector<G4double> vDbl;

class BDSSpline
{
public:
  BDSSpline(G4int nIn);
  ~BDSSpline();

  void initialise(vDbl* xIn,G4int xscalIn, 
		  vDbl* yIn,G4int yscalIn);
  G4double integrate(G4double xIn)const;

private:
  vTab tab;
  vDbl u;
  /// nr of points, x/y scale linear (0) or logarithmic (1)
  G4int n,xscal,yscal;
};

#endif
