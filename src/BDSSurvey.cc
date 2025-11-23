/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2024.

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
#include "BDSSurvey.hh"

#include "BDSAcceleratorComponent.hh"
#include "BDSBeamline.hh"
#include "BDSBeamlineElement.hh"
#include "BDSBeamPipeInfo.hh"
#include "BDSDebug.hh"
#include "BDSMagnet.hh"
#include "BDSMagnetStrength.hh"

#include <fstream>
#include <iomanip>
#include <ctime>
#include <string>

using std::setw;

BDSSurvey::BDSSurvey(const G4String& filename):
  nullStrength(new BDSMagnetStrength()),
  gp(14)
{
  magnetKeys = BDSMagnetStrength::AllKeys();
  
  G4cout << __METHOD_NAME__ << "Generating Survey: " << filename << " ..." << G4endl;
  survey.open(filename);
  WriteHeader();
}

BDSSurvey::~BDSSurvey()
{
  if (survey.is_open())
    {survey.close();}
  delete nullStrength;
}

void BDSSurvey::WriteHeader()
{
  time_t currenttime;
  time(&currenttime);
  std::string timestring = asctime(localtime(&currenttime));
  timestring = timestring.substr(0,timestring.size()-1);
  
  survey << "### BDSIM output - created "<< timestring << G4endl;
  survey << std::left 
	 << setw(15) << "Type          " << " "
	 << setw(40) << "Name          " << " "
	 << setw(gp) << "SStart[m]     " << " "
	 << setw(gp) << "SMid[m]       " << " "
	 << setw(gp) << "SEnd[m]       " << " "
	 << setw(gp) << "ChordLength[m]" << " "
	 << setw(gp) << "ArcLength[m]  " << " "
	 << setw(gp) << "X[m]          " << " "
	 << setw(gp) << "Y[m]          " << " "
	 << setw(gp) << "Z[m]          " << " "
	 << setw(gp) << "Phi[rad]      " << " "
	 << setw(gp) << "Theta[rad]    " << " "
	 << setw(gp) << "Psi[rad]      " << " "
	 << setw(gp) << "Aper1[m]      " << " "
	 << setw(gp) << "Aper2[m]      " << " "
         << setw(gp) << "Aper3[m]      " << " "
         << setw(gp) << "Aper4[m]      " << " "
	 << setw(15) << "Aper_Type     " << " "
	 << setw(gp) << "Angle[rad]    ";
  
  for (auto const& key : magnetKeys)
    {
      const G4String unit = BDSMagnetStrength::UnitName(key);
      if (!unit.empty())
	{survey << " " << setw(18) << key + "[" + unit + "]";}
      else
	{survey << " " << setw(18) << key;}
    }
  survey << G4endl;

  survey.setf(std::ios::fixed, std::ios::floatfield);
  survey.setf(std::ios::showpoint);
  // print up to 7 significant numbers
  survey.precision(7);      
}

void BDSSurvey::Write(BDSBeamline* beamline)
{
  for (auto element : *beamline)
    {Write(element);}

  survey << "### Total length = " << beamline->GetTotalChordLength()/CLHEP::m << "m" << G4endl;
  survey << "### Total arc length = " <<  beamline->GetTotalArcLength()/CLHEP::m << "m" << G4endl;
  survey.close();
}

void BDSSurvey::Write(BDSBeamlineElement* beamlineElement)
{  
  BDSAcceleratorComponent* acceleratorComponent = beamlineElement->GetAcceleratorComponent();

  G4RotationMatrix* rm = beamlineElement->GetRotationMiddle();
  G4double phi         = rm->getPhi();
  G4double theta       = rm->getTheta();
  G4double psi         = rm->getPsi();
      
  G4double sStart      = beamlineElement->GetSPositionStart() /CLHEP::m;
  G4double sMiddle     = beamlineElement->GetSPositionMiddle()/CLHEP::m;
  G4double sEnd        = beamlineElement->GetSPositionEnd()   /CLHEP::m;
  G4ThreeVector pos    = beamlineElement->GetPositionMiddle();

  BDSBeamPipeInfo* beamPipeInfo = acceleratorComponent->GetBeamPipeInfo();
  
  survey << std::left << std::setprecision(6) << std::fixed
	 << setw(15) << acceleratorComponent->GetType()             << " "
	 << setw(40) << acceleratorComponent->GetName()             << " "
	 << setw(gp) << sStart                                      << " "
	 << setw(gp) << sMiddle                                     << " "
	 << setw(gp) << sEnd                                        << " "
	 << setw(gp) << acceleratorComponent->GetChordLength()/CLHEP::m   << " "
	 << setw(gp) << acceleratorComponent->GetArcLength()/CLHEP::m     << " "
	 << setw(gp) << pos.x()/CLHEP::m                            << " "
	 << setw(gp) << pos.y()/CLHEP::m                            << " "
	 << setw(gp) << pos.z()/CLHEP::m                            << " "
	 << setw(gp) << phi/CLHEP::radian                           << " "
	 << setw(gp) << theta/CLHEP::radian                         << " "
	 << setw(gp) << psi/CLHEP::radian                           << " "
	 << setw(gp) << (beamPipeInfo ? beamPipeInfo->aper1/CLHEP::m : 0) << " "
	 << setw(gp) << (beamPipeInfo ? beamPipeInfo->aper2/CLHEP::m : 0) << " "
    	 << setw(gp) << (beamPipeInfo ? beamPipeInfo->aper3/CLHEP::m : 0) << " "
    	 << setw(gp) << (beamPipeInfo ? beamPipeInfo->aper4/CLHEP::m : 0) << " "
	 << setw(15) << (beamPipeInfo ? beamPipeInfo->beamPipeType : 0)   << " "
	 << setw(gp) << acceleratorComponent->GetAngle();

  BDSMagnetStrength const* st;
  if (BDSMagnet* magCast = dynamic_cast<BDSMagnet*>(acceleratorComponent))
    {
      st = magCast->MagnetStrength();
      if (!st)
	{st = nullStrength;}
    }
  else
    {st = nullStrength;}

  st->WriteValuesInSIUnitsForSuvey(survey, 18);
  survey << G4endl;
}
