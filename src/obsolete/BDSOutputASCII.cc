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
#include "BDSGlobalConstants.hh"
#include "BDSOutputASCII.hh"
#include "BDSEnergyCounterHit.hh"
#include "BDSHistogram.hh"
#include "BDSSamplerHit.hh"
#include "BDSUtilities.hh"       // for BDS::non_alpha

#include "globals.hh" // geant4 types / globals

#include <cmath>
#include <ctime>
#include <string>
#include <fstream>
#include <ios>
#include <iomanip>
#include <sys/stat.h>

BDSOutputASCII::BDSOutputASCII()
{
  time_t currenttime;
  time(&currenttime);
  timestring = asctime(localtime(&currenttime));
  timestring = timestring.substr(0,timestring.size()-1);

  // initialise file in constructor since multiple file writing not implemented for ASCII
  // should in principle be in Initialise()
  
  // policy overwrite if output filename specifically set, otherwise increase
  // generate filenames
  basefilename = BDSGlobalConstants::Instance()->OutputFileName();
  G4String originalname = basefilename;
  // lots of files - make a directory with the users permissions
  // see sysstat.h, e.g. http://pubs.opengroup.org/onlinepubs/007908799/xsh/sysstat.h.html
  int status        = -1;
  int nTimeAppended = 0;
  while (status != 0)
    {
      status = mkdir(basefilename.c_str(),S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
      if (BDSGlobalConstants::Instance()->OutputFileNameSet())
	{
	  // if this directory already exists or not, use this directory (should be checked for other error codes)
	  break;
	}
      if (status != 0)
	{
	  if (nTimeAppended > 0)
	    {basefilename = basefilename.substr(0, basefilename.size()-3);}
	  basefilename += "_";
	  // if smaller than 10 add leading 0
	  if (nTimeAppended < 10)
	    {basefilename += std::to_string(0);}
	  basefilename += std::to_string(nTimeAppended);
	  nTimeAppended += 1;
	}
    }
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "output directory called \"" << basefilename << "\" created." << G4endl;
#endif
  if (nTimeAppended > 0)
    {
      G4cout << __METHOD_NAME__ << "output directory \""
	     << originalname << "\" already exists - making unique output directory called \""
	     << basefilename << "\"" << G4endl;
    }
  basefilename = basefilename + "/" + basefilename;
    
  filename = basefilename + ".txt"; //main output filename - for samplers
  G4String filenamePrimaries  = basefilename + ".primaries.txt"; // primaries
  G4String filenameELoss      = basefilename + ".eloss.txt";     // energy loss hits
  G4String filenamePLoss      = basefilename + ".ploss.txt";     // primary loss hits

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "Output format ASCII - filenames:" << G4endl;
  G4cout << "Output                 : " << filename           << G4endl;
  G4cout << "Primaries              : " << filenamePrimaries  << G4endl;
  G4cout << "Energy Loss Hits       : " << filenameELoss      << G4endl;
  G4cout << "Primary Loss Hits      : " << filenamePLoss      << G4endl;
#endif
  std::stringstream headerstream;
  headerstream << std::left << std::setprecision(10) << std::fixed
	       << std::setw(6)  << "PDGID"    << " "
	       << std::setw(15) << "E[GeV]"   << " "
	       << std::setw(15) << "x[m]"     << " "
	       << std::setw(15) << "y[m]"     << " "
	       << std::setw(15) << "Z[m]"     << " "
	       << std::setw(20) << "S[m]"     << " "
	       << std::setw(15) << "Xp[rad]"  << " "
	       << std::setw(15) << "Yp[rad]"  << " "
	       << std::setw(6)  << "NEvent"   << " "
	       << std::setw(15) << "Weight"   << " "
	       << std::setw(9)  << "ParentID" << " "
	       << std::setw(8)  << "TrackID"  << " "
	       << std::setw(5)  << "Turn"
	       << G4endl;
  G4String headerstring = headerstream.str();

  // main output file initialisation
  ofMain.open(filename.c_str());
  ofMain       << "### BDSIM output - created "<< timestring << G4endl;
  ofMain       << headerstring;

  // primaries output file initialisation
  ofPrimaries.open(filenamePrimaries.c_str());
  ofPrimaries  << "### BDSIM primaries output - created "<< timestring << G4endl;
  ofPrimaries  << headerstring;

  // energy loss hits output file initialisation
  ofELoss.open(filenameELoss.c_str());
  ofELoss      << "### BDSIM energy loss hits output - created " << timestring <<G4endl;
  ofELoss      << headerstring;

  // primary loss hits output file initialisation
  ofPLoss.open(filenamePLoss.c_str());
  ofPLoss      << "### BDSIM primary loss hits output - created " << timestring <<G4endl;
  ofPLoss      << headerstring;
}

BDSOutputASCII::~BDSOutputASCII()
{
  if (ofMain.is_open())
    {
      ofMain.flush();
      ofMain.close();
    }
  if (ofPrimaries.is_open())
    {
      ofPrimaries.flush();
      ofPrimaries.close();
    }
  if (ofELoss.is_open())
    {
      ofELoss.flush();
      ofELoss.close();
    }
  if (ofPLoss.is_open())
    {
      ofPLoss.flush();
      ofPLoss.close();
    }
}

void BDSOutputASCII::WriteAsciiHit(std::ofstream* outfile,
				   G4int    PDGType,
				   G4double totalEnergy,
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
				   G4int    TurnsTaken)
{
  // save flags since G4cout flags are changed
  std::ios_base::fmtflags ff = outfile->flags();

  *outfile << std::left     << std::setprecision(10)  << std::fixed
	   << std::setw(6)  << PDGType                << " "
	   << std::setw(15) << totalEnergy/CLHEP::GeV << " "
	   << std::setw(15) << X/CLHEP::m             << " "
	   << std::setw(15) << Y/CLHEP::m             << " "
	   << std::setw(15) << Z/CLHEP::m             << " "
	   << std::setw(20) << S/CLHEP::m             << " "
	   << std::setw(15) << XPrime/CLHEP::radian   << " "
	   << std::setw(15) << YPrime/CLHEP::radian   << " "
	   << std::setw(6)  << EventNo                << " "
	   << std::setw(15) << Weight                 << " "
	   << std::setw(9)  << ParentID               << " "
	   << std::setw(8)  << TrackID                << " "
	   << std::setw(5)  << TurnsTaken
	   << G4endl;
  // reset flags
  outfile->flags(ff);
}

void BDSOutputASCII::WritePrimary(G4double E,
				  G4double x0,
				  G4double y0,
				  G4double z0,
				  G4double xp,
				  G4double yp,
				  G4double /*zp*/,
				  G4double /*t*/,
				  G4double weight,
				  G4int    PDGType,
				  G4int    nEvent,
				  G4int    TurnsTaken)
{
  WriteAsciiHit(&ofPrimaries, PDGType, E, x0, y0, z0, /*s=*/0.0, xp, yp, nEvent, weight, 0, 1, TurnsTaken);
  ofPrimaries.flush();
}

void BDSOutputASCII::WriteHits(BDSSamplerHitsCollection* hc)
{
  for (G4int i=0; i<hc->entries(); i++)
    {
      WriteAsciiHit(&ofMain,
		    (*hc)[i]->GetPDGtype(),
		    (*hc)[i]->GetTotalEnergy(),
		    (*hc)[i]->GetX(),          // local x
		    (*hc)[i]->GetY(),          // local y
		    (*hc)[i]->GetGlobalZ(),    // global z
		    (*hc)[i]->GetS(),
		    (*hc)[i]->GetXPrime(),
		    (*hc)[i]->GetYPrime(),
		    (*hc)[i]->GetEventNo(),
		    (*hc)[i]->GetWeight(),
		    (*hc)[i]->GetParentID(),
		    (*hc)[i]->GetTrackID(),
		    (*hc)[i]->GetTurnsTaken()
		    );
    }
  ofMain.flush();
}

// write a trajectory to a root/ascii file
// TODO: ASCII file not implemented - JS
void BDSOutputASCII::WriteTrajectory(std::vector<BDSTrajectory*> &/*TrajVec*/)
{
  G4cout << __METHOD_NAME__ << "WARNING trajectory writing not implemented for ASCII output" << G4endl;
}

// make energy loss histo
void BDSOutputASCII::WriteEnergyLoss(BDSEnergyCounterHitsCollection* hc)
{
  for (G4int i = 0; i < hc->entries(); i++)
    {
      // write the hits to the eloss file
      // there's no saving by writing out zeros instead of values
      WriteAsciiHit(&ofELoss,
		    (*hc)[i]->GetPartID(),
		    (*hc)[i]->GetEnergy(),
		    (*hc)[i]->Getx(),          // local x
		    (*hc)[i]->Gety(),          // local y
		    (*hc)[i]->GetZ(),    // global z
		    (*hc)[i]->GetSHit(),
		    0, //(*hc)[i]->GetXPrime(),
		    0, //(*hc)[i]->GetYPrime(),
		    (*hc)[i]->GetEventNo(),
		    (*hc)[i]->GetWeight(),
		    -1,//(*hc)[i]->GetParentID(),
		    0, //(*hc)[i]->GetTrackID(),
		    (*hc)[i]->GetTurnsTaken()
		    );
    }
  ofELoss.flush();
}

void BDSOutputASCII::WritePrimaryLoss(BDSTrajectoryPoint* /*ploss*/)
{;}

void BDSOutputASCII::WritePrimaryHit(BDSTrajectoryPoint* /*phit*/)
{;}

void BDSOutputASCII::WriteTunnelHits(BDSEnergyCounterHitsCollection* /*hits*/)
{;}

void BDSOutputASCII::WriteHistogram(BDSHistogram1D* histogramIn)
{
  //prepare file name
  G4String title = histogramIn->GetName();
  title = BDS::PrepareSafeName(title);
  G4String histfilename = basefilename + "." + title + ".hist.txt";
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "filename determined to be: " << histfilename << G4endl;
#endif
  
  //open file and write header info
  std::ofstream histOS;
  histOS.open(histfilename.c_str());
  histOS << *histogramIn << " - created " << timestring << G4endl;
  histOS << std::left << std::setprecision(10) << std::fixed
	 << std::setw(20) << "SLower[m]" << " "
    	 << std::setw(20) << "SUpper[m]" << " "
	 << std::setw(20) << "Value"     << G4endl;
  histOS << "Underflow: " << histogramIn->GetUnderflowBin().GetValue() << G4endl;
  histOS << "Overflow:  " << histogramIn->GetOverflowBin().GetValue()  << G4endl;
  
  //iterate over bins and fill them in
  histOS << std::scientific;
  for (const auto& bin : *histogramIn)
    {
      histOS << std::setw(20) << bin.GetLowerEdge() << " ";
      histOS << std::setw(20) << bin.GetUpperEdge() << " ";
      histOS << std::setw(20) << bin.GetValue() << G4endl;
    }
  histOS.close();
}

void BDSOutputASCII::Initialise()
{
  // Multiple file writing not implemented for ASCII
}

void BDSOutputASCII::Write(const time_t& /*startTime*/,
			   const time_t& /*stopTime*/,
			   const G4float& /*duration*/,
			   const std::string& /*seedStateAtStart*/)
{
  ofMain.flush();
  ofPrimaries.flush();
  ofELoss.flush();
  ofPLoss.flush();
}

void BDSOutputASCII::Close()
{
  ofMain.close();
  ofPrimaries.close();
  ofELoss.close();
  ofPLoss.close();
}
