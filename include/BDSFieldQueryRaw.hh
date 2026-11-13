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
#ifndef BDSFIELDQUERYRAW_H
#define BDSFIELDQUERYRAW_H

#include "BDSFieldQuery.hh"

#include "G4ThreeVector.hh"
#include "G4Types.hh"

class BDSFieldQueryInfo;
class G4Field;

/**
 *  @brief Class for querying the a raw field object.
 *
 *  Output is a BDSIM-format field map. Unique files for electric and magnetic
 *  field maps as would be required to read the field maps back into BDSIM.
 *
 *  @author Laurie Nevay
 */
class BDSFieldQueryRaw: public BDSFieldQuery
{
public:
  BDSFieldQueryRaw();
  virtual ~BDSFieldQueryRaw();
  
  /// Query the field in the Geant4 model according to information in query.
  void QueryFieldRaw(G4Field* field,
		     const BDSFieldQueryInfo* query);
  
protected:
  /// Get the electric and magnetic field at the specified coordinates. The navigator requires
  /// a direction for safe hierarchy searching. The output is written to array argument, where
  /// the values are Bx,By,Bz,Ex,Ey,Ez as in Geant4.
  virtual void GetFieldValue(const G4ThreeVector& globalXYZ,
			     const G4ThreeVector& globalDirection,
			     G4double tGlobal,
			     G4double fieldValue[6]);
  
  /// Do the opposite for this class as it's only used for the interpolator and we want
  /// fieldObject to be specified.
  virtual void CheckIfFieldObjectSpecified(const BDSFieldQueryInfo* query) const;
  
private:
  /// @{ Make regular interface private and unusable for this version of the class.
  using BDSFieldQuery::QueryFields;
  using BDSFieldQuery::QueryField;
  /// @}

  G4Field* field; ///< The field object to query.
};

#endif
