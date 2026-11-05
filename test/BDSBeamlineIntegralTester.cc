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
#include "BDSBeamlineIntegral.hh"
#include "BDSParticleDefinition.hh"

#include "parser/element.h"
#include "parser/elementtype.h"

#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>
#include <iostream>
#include <string>

/// Check that the beam line integral (arc length and synchronous time) uses
/// the length of the built component for a tapered jcoltip, which is longer
/// than the element length l (the flat jaw face).
G4bool Check(const std::string& label, G4double value, G4double expected)
{
  G4bool ok = std::abs(value - expected) < 1e-9 * std::max(1.0, std::abs(expected));
  std::cout << label << ": " << value << " expected " << expected << (ok ? " OK" : " FAIL") << std::endl;
  return ok;
}

G4bool CheckIntegral(const std::string& label, const GMAD::Element& el, G4double expectedLength)
{
  // 7 GeV electron
  BDSParticleDefinition electron("e-", CLHEP::electron_mass_c2, -1, 7*CLHEP::GeV, 0, 0, 1);
  BDSBeamlineIntegral integral(electron);
  G4double tMiddle = integral.ProvideSynchronousTAtCentreOfNextElement(&el);
  integral.Integrate(el);
  G4double v = electron.Velocity();
  G4bool ok = Check(label + " arc length [mm]", integral.arcLength/CLHEP::mm, expectedLength/CLHEP::mm);
  ok = Check(label + " synchronous T at end [ns]", integral.synchronousTAtEnd/CLHEP::ns, expectedLength/v/CLHEP::ns) && ok;
  ok = Check(label + " synchronous T at centre [ns]", tMiddle/CLHEP::ns, 0.5*expectedLength/v/CLHEP::ns) && ok;
  return ok;
}

int main()
{
  GMAD::Element el;
  el.type = GMAD::ElementType::_JCOLTIP;
  el.name = "col";
  el.l = 0.01; // m
  el.horizontalWidth = 0.08; // m
  el.xsize = 0.01; // m
  el.tipThickness = 0.005; // m

  G4bool ok = CheckIntegral("untapered", el, 10*CLHEP::mm);

  // 10 mm + 2 * 30 mm / tan(12 deg) - as col21 in examples/features/components/jcoltip.gmad
  el.taperAngleDeg = 12;
  ok = CheckIntegral("tapered", el, 10*CLHEP::mm + 2*30*CLHEP::mm/std::tan(12*CLHEP::deg)) && ok;

  // the taper ends at taper_depth
  el.taperDepth = 0.02; // m
  ok = CheckIntegral("tapered with taper_depth", el, 10*CLHEP::mm + 2*20*CLHEP::mm/std::tan(12*CLHEP::deg)) && ok;

  std::cout << (ok ? "All checks passed" : "Some checks failed") << std::endl;
  return ok ? 0 : 1;
}
