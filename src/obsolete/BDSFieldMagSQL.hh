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
#ifndef BDSFIELDMAGSQL_H
#define BDSFIELDMAGSQL_H

#include <fstream>
#include <list>
#include <map>
#include <vector>

#include "G4Types.hh"
#include "G4MagneticField.hh"

#include "G4VPhysicalVolume.hh"
#include "G4Navigator.hh"
#include "G4TouchableHistory.hh"
#include "G4TouchableHistoryHandle.hh"

#include "BDSAuxiliaryNavigator.hh"
#include "BDSMagFieldMesh.hh"
#include "BDSFieldMag.hh"

class BDSFieldMagSQL: public BDSFieldMag, public BDSAuxiliaryNavigator
{
public:  // with description

#if 0
  BDSFieldMagSQL(const G4String& FieldFile, G4double aMarkerLength,
		 std::list<G4String> Quadvol, std::list<G4double> aQuadBgrad,
		 std::list<G4String> Sextvol, std::list<G4double> aSextBgrad,
		 std::list<G4String> Octvol, std::list<G4double> aOctBgrad,
		 std::list<G4String> Fieldvol, std::list<G4ThreeVector> aUniformField);
#endif

  BDSFieldMagSQL(const G4String& aFieldFile,
		 G4double aMarkerLength,
		 std::map<G4String, G4double> aQuadVolBgrad,
		 std::map<G4String, G4double> aSextVolBgrad,
		 std::map<G4String, G4double> aOctVolBgrad,
		 std::map<G4String, G4ThreeVector> aUniformFieldVolField,
		 G4bool aHasNPoleFields, G4bool aHasUniformField);
  
  virtual ~BDSFieldMagSQL();
  // Constructor and destructor. No actions.

  /// Get the magnetic field vector in local coordinates.
  virtual G4ThreeVector GetField(const G4ThreeVector &position,
				 const G4double       t = 0) const;

  void  GetFieldValue( const G4double Point[4],G4double *Bfield ) const;

  G4bool GetHasNPoleFields();
  G4bool GetHasUniformField();
  G4bool GetHasFieldMap();

  void Prepare(G4VPhysicalVolume *referenceVolume);

private:
  G4bool itsHasNPoleFields;
  G4bool itsHasUniformField;
  G4bool itsHasFieldMap;

  std::ofstream ofs;
  std::ifstream ifs;

  G4double itsMarkerLength;
  G4String FieldFile;

  // For Quad/Sext/Oct Fields
  std::list<G4double> itsQuadBgrad;
  std::list<G4String> itsQuadVol; 
  std::list<G4double> itsSextBgrad;
  std::list<G4String> itsSextVol; 
  std::list<G4double> itsOctBgrad;
  std::list<G4String> itsOctVol; 

  // For Uniform Fields
  std::list<G4ThreeVector> itsUniformField;
  std::list<G4String> itsFieldVol; 

  //Maps for fields
  std::map<G4String, G4ThreeVector> itsUniformFieldVolField;
  std::map<G4String, G4double> itsQuadVolBgrad;
  std::map<G4String, G4double> itsSextVolBgrad;
  std::map<G4String, G4double> itsOctVolBgrad;

  // For Solenoid FieldMaps
  std::vector<G4double> itsBz;
  std::vector<G4double> itsZ;
  std::vector<G4double> itsBr_over_r;
  std::vector<G4double> itsdBz_by_dz;
  double itsdz;
};

#endif
