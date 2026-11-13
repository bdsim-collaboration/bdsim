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
#ifndef BDSHISTBINMAPPER_H
#define BDSHISTBINMAPPER_H

#include "BDSBH4DTypeDefs.hh"
#include "G4ThreeVector.hh"

/**
 * @brief Mapping from axis indices to 1D index.
 * 
 * @author Eliott Ramoisiaux
 */

class BDSHistBinMapper
{
public:
  BDSHistBinMapper(G4int nBinsIIn,
		   G4int nBinsJIn,
		   G4int nBinsKIn,
		   G4int nBinsLIn);
#ifdef USE_BOOST
  BDSHistBinMapper(G4int nBinsIIn,
		   G4int nBinsJIn,
		   G4int nBinsKIn,
		   G4int nBinsLIn,
		   boost_histogram_axes_variant energyAxisIn);
#endif
  
  virtual ~BDSHistBinMapper(){;}
  
  virtual G4int GlobalFromIJKLIndex(G4int iIndex,
				    G4int jIndex,
				    G4int kIndex,
				    G4int lIndex = 0) const;
  
  virtual void IJKLFromGlobal(G4int  globalBin,
			      G4int& iIndex,
			      G4int& jIndex,
			      G4int& kIndex,
			      G4int& lIndex) const;
  
  inline G4double NBinsI() const {return nBinsI;}
  inline G4double NBinsJ() const {return nBinsJ;}
  inline G4double NBinsK() const {return nBinsK;}
  inline G4double NBinsL() const {return nBinsL;}
  
#ifdef USE_BOOST
  inline boost_histogram_axes_variant GetEnergyAxis() const {return energyAxis;}
#endif
  
protected:
  G4int nBinsI;
  G4int nBinsJ;
  G4int nBinsK;
  G4int nBinsL;
#ifdef USE_BOOST
  boost_histogram_axes_variant energyAxis;
#endif
};

#endif
