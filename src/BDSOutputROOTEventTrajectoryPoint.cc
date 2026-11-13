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
#include "BDSOutputROOTEventTrajectoryPoint.hh"

ClassImp(BDSOutputROOTEventTrajectoryPoint)

BDSOutputROOTEventTrajectoryPoint::BDSOutputROOTEventTrajectoryPoint()
{
  ClearContents();
}

BDSOutputROOTEventTrajectoryPoint::BDSOutputROOTEventTrajectoryPoint(int             partIDIn,
								     unsigned int    trackIDIn,
								     unsigned int    parentIDIn,
								     unsigned int    parentIndexIn,
								     int             processTypeIn,
								     int             processSubTypeIn,
								     double          weightIn,
								     double          energyDepositedIn,
								     const TVector3& positionIn,
								     const TVector3& momentumIn,
								     int             modelIn,
								     double          timeIn,
								     const TVector3& positionLocalIn,
								     const TVector3& momentumLocalIn,
								     double chargeIn,
								     double kineticEnergyIn,
								     int    turnsTakenIn,
								     double rigidityIn,
								     double massIn,
								     bool   isIonIn,
								     int    ionAIn,
								     int    ionZIn,
								     int    nElectronsIn,
								     int    materialIDIn,
								     int    stepIndexIn):
  partID(partIDIn),
  trackID(trackIDIn),
  parentID(parentIDIn),
  parentIndex(parentIndexIn),
  postProcessType(processTypeIn),
  postProcessSubType(processSubTypeIn),
  weight(weightIn),
  energyDeposited(energyDepositedIn),
  position(positionIn),
  momentum(momentumIn),
  model(modelIn),
  time(timeIn),
  positionLocal(positionLocalIn),
  momentumLocal(momentumLocalIn),
  charge(chargeIn),
  kineticEnergy(kineticEnergyIn),
  turnsTaken(turnsTakenIn),
  rigidity(rigidityIn),
  mass(massIn),
  isIon(isIonIn),
  ionA(ionAIn),
  ionZ(ionZIn),
  nElectrons(nElectronsIn),
  materialID(materialIDIn),
  stepIndex(stepIndexIn)
{;}

void BDSOutputROOTEventTrajectoryPoint::ClearContents()
{
  partID = 0;
  trackID = 0;
  parentID = 0;
  parentIndex = 0;
  postProcessType = -1;
  postProcessSubType = -1;
  weight = -1.0;
  energyDeposited = -1.0;
  position = {0,0,0};
  momentum = {0,0,0};
  model = -1;
  time = 0;
  positionLocal = {0,0,0};
  momentumLocal = {0,0,0};
  charge = 0;
  kineticEnergy = 0;
  turnsTaken = 0;
  rigidity = 0;
  mass = 0;
  isIon = false;
  ionA = 0;
  ionZ = 0;
  nElectrons = 0;
  materialID = -1;
  stepIndex = -1;
}

BDSOutputROOTEventTrajectoryPoint::~BDSOutputROOTEventTrajectoryPoint()
{;}
