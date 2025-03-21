/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2024.

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
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSModulatorLinearT.hh"

#include "G4String.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include <string>

BDSModulatorLinearT::BDSModulatorLinearT(G4double T0In,
                                         G4double T1In,
                                         G4double slopeIn,
                                         G4double yInterceptIn):
  T0(T0In),
  T1(T1In),
  slope(slopeIn / CLHEP::s),
  yIntercept(yInterceptIn)
{
  if (T1 < T0)
    {
      G4String msg = "T1 (" + std::to_string(T1) + ") must be greater equal than T0 (";
      msg += std::to_string(T0) + ")";
      throw BDSException(__METHOD_NAME__, msg);
    }
}

G4double BDSModulatorLinearT::Factor(const G4ThreeVector& /*xyz*/,
                                 G4double T) const
{
  return T <= T1 && T >= T0 ? yIntercept + slope * T : 0;
}

G4double BDSModulatorLinearT::RecommendedMaxStepLength() const
{
  G4double dT = T1 - T0;
  return dT / 20;
}
