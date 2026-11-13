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
#include "BDSArray2DCoords.hh"
#include "BDSArray2DCoordsRQuad.hh"
#include "BDSArray4D.hh"
#include "BDSException.hh"
#include "BDSFieldFormat.hh"
#include "BDSFieldInfo.hh"
#include "BDSFieldLoader.hh"
#include "BDSFieldMag.hh"
#include "BDSFieldMagInterpolated2D.hh"
#include "BDSFieldType.hh"
#include "BDSFieldValue.hh"
#include "BDSIntegratorType.hh"
#include "BDSInterpolator2D.hh"
#include "BDSInterpolatorType.hh"

#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include <fstream>
#include <iomanip>
#include <ostream>
#include <string>
#include <stdexcept>
#include <typeinfo>

#include "BDSFieldLoaderBDSIM.hh"
#include "BDSArray2DCoordsTransformed.hh"

void Query(BDSFieldMag* field,
	   G4double ymin, G4double ymax, G4double xmin, G4double xmax,
	   G4int nX, G4int nY,
	   const G4String& outputName)
{
  G4cout << "Querying " << outputName << G4endl;
  
  double xStep = (xmax - xmin) / (double)nX;
  double yStep = (ymax - ymin) / (double)nY;
  
  std::ofstream ofile;
  ofile.open(outputName+".dat");

  int i = 0;
  double yc = ymin;
  double xc = xmin;
  for (int iy = 0; iy < nY; iy++)
    {
      for (int ix = 0; ix < nX; ix++)
	{
	  if (i%1000 == 0)
	    {std::cout << "\r" << i;}
	  G4ThreeVector result = field->GetField(G4ThreeVector(xc,yc,0));
	  ofile << std::setw(8) << xc << "\t" << std::setw(8) << yc << "\t"
	  << std::setw(8) << result.x() / CLHEP::tesla << "\t"
	  << std::setw(8) << result.y() / CLHEP::tesla << "\t"
	  << std::setw(8) << result.z() / CLHEP::tesla << "\n";
	  i++;
	  
	  xc += xStep;
	}
      yc += yStep;
      xc = xmin;
    }
  ofile.close();
  std::cout << std::endl;
  
  std::ofstream ofile2;
  ofile2.open(outputName+"_raw.dat");
  auto ri = dynamic_cast<BDSFieldMagInterpolated2D*>(field);
  if (ri)
  {
    auto r = ri->Interpolator()->Array();
    if (r)
      {ofile2 << *r;}
  }
  ofile2.close();
}

int main(int /*argc*/, char** /*argv*/)
{
  const std::string exampleFile2D = "../examples/features/fields/maps_bdsim/2dexample.dat";
  
  // field map example is from x(-30:26cm) and y(-25:22.6cm)
  // query a larger range to test interpolators fully
  double xmin = -40*CLHEP::cm;
  double xmax =  40*CLHEP::cm;
  int    nX   = 100;
  double ymin = -40*CLHEP::cm;
  double ymax =  40*CLHEP::cm;
  int    nY   = 100;
  
  // zoom range
  double xminZ = -11*CLHEP::cm;
  double xmaxZ =  11*CLHEP::cm;
  double yminZ =  10*CLHEP::cm;
  double ymaxZ =  11*CLHEP::cm;
  int    nXZ   = 50;
  int    nYZ   = 47;  // try odd number and not the same as x

  // 2D Nearest Neighbour
  BDSFieldInfo* infoBiNearest = new BDSFieldInfo(BDSFieldType::bmap2d,
						 0,
						 BDSIntegratorType::g4classicalrk4,
						 nullptr,
						 false,
						 G4Transform3D(),
                                                 exampleFile2D,
						 BDSFieldFormat::bdsim2d,
						 BDSInterpolatorType::nearest2d);
  
  BDSFieldMag* biNearest = nullptr;
  try
    {biNearest = BDSFieldLoader::Instance()->LoadMagField(*infoBiNearest);}
  catch (const BDSException& e)
    {std::cerr << e.what() << std::endl; exit(1);}
  catch (const std::exception& e)
    {std::cerr << e.what() << std::endl; exit(1);}

  // 2D Linear
  BDSFieldInfo* infoBiLinear = new BDSFieldInfo(BDSFieldType::bmap2d,
						0,
						BDSIntegratorType::g4classicalrk4,
						nullptr,
						false,
						G4Transform3D(),
                                                exampleFile2D,
						BDSFieldFormat::bdsim2d,
						BDSInterpolatorType::linear2d);

  BDSFieldMag* biLinear = nullptr;
  try
    {biLinear = BDSFieldLoader::Instance()->LoadMagField(*infoBiLinear);}
  catch (const BDSException& e)
    {std::cerr << e.what() << std::endl; exit(1);}
  catch (const std::exception& e)
    {std::cerr << e.what() << std::endl; exit(1);}

  // 2D Cubic
  BDSFieldInfo* infoBiCubic = new BDSFieldInfo(BDSFieldType::bmap2d,
					       0,
					       BDSIntegratorType::g4classicalrk4,
					       nullptr,
					       false,
					       G4Transform3D(),
                                               exampleFile2D,
					       BDSFieldFormat::bdsim2d,
					       BDSInterpolatorType::cubic2d);
  
  BDSFieldMag* biCubic = nullptr;
  try
    {biCubic = BDSFieldLoader::Instance()->LoadMagField(*infoBiCubic);}
  catch (const BDSException& e)
    {std::cout << e.what() << std::endl; return 1;}
  catch (const std::exception& e)
    {std::cout << e.what() << std::endl; return 1;}

  // Get the raw data
  if (biNearest)
    {
      BDSFieldMagInterpolated2D* fieldInterp = dynamic_cast<BDSFieldMagInterpolated2D*>(biNearest);
      if (fieldInterp)
	{
	  auto interp = fieldInterp->Interpolator();
	  auto arrCoords = interp->Array();
	  const BDSArray4D* arr = static_cast<const BDSArray4D*>(arrCoords);
	  std::ofstream ofile;
	  ofile.open("raw.dat");
	  ofile << *arr;
	  ofile.close();
	}
    }

  // Query across full range of magnet including just outside range too.
  // Then query in small region where there's large variation.
  if (biNearest)
    {
      Query(biNearest, ymin, ymax, xmin, xmax, nX, nY, "nearest");
      Query(biNearest, yminZ, ymaxZ, xminZ, xmaxZ, nXZ, nYZ, "nearest_zoom");
      G4cout << biNearest->GetField(G4ThreeVector(10, 11, 0)) << G4endl;
    }
  if (biLinear)
    {
      Query(biLinear, ymin, ymax, xmin, xmax, nX, nY, "linear");
      Query(biLinear, yminZ, ymaxZ, xminZ, xmaxZ, nXZ, nYZ, "linear_zoom");
      G4cout << biLinear->GetField(G4ThreeVector(10, 11, 0)) << G4endl;
    }
  if (biCubic)
    {
      Query(biCubic, ymin, ymax, xmin, xmax, nX, nY, "cubic");
      Query(biCubic, yminZ, ymaxZ, xminZ, xmaxZ, nXZ, nYZ, "cubic_zoom");
      G4cout << biCubic->GetField(G4ThreeVector(10, 11, 0)) << G4endl;
    }
  
  //const G4String exampleFile2DQuadrant = "../examples/features/fields/maps_bdsim/2d_dipole_quadrant.dat";
  //BDSFieldLoaderBDSIM<std::ifstream> loader;
  //BDSArray2DCoords* result = loader.Load2D(exampleFile2DQuadrant);
  //BDSArrayCoordOperatorFlip* transform = new BDSArrayCoordOperatorFlip(true, false, false, false);
  //auto transformed = new BDSArray2DCoordsTransformed(result, transform);

  return 0;
}
