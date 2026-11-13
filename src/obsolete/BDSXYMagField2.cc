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
//Based on the Geant4 example examples/advanced/purging_magnet/src/PurgMagTabulatedField3D.cc
#include "BDSGlobalConstants.hh"
#include "BDSXYMagField2.hh"

BDSXYMagField2::BDSXYMagField2( const char* filename ) 
  :invertX(false),invertY(false)
{    
 
  double lenUnit= m;
  double fieldUnit= tesla; 
  G4cout << "\n-----------------------------------------------------------"
	 << "\n      Magnetic field"
	 << "\n-----------------------------------------------------------";
    
  G4cout << "\n ---> " "Reading the field grid from " << filename << " ... " << G4endl; 
  std::ifstream file( filename ); // Open the file for reading.
  
  // Ignore first blank line
  char buffer[256];
  file.getline(buffer,256);
  
  // Read table dimensions 
  file >> nx >> ny; // Note dodgy order

  G4cout << "  [ Number of values x,y: " 
	 << nx << " " << ny << " ] "
	 << G4endl;

  // Set up storage space for table
  xField.resize( nx );
  yField.resize( nx );
  zField.resize( nx );
  int ix, iy;
  for (ix=0; ix<nx; ix++) {
    xField[ix].resize(ny);
    yField[ix].resize(ny);
    zField[ix].resize(ny);
  }
  
  // Ignore other header information    
  // The first line whose second character is '0' is considered to
  // be the last line of the header.
  do {
    file.getline(buffer,256);
  } while ( buffer[1]!='0');
  
  // Read in the data
  double xval,yval,bx,by,bz;
  int ii=0;
  for (ix=0; ix<nx; ix++) {
    for (iy=0; iy<ny; iy++) {
      file >> xval >> yval >> bx >> by >> bz;
      //      G4cout << ii << " " << xval << " " << yval << " " << bx << " " << by << " "  << bz << G4endl;
      ii++;
      if ( ix==0 && iy==0 ) {
	minx = xval * lenUnit;
	miny = yval * lenUnit;
      }
      xField[ix][iy] = bx * fieldUnit;
      yField[ix][iy] = by * fieldUnit;
      zField[ix][iy] = bz * fieldUnit;
    }
  }
  file.close();

  maxx = xval * lenUnit;
  maxy = yval * lenUnit;

  G4cout << "\n ---> ... done reading " << G4endl;

  // G4cout << " Read values of field from file " << filename << G4endl; 
  G4cout << " ---> assumed the order:  x, y, Bx, By, Bz "
	 << "\n ---> Min values x,y: " 
	 << minx/cm << " " << miny/cm << " cm "
	 << "\n ---> Max values x,y: " 
	 << maxx/cm << " " << maxy/cm << " " << G4endl;


  // Should really check that the limits are not the wrong way around.
  if (maxx < minx) {std::swap(maxx,minx); invertX = true;} 
  if (maxy < miny) {std::swap(maxy,miny); invertY = true;} 

  G4cout << "\nAfter reordering if neccesary"  
	 << "\n ---> Min values x,y: " 
	 << minx/cm << " " << miny/cm << " cm "
	 << " \n ---> Max values x,y: " 
	 << maxx/cm << " " << maxy/cm << " cm ";

  dx = maxx - minx;
  dy = maxy - miny;
  G4cout << "\n ---> Dif values x,y (range): " 
	 << dx/cm << " " << dy/cm << " " << " cm"
	 << "\n-----------------------------------------------------------" << G4endl;
}

void BDSXYMagField2::GetFieldValue(const double point[4],
				      double *Bfield ) const
{

  double x = point[0];
  double y = point[1];

  // Check that the point is within the defined region 
  if ( x>=minx && x<=maxx &&
       y>=miny && y<=maxy ) {
    
    // Position of given point within region, normalized to the range
    // [0,1]
    double xfraction = (x - minx) / dx;
    double yfraction = (y - miny) / dy; 

    if (invertX) { xfraction = 1 - xfraction;}
    if (invertY) { yfraction = 1 - yfraction;}

    // Need addresses of these to pass to modf below.
    // modf uses its second argument as an OUTPUT argument.
    double xdindex, ydindex;
    
    // Position of the point within the cuboid defined by the
    // nearest surrounding tabulated points
    double xlocal = ( std::modf(xfraction*(nx-1), &xdindex));
    double ylocal = ( std::modf(yfraction*(ny-1), &ydindex));
    
    // The indices of the nearest tabulated point whose coordinates
    // are all less than those of the given point
    int xindex = static_cast<int>(xdindex);
    int yindex = static_cast<int>(ydindex);
    

#ifdef DEBUG_INTERPOLATING_FIELD
    G4cout << "Local x,y: " << xlocal << " " << ylocal << G4endl;
    G4cout << "Index x,y: " << xindex << " " << yindex << G4endl;
    double valx0z0, mulx0z0, valx1z0, mulx1z0;
    double valx0z1, mulx0z1, valx1z1, mulx1z1;
    //   valx0z0= table[xindex  ][0];  mulx0z0=  (1-xlocal);
    //    valx1z0= table[xindex+1][0];  mulx1z0=   xlocal;
    //    valx0z1= table[xindex  ][0];  mulx0z1= (1-xlocal);
    //    valx1z1= table[xindex+1][0];  mulx1z1=  xlocal;
#endif

        // 2-dimensional version
    Bfield[0] =
      xField[xindex  ][yindex  ] * (1-xlocal) * (1-ylocal)  +
      xField[xindex  ][yindex  ] * (1-xlocal) * (1-ylocal)   +
      xField[xindex  ][yindex+1] * (1-xlocal) *    ylocal   +
      xField[xindex  ][yindex+1] * (1-xlocal) *    ylocal    +
      xField[xindex+1][yindex  ] *    xlocal  * (1-ylocal)  +
      xField[xindex+1][yindex  ] *    xlocal  * (1-ylocal)   +
      xField[xindex+1][yindex+1] *    xlocal  *    ylocal   +
      xField[xindex+1][yindex+1] *    xlocal  *    ylocal   ;
    Bfield[1] =
      yField[xindex  ][yindex  ] * (1-xlocal) * (1-ylocal)  +
      yField[xindex  ][yindex  ] * (1-xlocal) * (1-ylocal)   +
      yField[xindex  ][yindex+1] * (1-xlocal) *    ylocal   +
      yField[xindex  ][yindex+1] * (1-xlocal) *    ylocal    +
      yField[xindex+1][yindex  ] *    xlocal  * (1-ylocal)  +
      yField[xindex+1][yindex  ] *    xlocal  * (1-ylocal)   +
      yField[xindex+1][yindex+1] *    xlocal  *    ylocal   +
      yField[xindex+1][yindex+1] *    xlocal  *    ylocal   ;
    Bfield[2] =
      zField[xindex  ][yindex  ] * (1-xlocal) * (1-ylocal)  +
      zField[xindex  ][yindex  ] * (1-xlocal) * (1-ylocal)   +
      zField[xindex  ][yindex+1] * (1-xlocal) *    ylocal   +
      zField[xindex  ][yindex+1] * (1-xlocal) *    ylocal    +
      zField[xindex+1][yindex  ] *    xlocal  * (1-ylocal)  +
      zField[xindex+1][yindex  ] *    xlocal  * (1-ylocal)   +
      zField[xindex+1][yindex+1] *    xlocal  *    ylocal   +
      zField[xindex+1][yindex+1] *    xlocal  *    ylocal   ;

  } else {
    Bfield[0] = 0.0;
    Bfield[1] = 0.0;
    Bfield[2] = 0.0;
  }
}
