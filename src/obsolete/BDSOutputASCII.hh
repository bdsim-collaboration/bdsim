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
#ifndef BDSOUTPUTASCII_H
#define BDSOUTPUTASCII_H 

#include "BDSOutputBase.hh"

#include <fstream>

/**
 * @brief ASCII output class
 * 
 * Write BDSIM output to multiple ascii text files. Originally
 * part of BDSIM code base and recently developed and maintained
 * by Jochem Snuverink & Laurie Nevay
 * 
 * @author Laurie Nevay <Laurie.Nevay@rhul.ac.uk>
 */

class BDSOutputASCII: public BDSOutputBase
{
public: 

  BDSOutputASCII(); ///< default constructor
  //  BDSOutput(BDSOutputFormat format);
  virtual ~BDSOutputASCII();

  /// write sampler hit collection
  virtual void WriteHits(BDSSamplerHitsCollection* hc);
  /// make energy loss histo
  virtual void WriteEnergyLoss(BDSEnergyCounterHitsCollection* hc);
  /// write primary loss histo
  virtual void WritePrimaryLoss(BDSTrajectoryPoint* ploss);
  /// write primary hits histo
  virtual void WritePrimaryHit(BDSTrajectoryPoint* phit);
  /// write tunnel hits
  virtual void WriteTunnelHits(BDSEnergyCounterHitsCollection* hits);
  /// write a trajectory
  virtual void WriteTrajectory(std::vector<BDSTrajectory*> &TrajVec);
  /// write primary hit
  virtual void WritePrimary(G4double E,
			    G4double x0, G4double y0, G4double z0,
			    G4double xp, G4double yp, G4double zp,
			    G4double t,
			    G4double weight,
			    G4int PDGType,
			    G4int nEvent,
			    G4int TurnsTaken);
  /// write a histogram
  virtual void WriteHistogram(BDSHistogram1D* histogramIn);
  /// write event info
  virtual void WriteEventInfo(const time_t&  /*startTime*/,
			      const time_t&  /*stopTime*/,
			      const G4float& /*duration*/,
			      const std::string &/*seedStateAtStart*/)
  {;}
  virtual void WriteEventInfo(const BDSOutputROOTEventInfo* /*info*/){;}
  virtual void FillEvent() {;}
  virtual void Initialise(); ///< open the file
  /// Write to file
  virtual void Write(const time_t &startTime,
		     const time_t &stopTime,
		     const G4float &duration,
		     const std::string &seedStateAtStart);
  virtual void Close();      ///< close the file

private:
  G4String basefilename;
  G4String timestring;
  
  /// main output file
  std::ofstream ofMain;
  /// primaries output file
  std::ofstream ofPrimaries;
  /// energy loss hits output file
  std::ofstream ofELoss;
  /// primary loss hits output file
  std::ofstream ofPLoss;

  void WriteAsciiHit(std::ofstream* outfile, 
		     G4int    PDGType, 
		     G4double Mom, 
		     G4double X, 
		     G4double Y,
		     G4double Z,
		     G4double S, 
		     G4double XPrime, 
		     G4double YPrime, 
		     G4int    EventNo, 
		     G4double Weight, 
		     G4int    ParentID, 
		     G4int    TrackID, 
		     G4int    TurnsTaken);
};

extern BDSOutputBase* bdsOutput;
#endif
