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
#include "BDSMagField3D.hh"
#include "BDSDebug.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"
#include "G4VPhysicalVolume.hh"

#include <fstream>

BDSMagField3D::BDSMagField3D(const char*   filename,
			     G4ThreeVector offsetIn):
  offset(offsetIn),
  invertX(false),
  invertY(false),
  invertZ(false)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "filename: " << filename << G4endl;
#endif
  lenUnit   = CLHEP::cm;
  fieldUnit = CLHEP::tesla; 

  // Open the file for reading.
  std::ifstream file;
  file.open(filename);
  G4String exceptionString = "Unable to load field map file: " + (G4String)filename;
  if(!file) G4Exception(exceptionString.c_str(), "-1", FatalException, "");
  
  // Ignore first blank line
  char buffer[256];
  file.getline(buffer,256);
  
  // Read table dimensions 
  file >> nx >> ny >> nz; // Note dodgy order

#ifdef BDSDEBUG
  G4cout << "  [ Number of values x,y,z: " 
	 << nx << " " << ny << " " << nz << " ] "
	 << G4endl;
#endif
  
  // Set up storage space for table
  // std::vector.resize initialises (as opposed to reserve)
  xField.resize(nx);
  yField.resize(nx);
  zField.resize(nx);
  int ix, iy, iz;
  for (ix=0; ix<nx; ix++)
    {
      xField[ix].resize(ny);
      yField[ix].resize(ny);
      zField[ix].resize(ny);
      for (iy=0; iy<ny; iy++)
	{
	  xField[ix][iy].resize(nz);
	  yField[ix][iy].resize(nz);
	  zField[ix][iy].resize(nz);
	}
    }
  
  // Ignore other header information    
  // The first line whose second character is '0' is considered to
  // be the last line of the header.
  do
    {file.getline(buffer,256);}
  while ( buffer[1]!='0');
  
  //Set up progress display
  //double ndis = static_cast<double>(nx);
  //BDSProgressBar* progDis = new BDSProgressBar(ndis);
  //double inc = 1;

  // Read in the data
  double xval=0.0,yval=0.0,zval=0.0,bx,by,bz;
  //  double permeability; // Not used in this example.
  for (ix=0; ix<nx; ix++) {
    //    G4cout << progDis->count() << " " << progDis->maxCount() << G4endl;
     for (iy=0; iy<ny; iy++) {
      for (iz=0; iz<nz; iz++) {
        file >> xval >> yval >> zval >> bx >> by >> bz;
#ifdef BDSDEBUG
	G4cout << "Read: " << xval << " " << yval << " " << zval << " " << bx << " " << by << " " << bz << G4endl;
#endif
        if ( ix==0 && iy==0 && iz==0 ) {
          minx = xval * lenUnit + offset.x();
          miny = yval * lenUnit + offset.y();
          minz = zval * lenUnit + offset.z();
        }
        xField[ix][iy][iz] = bx * fieldUnit;
        yField[ix][iy][iz] = by * fieldUnit;
        zField[ix][iy][iz] = bz * fieldUnit;
      }
    }
     //progDis->increment(inc);
  }
  file.close();

  maxx = xval * lenUnit + offset.x();
  maxy = yval * lenUnit + offset.y();
  maxz = zval * lenUnit + offset.z();

  G4cout << __METHOD_NAME__ << "finished reading file" << G4endl;

#ifdef BDSDEBUG
  G4cout << " ---> assumed the order:  x, y, z, Bx, By, Bz "
	 << "\n ---> Min values x,y,z: " 
	 << minx/CLHEP::cm << " " << miny/CLHEP::cm << " " << minz/CLHEP::cm << " cm "
	 << "\n ---> Max values x,y,z: " 
	 << maxx/CLHEP::cm << " " << maxy/CLHEP::cm << " " << maxz/CLHEP::cm << " cm "
	 << "The field will be offset by " << offset << " m" << G4endl;
#endif
  // Should really check that the limits are not the wrong way around.
  if (maxx < minx) {std::swap(maxx,minx); invertX = true;} 
  if (maxy < miny) {std::swap(maxy,miny); invertY = true;} 
  if (maxz < minz) {std::swap(maxz,minz); invertZ = true;} 
#ifdef BDSDEBUG
  G4cout << "\nAfter reordering if neccesary"  
	 << "\n ---> Min values x,y,z: " 
	 << minx/CLHEP::cm << " " << miny/CLHEP::cm << " " << minz/CLHEP::cm << " cm "
	 << " \n ---> Max values x,y,z: " 
	 << maxx/CLHEP::cm << " " << maxy/CLHEP::cm << " " << maxz/CLHEP::cm << " cm ";
#endif
  dx = maxx - minx;
  dy = maxy - miny;
  dz = maxz - minz;
#ifdef BDSDEBUG
  G4cout << "\n ---> Dif values x,y,z (range): " 
	 << dx/CLHEP::cm << " " << dy/CLHEP::cm << " " << dz/CLHEP::cm << " cm in z "
	 << "\n-----------------------------------------------------------" << G4endl;
#endif
}

void BDSMagField3D::GetFieldValue(const double point[4],
				      double *Bfield ) const
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif

  //Default - field is zero.
  Bfield[0]=0; Bfield[1]=0; Bfield[2]=0;
   
  CheckPrepared();

  G4ThreeVector local;

  local[0] = point[0] + translation[0] - offset.x();
  local[1] = point[1] + translation[1] - offset.y();
  local[2] = point[2] + translation[2] - offset.z();
  local *= Rotation();

#ifdef BDSDEBUG
  G4cout << "BDSMagField3D::GetFieldValue" << G4endl;
  G4cout << "point x       = " << point[0]/CLHEP::cm       << " cm" << G4endl;
  G4cout << "point y       = " << point[1]/CLHEP::cm       << " cm" << G4endl;
  G4cout << "point z       = " << point[2]/CLHEP::cm       << " cm" << G4endl;
  G4cout << "translation x = " << translation[0]/CLHEP::cm << " cm" << G4endl;
  G4cout << "translation y = " << translation[1]/CLHEP::cm << " cm" << G4endl;
  G4cout << "translation z = " << translation[2]/CLHEP::cm << " cm" << G4endl;
  G4cout << "offset        = " << offset/CLHEP::cm         << " cm" << G4endl;
  G4cout << "local x       = " << local[0]/CLHEP::cm       << " cm" << G4endl;
  G4cout << "local y       = " << local[1]/CLHEP::cm       << " cm" << G4endl;
  G4cout << "local z       = " << local[2]/CLHEP::cm       << " cm" << G4endl;
#endif

  double signy=1;
  double signz=1;

  //Mirror in x=0 plane and z=0 plane
  /*
  if( local[0] < 0 ){
#ifdef BDSDEBUG
    G4cout << "x = " << local[0]/cm << " cm. Mirroring in x=0 plane." << G4endl;
#endif
    local[0] *= -1;
    signy = -1;
    signz = -1;
  }

  if( local[2] <0 ){
#ifdef BDSDEBUG
    G4cout << "z = " << local[2]/cm << " cm. Mirroring in z=0 plane." << G4endl;
#endif
    local[2] *= -1;
    signz = -1;
  }
  */
  // Check that the point is within the defined region 
  if ( local[0]>=minx && local[0]<=maxx &&
       local[1]>=miny && local[1]<=maxy && 
       local[2]>=minz && local[2]<=maxz ) {
    
    // Position of given point within region, normalized to the range
    // [0,1]
    double xfraction = (local[0] - minx) / dx;
    double yfraction = (local[1] - miny) / dy; 
    double zfraction = (local[2] - minz) / dz;
    
    if (invertX) { xfraction = 1 - xfraction;}
    if (invertY) { yfraction = 1 - yfraction;}
    if (invertZ) { zfraction = 1 - zfraction;}
    
    // Need addresses of these to pass to modf below.
    // modf uses its second argument as an OUTPUT argument.
    double xdindex, ydindex, zdindex;
    
    // Position of the point within the cuboid defined by the
    // nearest surrounding tabulated points
    double xlocal = ( std::modf(xfraction*(nx-1), &xdindex));
    double ylocal = ( std::modf(yfraction*(ny-1), &ydindex));
    double zlocal = ( std::modf(zfraction*(nz-1), &zdindex));
    
    // The indices of the nearest tabulated point whose coordinates
    // are all less than those of the given point
    int xindex = static_cast<int>(xdindex);
    int yindex = static_cast<int>(ydindex);
    int zindex = static_cast<int>(zdindex);
    //The indices must be less than nx-1 in order to be able to interpolate
    if((xindex<nx-1) && (yindex < ny-1) && (zindex < nz -1) &&
       (xindex>0) && (yindex >0) && (zindex >0)){
#ifdef DEBUG_INTERPOLATING_FIELD
      G4cout << "Local x,y,z: " << xlocal << " " << ylocal << " " << zlocal << G4endl;
      G4cout << "Index x,y,z: " << xindex << " " << yindex << " " << zindex << G4endl;
      G4cout << "n x,y,z: " << nx << " " << ny << " " << nz << G4endl;
      double valx0z0, mulx0z0, valx1z0, mulx1z0;
      double valx0z1, mulx0z1, valx1z1, mulx1z1;
      //    valx0z0= table[xindex  ][0][zindex];  mulx0z0=  (1-xlocal) * (1-zlocal);
      //    valx1z0= table[xindex+1][0][zindex];  mulx1z0=   xlocal    * (1-zlocal);
      //    valx0z1= table[xindex  ][0][zindex+1]; mulx0z1= (1-xlocal) * zlocal;
      //    valx1z1= table[xindex+1][0][zindex+1]; mulx1z1=  xlocal    * zlocal;
#endif
      
      // Full 3-dimensional version
      Bfield[0] =
	xField[xindex  ][yindex  ][zindex  ] * (1-xlocal) * (1-ylocal) * (1-zlocal) +
	xField[xindex  ][yindex  ][zindex+1] * (1-xlocal) * (1-ylocal) *    zlocal  +
	xField[xindex  ][yindex+1][zindex  ] * (1-xlocal) *    ylocal  * (1-zlocal) +
	xField[xindex  ][yindex+1][zindex+1] * (1-xlocal) *    ylocal  *    zlocal  +
	xField[xindex+1][yindex  ][zindex  ] *    xlocal  * (1-ylocal) * (1-zlocal) +
	xField[xindex+1][yindex  ][zindex+1] *    xlocal  * (1-ylocal) *    zlocal  +
	xField[xindex+1][yindex+1][zindex  ] *    xlocal  *    ylocal  * (1-zlocal) +
	xField[xindex+1][yindex+1][zindex+1] *    xlocal  *    ylocal  *    zlocal ;
      Bfield[1] = signy *(
			  yField[xindex  ][yindex  ][zindex  ] * (1-xlocal) * (1-ylocal) * (1-zlocal) +
			  yField[xindex  ][yindex  ][zindex+1] * (1-xlocal) * (1-ylocal) *    zlocal  +
			  yField[xindex  ][yindex+1][zindex  ] * (1-xlocal) *    ylocal  * (1-zlocal) +
			  yField[xindex  ][yindex+1][zindex+1] * (1-xlocal) *    ylocal  *    zlocal  +
			  yField[xindex+1][yindex  ][zindex  ] *    xlocal  * (1-ylocal) * (1-zlocal) +
			  yField[xindex+1][yindex  ][zindex+1] *    xlocal  * (1-ylocal) *    zlocal  +
			  yField[xindex+1][yindex+1][zindex  ] *    xlocal  *    ylocal  * (1-zlocal) +
			  yField[xindex+1][yindex+1][zindex+1] *    xlocal  *    ylocal  *    zlocal );
      Bfield[2] = signz *(
			  zField[xindex  ][yindex  ][zindex  ] * (1-xlocal) * (1-ylocal) * (1-zlocal) +
			  zField[xindex  ][yindex  ][zindex+1] * (1-xlocal) * (1-ylocal) *    zlocal  +
			  zField[xindex  ][yindex+1][zindex  ] * (1-xlocal) *    ylocal  * (1-zlocal) +
			  zField[xindex  ][yindex+1][zindex+1] * (1-xlocal) *    ylocal  *    zlocal  +
			  zField[xindex+1][yindex  ][zindex  ] *    xlocal  * (1-ylocal) * (1-zlocal) +
			  zField[xindex+1][yindex  ][zindex+1] *    xlocal  * (1-ylocal) *    zlocal  +
			  zField[xindex+1][yindex+1][zindex  ] *    xlocal  *    ylocal  * (1-zlocal) +
			  zField[xindex+1][yindex+1][zindex+1] *    xlocal  *    ylocal  *    zlocal );
    }else{
#ifdef DEBUG_INTERPOLATING_FIELD
      G4cout << __METHOD_NAME__ << " - point at edge of field map. Unable to interpolate. Setting field to zero."  << G4endl;
#endif
    }
  } else {
#ifdef DEBUG_INTERPOLATING_FIELD
    G4cout << __METHOD_NAME__ << " - outside field map. Unable to interpolate. Setting field to zero."  << G4endl;
#endif
  }

#ifdef BDSDEBUG
  G4cout << "Bfield x,y,z = " << 
    Bfield[0] / fieldUnit << " " <<
    Bfield[1] / fieldUnit << " " <<
    Bfield[2] / fieldUnit <<
    G4endl;
  G4cout << __METHOD_END__ << G4endl;
#endif
}

void BDSMagField3D::Prepare(G4VPhysicalVolume *referenceVolume)
{
  isPrepared = true;
  const G4RotationMatrix* Rot = referenceVolume->GetFrameRotation();
  const G4ThreeVector Trans   = referenceVolume->GetFrameTranslation();
  SetOriginRotation(*Rot);
  SetOriginTranslation(Trans);
}
