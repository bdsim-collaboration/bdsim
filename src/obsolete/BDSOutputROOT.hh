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
#ifndef BDSOUTPUTROOT_H
#define BDSOUTPUTROOT_H

#include "BDSOutputBase.hh"

#include "TH2F.h"
#include "TFile.h"
#include "TTree.h"

class BDSOutputROOTEventInfo;
class BDSTrajectory;
class BDSTrajectoryPoint;

/**
 * @brief Lightweight ROOT output class
 * 
 * This contains the basic ROOT output information.
 * 
 * @author Laurie Nevay
 */

template <typename Type> // Note 'T' conflicts with global time.
class BDSOutputROOT: public BDSOutputBase
{
public:
  BDSOutputROOT(); 
  virtual ~BDSOutputROOT();

  /// write sampler hit collection
  virtual void WriteHits(BDSSamplerHitsCollection*) override;
  /// make energy loss histo
  virtual void WriteEnergyLoss(BDSEnergyCounterHitsCollection*) override;
  /// write primary loss histo
  virtual void WritePrimaryLoss(BDSTrajectoryPoint* ploss) override;
  /// write primary hits histo
  virtual void WritePrimaryHit(BDSTrajectoryPoint* phit) override;
  /// write tunnel hits
  virtual void WriteTunnelHits(BDSEnergyCounterHitsCollection*) override;
  /// write a trajectory 
  virtual void WriteTrajectory(std::vector<BDSTrajectory*> &TrajVec) override;
  /// write primary hit
  virtual void WritePrimary(G4double totalEnergy,
			    G4double x0,
			    G4double y0,
			    G4double z0,
			    G4double xp,
			    G4double yp,
			    G4double zp,
			    G4double t,
			    G4double weight,
			    G4int    PDGType, 
			    G4int    nEvent, 
			    G4int    turnsTaken) override;

  /// write a histogram
  virtual void WriteHistogram(BDSHistogram1D* histogramIn) override;
  /// write event info
  virtual void WriteEventInfo(const time_t&  /*startTime*/,
			      const time_t&  /*stopTime*/,
			      const G4float& /*duration*/,
			      const std::string& /*seedStateAtStart*/) override {}
  virtual void WriteEventInfo(const BDSOutputROOTEventInfo* /*info*/) override {;}
  virtual void FillEvent() override {;} ///< Fill the event
  virtual void Initialise() override; ///< open the file
  virtual void Write(const time_t&  startTime,
		     const time_t&  stopTime,
		     const G4float& duration,
		     const std::string& seedStateAtStart) override;      ///< write to file
  virtual void Close() override;      ///< close the file

protected:
  /// The number type identifier string to put into root.
  G4String type;
  
  virtual TTree* BuildSamplerTree(G4String name);
  
  /// Write a root hit to file based on the sampler hit.
  virtual void WriteRootHit(TTree*         tree,
			    BDSSamplerHit* hit,
			    G4bool         fillTree = true);

  
  /// Write hit to TTree
  void WriteRootHit(TTree*   Tree,
		    G4double totalEnergy,
		    G4double x,
		    G4double y,
		    G4double z,
		    G4double xPrime,
		    G4double yPrime,
		    G4double zPrime,
		    G4double T,
		    G4double S,
		    G4double Weight,
		    G4int    PDGtype,
		    G4int    EventNo,
		    G4int    ParentID,
		    G4int    TrackID,
		    G4int    TurnsTaken,
		    G4String Process,
		    G4bool   fillTree = true);
  
  /// Fill members so that trees can be written
  void FillHit(BDSEnergyCounterHit* hit);

  /// Fill members so that tree can be written but based on Trajectory Point
  void FillHit(BDSTrajectoryPoint* hit);

  /// Members for writing to TTrees
  /// Local parameters
  Type x=0.0,xp=0.0,y=0.0,yp=0.0,z=0.0,zp=0.0,E=0.0,t=0.0,S=0.0;
  /// Global parameters
  Type X=0.0,Y=0.0,Z=0.0,T=0.0,weight=0.0,stepLength=0.0;
  /// PDG id, event number, parentID, trackID, turn number
  int part=-1,eventno=-1, pID=-1, track_id=-1, turnnumber=-1;
  std::string process;
  
  /// tunnel hits variables
  Type E_tun=0.0, S_tun=0.0, r_tun=0.0, angle_tun=0.0;

  /// vector of Sampler trees
  std::vector<TTree*> samplerTrees;

private:
  TFile* theRootOutputFile;
  
  TTree* PrecisionRegionEnergyLossTree;
  TTree* EnergyLossTree;
  TTree* PrimaryLossTree;
  TTree* PrimaryHitsTree;
  TTree* TunnelLossTree;

  TH2F*  tunnelHitsHisto;

  /// volume name
  char volumeName[100];
};

extern BDSOutputBase* bdsOutput;

#endif
