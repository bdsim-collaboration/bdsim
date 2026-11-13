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
#ifndef BDSOUTPUTROOTEVENTTRAJECTORYPOINT_H
#define BDSOUTPUTROOTEVENTTRAJECTORYPOINT_H
#include "Rtypes.h"
#include "TVector3.h"

/**
 * @brief Structure to record a trajectory point.
 * 
 * @author Stewart Boogert
 */

class BDSOutputROOTEventTrajectoryPoint: public TObject
{
public:
  BDSOutputROOTEventTrajectoryPoint();
  BDSOutputROOTEventTrajectoryPoint(int             partIDIn,
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
				    int    stepIndexIn = -1);
  virtual ~BDSOutputROOTEventTrajectoryPoint();
  
  void ClearContents();
  virtual void Clear(Option_t*) {ClearContents();}
  
  int          partID;
  unsigned int trackID;
  unsigned int parentID;
  unsigned int parentIndex;
  int      postProcessType;
  int      postProcessSubType;
  double   weight;
  double   energyDeposited;
  TVector3 position;
  TVector3 momentum;
  int      model;
  double   time;
  TVector3 positionLocal;
  TVector3 momentumLocal;
  double   charge;
  double   kineticEnergy;
  int      turnsTaken;
  double   rigidity;
  double   mass;
  bool     isIon;
  int      ionA;
  int      ionZ;
  int      nElectrons;
  int      materialID; ///< We allow this to be signed so we can use -1 to signify unknown.
  int      stepIndex;  ///< Index along trajectory this point is.

  ClassDef(BDSOutputROOTEventTrajectoryPoint,6);
};

#endif
