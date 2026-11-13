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
#ifndef BDSFIELDQUERYFORVIS_H
#define BDSFIELDQUERYFORVIS_H

#include "BDSFieldQuery.hh"

#include "G4ThreeVector.hh"
#include "G4Types.hh"

#include <array>
#include <vector>

class BDSFieldQueryInfo;

/**
 *  @brief Wrapper class of BDSFieldQuery that accumulates values for visualisation.
 *
 *  @author Laurie Nevay
 */
class BDSFieldQueryForVis: public BDSFieldQuery
{
public:
  BDSFieldQueryForVis();
  virtual ~BDSFieldQueryForVis();
  
  inline const std::vector<std::array<G4double, 9>> Values() const {return values;}
  inline G4double MaxFieldE() const {return maxFieldE;}
  inline G4double MaxFieldB() const {return maxFieldB;}
  
  /// Reserve the vector size we need first, then proceed as normal. Avoids big copies.
  virtual void QueryField(const BDSFieldQueryInfo* query);
  
  /// Call the base class method and also clear the vector of values in this class.
  virtual void CleanUp();
  
  /// Calculate magnitude of 3-vector without having to construct one
  static G4double SimpleMag(G4double x, G4double y, G4double z);
  
  /// @{ Replace with empty implementation so no file handling is done for visualisation
  virtual void OpenFiles(const BDSFieldQueryInfo*){;}
  virtual void WriteHeader(std::ofstream&, const BDSFieldQueryInfo*) const {;}
  virtual void CloseFiles() {;}
  virtual void WriteFieldValue(const G4ThreeVector&,
                               G4double,
                               const G4double[6]) {;}
  virtual void PrintBAndEInfo(const BDSFieldQueryInfo*) const {;}
  /// @}
  
protected:
  /// Call base class method but make a copy of the global position and fields in this class.
  virtual void GetFieldValue(const G4ThreeVector& globalXYZ,
			     const G4ThreeVector& globalDirection,
			     G4double tGlobal,
			     G4double fieldValue[6]);

private:
  std::vector<std::array<G4double, 9>> values;
  G4double maxFieldB;
  G4double maxFieldE;
  G4bool drawZeroValuePoints;
};

#endif
