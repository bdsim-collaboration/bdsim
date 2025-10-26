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
#ifndef BDSSCORERMESHINFO_H
#define BDSSCORERMESHINFO_H

#include "BDSBH4DTypeDefs.hh"
#include "BDSExtent.hh"

#include "G4String.hh"
#include "G4Types.hh"

namespace GMAD
{
  class ScorerMesh;
}

/**
 * @brief Recipe class for a scoring mesh.
 * 
 * Convert a parser scorermesh instance to one using Geant4
 * types and units. Check values are set correctly.
 *
 * @author Robin Tesse
 */

class BDSScorerMeshInfo
{
public:
  explicit BDSScorerMeshInfo(const GMAD::ScorerMesh& scorer);

  virtual ~BDSScorerMeshInfo(){;}

  G4String name;
  G4String geometryType;
  G4int    nBinsX;
  G4int    nBinsY;
  G4int    nBinsZ;
  G4int    nBinsR;
  G4int    nBinsPhi;
  G4int    nBinsE;
  G4double xLow;
  G4double xHigh;
  G4double yLow;
  G4double yHigh;
  G4double zLow;
  G4double zHigh;
  G4double rInner;
  G4double rOuter;
  G4double eLow;
  G4double eHigh;
  std::string eScale;
  std::vector<double> eBinsEdges ={};
#ifdef USE_BOOST
  boost_histogram_axes_variant energyAxis;
#endif

  inline G4double  ScoringMeshX() const {return 0.5*(xHigh-xLow);}
  inline G4double  ScoringMeshY() const {return 0.5*(yHigh-yLow);}
  inline G4double  ScoringMeshZ() const {return 0.5*(zHigh-zLow);}
  inline G4double  ScoringMeshR() const {return 0.5*(rHigh-rLow);}
  inline G4double  ScoringMeshE() const {return 0.5*(eHigh-eLow);}
  inline BDSExtent Extent()      const {return extent;}

private:
  BDSScorerMeshInfo() = delete;

  BDSExtent extent;
};

#endif
