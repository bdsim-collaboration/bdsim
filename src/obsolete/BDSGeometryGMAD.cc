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
#include "BDSGlobalConstants.hh" 
#include "BDSDebug.hh"
#include "BDSGeometryGMAD.hh"

#include "BDSMaterials.hh"

#include "globals.hh" // geant4 types / globals
#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4Trd.hh"
#include "G4Tubs.hh"
#include "G4VisAttributes.hh"
#include "G4VPhysicalVolume.hh"


BDSGeometryGMAD::BDSGeometryGMAD(G4String file)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "> reading file : " << file << G4endl;
#endif
  inputf.open(file);
  if(!inputf.good()) {G4cerr<<"ggmad driver: error opening input file "<<file<<G4endl; exit(1);}
}

void BDSGeometryGMAD::Error()
{G4cout << "ggmad: error in file format" << G4endl;}

void BDSGeometryGMAD::Construct(G4LogicalVolume *marker)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  
  G4String token;
  G4String token1;

  G4double x0=0, y0=0, z0=0, x=0, y=0, z=0; // box parameters
  G4double rmin=0, rmax=0, rmin2=0, rmax2=0, phi0=0, dphi=0; // cylindrical parameters
  G4double x1=0, x2=0, y1=0, y2=0;
  G4double phi=0, theta=0, psi=0; // Euler angles - for rotation of components
  G4String material;
  // G4double FieldX, FieldY, FieldZ;
  // FieldX = FieldY = FieldZ = 0.0;

  G4Material *theMaterial;
  G4Box *aBox;
  G4Tubs *aTubs;
  G4Cons *aCons;
  //G4Trap *aTrap;
  G4Trd *aTrd;

  G4LogicalVolume *lvol;

  G4VisAttributes *visAttr = new G4VisAttributes(true, G4Colour(0.2,0.2,0.2));
  visAttr->SetForceSolid(true);
						 

  G4int count = 0;

  while (inputf.good())
    {
      token = GetWord();
 
      if(token=="Box") {
	if(GetWord() == "{")
	  {
	    
	    while((token = GetWord()))
	      {
		if (token == "}") break;
		
		GetParameter(x0,"x0",token); // origin
		GetParameter(y0,"y0",token);
		GetParameter(z0,"z0",token);
		GetParameter(x,"x",token); //half lengths
		GetParameter(y,"y",token);
		GetParameter(z,"z",token);
		GetParameter(phi,"phi",token);
		GetParameter(theta,"theta",token);
		GetParameter(psi,"psi",token);
		GetParameter(material,"material",token);
	      }

	    //create Box

	    theMaterial = BDSMaterials::Instance()->GetMaterial(material);
	    
	  
	    G4cout<<"creating box : "<<x0<<"  "<<y0<<" "<<z0<<G4endl;


	    aBox = new G4Box("aBox" + G4String(count),
			     x,   // half x
			     y, // half y
			     z ); // half z
	    
	    lvol = new G4LogicalVolume(aBox,
			theMaterial,
			"_bmp_logical");


	    // place into mother volume

	    G4RotationMatrix *rot = new G4RotationMatrix;
	    rot->rotateX(CLHEP::twopi*phi/360.);
	    rot->rotateY(CLHEP::twopi*theta/360.);
	    rot->rotateZ(CLHEP::twopi*psi/360.);

	    // G4VPhysicalVolume* PhysiComp =
	    new G4PVPlacement(
	      rot,		       // rotation
	      G4ThreeVector(x0,y0,z0), // at (x0,y0,z0)
	      lvol,                    // its logical volume
	      "vol_"+G4String(count),  // its name
	      marker,                  // its mother  volume
	      false,		       // no boolean operation
	      0, BDSGlobalConstants::Instance()->CheckOverlaps());
	    count++;

	  } else Error();
	
      }// G4Box

      if(token=="Tubs") {
	if(GetWord() == "{")
	  {
	    
	    while((token = GetWord()))
	      {
		if (token == "}") break;
		
		GetParameter(x0,"x0",token); // origin
		GetParameter(y0,"y0",token);
		GetParameter(z0,"z0",token);
		GetParameter(rmin,"rmin",token); //half lengths
		GetParameter(rmax,"rmax",token);
		GetParameter(z,"z",token);
		GetParameter(phi0,"phi0",token);
		GetParameter(dphi,"dphi",token);
		GetParameter(phi,"phi",token);
		GetParameter(theta,"theta",token);
		GetParameter(psi,"psi",token);
		GetParameter(material,"material",token);
	      }
	    
	    //create Box
	    
	    theMaterial = BDSMaterials::Instance()->GetMaterial(material);
	    
	    
	    G4cout<<"creating tubs : "<<x0<<"  "<<y0<<" "<<z0<<G4endl;
	    
	    // default - tube
	    phi0 = 0;
	    dphi = 360; // degrees
	    
	    aTubs = new G4Tubs("aTubs" + G4String(count),
			       rmin,   // inner R
			       rmax, // outer R
			       z, //z
			       phi0,//phi 0 
			       CLHEP::twopi * dphi / 360  ); //delta phi
	    
	    lvol = new G4LogicalVolume(aTubs,
				       theMaterial,
				       "_bmp_logical");
	    
	    
	    // place into mother volume
	    
	    G4RotationMatrix *rot = new G4RotationMatrix;
	    rot->rotateX(CLHEP::twopi*phi/360.);
	    rot->rotateY(CLHEP::twopi*theta/360.);
	    rot->rotateZ(CLHEP::twopi*psi/360.);

	    // G4VPhysicalVolume* PhysiComp =
	    new G4PVPlacement(
			      rot,			     // rotation
			      G4ThreeVector(x0,y0,z0),     // at (0,0,0)
			      lvol,  // its logical volume
			      "vol_"+G4String(count),	     // its name
			      marker,     // its mother  volume
			      false,		     // no boolean operation
			      0, BDSGlobalConstants::Instance()->CheckOverlaps());
	    
	    count++;
	    
	  } else Error();
      }

      if(token=="Cons") {
	if(GetWord() == "{")
	  {
	    
	    while((token = GetWord()))
	      {
		if (token == "}") break;
		
		GetParameter(x0,"x0",token); // origin
		GetParameter(y0,"y0",token);
		GetParameter(z0,"z0",token);
		GetParameter(rmin,"rmin1",token); //half lengths
		GetParameter(rmax,"rmax1",token);
		GetParameter(rmin2,"rmin2",token); //half lengths
		GetParameter(rmax2,"rmax2",token);
		GetParameter(z,"z",token);
		GetParameter(phi0,"phi0",token);
		GetParameter(dphi,"dphi",token);
		GetParameter(phi,"phi",token);
		GetParameter(theta,"theta",token);
		GetParameter(psi,"psi",token);
		GetParameter(material,"material",token);
	      }
	    
	    //create Box
	    
	    theMaterial = BDSMaterials::Instance()->GetMaterial(material);
	    
	    
	    G4cout<<"creating cons : "<<x0<<"  "<<y0<<" "<<z0<<G4endl;
	    G4cout<<"rotation : "<<phi<<"  "<<theta<<" "<<psi<<G4endl;
	    
	    // default - tube
	    phi0 = 0;
	    dphi = 360; // degrees
	    
	    aCons = new G4Cons("aCons" + G4String(count),
			       rmin,   // inner R
			       rmax,
			       rmin2,
			       rmax2,
			       z, //z
			       phi0,//phi 0 
			       CLHEP::twopi * dphi / 360  ); //delta phi
	    
	    lvol = new G4LogicalVolume(aCons,
				       theMaterial,
				       "_bmp_logical");

	    lvol->SetVisAttributes( visAttr );
	    
	    
	    // place into mother volume

	    G4RotationMatrix *rot = new G4RotationMatrix;
	    rot->rotateX(CLHEP::twopi*phi/360.);
	    rot->rotateY(CLHEP::twopi*theta/360.);
	    rot->rotateZ(CLHEP::twopi*psi/360.);

	    // G4VPhysicalVolume* PhysiComp =
	    new G4PVPlacement(
			      rot,			     // rotation
			      G4ThreeVector(x0,y0,z0),     // at (0,0,0)
			      lvol,  // its logical volume
			      "vol_"+G4String(count),	     // its name
			      marker,     // its mother  volume
			      false,		     // no boolean operation
			      0, BDSGlobalConstants::Instance()->CheckOverlaps());
	    
	    count++;
	    
	  } else Error();
      }
      if(token=="Trd") {   // trapezoid
	if(GetWord() == "{")
	  {
	    
	    while((token = GetWord()))
	      {
		if (token == "}") break;
		
		GetParameter(x0,"x0",token); // origin
		GetParameter(y0,"y0",token);
		GetParameter(z0,"z0",token);
		GetParameter(x1,"x1",token); //half length at wider side
		GetParameter(x2,"x2",token); //half length at narrow side
		GetParameter(y1,"y1",token); //half lengths
		GetParameter(y2,"y2",token); //half lengths
		GetParameter(z,"z",token);
		GetParameter(phi,"phi",token);
		GetParameter(theta,"theta",token);
		GetParameter(psi,"psi",token);
		GetParameter(material,"material",token);
	      }
	    
	    //create Box
	    
	    theMaterial = BDSMaterials::Instance()->GetMaterial(material);
	    
	    
	    G4cout<<"creating trd : "<<x0<<"  "<<y0<<" "<<z0<<
		  z<<" "<<y<<" "<<x1<<" "<<x2<<G4endl;
	    
	    
	    aTrd = new G4Trd("aTrd" + G4String(count),
			     x1,x2,   // inner R
			     y1,y2,
			     z);
	    
	    lvol = new G4LogicalVolume(aTrd,
				       theMaterial,
				       "_bmp_logical");

	    // place into mother volume

	    G4RotationMatrix *rot = new G4RotationMatrix;
	    rot->rotateX(CLHEP::twopi*phi/360.);
	    rot->rotateY(CLHEP::twopi*theta/360.);
	    rot->rotateZ(CLHEP::twopi*psi/360.);
	    
	    
	    // G4VPhysicalVolume* PhysiComp =
	    new G4PVPlacement(
			      rot,			     // rotation
			      G4ThreeVector(x0,y0,z0),     // at (0,0,0)
			      lvol,  // its logical volume
			      "vol_"+G4String(count),	     // its name
			      marker,     // its mother  volume
			      false,		     // no boolean operation
			      0, BDSGlobalConstants::Instance()->CheckOverlaps());
	    
	    count++;
	    
	  } else Error();
      }
    }


  return;
}


G4String BDSGeometryGMAD::GetWord()
{
  G4String str="";
  char c = 'a';

  while (inputf.good())// loop while extraction from file is possible
  {
    inputf.get(c);// get character from file

    // return char tokens 
    if(c=='=') return G4String(c);

    // skip whitespace
    if( (c != ' ' ) && (c != '\t' )&& (c != ',' ) && (c != '\n' )) break;
  }

  while (inputf.good())     // loop while extraction from file is possible
  {
    str += c;
    inputf.get(c);       // get character from file
    if( (c == ' ' ) || (c == '\t' )|| (c == ',' )|| (c == '\n' )|| (c == '=' ) ) 
      {
	inputf.putback(c);
	break;
      }
  }

  return str;

}

void BDSGeometryGMAD::GetParameter(G4double& x, G4String name, G4String lastToken)
{
  G4String token;

  if(lastToken == name)
    {
      token = GetWord();
      if(token == "=")
	{
	  token = GetWord();
	  x = strtod(token.c_str(),NULL);
	}
    }
}

void BDSGeometryGMAD::GetParameter(G4String& lval, G4String name, G4String lastToken)
{
  G4String token;

  if(lastToken == name)
    {
      token = GetWord();
      if(token == "=")
	{
	  token = GetWord();
	  lval = token;
	}
    }
}
