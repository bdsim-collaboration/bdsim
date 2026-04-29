/*
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway,
University of London 2001 - 2022.

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
#ifndef BDSFIELDEMMUONCOOLER_H
#define BDSFIELDEMMUONCOOLER_H

#include "BDSFieldEM.hh"

#include "G4ThreeVector.hh"
#include "G4Types.hh"

#include <utility>
#include <vector>

class BDSFieldEM;
class BDSFieldInfoExtraMuonCooler;
class BDSFieldMag;

/**
 * @brief A composite RF and B field for a muon cooler.
 *
 * All coil, dipole, and RF cavity fields are registered into a single 1D
 * z-bin map at construction time. GetField does one bin lookup and sums
 * only the fields whose z range overlaps the query position.
 *
 * @author Laurie Nevay, Chris Rogers, Paul Jurj, Rohan Kamath
 */
class BDSFieldEMMuonCooler: public BDSFieldEM
{
public:
  BDSFieldEMMuonCooler() = delete;
  BDSFieldEMMuonCooler(const BDSFieldInfoExtraMuonCooler* info,
                       G4double brho);
  virtual ~BDSFieldEMMuonCooler();

  void BuildMagnets(const BDSFieldInfoExtraMuonCooler* info);
  void BuildDipoles(const BDSFieldInfoExtraMuonCooler* info);
  void BuildRF(const BDSFieldInfoExtraMuonCooler* info);

  virtual std::pair<G4ThreeVector, G4ThreeVector> GetField(const G4ThreeVector& position,
                                                           const G4double       t) const;

private:
  struct FieldEntry
  {
    enum class Type { Mag, EM };
    Type          type;
    BDSFieldMag*  mag{nullptr};  ///< non-null when type == Mag
    BDSFieldEM*   em{nullptr};   ///< non-null when type == EM
    G4ThreeVector offset;
    G4double      timeOffset{0}; ///< used only for EM entries
    G4double      zHalfExtent;
  };

  void BuildZBins();

  std::vector<FieldEntry>         entries;
  G4double                        zBinMin{0};
  G4double                        binWidth{0};
  G4int                           nBins{0};
  std::vector<std::vector<G4int>> zbins;
  std::vector<G4int>              alwaysOn;
};

#endif
