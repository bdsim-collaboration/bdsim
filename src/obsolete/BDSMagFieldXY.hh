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
#ifndef BDSXYMAGFIELD_H
#define BDSXYMAGFIELD_H

#include "BDSMagFieldMesh.hh"

#include "globals.hh"
#include "G4ElectroMagneticField.hh"
#include "G4RotationMatrix.hh"

#include <vector>


struct XYFieldRecord
{
  G4double x;
  G4double y;
  G4double Bx;
  G4double By;
  G4double Bz;
};

class BDSMagFieldXY: public BDSMagFieldMesh
{
public:
  // mandatory members
  BDSMagFieldXY(G4String fname);
  
  ~BDSMagFieldXY();

  G4bool DoesFieldChangeEnergy() const;

  void GetFieldValue(const G4double Point[4],G4double *Bfield ) const;



  // aux members

  G4int AllocateMesh(G4int nX, G4int nY);

  G4int ReadFile(G4String fname);

  virtual void Prepare(G4VPhysicalVolume *referenceVolume);

  void SetBx(G4int i,G4int j,G4double val);
  void SetBy(G4int i,G4int j,G4double val);
  void SetBz(G4int i,G4int j,G4double val);

  G4double GetBx(G4int i,G4int j);
  G4double GetBy(G4int i,G4int j);
  G4double GetBz(G4int i,G4int j);

private:
  
  G4double **Bx, **By, **Bz;

  std::vector<struct XYFieldRecord> itsFieldValues;

public:
  G4double xHalf, yHalf; // field mesh dimensions

  G4int nX, nY; // dimensions
  
  G4String itsFileName;
  
};

inline void BDSMagFieldXY::SetBx(int i,int j,double val)
{Bx[i][j] = val;}

inline void BDSMagFieldXY::SetBy(int i,int j,double val)
{By[i][j] = val;}

inline void BDSMagFieldXY::SetBz(int i,int j,double val)
{Bz[i][j] = val;}

inline G4double BDSMagFieldXY::GetBx(int i,int j)
{return Bx[i][j];}

inline G4double BDSMagFieldXY::GetBy(int i,int j)
{return By[i][j];}

inline G4double BDSMagFieldXY::GetBz(int i,int j)
{return Bz[i][j];}

#endif
