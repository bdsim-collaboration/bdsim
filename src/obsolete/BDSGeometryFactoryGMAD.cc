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
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSExtent.hh"
#include "BDSGeometryExternal.hh"
#include "BDSGeometryFactoryGMAD.hh"
#include "BDSGlobalConstants.hh"
#include "BDSMaterials.hh"

#include "globals.hh"
#include "G4Box.hh"
#include "G4Colour.hh"
#include "G4Cons.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"
#include "G4Trd.hh"
#include "G4Tubs.hh"
#include "G4VisAttributes.hh"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <map>
#include <set>

BDSGeometryFactoryGMAD::BDSGeometryFactoryGMAD()
{;}

BDSGeometryExternal* BDSGeometryFactoryGMAD::Build(G4String /*componentName*/,
						   G4String fileName,
						   std::map<G4String, G4Colour*>* mapping,
						   G4bool                  autoColour,
						   G4double              /*suggestedLength*/,
						   G4double              /*suggestedHorizontalWidth*/,
						   std::vector<G4String>* /*vacuumBiasVolumeNames*/)
{
  CleanUp();
  
  std::ifstream inputf;
  inputf.open(fileName);
  if (!inputf.good())
    {throw BDSException(__METHOD_NAME__, "Cannot open file \"" + fileName + "\"");}

  const G4String emptyMaterialName = BDSGlobalConstants::Instance()->EmptyMaterial();
  G4Material* emptyMaterial  = BDSMaterials::Instance()->GetMaterial(emptyMaterialName);

  // create a stupidly large container so that any placements shouldn't overlap.
  // accumulate extents and then replace the solid later.
  G4VSolid* containerSolid = new G4Box("container_solid",
				       1*CLHEP::km,
				       1*CLHEP::km,
				       1*CLHEP::km);
  G4LogicalVolume* containerLV = new G4LogicalVolume(containerSolid,
						     emptyMaterial,
						     "container_lv");
  // these are registered independently so aren't added to transient storage sets
  
  G4String token;
  G4String token1;

  G4double x0=0, y0=0, z0=0, x=0, y=0, z=0; // box parameters
  G4double rmin=0, rmax=0, rmin2=0, rmax2=0, phi0=0, dphi=0; // cylindrical parameters
  G4double x1=0, x2=0, y1=0, y2=0;
  G4double phi=0, theta=0, psi=0; // Euler angles - for rotation of components
  G4String materialName;

  G4VisAttributes*   visAttr = new G4VisAttributes(true, G4Colour(0.2,0.2,0.2));
  visAttr->SetForceSolid(true);
  allVisAttributes.insert(visAttr);

  G4int count = 0;

  while (inputf.good())
    {
      token = GetWord(inputf);
 
      if (token=="Box")
	{
	  if (GetWord(inputf) == "{")
	    {
	      while ((token = GetWord(inputf)))
		{
		  if (token == "}") break;
		  
		  GetParameter(inputf, x0,          "x0",      token); // origin
		  GetParameter(inputf, y0,          "y0",      token);
		  GetParameter(inputf, z0,          "z0",      token);
		  GetParameter(inputf, x,           "x",       token); //half lengths
		  GetParameter(inputf, y,           "y",       token);
		  GetParameter(inputf, z,           "z",       token);
		  GetParameter(inputf, phi,         "phi",     token);
		  GetParameter(inputf, theta,       "theta",   token);
		  GetParameter(inputf, psi,         "psi",     token);
		  GetParameter(inputf, materialName,"material",token);
		}
	      G4cout<<"creating box : "<<x0<<"  "<<y0<<" "<<z0<<G4endl;
	      
	      G4String name = "aBox_" + std::to_string(count);
	      auto solid = new G4Box(name,
				     x,   // half x
				     y,   // half y
				     z);  // half z

	      ExpandExtent(x0, x, y0, y, z0, z);
	      Finish(name, solid, materialName, containerLV, phi, theta, psi, x0, y0, z0);
	      count++;	    
	    }
	  else
	    {Error();}
	}// G4Box
      
      if (token=="Tubs")
	{
	  if (GetWord(inputf) == "{")
	    {
	      while ((token = GetWord(inputf)))
		{
		  if (token == "}") break;
		  
		  GetParameter(inputf, x0,"x0",token); // origin
		  GetParameter(inputf, y0,"y0",token);
		  GetParameter(inputf, z0,"z0",token);
		  GetParameter(inputf, rmin,"rmin",token); //half lengths
		  GetParameter(inputf, rmax,"rmax",token);
		  GetParameter(inputf, z,"z",token);
		  GetParameter(inputf, phi0,"phi0",token);
		  GetParameter(inputf, dphi,"dphi",token);
		  GetParameter(inputf, phi,"phi",token);
		  GetParameter(inputf, theta,"theta",token);
		  GetParameter(inputf, psi,"psi",token);
		  GetParameter(inputf, materialName,"material",token);
		}
	      G4cout<<"creating tubs : "<<x0<<"  "<<y0<<" "<<z0<<G4endl;
	      
	      // default - tube
	      phi0 = 0;
	      dphi = 360; // degrees
	      
	      G4String name = "aTubs_" + std::to_string(count);
	      auto solid = new G4Tubs(name,
				      rmin,             // inner R
				      rmax,             // outer R
				      z,                // z
				      phi0,             // phi 0 
				      dphi*CLHEP::deg); // delta phi
	      ExpandExtent(x0, rmax, y0, rmax, z0, z);
	      Finish(name, solid, materialName, containerLV, phi, theta, psi, x0, y0, z0);
	      count++;   
	    }
	  else
	    {Error();}
	}
      
      if (token=="Cons")
	{
	  if (GetWord(inputf) == "{")
	    {
	      
	      while ((token = GetWord(inputf)))
		{
		  if (token == "}")
		    {break;}
		
		  GetParameter(inputf, x0,"x0",token); // origin
		  GetParameter(inputf, y0,"y0",token);
		  GetParameter(inputf, z0,"z0",token);
		  GetParameter(inputf, rmin,"rmin1",token); //half lengths
		  GetParameter(inputf, rmax,"rmax1",token);
		  GetParameter(inputf, rmin2,"rmin2",token); //half lengths
		  GetParameter(inputf, rmax2,"rmax2",token);
		  GetParameter(inputf, z,"z",token);
		  GetParameter(inputf, phi0,"phi0",token);
		  GetParameter(inputf, dphi,"dphi",token);
		  GetParameter(inputf, phi,"phi",token);
		  GetParameter(inputf, theta,"theta",token);
		  GetParameter(inputf, psi,"psi",token);
		  GetParameter(inputf, materialName,"material",token);
		}
	      G4cout<<"creating cons : "<<x0<<"  "<<y0<<" "<<z0<<G4endl;
	      G4cout<<"rotation : "<<phi<<"  "<<theta<<" "<<psi<<G4endl;
	      
	      // default - tube
	      phi0 = 0;
	      dphi = 360; // degrees
	      
	      G4String name = "aCons_" + std::to_string(count);
	      auto solid = new G4Cons("aCons" + std::to_string(count),
				      rmin,   // inner R
				      rmax,
				      rmin2,
				      rmax2,
				      z,    //z
				      phi0, //phi 0 
				      dphi*CLHEP::deg); //delta phi
	      G4double rmm = std::max(rmax, rmax2);
	      ExpandExtent(x0, rmm, y0, rmm, z0, z);
	      Finish(name, solid, materialName, containerLV, phi, theta, psi, x0, y0, z0);
	      count++;
	      
	    }
	  else
	    {Error();}
	}
      if (token=="Trd")
	{// trapezoid
	  if (GetWord(inputf) == "{")
	    {
	      while ((token = GetWord(inputf)))
		{
		  if (token == "}") break;
		  
		  GetParameter(inputf, x0,"x0",token); // origin
		  GetParameter(inputf, y0,"y0",token);
		  GetParameter(inputf, z0,"z0",token);
		  GetParameter(inputf, x1,"x1",token); //half length at wider side
		  GetParameter(inputf, x2,"x2",token); //half length at narrow side
		  GetParameter(inputf, y1,"y1",token); //half lengths
		  GetParameter(inputf, y2,"y2",token); //half lengths
		  GetParameter(inputf, z,"z",token);
		  GetParameter(inputf, phi,"phi",token);
		  GetParameter(inputf, theta,"theta",token);
		  GetParameter(inputf, psi,"psi",token);
		  GetParameter(inputf, materialName,"material",token);
		}
	      
	      G4cout<<"creating trd : "<<x0<<"  "<<y0<<" "<<z0<<
		z<<" "<<y<<" "<<x1<<" "<<x2<<G4endl;
	      
	      G4String name = "aTrd_" + std::to_string(count);
	      auto solid = new G4Trd(name,
				     x1,x2,   // inner R
				     y1,y2,
				     z);
	      ExpandExtent(x0, -std::min(x1,x2), std::max(x1,x2),
			   y0, -std::min(y1,y2), std::max(y1,y2),
			   z0, -z, z);
	      Finish(name, solid, materialName, containerLV, phi, theta, psi, x0, y0, z0);
	      count++;
	      
	    }
	  else
	    {Error();}
	}
    }

  // close input file
  inputf.close();

  // update solid
  delete containerSolid; // delete existing solid
  containerSolid = new G4Box("container_solid",
			     std::max(std::abs(xmax), std::abs(xmin)),
                             std::max(std::abs(ymax), std::abs(ymin)),
                             std::max(std::abs(zmax), std::abs(zmin)));
  containerLV->SetSolid(containerSolid); // update container solid

  ApplyColourMapping(allLogicalVolumes, mapping, autoColour);
  ApplyUserLimits(allLogicalVolumes, BDSGlobalConstants::Instance()->DefaultUserLimits());

  BDSGeometryExternal* result = new BDSGeometryExternal(containerSolid, containerLV, Extent());
  result->RegisterRotationMatrix(allRotationMatrices);
  result->RegisterVisAttributes(allVisAttributes);
  result->RegisterSolid(allSolids);
  result->RegisterLogicalVolume(allLogicalVolumes);
  result->RegisterPhysicalVolume(allPhysicalVolumes);

  return result;
}

G4String BDSGeometryFactoryGMAD::GetWord(std::ifstream& inputf) const
{
  G4String str="";
  char c = 'a';

  while (inputf.good())// loop while extraction from file is possible
  {
    inputf.get(c);// get character from file

    // return char tokens 
    if (c=='=')
      {return std::to_string(c);}

    // skip whitespace
    if ( (c != ' ' ) && (c != '\t' )&& (c != ',' ) && (c != '\n' ))
      {break;}
  }

  while (inputf.good())     // loop while extraction from file is possible
  {
    str += c;
    inputf.get(c);       // get character from file
    if ( (c == ' ' ) || (c == '\t' )|| (c == ',' )|| (c == '\n' )|| (c == '=' ) ) 
      {
	inputf.putback(c);
	break;
      }
  }
  return str;
}

void BDSGeometryFactoryGMAD::GetParameter(std::ifstream&  inputf,
					                      G4double&       x,
					                      const G4String& name,
					                      const G4String& lastToken) const
{
  G4String token;

  if (lastToken == name)
    {
      token = GetWord(inputf);
      if (token == "=")
	{
	  token = GetWord(inputf);
	  x = strtod(token.c_str(),nullptr);
	}
    }
}

void BDSGeometryFactoryGMAD::GetParameter(std::ifstream&  inputf,
					                      G4String&       lval,
					                      const G4String& name,
					                      const G4String& lastToken) const
{
  G4String token;

  if (lastToken == name)
    {
      token = GetWord(inputf);
      if (token == "=")
	{
	  token = GetWord(inputf);
	  lval = token;
	}
    }
}

void BDSGeometryFactoryGMAD::Finish(G4String         name,
				    G4VSolid*        solid,
				    G4String         materialName,
				    G4LogicalVolume* mother,
				    G4double         phi,
				    G4double         theta,
				    G4double         psi,
				    G4double         x0,
				    G4double         y0,
				    G4double         z0)
{
  allSolids.insert(solid);
  
  G4Material* material = BDSMaterials::Instance()->GetMaterial(materialName);
  
  auto lv = new G4LogicalVolume(solid,
				material,
				name + "_lv");
  allLogicalVolumes.insert(lv);
  
  auto rot = new G4RotationMatrix;
  rot->rotateX(phi*CLHEP::deg);
  rot->rotateY(theta*CLHEP::deg);
  rot->rotateZ(psi*CLHEP::deg);
  allRotationMatrices.insert(rot);
  
  auto pv = new G4PVPlacement(rot,		       // rotation
			      G4ThreeVector(x0,y0,z0), // at (x0,y0,z0)
			      lv,                      // its logical volume
			      name + "_pv",            // its name
			      mother,                  // its mother  volume
			      false,		       // no boolean operation
			      0,
			      checkOverlaps);
  allPhysicalVolumes.insert(pv);
}
