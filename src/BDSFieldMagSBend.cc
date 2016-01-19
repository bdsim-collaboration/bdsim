#include "BDSDebug.hh"
#include "BDSFieldMagSBend.hh"
#include "BDSMagnetStrength.hh"

#include "globals.hh"
#include "G4ThreeVector.hh"


BDSFieldMagSBend::BDSFieldMagSBend(BDSMagnetStrength* strength,
				   G4double           /*brho*/,
				   G4ThreeVector      unitDirection)
{
  localField  = unitDirection.unit() * (*strength)["field"];
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "B (local) = " << localField << G4endl;
#endif
}

G4ThreeVector BDSFieldMagSBend::GetFieldValue(const G4ThreeVector& /*position*/) const
{return localField;}
