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
#include "BDSFieldClassType.hh"
#include "BDSFieldType.hh"
#include "BDSFieldMagInterpolated1D.hh"
#include "BDSFieldMagInterpolated2D.hh"
#include "BDSFieldMagInterpolated3D.hh"
#include "BDSFieldMagInterpolated4D.hh"
#include "BDSInterpolator1D.hh"
#include "BDSInterpolator2D.hh"
#include "BDSInterpolator3D.hh"
#include "BDSInterpolator4D.hh"
#include "BDSArray1DCoords.hh"
#include "BDSArray2DCoords.hh"
#include "BDSArray3DCoords.hh"
#include "BDSArray4DCoords.hh"

#include "BDSIQuery.hh"

#include "parser/query.h"

#include "globals.hh" // geant4 types / globals
#include "G4Field.hh"
#include "G4String.hh"
#include "G4ThreeVector.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>
#include <fstream>
#include <iostream>

#ifdef USE_GZSTREAM
//#include "gzstream.h"
#endif

void BDSI::Query(G4Field* field,
		 const GMAD::Query& params,
		 const BDSFieldType fieldType)
{
  BDSFieldClassType fieldClass = BDS::DetermineFieldClassType(fieldType);

  G4cout << "Query: " << params.name << " -> field object: " << params.fieldObject << G4endl;
  G4cout << "Writing to output file: " << params.outfileMagnetic << G4endl;
  switch (params.nDimensions)
    {
    case 1:
      {BDSI::Query1D(field, params, fieldClass); break;}
    case 2:
      {BDSI::Query2D(field, params, fieldClass); break;}
    case 3:
      {BDSI::Query3D(field, params, fieldClass); break;}
    case 4:
      {BDSI::Query4D(field, params, fieldClass); break;}
    default:
      {
	G4cout << "Invalid number of dimensions in query definition:" << G4endl;
	params.print();
	break;
      }
    }
}

void BDSI::Query1D(G4Field* field, const GMAD::Query& params, const BDSFieldClassType type)
{
  G4double xmin = G4double(params.xmin*CLHEP::m);
  G4double xmax = G4double(params.xmax*CLHEP::m);
  G4int    nX   = G4int(params.nx);
  G4String outputName = G4String(params.outfileMagnetic);
  
  double xStep = (xmax - xmin) / ((G4double)nX - 1);

#ifdef USE_GZSTREAM
  //ogzstream ofile;
  //outputName += ".tar.gz";
  std::ofstream ofile;
#else
  std::ofstream ofile;
#endif
  ofile.open(outputName);

  ofile << "nx> "   << nX << "\n";
  ofile << "xmin> " << xmin/CLHEP::cm << "\n";
  ofile << "xmax> " << xmax/CLHEP::cm << "\n";
  ofile << "! X     Fx    Fy    Fz\n";
  
  G4double totalN = (G4double)nX;

  G4double i = 0;
  for (G4double x = xmin; x < xmax + margin; x += xStep)
    {
      G4double result[6] = {0,0,0,0,0,0};
      G4double coords[4] = {x,0,0,0};
      field->GetFieldValue(coords, result);
      WriteOut(&ofile, 1, coords, result, type);
      i += 1;
      G4double percentage = (i / totalN) *100;
      std::cout << "\r" << std::floor(percentage) << "%" << std::flush;
    }
  
  ofile.close();
  std::cout << std::endl;

  BDSFieldMagInterpolated1D* magInt = dynamic_cast<BDSFieldMagInterpolated1D*>(field);
  if (magInt)
    {
      const BDSArray1DCoords* data = magInt->Interpolator()->Array();
      G4String name = "raw_" + outputName;
      ofile.open(name);
      ofile << *data;
      ofile.close();
    }
}

void BDSI::Query2D(G4Field* field, const GMAD::Query& params, const BDSFieldClassType type)
{
  G4double xmin = G4double(params.xmin*CLHEP::m);
  G4double xmax = G4double(params.xmax*CLHEP::m);
  G4double ymin = G4double(params.ymin*CLHEP::m);
  G4double ymax = G4double(params.ymax*CLHEP::m);
  G4int    nX   = G4int(params.nx);
  G4int    nY   = G4int(params.ny);
  G4String outputName = G4String(params.outfileMagnetic);
  
  double xStep = (xmax - xmin) / ((G4double)nX - 1);
  double yStep = (ymax - ymin) / ((G4double)nY - 1);
  
#ifdef USE_GZSTREAM
  //ogzstream ofile;
  //outputName += ".tar.gz";
  std::ofstream ofile;
#else
  std::ofstream ofile;
#endif
  ofile.open(outputName);

  ofile << "nx> "   << nX << "\n";
  ofile << "ny> "   << nY << "\n";
  ofile << "xmin> " << xmin/CLHEP::cm << "\n";
  ofile << "xmax> " << xmax/CLHEP::cm << "\n";
  ofile << "ymin> " << ymin/CLHEP::cm << "\n";
  ofile << "ymax> " << ymax/CLHEP::cm << "\n";
  ofile << "! X     Y     Fx    Fy    Fz\n";
  
  G4double totalN = (G4double)nX * (G4double)nY;

  G4int i = 0;
  for (G4double y = ymin; y < ymax + margin; y += yStep)
    {
      G4double percentage = ((G4double)i / totalN) *100;
      std::cout << "\r" << std::floor(percentage) << "%" << std::flush;
      for (G4double x = xmin; x < xmax + margin; x += xStep)
	{
	  G4double result[6] = {0,0,0,0,0,0};
	  G4double coords[4] = {x,y,0,0};
	  field->GetFieldValue(coords, result);
	  WriteOut(&ofile, 2, coords, result, type);
	  i++;
	}
    }
  ofile.close();
  std::cout << std::endl;

  BDSFieldMagInterpolated2D* magInt = dynamic_cast<BDSFieldMagInterpolated2D*>(field);
  if (magInt)
    {
      const BDSArray2DCoords* data = magInt->Interpolator()->Array();
      G4String name = "raw_" + outputName;
      ofile.open(name);
      ofile << *data;
      ofile.close();
    }
}

void BDSI::Query3D(G4Field* field, const GMAD::Query& params, const BDSFieldClassType type)
{
  G4double xmin = G4double(params.xmin*CLHEP::m);
  G4double xmax = G4double(params.xmax*CLHEP::m);
  G4double ymin = G4double(params.ymin*CLHEP::m);
  G4double ymax = G4double(params.ymax*CLHEP::m);
  G4double zmin = G4double(params.zmin*CLHEP::m);
  G4double zmax = G4double(params.zmax*CLHEP::m);
  G4int    nX   = G4int(params.nx);
  G4int    nY   = G4int(params.ny);
  G4int    nZ   = G4int(params.nz);
  G4String outputName = G4String(params.outfileMagnetic);
  
  double xStep = (xmax - xmin) / ((G4double)nX - 1);
  double yStep = (ymax - ymin) / ((G4double)nY - 1);
  double zStep = (zmax - zmin) / ((G4double)nZ - 1);
  
#ifdef USE_GZSTREAM
  //ogzstream ofile;
  //outputName += ".tar.gz";
  std::ofstream ofile;
#else
  std::ofstream ofile;
#endif
  ofile.open(outputName);

  ofile << "nx> "   << nX << "\n";
  ofile << "ny> "   << nY << "\n";
  ofile << "nz> "   << nZ << "\n";
  ofile << "xmin> " << xmin/CLHEP::cm << "\n";
  ofile << "xmax> " << xmax/CLHEP::cm << "\n";
  ofile << "ymin> " << ymin/CLHEP::cm << "\n";
  ofile << "ymax> " << ymax/CLHEP::cm << "\n";
  ofile << "zmin> " << zmin/CLHEP::cm << "\n";
  ofile << "zmax> " << zmax/CLHEP::cm << "\n";
  ofile << "! X     Y     Z     Fx    Fy    Fz\n";
  
  G4double totalN = (G4double)nX * (G4double)nY * (G4double)nZ;

  G4int i = 0;
  for (G4double z = zmin; z < zmax + margin; z += zStep)
    {
      G4double percentage = ((G4double)i / totalN) *100;
      std::cout << "\r" << std::floor(percentage) << "%" << std::flush;
      for (G4double y = ymin; y < ymax + margin; y += yStep)
	{
	  for (G4double x = xmin; x < xmax + margin; x += xStep)
	    {
	      G4double result[6] = {0,0,0,0,0,0};
	      G4double coords[4] = {x,y,z,0};
	      field->GetFieldValue(coords, result);
	      WriteOut(&ofile, 3, coords, result, type);
	      i++;
	    }
	}
    }
  ofile.close();
  std::cout << std::endl;

  BDSFieldMagInterpolated3D* magInt = dynamic_cast<BDSFieldMagInterpolated3D*>(field);
  if (magInt)
    {
      const BDSArray3DCoords* data = magInt->Interpolator()->Array();
      G4String name = "raw_" + outputName;
      ofile.open(name);
      ofile << *data;
      ofile.close();
    }
}

void BDSI::Query4D(G4Field* field, const GMAD::Query& params, const BDSFieldClassType type)
{
  G4double xmin = G4double(params.xmin*CLHEP::m);
  G4double xmax = G4double(params.xmax*CLHEP::m);
  G4double ymin = G4double(params.ymin*CLHEP::m);
  G4double ymax = G4double(params.ymax*CLHEP::m);
  G4double zmin = G4double(params.zmin*CLHEP::m);
  G4double zmax = G4double(params.zmax*CLHEP::m);
  G4double tmin = G4double(params.tmin*CLHEP::s);
  G4double tmax = G4double(params.tmax*CLHEP::s);
  G4int    nX   = G4int(params.nx);
  G4int    nY   = G4int(params.ny);
  G4int    nZ   = G4int(params.nz);
  G4int    nT   = G4int(params.nt);
  G4String outputName = G4String(params.outfileMagnetic);
  
  double xStep = (xmax - xmin) / ((G4double)nX - 1);
  double yStep = (ymax - ymin) / ((G4double)nY - 1);
  double zStep = (zmax - zmin) / ((G4double)nZ - 1);
  double tStep = (tmax - tmin) / ((G4double)nT - 1);
  
#ifdef USE_GZSTREAM
  //ogzstream ofile;
  //outputName += ".tar.gz";
  std::ofstream ofile;
#else
  std::ofstream ofile;
#endif
  ofile.open(outputName);

  ofile << "nx> "   << nX << "\n";
  ofile << "ny> "   << nY << "\n";
  ofile << "nz> "   << nZ << "\n";
  ofile << "xmin> " << xmin/CLHEP::cm << "\n";
  ofile << "xmax> " << xmax/CLHEP::cm << "\n";
  ofile << "ymin> " << ymin/CLHEP::cm << "\n";
  ofile << "ymax> " << ymax/CLHEP::cm << "\n";
  ofile << "zmin> " << zmin/CLHEP::cm << "\n";
  ofile << "zmax> " << zmax/CLHEP::cm << "\n";
  ofile << "tmin> " << tmin/CLHEP::s << "\n";
  ofile << "tmax> " << tmax/CLHEP::s << "\n";
  ofile << "! X     Y     Z     Fx    Fy    Fz\n";
  
  G4double totalN = (G4double)nX * (G4double)nY * (G4double)nZ * (G4double)nT;

  G4int i = 0;
  for (G4double t = tmin; t < tmax + margin; t += tStep)
    {
      G4double percentage = ((G4double)i / totalN) * 100;
      std::cout << "\r" << std::floor(percentage) << "%" << std::flush;
      for (G4double z = zmin; z < zmax + margin; z += zStep)
	{
	  for (G4double y = ymin; y < ymax + margin; y += yStep)
	    {
	      for (G4double x = xmin; x < xmax + margin; x += xStep)
		{
		  G4double result[6] = {0,0,0,0,0,0};
		  G4double coords[4] = {x,y,z,t};
		  field->GetFieldValue(coords, result);
		  WriteOut(&ofile, 4, coords, result, type);
		  i++;
		}
	    }
	}
    }
  ofile.close();
  std::cout << std::endl;

  BDSFieldMagInterpolated4D* magInt = dynamic_cast<BDSFieldMagInterpolated4D*>(field);
  if (magInt)
    {
      const BDSArray4DCoords* data = magInt->Interpolator()->Array();
      G4String name = "raw_" + outputName;
      ofile.open(name);
      ofile << *data;
      ofile.close();
    }
}

void BDSI::WriteOut(std::ostream* out,
		    const G4int    nDim,
		    const G4double coords[4],
		    const G4double result[6],
		    const BDSFieldClassType type)
{
  // write coordinates
  for (G4int i = 0; i < nDim; i++)
    {*out << coords[i]/CLHEP::cm << "\t";}

  // write field components
  switch (type.underlying())
    {
    case BDSFieldClassType::magnetic:
      {	
	*out << result[0] / CLHEP::tesla << "\t"
	     << result[1] / CLHEP::tesla << "\t"
	     << result[2] / CLHEP::tesla << "\n";
	break;
      }
    case BDSFieldClassType::electric:
      {
	*out << result[3] / CLHEP::volt << "\t"
	     << result[4] / CLHEP::volt << "\t"
	     << result[5] / CLHEP::volt << "\n";
	break;
      }
    case BDSFieldClassType::electromagnetic:
      {
	*out << result[0] / CLHEP::tesla << "\t"
	     << result[1] / CLHEP::tesla << "\t"
	     << result[2] / CLHEP::tesla << "\t"
	     << result[3] / CLHEP::volt  << "\t"
	     << result[4] / CLHEP::volt  << "\t"
	     << result[5] / CLHEP::volt  << "\n";
	break;
      }
    default:
      {break;}
    }
}
