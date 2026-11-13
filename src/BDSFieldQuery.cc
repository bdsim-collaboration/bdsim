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
#include "BDSAuxiliaryNavigator.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSFieldQuery.hh"
#include "BDSFieldQueryInfo.hh"
#include "BDSUtilities.hh"
#include "BDSWarning.hh"

#include "globals.hh"
#include "G4Field.hh"
#include "G4FieldManager.hh"
#include "G4LogicalVolume.hh"
#include "G4Navigator.hh"
#include "G4String.hh"
#include "G4ThreeVector.hh"
#include "G4Types.hh"
#include "G4VPhysicalVolume.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>
#include <fstream>
#include <iomanip>
#include <vector>

G4Navigator* BDSFieldQuery::navigator = new G4Navigator();

BDSFieldQuery::BDSFieldQuery():
  queryMagnetic(false),
  queryElectric(false),
  writeX(false),
  writeY(false),
  writeZ(false),
  writeT(false)
{
  CleanUp();
}

BDSFieldQuery::~BDSFieldQuery()
{;}

void BDSFieldQuery::AttachWorldVolumeToNavigator(G4VPhysicalVolume* worldPVIn)
{
  navigator->SetWorldVolume(worldPVIn);
}

void BDSFieldQuery::CleanUp()
{
  CloseFiles();
  queryMagnetic = false;
  queryElectric = false;
  writeX = false;
  writeY = false;
  writeZ = false;
  writeT = false;
  navigator->ResetStackAndState();
}

void BDSFieldQuery::QueryFields(const std::vector<BDSFieldQueryInfo*>& fieldQueries)
{
  for (const auto& fieldQuery : fieldQueries)
    {QueryField(fieldQuery);}
}

void BDSFieldQuery::QueryField(const BDSFieldQueryInfo* query)
{
  CleanUp();
  
  if (!query)
    {return;} // protection - now we assume this pointer is always valid in the rest of this class
    
  // warn the user if a fieldObject is specified. In this context (bdsim) this has no effect
  // derived class may change this
  if (query->checkParameters)
    {CheckIfFieldObjectSpecified(query);}
  
  if (query->SpecificPoints())
    {
      QuerySpecificPoints(query);
      return;
    }
  
  G4cout << "FieldQuery> \"" << query->name << "\" with N (x,y,z,t) points: ("
         << query->xInfo.n << ", " << query->yInfo.n << ", " << query->zInfo.n << ", " << query->tInfo.n
         << ")";
  PrintBAndEInfo(query);

  if (query->printTransform)
    {G4cout << query->globalTransform << G4endl;}

  /// Ensure field transform navigator is in a completely clean state.
  BDSAuxiliaryNavigator::ResetNavigatorStates();
  
  G4double xMin    = query->xInfo.min;
  G4double nStepsX = query->xInfo.n == 1 ? 1 : (G4double)query->xInfo.n-1;
  G4double xStep   = (query->xInfo.max - query->xInfo.min) / nStepsX;
  if (std::isnan(xStep))
    {xStep = 1.0;}
  CheckNStepsAndRange(query->xInfo, "x", query->name);
  
  G4double yMin    = query->yInfo.min;
  G4double nStepsY = query->yInfo.n == 1 ? 1 : (G4double)query->yInfo.n-1;
  G4double yStep   = (query->yInfo.max - query->yInfo.min) / nStepsY;
  if (std::isnan(yStep))
    {yStep = 1.0;}
  CheckNStepsAndRange(query->yInfo, "y", query->name);
  
  G4double zMin    = query->zInfo.min;
  G4double nStepsZ = query->zInfo.n == 1 ? 1 : (G4double)query->zInfo.n-1;
  G4double zStep   = (query->zInfo.max - query->zInfo.min) / nStepsZ;
  if (std::isnan(zStep))
    {zStep = 1.0;}
  CheckNStepsAndRange(query->zInfo, "z", query->name);
  
  G4double tMin    = query->tInfo.min;
  G4double nStepsT = query->yInfo.n == 1 ? 1 : (G4double)query->tInfo.n-1;
  G4double tStep   = (query->tInfo.max - query->tInfo.min) / nStepsT;
  if (std::isnan(tStep))
    {tStep = 1.0;}
  CheckNStepsAndRange(query->tInfo, "t", query->name);
  
  G4ThreeVector xyzGlobal = G4ThreeVector();
  const G4AffineTransform& localToGlobalTransform = query->globalTransform;
  G4AffineTransform globalToLocalTransform = localToGlobalTransform.Inverse();
  
  G4ThreeVector generalUnitZ(0,0,1);
  localToGlobalTransform.ApplyAxisTransform(generalUnitZ);
  
  OpenFiles(query);
  
  G4double globalFieldValue[6];
  G4double localFieldValue[6];
  
  G4double tLocal = tMin;
  for (G4int i = 0; i < query->tInfo.n; i++)
    {
      G4double zLocal = zMin;
      for (G4int j = 0; j < query->zInfo.n; j++)
        {
          G4double yLocal = yMin;
          for (G4int k = 0; k < query->yInfo.n; k++)
            {
              G4double xLocal = xMin;
              for (G4int l = 0; l < query->xInfo.n; l++)
                {
                  xyzGlobal = LocalToGlobalPoint(localToGlobalTransform, xLocal, yLocal, zLocal);
                  GetFieldValue(xyzGlobal, generalUnitZ, tLocal, globalFieldValue);
                  GlobalToLocalAxisField(globalToLocalTransform,
                                         globalFieldValue,
                                         localFieldValue);
                  WriteFieldValue({xLocal, yLocal, zLocal}, tLocal, localFieldValue);
                  
                  xLocal += xStep;
                }
              yLocal += yStep;
            }
          zLocal += zStep;
        }
      tLocal += tStep;
    }
  
  CloseFiles();
  G4cout << "FieldQuery> Complete" << G4endl;
}

void BDSFieldQuery::CheckNStepsAndRange(const BDSFieldQueryInfo::QueryDimensionInfo& dimensionInfo,
                                        const G4String& dimensionName,
                                        const G4String& queryName) const
{
  const G4String& d = dimensionName;
  G4bool nonZeroRange = BDS::IsFinite(std::abs(dimensionInfo.max - dimensionInfo.min));
  if (dimensionInfo.n > 1 && !nonZeroRange)
    {
      G4String msg = "Problem in query \"" + queryName + "\": n"+d+" > 1, but |"+d;
      msg += "max - "+d+"min| = 0 -> cannot take more than 1 step in 0 range.";
      throw BDSException(__METHOD_NAME__, msg);
    }
  if (dimensionInfo.n == 1 && nonZeroRange)
    {BDS::Warning("Only 1 point to query in \"" + queryName + "\" over a non-zero range - check n"+d);}
}

void BDSFieldQuery::CheckIfFieldObjectSpecified(const BDSFieldQueryInfo* query) const
{
  if (!(query->fieldObject.empty()))
    {
      G4String msg = "The \"fieldObject\" variable is specified in the query definition \"" + query->name;
      msg += "\".\nThis has no effect when using a query in bdsim (instead of bdsinterpolator)\n";
      msg += "Remove \"fieldObject\" to suppress this warning.";
      BDS::Warning(msg);
    }
}

void BDSFieldQuery::QuerySpecificPoints(const BDSFieldQueryInfo* query)
{
  const std::vector<BDSFourVector<G4double>> points = query->pointsToQuery;
  
  G4cout << "FieldQuery> \"" << query->name << "\" with N points: "
         << points.size() << ", writing to file";
  PrintBAndEInfo(query);
  
  OpenFiles(query);
  G4ThreeVector xyz;
  G4double t;
  G4ThreeVector generalUnitZ(0,0,1);
  G4double globalFieldValue[6];
  for (auto const& xyzt : points)
    {
      xyz = G4ThreeVector(xyzt.x(), xyzt.y(), xyzt.z());
      t = xyzt.t();
      GetFieldValue(xyz, generalUnitZ, t, globalFieldValue);
      WriteFieldValue(xyz, t, globalFieldValue);
    }
  CloseFiles();
  G4cout << "FieldQuery> Complete" << G4endl;
}

void BDSFieldQuery::PrintBAndEInfo(const BDSFieldQueryInfo* query) const
{
  G4cout << ", writing to file";
  if (query->queryMagnetic && query->queryElectric)
    {G4cout << "s B: \"" << query->outfileMagnetic << "\" and E: \"" << query->outfileElectric << "\"" << G4endl;}
  else if (query->queryMagnetic)
    {G4cout << " B: \"" << query->outfileMagnetic << "\"" << G4endl;}
  else
    {G4cout << " E: \"" << query->outfileElectric << "\"" << G4endl;}
}

void BDSFieldQuery::OpenFiles(const BDSFieldQueryInfo* query)
{
  if (!(query->pointsColumnNames.empty()))
    {
      std::map<G4String, G4bool*> columnNamesToFlag = { {"x", &writeX},
							{"y", &writeY},
							{"z", &writeZ},
							{"t", &writeT} };
      for (const auto& columnName : query->pointsColumnNames)
	{ *(columnNamesToFlag[columnName]) = true;}
      BDS::Warning(__METHOD_NAME__, "the number of points and min max values in the output header will not be correct when using points");
    }
  else
    {
      writeX = query->xInfo.n > 1;
      writeY = query->yInfo.n > 1;
      writeZ = query->zInfo.n > 1;
      writeT = query->tInfo.n > 1;
    }
  
  if (query->queryMagnetic)
    {
      if (BDS::FileExists(query->outfileMagnetic) && !(query->overwriteExistingFiles))
	{
	  G4String msg = "\"" + query->outfileMagnetic + "\" file already exists and \"overwriteExistingFiles\" in query \"";
	  msg += query->name + "\" is false";
	  throw BDSException(__METHOD_NAME__, msg);
	}
      queryMagnetic = true;
      oFileMagnetic.open(query->outfileMagnetic);
      WriteHeader(oFileMagnetic, query);
    }
  
  if (query->queryElectric)
    {
      if (BDS::FileExists(query->outfileElectric) && !(query->overwriteExistingFiles))
	{
	  G4String msg = "\"" + query->outfileElectric + "\" file already exists and \"overwriteExistingFiles\" in query \"";
	  msg += query->name + "\" is false";
	  throw BDSException(__METHOD_NAME__, msg);
	}
      queryElectric = true;
      oFileElectric.open(query->outfileElectric);
      WriteHeader(oFileElectric, query);
    }
}

void BDSFieldQuery::WriteHeader(std::ofstream& out,
                                const BDSFieldQueryInfo* query) const
{
  G4String columns = "!    ";
  if (writeX)
    {
      out << "nx> "   << query->xInfo.n << "\n";
      out << "xmin> " << query->xInfo.min/CLHEP::cm << "\n";
      out << "xmax> " << query->xInfo.max/CLHEP::cm << "\n";
      columns += "    X      ";
    }
  if (writeY)
    {
      out << "ny> "   << query->yInfo.n << "\n";
      out << "ymin> " << query->yInfo.min/CLHEP::cm << "\n";
      out << "ymax> " << query->yInfo.max/CLHEP::cm << "\n";
      columns += "    Y      ";
    }
  if (writeZ)
    {
      out << "nz> "   << query->zInfo.n << "\n";
      out << "zmin> " << query->zInfo.min/CLHEP::cm << "\n";
      out << "zmax> " << query->zInfo.max/CLHEP::cm << "\n";
      columns += "    Z      ";
    }
  if (writeT)
    {
      out << "nt> "   << query->tInfo.n << "\n";
      out << "tmin> " << query->tInfo.min/CLHEP::s << "\n";
      out << "tmax> " << query->tInfo.max/CLHEP::s << "\n";
      columns += "    T      ";
    }
  columns += "          Fx            Fy            Fz\n";
  out << columns;
}

void BDSFieldQuery::CloseFiles()
{
  if (oFileMagnetic.is_open())
    {oFileMagnetic.close();}
  if (oFileElectric.is_open())
    {oFileElectric.close();}
}

G4ThreeVector BDSFieldQuery::LocalToGlobalPoint(const G4AffineTransform& localToGlobalTransform,
                                                G4double xLocal,
						G4double yLocal,
						G4double zLocal) const
{
  G4ThreeVector xyzGlobal = G4ThreeVector(xLocal, yLocal, zLocal);
  localToGlobalTransform.ApplyPointTransform(xyzGlobal);
  return xyzGlobal;
}

void BDSFieldQuery::GlobalToLocalAxisField(const G4AffineTransform& globalToLocalTransform,
                                           const G4double globalBEField[6],
                                           G4double localBEField[6])
{
  G4ThreeVector B(globalBEField[0], globalBEField[1], globalBEField[2]);
  G4ThreeVector E(globalBEField[3], globalBEField[4], globalBEField[5]);
  globalToLocalTransform.ApplyAxisTransform(B);
  globalToLocalTransform.ApplyAxisTransform(E);
  for (G4int i = 0; i < 3; i++)
    {
      localBEField[i]   = B[i];
      localBEField[i+3] = E[i];
    }
}

void BDSFieldQuery::GetFieldValue(const G4ThreeVector& globalXYZ,
                                  const G4ThreeVector& globalDirection,
                                  G4double tGlobal,
                                  G4double fieldValue[6])
{
  // always update as 0 as can't predict behaviour of Geant4 - takes fieldValue* as
  // variable array length, so we rely on it definitely writing E field as well as B??
  for (G4int i = 0; i < 6; i++)
    {fieldValue[i] = 0;}
  G4VPhysicalVolume* pv = navigator->LocateGlobalPointAndSetup(globalXYZ, &globalDirection, /*relativeSearch*/false);
  if (!pv)
    {return;} // this would happen if we query a point outside the world
  G4LogicalVolume* lv = pv->GetLogicalVolume();
  G4FieldManager* fm = lv->GetFieldManager();
  if (fm)
    {
      const G4Field* field = fm->GetDetectorField();
      G4double position[4] = {globalXYZ.x(), globalXYZ.y(),globalXYZ.z(), tGlobal};
      field->GetFieldValue(position, fieldValue);
    }
}

void BDSFieldQuery::WriteFieldValue(const G4ThreeVector& xyzLocal,
                                    G4double tLocal,
                                    const G4double fieldValue[6])
{
  if (queryMagnetic)
    {
      if (writeX)
        {oFileMagnetic << std::setprecision(6) << std::setw(10) << xyzLocal.x() / CLHEP::cm << "  ";}
      if (writeY)
        {oFileMagnetic << std::setprecision(6) << std::setw(10) << xyzLocal.y() / CLHEP::cm << "  ";}
      if (writeZ)
        {oFileMagnetic << std::setprecision(6) << std::setw(10) << xyzLocal.z() / CLHEP::cm << "  ";}
      if (writeT)
        {oFileMagnetic << std::setprecision(6) << std::setw(10) << tLocal / CLHEP::s << "  ";}
      for (G4int i = 0; i < 3; i++)
        {oFileMagnetic << std::setprecision(6) << std::setw(15) << fieldValue[i] / CLHEP::tesla << "  ";}
      oFileMagnetic << "\n";
    }
  if (queryElectric)
    {
      if (writeX)
        {oFileElectric << std::setprecision(6) << std::setw(10) << xyzLocal.x() / CLHEP::cm << "  ";}
      if (writeY)
        {oFileElectric << std::setprecision(6) << std::setw(10) << xyzLocal.y() / CLHEP::cm << "  ";}
      if (writeZ)
        {oFileElectric << std::setprecision(6) << std::setw(10) << xyzLocal.z() / CLHEP::cm << "  ";}
      if (writeT)
        {oFileElectric << std::setprecision(6) << std::setw(10) << tLocal / CLHEP::s << "  ";}
      for (G4int i = 3; i < 6; i++)
        {oFileElectric << std::setprecision(6) << std::setw(15) << fieldValue[i] / (CLHEP::volt/CLHEP::m) << "  ";}
      oFileElectric << "\n";
   }
}
