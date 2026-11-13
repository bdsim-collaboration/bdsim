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

#include "BDSHistBinMapper.hh"

BDSHistBinMapper::BDSHistBinMapper(G4int nBinsIIn, G4int nBinsJIn, G4int nBinsKIn, G4int nBinsLIn):
  nBinsI(nBinsIIn),
  nBinsJ(nBinsJIn),
  nBinsK(nBinsKIn),
  nBinsL(nBinsLIn)
{;}

#ifdef USE_BOOST
BDSHistBinMapper::BDSHistBinMapper(G4int nBinsIIn, G4int nBinsJIn, G4int nBinsKIn, G4int nBinsLIn,
                                   boost_histogram_axes_variant energyAxisIn):
  nBinsI(nBinsIIn),
  nBinsJ(nBinsJIn),
  nBinsK(nBinsKIn),
  nBinsL(nBinsLIn),
  energyAxis(energyAxisIn)
{
  if (nBinsL != 1) // it means we are in the 4D case
    {nBinsL += 2;} // + 2 to take into account in the mapping the two under and overflow bins
}
#endif

G4int BDSHistBinMapper::GlobalFromIJKLIndex(G4int iIndex,
                                            G4int jIndex,
                                            G4int kIndex,
                                            G4int lIndex) const
{
  return iIndex * nBinsJ * nBinsK * nBinsL + jIndex * nBinsK * nBinsL + kIndex * nBinsL + lIndex;
}

void BDSHistBinMapper::IJKLFromGlobal(G4int globalBin,
                                      G4int& iIndex,
                                      G4int& jIndex,
                                      G4int& kIndex,
                                      G4int& lIndex) const
{
  iIndex = globalBin / (nBinsL * nBinsK * nBinsJ);
  jIndex = (globalBin - iIndex*nBinsL * nBinsK * nBinsJ) / (nBinsL * nBinsK);
  kIndex = (globalBin - jIndex*nBinsL * nBinsK - iIndex * nBinsL * nBinsK * nBinsJ) / nBinsL;
  lIndex = globalBin - kIndex*nBinsL - jIndex * nBinsL * nBinsK - iIndex*nBinsL * nBinsK * nBinsJ;
}
