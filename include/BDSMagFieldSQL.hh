/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/

#ifndef BDSMagFieldSQL_h
#define BDSMagFieldSQL_h 

#include "G4Types.hh"
#include "G4MagneticField.hh"

#include "G4VPhysicalVolume.hh"
#include "BDSField.hh"
#include <string>
#include <vector>

using namespace std;

class BDSMagFieldSQL : public BDSField
{
public:  // with description

  BDSMagFieldSQL(const G4String& FieldFile, G4double aMarkerLength,
		 vector<G4String> Quadvol, vector<G4double> QuadBgrad,
		 vector<G4String> Sextvol, vector<G4double> SextBgrad);
  virtual ~BDSMagFieldSQL();
  // Constructor and destructor. No actions.


  void  GetFieldValue( const G4double Point[4],G4double *Bfield ) const;

  void Prepare(G4VPhysicalVolume *referenceVolume);

private:
  ofstream ofs;
  ifstream ifs;

  G4double itsMarkerLength;
  G4String FieldFile;

  // For Quad/Sext Fields
  vector<G4double> itsQuadBgrad;
  vector<G4String> itsQuadVol; 
  vector<G4double> itsSextBgrad;
  vector<G4String> itsSextVol; 

  // For Solenoid FieldMaps
  vector<G4double> itsBz;
  vector<G4double> itsZ;
  vector<G4double> itsBr_over_r;
  vector<G4double> itsdBz_by_dz;
  double itsdz;
};

#endif
