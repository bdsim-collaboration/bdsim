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
#include "BDSException.hh"
#include "BDSFieldMag.hh"
#include "BDSFieldFactory.hh"
#include "BDSFieldInfo.hh"
#include "BDSFieldType.hh"
#include "BDSIntegratorType.hh"
#include "BDSMagnetStrength.hh"

#include "BDSFieldMagDipole.hh"
#include "BDSFieldMagDipoleOuter.hh"
#include "BDSFieldMagQuadrupole.hh"
#include "BDSFieldMagSextupole.hh"
#include "BDSFieldMagOctupole.hh"
#include "BDSFieldMagDecapole.hh"
#include "BDSFieldMagMuonSpoiler.hh"
#include "BDSFieldMagMultipole.hh"
#include "BDSFieldMagMultipoleOuter.hh"
#include "BDSFieldMagMultipoleOuterDual.hh"
#include "BDSFieldMagSkewOwn.hh"
#include "BDSMagnetOuterFactoryLHC.hh"

#include "globals.hh"
#include "G4ThreeVector.hh"

#include "CLHEP/Units/PhysicalConstants.h"

#include <cmath>
#include <fstream>
#include <iostream>
#include <ostream>
//#include <string>
#include <vector>


int main(int /*argc*/, char** /*argv*/)
{
  G4double lhcDX = BDSMagnetOuterFactoryLHC::beamSeparation;
  
  try
    {
  BDSMagnetStrength* st = new BDSMagnetStrength();
  (*st)["field"] = 1.3*CLHEP::tesla;   // T
  (*st)["angle"] = 0.014; // mrad
  (*st)["k1"] = 0.34;
  (*st)["k2"] = 3.91;
  (*st)["k3"] = 12.56;
  (*st)["k4"] = 45567.32;
  (*st)["k5"] = 3535.2;

  BDSMagnetStrength* st2 = new BDSMagnetStrength();
  (*st2)["k1"] = 3.4;
  (*st2)["k2"] = 123.4;
  (*st2)["k3"] = 1234.5;
  (*st2)["k4"] = 12345.6;
  (*st2)["k5"] = 123456.7;

  BDSMagnetStrength* st3 = new BDSMagnetStrength();
  (*st3)["k1"] = -0.532;

  BDSMagnetStrength* st4 = new BDSMagnetStrength();
  G4ThreeVector ff = G4ThreeVector(0.23,0.56,0);
  ff *= CLHEP::tesla;
  (*st4)["bx"]    = ff.x();
  (*st4)["by"]    = ff.y();
  (*st4)["bz"]    = ff.z();
  (*st4)["field"] = ff.mag();

  BDSMagnetStrength* st5 = new BDSMagnetStrength();
  (*st5)["k1"] = 0.12;
  (*st5)["k2"] = 0.02;
  (*st5)["k3"] = -0.003;
  (*st5)["k4"] = 0.0004;
  (*st5)["k5"] = -0.00005;
  
  const G4double brho = 4.333; // around 1GeV electron

  std::vector<std::string> names = {"dipole", "quadrupole", "sextupole", "octupole", "decapole",
				    "skewquadrupole", "skewsextupole", "skewoctupole",
				    "skewdecapole", "muonspoiler", "multipole"};
  std::vector<BDSFieldMag*> fields;
  
  fields.push_back(new BDSFieldMagDipole(st));
  fields.push_back(new BDSFieldMagQuadrupole(st, brho));
  fields.push_back(new BDSFieldMagSextupole(st, brho));
  fields.push_back(new BDSFieldMagOctupole(st, brho));
  fields.push_back(new BDSFieldMagDecapole(st, brho));
  fields.push_back(new BDSFieldMagSkewOwn(new BDSFieldMagQuadrupole(st, brho), CLHEP::pi/4.));
  fields.push_back(new BDSFieldMagSkewOwn(new BDSFieldMagSextupole(st, brho), CLHEP::pi/6.));
  fields.push_back(new BDSFieldMagSkewOwn(new BDSFieldMagOctupole(st, brho), CLHEP::pi/8.));
  fields.push_back(new BDSFieldMagSkewOwn(new BDSFieldMagDecapole(st, brho), CLHEP::pi/10.));
  fields.push_back(new BDSFieldMagMuonSpoiler(st, brho));
  fields.push_back(new BDSFieldMagMultipole(st5, brho));

  G4double poleTipRadius = 40;
  G4bool   positiveField = true;
  BDSFieldMag* innerField;
  BDSFieldMag* field;

  // outer dipole
  innerField = new BDSFieldMagDipole(st);
  positiveField = (*st)["field"] > 0;
  field = new BDSFieldMagMultipoleOuter(1, poleTipRadius, innerField, positiveField, brho);
  delete innerField;
  fields.push_back(field);
  names.emplace_back("multipoleouterdipole");

  // outer dipole 3d
  field = new BDSFieldMagDipoleOuter(st4, poleTipRadius);
  fields.push_back(field);
  names.emplace_back("outerdipole3d");
  
  // outer quadrupole
  innerField = new BDSFieldMagQuadrupole(st, brho);
  positiveField = (*st)["k1"] > 0;
  field = new BDSFieldMagMultipoleOuter(2, poleTipRadius, innerField, positiveField, brho);
  delete innerField;
  fields.push_back(field);
  names.emplace_back("multipoleouterquadrupole");
  
  // outer sextupole
  innerField = new BDSFieldMagSextupole(st, brho);
  positiveField = (*st)["k2"] > 0;
  field = new BDSFieldMagMultipoleOuter(3, poleTipRadius, innerField, positiveField, brho);
  delete innerField;
  fields.push_back(field);
  names.emplace_back("multipoleoutersextupole");
  
  // outer octupole
  innerField = new BDSFieldMagOctupole(st, brho);
  positiveField = (*st)["k3"] > 0;
  field = new BDSFieldMagMultipoleOuter(4, poleTipRadius, innerField, positiveField, brho);
  delete innerField;
  fields.push_back(field);
  names.emplace_back("multipoleouteroctupole");
  
  // outer decapole
  innerField = new BDSFieldMagDecapole(st, brho);
  positiveField = (*st)["k4"] > 0;
  field = new BDSFieldMagMultipoleOuter(5, poleTipRadius, innerField, positiveField, brho);
  delete innerField;
  fields.push_back(field);
  names.emplace_back("multipoleouterdecapole");
  
  // outer quad with -ve k1
  innerField = new BDSFieldMagQuadrupole(st3, brho); // -ve k1
  positiveField = (*st3)["k1"] > 0;
  field = new BDSFieldMagMultipoleOuter(2, poleTipRadius, innerField, positiveField, brho);
  delete innerField;
  fields.push_back(field);
  names.emplace_back("multipoleouterquadrupole-ve");
  
  // outer skew quadrupole
  BDSFieldMag* normalField;
  innerField = new BDSFieldMagQuadrupole(st, brho);
  positiveField = (*st)["k1"] > 0;
  normalField = new BDSFieldMagMultipoleOuter(2, poleTipRadius, innerField, positiveField, brho);
  delete innerField;
  field = new BDSFieldMagSkewOwn(normalField, CLHEP::pi/4.);
  fields.push_back(field);
  names.emplace_back("skewmultipoleouterquadrupole");
  
  // outer skew sextupole
  innerField = new BDSFieldMagSextupole(st, brho);
  positiveField = (*st)["k2"] > 0;
  normalField = new BDSFieldMagMultipoleOuter(3, poleTipRadius, innerField, positiveField, brho);
  delete innerField;
  field = new BDSFieldMagSkewOwn(normalField, CLHEP::pi/6.);
  fields.push_back(field);
  names.emplace_back("skewmultipoleouterssextupole");
  
  // outer skew octupole
  innerField = new BDSFieldMagOctupole(st, brho);
  positiveField = (*st)["k3"] > 0;
  normalField = new BDSFieldMagMultipoleOuter(4, poleTipRadius, innerField, positiveField, brho);
  delete innerField;
  field = new BDSFieldMagSkewOwn(normalField, CLHEP::pi/8.);
  fields.push_back(field);
  names.emplace_back("skewmultipoleouteroctupole");
  
  // outer skew decapole
  innerField = new BDSFieldMagDecapole(st, brho);
  positiveField = (*st)["k4"] > 0;
  normalField = new BDSFieldMagMultipoleOuter(5, poleTipRadius, innerField, positiveField, brho);
  delete innerField;
  field = new BDSFieldMagSkewOwn(normalField, CLHEP::pi/10.);
  fields.push_back(field);
  names.emplace_back("skewmultipoleouterdecapole");

  // outer lhc dipole
  innerField = new BDSFieldMagDipole(st);
  positiveField = (*st)["field"] < 0; // note convention for dipoles here
  field = new BDSFieldMagMultipoleOuterDual(1, poleTipRadius, innerField, positiveField, lhcDX, true, true, true);
  delete innerField;
  fields.push_back(field);
  names.emplace_back("multipoleouterdipolelhc");

  // outer lhc quadrupole
  innerField = new BDSFieldMagQuadrupole(st, brho);
  positiveField = (*st)["k1"] > 0;
  field = new BDSFieldMagMultipoleOuterDual(2, poleTipRadius, innerField, positiveField, lhcDX, true, true, false);
  delete innerField;
  fields.push_back(field);
  names.emplace_back("multipoleouterquadrupolelhc");

  // outer lhc sextupole
  innerField = new BDSFieldMagSextupole(st, brho);
  positiveField = (*st)["k2"] > 0;
  field = new BDSFieldMagMultipoleOuterDual(3, poleTipRadius, innerField, positiveField, lhcDX, true, true, true);
  delete innerField;
  fields.push_back(field);
  names.emplace_back("multipoleoutersextupolelhc");
  
  // Angular data
  const G4int    nR    = 20;
  const G4int    nPhi  = 100;
  const G4double rMax  = 100; // mm
  const G4double rStep = rMax / (G4double) nR;
  const G4double pStep = CLHEP::twopi / (G4double) (nPhi-1);

  // Regular carteasian grid - symmetric for x,y just now
  const G4int    nX    = 100;
  const G4double xMin  = -200; // mm 
  const G4double xMax  = 200;  // mm
  const G4int    nY    = 100;
  const G4double yMin  = -200; // mm
  const G4double yMax  = 200;  // mm
  
  /*
  // for lhc style magnets
  const G4int    nX    = 200;
  const G4double xMin  = -200; // mm 
  const G4double xMax  = 400;  // mm
  const G4int    nY    = 200;
  const G4double yMin  = -250; // mm
  const G4double yMax  = 250;  // mm
  */
  const G4double xStep = (xMax - xMin) / (G4double) (nX-1);
  const G4double yStep = (yMax - yMin) / (G4double) (nY-1);
   
  for (int f = 0; f < (int)fields.size(); ++f)
    {
      field = fields[f]; // overwrite local pointer variable
      std::string nm = names[f];
      G4cout << "Generating field for type \"" << nm << "\"" << G4endl;
      std::ofstream rfile;
      rfile.open(std::string(nm+"_radial.dat").c_str());
      rfile << "> nR = "   << nR   << "\n";
      rfile << "> nPhi = " << nPhi << "\n";
      rfile << "> brho = " << brho << "\n";
      rfile << "# (x,y,z)\t\tField\n";
      G4double r, phi;
      G4int i, j;
      for (r=rStep, i=0; i < nR; r+= rStep, i++)
	{
	  for (phi=0, j=0; j < nPhi; phi+=pStep, j++)
	    {
	      G4double x = r*cos(phi);
	      G4double y = r*sin(phi);
	      G4ThreeVector position(x,y,0);
	      rfile << position << "\t" << field->GetField(position)/CLHEP::tesla << "\n";
	    }
	}
      rfile.close();
      
      std::ofstream cfile;
      cfile.open(std::string(nm+"_carteasian.dat").c_str());
      cfile << "> nX = "   << nX   << "\n";
      cfile << "> nY = "   << nY   << "\n";
      cfile << "> brho = " << brho << "\n";
      cfile << "# (x,y,z)\t\tField\n";
      G4double x,y;
      for (y=yMin, i=0; i < nY; y+=yStep, ++i)
	{
	  for (x=xMin, j=0; j < nX; x+=xStep, ++j)
	    {
	      G4ThreeVector position(x,y,0);
	      cfile << position << "\t" << field->GetField(position)/CLHEP::tesla << "\n";
	    }
	}
      cfile.close();
    }


  /*
  field = fields[fields.size() - 4]; // should be quadrupole
  G4double xMin2 = -100;
  G4double xMax2 = 100;
  G4double xStep2 = (xMax2 - xMin2)/ (G4double) (nX-1);
  for (G4double x = xMin2, i=0; i < nX; x += xStep2, ++i)
    {
      G4ThreeVector pos(x,10,0);
      G4cout << field->GetField(pos)/CLHEP::tesla << G4endl;
    }
  */
    }
  catch (const BDSException& e)
    {std::cout << e.what() << std::endl;}
  return 0;
}
