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


#include "BDSMagFieldXY.hh"

#include "globals.hh" // geant4 types / globals
#include "G4VPhysicalVolume.hh"

#include "CLHEP/Vector/Rotation.h"

#include <fstream>
#include <vector>

// global function?? perhaps should be removed or namespaced...
G4double GetNearestValue(std::vector<struct XYFieldRecord> fieldValues, G4double x, G4double y,
			 G4double &bx,G4double &by, G4double &bz);

BDSMagFieldXY::BDSMagFieldXY(G4String fname):
  Bx(nullptr),
  By(nullptr),
  Bz(nullptr),
  xHalf(0.0),
  yHalf(0.0),
  nX(0),
  nY(0),
  itsFileName(fname)
{;}

BDSMagFieldXY::~BDSMagFieldXY()
{
  // release the b-map memory
  if (Bx) {
    for(int i=0;i<nX;i++) {
      delete[] Bx[i];
    }
    delete[] Bx;
  }
  if (By) {
    for(int i=0;i<nX;i++) {
      delete[] By[i];
    }
    delete[] By;
  }
  if (Bz) {
    for(int i=0;i<nX;i++) {
      delete[] Bz[i];
    }
    delete[] Bz;
  }
}

G4bool  BDSMagFieldXY::DoesFieldChangeEnergy() const
{
  return false;
}

G4int BDSMagFieldXY::ReadFile(G4String fname)
{
#ifdef BDSDEBUG
  G4cout<<"reading file "<<fname<<G4endl;
#endif
  struct XYFieldRecord rec;
  

  // Open the file for reading.
  std::ifstream bmapif;
  bmapif.open(fname.c_str());
  G4String exceptionString = "Unable to load field map file: " + fname;
  if(!bmapif) G4Exception(exceptionString.c_str(), "-1", FatalException, "");

  while(bmapif.good())
    {
      bmapif>>rec.x>>rec.y>>rec.Bx>>rec.By>>rec.Bz; 
      
      if(!bmapif.good()) break;
#ifdef BDSDEBUG 
      G4cout<<"read: "<<rec.x<<" "<<rec.y<<" "<<rec.Bx<<" "<<rec.By<<" "<<rec.Bz<<" "<<G4endl;
#endif
      itsFieldValues.push_back(rec);
    }
  
  bmapif.close();
#ifdef BDSDEBUG
  G4cout<<"done"<<G4endl;
#endif
  return 0;

}

// create a field mesh in the "world" coordinates from list of field values
void BDSMagFieldXY::Prepare(G4VPhysicalVolume *referenceVolume)
{
#ifdef BDSDEBUG
  G4cout<<"BDSMagFieldXY:: create XY field mesh"<<G4endl;
#endif
  ReadFile(itsFileName);

  if( itsFieldValues.size() == 0 )
    {
      G4cerr<<"empty bmap file "<<itsFileName<<" exiting..."<<G4endl;
      exit(1);
    }
  
  const G4RotationMatrix* Rot= referenceVolume->GetFrameRotation();
  const G4ThreeVector Trans=referenceVolume->GetFrameTranslation();
  
  G4ThreeVector B; 
  
  // determine mesh physical dimensions
  std::vector<struct XYFieldRecord>::iterator it, itt;
  
  double xmax=0, ymax=0;

  for(it=itsFieldValues.begin();it!=itsFieldValues.end();it++)
    {
      if( fabs((*it).x ) > xmax ) xmax = fabs((*it).x);
      if( fabs((*it).y ) > ymax) ymax = fabs((*it).y);
    }

  //determine mesh step - minimum distance between measurement points
  double hx=xmax, hxold=xmax; //, hxmax=0;
  double hy=ymax, hyold=ymax; //, hymax=0;

  xHalf = xmax / 2;
  yHalf = ymax / 2;
  
  for(it=++itsFieldValues.begin();it!=itsFieldValues.end();it++)
    {
      for(itt=itsFieldValues.begin();itt!=itsFieldValues.end();itt++)
	{
#ifdef BDSDEBUG
	  G4cout<<(*it).x<<" "<<(*it).y<<" "<<" "<<(*it).Bx<<G4endl;
#endif
	  hxold = fabs((*it).x - (*itt).x);
	  if( (hxold > 1.0e-11*CLHEP::m)&&(hxold<hx) ) hx = hxold;
	  
	  hyold = fabs((*it).y - (*itt).y);
	  if( (hyold > 1.0e-11*CLHEP::m)&&(hyold<hy) ) hy = hyold; 
	}
    }
  
  //  hxmax = hx; hymax = hy;
  G4cout<<"h ="<<hx<<":"<<hy<<":"<<G4endl;
  G4cout<<"xmax ="<<xmax<<":"<<ymax<<":"<<G4endl;
  
  // allocate mesh
  G4int nX = static_cast<int> ( 2*xHalf / hx ) + 1;
  G4int nY = static_cast<int> ( 2*yHalf / hy ) + 1;

  G4cout<<"N ="<<nX<<":"<<nY<<G4endl;

  AllocateMesh(nX,nY);
  
  SetOriginRotation(*Rot);
  SetOriginTranslation(Trans);

  G4double bx = 0;
  G4double by = 0;
  G4double bz = 0;
  G4double x, y;

  for(int i=0; i<nX;i++)
    for(int j=0;j<nY;j++)
      {
	x =  i * hx - xHalf;
	y =  j * hy - yHalf;
	
	// find the closest measured point
	// if the point is further than ... set to zero 
	// has to be replaced by proper interpolation
	
	G4double dist = GetNearestValue(itsFieldValues,x,y,bx,by,bz);
	
	G4double tol = 100 * hx;// dummy
	
	G4cout << "dist, tol: " << dist << " " << tol << G4endl;

	if(dist < tol) {
	  SetBx(i,j,bx * CLHEP::tesla);
	  SetBy(i,j,by * CLHEP::tesla);
	  SetBz(i,j,bz * CLHEP::tesla);
	  
	} else {
	  SetBx(i,j,0.);
	  SetBy(i,j,0.);
	  SetBz(i,j,0.);
	}
      }  
  
  // dump the field mes for test
  G4cout<<"writing test file"<<G4endl;
  
  std::ofstream testf("btest.dat");
  
  for(int i=0; i<nX;i++)
    for(int j=0;j<nY;j++)
      {
	testf<<i<<" "<<j<<" "" "<<
	  GetBx(i,j)<<" "<<
	  GetBy(i,j)<<" "<<
	  GetBz(i,j)<<std::endl;
      }
  
  testf.close();
  
  isPrepared = true;
}

void BDSMagFieldXY::GetFieldValue(const G4double Point[4], G4double *Bfield ) const
{
  //Field must be prepared!
  CheckPrepared();

  G4double bx=0., by=0.;
#if BDSDEBUG
  G4double bz=0.;
#endif
  G4int i=0,j=0;

  G4ThreeVector local;

  if( (nX <= 0) || (nY<=0) )
    {
      G4cerr<<"BDSMagFieldXY::GetField> Error: no mesh"<<G4endl;
    }
  else
    {
      local[0] = Point[0] - translation[0];
      local[1] = Point[1] - translation[1];
      local[2] = Point[2] - translation[2];

      local *= Rotation();

      i = (G4int)(nX/2.0 + nX * local[0] / (2.0 * xHalf));
      j = (G4int)(nY/2.0 + nY * local[1] / (2.0 * yHalf));

      if( (i>=nX) || (j>=nY) || (i<0) || (j<0)){ 
	// outside mesh dimensions
	// 0 field
      } else {
	bx = Bx[i][j];
	by = By[i][j];
#if BDSDEBUG
	bz = Bz[i][j];
	G4cout << "Bx[" << i << "][" << j << "]=" << Bx[i][j] << G4endl;
	G4cout << "By[" << i << "][" << j << "]=" << By[i][j] << G4endl;
	G4cout << "Bz[" << i << "][" << j << "]=" << Bz[i][j] << G4endl;
	G4cout << "nX = " << nX << ", nY = " << nY << G4endl;
#endif
      }
    }

  // b-field
  Bfield[0] = bx;
  Bfield[1] = by;

  // e-field
  Bfield[3] = 0;
  Bfield[4] = 0;
  Bfield[5] = 0;

#ifdef BDSDEBUG
  G4cout<<" field value requested : "<<Point[0]<<" , "<<Point[1]<<" , "<<Point[2]<<" , "<<Point[3]<<" : "<<
    i<<" , "<<j<<" , "<<"    "<<local[0]<<" "<<local[1]<<" "<<local[2]<<" "<<bx<<" "<<by<<" "<<bz<<G4endl;
#endif
}

int BDSMagFieldXY::AllocateMesh(int nx, int ny) 
{
  nX = nx;
  nY = ny;
  
  Bx = new double*[nX];
  for(int i=0;i<nX;i++)
    {
      Bx[i] = new double[nY];
    }
  
  By = new double*[nX];
  for(int i=0;i<nX;i++)
    {
      By[i] = new double[nY];
    }
  
  Bz = new double*[nX];
  for(int i=0;i<nX;i++)
    {
      Bz[i] = new double[nY];
    }
  
  return 0;
}

G4double GetNearestValue(std::vector<struct XYFieldRecord> fieldValues, G4double x, G4double y,
			 G4double &bx,G4double &by, G4double &bz)
{
  std::vector<struct XYFieldRecord>::iterator it;

  G4double dist = 10.e+10;

  for(it = fieldValues.begin(); it!=fieldValues.end();it++)
    {
      dist = sqrt( (x-(*it).x)*(x-(*it).x) + (y-(*it).y)*(y-(*it).y));
      bx = (*it).Bx;
      by = (*it).By;
      bz = (*it).Bz;
    }

  return dist;
  
}

// code for 3d interpolation

// // create a field mesh in the "world" coordinates from list of field values
// void BDSMagFieldXY::Prepare(G4VPhysicalVolume *referenceVolume)
// {
//   G4cout<<"BDSMagFieldXY:: create XY field mesh"<<G4endl;
  
//   const G4RotationMatrix* Rot=referenceVolume->GetFrameRotation();
//   const G4ThreeVector Trans=referenceVolume->GetFrameTranslation();
  
//   G4ThreeVector B;


//   // mesh physical dimensions


//   std::vector<struct XYFieldRecord>::iterator it, itt;


//   double xmax=0, ymax=0;

//   for(it=itsFieldValues.begin();it!=itsFieldValues.end();it++)
//     {
//       if( fabs((*it).x ) > xmax ) xmax = fabs((*it).x);
//       if( fabs((*it).y ) > ymax) ymax = fabs((*it).y);
//     }

//   itsField->xHalf = xmax;
//   itsField->yHalf = ymax;
//   itsField->zHalf = (zmax > 0) ? zmax : itsLength/2;

//   G4double elementSizeX = itsField->xHalf;
//   G4double elementSizeY = itsField->yHalf;

//   //determine mesh step - minimum distance between measurement points
  
//   double hx=elementSizeX*2, hxold=elementSizeX*2, hxmax=0;
//   double hy=elementSizeY*2, hyold=elementSizeY*2, hymax=0;
//   double hz=itsLength, hzold=itsLength, hzmax=0;


//   for(it=++itsFieldValues.begin();it!=itsFieldValues.end();it++)
//     {

//       for(itt=itsFieldValues.begin();itt!=itsFieldValues.end();itt++)
// 	{
      
// 	  //G4cout<<(*it).x<<" "<<(*it).y<<" "<<(*it).z<<" "<<(*it).Bx<<G4endl;
//             hxold = fabs((*it).x - (*itt).x);
//             if( (hxold > 1.0e-1)&&(hxold<hx) ) hx = hxold;
      
//             hyold = fabs((*it).y - (*itt).y);
//             if( (hyold > 1.0e-1)&&(hyold<hy) ) hy = hyold;
      
//             hzold = fabs((*it).z - (*itt).z);
//             if( (hzold > 1.0e-1)&&(hzold<hz) ) hz = hzold;
// 	}
//     }

//   hxmax = hx;
//   hymax = hy;
//   hzmax = hz;

//   G4cout<<"h ="<<hx<<":"<<hy<<":"<<hz<<G4endl;
//   G4cout<<"xmax ="<<xmax<<":"<<ymax<<":"<<zmax<<G4endl;


//   // allocate mesh


//   G4int nX = static_cast<int> ( 2*elementSizeX / hx ) + 1;
//   G4int nY = static_cast<int> ( 2*elementSizeY / hy ) + 1;
//   G4int nZ = static_cast<int> ( itsLength / hz ) + 1;

  

//   G4cout<<"N ="<<nX<<":"<<nY<<G4endl;

//   AllocateMesh(nX,nY);
  
//   SetOriginRotation(Rot);
//   SetOriginTranslation(Trans);
  
//   itsField->AllocateMesh(nX,nY,nZ);


//   G4double rmax = 0.6 * sqrt(hxmax*hxmax + hymax*hymax + hzmax*hzmax);

//    // transverse maps
//   if (nZ ==2) rmax = sqrt(hxmax*hxmax + hymax*hymax);

//   G4cout<<"rmax ="<<rmax<<G4endl;

//   G4double x, y, z;
//   std::vector<struct FieldRecord> vals;
//   std::vector<double> rs;

//   for(int i=0; i<nX;i++)
//      for(int j=0;j<nY;j++)
//        for(int k=0; k<nZ;k++)
// 	 {
// 	   x =  i * hx - elementSizeX;
// 	   y =  j * hy - elementSizeY;
// 	   z =  k * hz - itsLength/2;
	   
// 	   //G4cout<<" x="<<x<<" y="<<y<<" z="<<z<<G4endl;
// 	   //G4cout<<" i="<<i<<" j="<<j<<" k="<<k<<G4endl;

// 	   for(it=itsFieldValues.begin();it!=itsFieldValues.end();it++)
// 	     {
// 	       G4double r = sqrt( ((*it).x-x)*((*it).x-x) +
// 				  ((*it).y-y)*((*it).y-y) + 
// 				  ((*it).z-z)*((*it).z-z) );

// 	       // transverse maps
// 	       if (nZ ==2) r = sqrt( ((*it).x-x)*((*it).x-x) +
// 				     ((*it).y-y)*((*it).y-y));

// 	       if(r < 1.e-3) r = 1.e-3;

// 	       //G4cout<<"trying "<<(*it).x<<" "<<(*it).y<<" "<<(*it).z<<G4endl;
// 	       //G4cout<<"r="<<r<<G4endl;
	       
// 	       if( r < rmax  )
// 		 {
// 		   //G4cout<<"yes"<<G4endl;

// 		   vals.push_back((*it));
// 		   rs.push_back(r);
// 		 }
// 	     }

// 	   //compute weighted means

// 	   G4int N = rs.size();

// 	   G4double bxmean=0, bymean=0,bzmean=0,rmean=0;

// 	   //G4cout<<"computing mean...N="<<N<<G4endl;

// 	   for(int npt=0;npt<N;npt++)
// 	     {
// 	       //G4cout<<"npt="<<npt<<G4endl;

// 	       bxmean += vals[npt].Bx / ( rs[npt] * rs[npt]* rs[npt] );
// 	       bymean += vals[npt].By / ( rs[npt] * rs[npt]* rs[npt] );
// 	       bzmean += vals[npt].Bz / ( rs[npt] * rs[npt]* rs[npt] );

// 	       rmean += 1/( rs[npt] * rs[npt]* rs[npt] );
// 	     }

// 	   if( N >0 )
// 	     {
// 	       bxmean /= (rmean);
// 	       bymean /= (rmean);
// 	       bzmean /= (rmean);
// 	     }
// 	   else
// 	     {
// 	       bxmean = 0;
// 	       bymean = 0;
// 	       bzmean = 0;
// 	     }
// 	   //G4cout<<"writing values..."<<i<<" "<<j<<" "<<k<<G4endl;
// 	   //G4cout<<"x"<<G4endl;
	   
// 	   itsField->SetBx(i,j,k,bxmean * CLHEP::tesla);
	   
// 	   itsField->SetBy(i,j,k,bymean * CLHEP::tesla);
	   
// 	   itsField->SetBz(i,j,k,bzmean * CLHEP::tesla);

	   

// 	   vals.clear();
// 	   rs.clear();
// 	 }


//   // dump the field mes for test

//   G4cout<<"writing test file"<<G4endl;

//   ofstream testf("btest.dat");

//    for(int i=0; i<nX;i++)
//      for(int j=0;j<nY;j++)
//        for(int k=0; k<nZ;k++)
// 	 {
// 	   testf<<i<<" "<<j<<" "<<k<<" "<<
// 	     itsField->GetBx(i,j,k)<<" "<<
// 	     itsField->GetBy(i,j,k)<<" "<<
// 	     itsField->GetBz(i,j,k)<<endl;
// 	 }

//    testf.close();

// }
