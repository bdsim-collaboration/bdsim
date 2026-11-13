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
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"
#include "G4PVPlacement.hh"               

#include "BDSMultipole.hh"
#include "BDSSkewSextMagField.hh"

class BDSSkewSextupole :public BDSMultipole
{
  public:
    BDSSkewSextupole(G4String& aName, G4double aLength,
		     G4double bpRad,G4double FeRad,
                     std::list<G4double> blmLocZ, std::list<G4double> blmLocTheta,
                     G4String aTunnelMaterial, G4String aMaterial,		     G4double BDblPrime);
  ~BDSSkewSextupole();

  protected:

  private:
  G4double itsBDblPrime;

  //  void BuildOuterLogicalVolume();
  void BuildBPFieldAndStepper();
  //void BuildMarkerLogicalVolume();

  G4VisAttributes* SetVisAttributes();

  // field related objects:
  BDSSkewSextStepper* itsStepper;
  BDSSkewSextMagField* itsMagField;
  G4Mag_UsualEqRhs* itsEqRhs;
};

#endif
