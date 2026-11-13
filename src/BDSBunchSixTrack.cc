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
#include "BDSBunchSixTrack.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSParticleCoordsFull.hh"

#include "parser/beam.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>
#include <fstream>

BDSBunchSixTrack::BDSBunchSixTrack():
  BDSBunch("sixtrack")
{
  useCurvilinear = true; // need to always convert from curvilinear
  iPart = 0;
  nPart = 0;
}

BDSBunchSixTrack::~BDSBunchSixTrack()
{
  for (auto i : sixtrackData)
    {delete i;}
}

void BDSBunchSixTrack::SetOptions(const BDSParticleDefinition* beamParticle,
                                  const GMAD::Beam& beam,
                                  const BDSBunchType& distrType,
                                  G4Transform3D beamlineTransformIn,
                                  const G4double beamlineSIn)
{
  BDSBunch::SetOptions(beamParticle, beam, distrType, beamlineTransformIn, beamlineSIn);
  fileName = G4String(beam.distrFile); 
  LoadSixTrackFile();
}

BDSParticleCoordsFull BDSBunchSixTrack::GetNextParticleLocal()
{
  G4double s  = sixtrackData[iPart][2] * CLHEP::m;
  G4double x  = sixtrackData[iPart][3] * CLHEP::mm; 
  G4double xp = sixtrackData[iPart][4] * CLHEP::mrad;
  G4double y  = sixtrackData[iPart][5] * CLHEP::mm;
  G4double yp = sixtrackData[iPart][6] * CLHEP::mrad;
  G4double E  = E0 * (1 + sixtrackData[iPart][7]);
  G4double zp = CalculateZp(xp,yp,Zp0);
  
  iPart++;

  // if all particles are read, start at 0 again
  if (iPart == nPart)
    {
      iPart=0;
      G4cout << __METHOD_NAME__ << "End of file reached. Returning to beginning of file." << G4endl;
    }
  return BDSParticleCoordsFull(x,y,s,xp,yp,zp,T0,s,E,/*weight=*/1.0);
}

void BDSBunchSixTrack::LoadSixTrackFile()
{ 
  // SixTrack file is always LPI file
  // header LPI : 1=sixtrackParticleID, 2=turn, 3=s [m], 4=x[mm], 5=xp[mrad], 6=y[mm], 7=yp[mrad], 8=dE/E, 9=type, 10=turns in machine after first hits on collimators

  // open file and read line by line and extract values
  std::ifstream infile(fileName.c_str());

  if (!infile)
    {throw BDSException(__METHOD_NAME__, "\"" + fileName + "\" file doesn't exist - exiting as no input");}
  else
    {G4cout << __METHOD_NAME__ << "Reading as SixTrack input " << fileName << "\"" << G4endl;}
  
  // variable for storage
  double sixtrackParticleID = 0.0;
  double turn  = 0.0;
  double s     = 0.0;
  double x     = 0.0; 
  double xp    = 0.0;
  double y     = 0.0;
  double yp    = 0.0;
  double en    = 0.0;
  double type  = 0.0;
  double turns = 0.0;

  // read single line 
  while (infile >> sixtrackParticleID >> turn >> s >> x >> xp >> y >> yp >> en >> type >> turns)
    {
      double* values =  new double[10];
    
      // append values to storage vector
      values[0] = sixtrackParticleID;
      values[1] = turn;
      values[2] = s;
      values[3] = x;
      values[4] = xp;
      values[5] = y;
      values[6] = yp;
      values[7] = en;
      values[8] = type;
      values[9] = turns;
      
      sixtrackData.push_back(values);
    }

  nPart = sixtrackData.size();
  infile.close();
}
