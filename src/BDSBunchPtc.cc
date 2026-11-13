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
#include "BDSBunchPtc.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSGlobalConstants.hh"
#include "BDSParticleCoordsFull.hh"
#include "BDSUtilities.hh"

#include "parser/beam.h"

#include "globals.hh"
#include "G4String.hh"
#include "G4Types.hh"

#include "CLHEP/Units/PhysicalConstants.h"

#include <array>
#include <fstream>
#include <regex>
#include <string>
#include <vector>


BDSBunchPtc::BDSBunchPtc():
  BDSBunchFileBased("ptc"),
  matchDistrFileLength(false),
  nRays(0),
  iRay(0),
  beta(1.0),
  nlinesSkip(0),
  lineCounter(0)
{;}

BDSBunchPtc::~BDSBunchPtc()
{
  ptcData.clear();
}

void BDSBunchPtc::LoadPtcFile()
{
  G4cout << "BDSBunchPtc::LoadPtcFile> opening " << fileName << G4endl;

  // open file and read line by line and extract values
  std::ifstream ifstr(fileName);

  if (!ifstr)
    {throw BDSException(__METHOD_NAME__, "\"" + fileName + "\" file doesn't exist - exiting as no input");}

  std::string line;
  lineCounter = 0;
  
  for (G4int i = 0; i < nlinesSkip; i++)
    {
      std::getline(ifstr, line);
      lineCounter++;
      // we must check explicitly if we've gone past the end of the file
      if (ifstr.eof())
        {
          G4String msg = "end of file reached on line " + std::to_string(lineCounter) + " before nlinesSkip + nlinesIgnore (";
          msg += std::to_string(nlinesSkip) + " reached.";
          throw BDSException(__METHOD_NAME__, msg);
        }
    }
  IncrementNEventsInFileSkipped((unsigned long long int)nlinesSkip);
  
  // create regular expressions
  std::regex rex("\\sx\\s*=\\s*([0-9eE.+-]+)");
  std::regex rey("\\sy\\s*=\\s*([0-9eE.+-]+)");
  std::regex repx("px\\s*=\\s*([0-9eE.+-]+)");
  std::regex repy("py\\s*=\\s*([0-9eE.+-]+)");
  std::regex ret("\\st\\s*=\\s*([0-9eE.+-]+)");
  std::regex rept("pt\\s*=\\s*([0-9eE.+-]+)");
  
  // read single line 
  while (std::getline(ifstr,line))
    {
      lineCounter++;
      int isComment = line.compare(0, 1, "!");
      if (isComment == 0)
        {continue;}
      else
        {
          // variable for storage
          double x  = 0.0;
          double y  = 0.0;
          double px = 0.0;
          double py = 0.0;
          double t  = 0.0;
          double pt = 0.0;
          
          // return search match objects
          std::smatch smx;
          std::smatch smy;
          std::smatch smpx;
          std::smatch smpy;
          std::smatch smt;
          std::smatch smpt;
          
          // perform search
          std::regex_search(line,smx, rex);
          std::regex_search(line,smy, rey);
          std::regex_search(line,smpx,repx);
          std::regex_search(line,smpy,repy);
          std::regex_search(line,smt, ret);
          std::regex_search(line,smpt, rept);
          
          if(smx.size() == 2)  x  = std::stod(smx[1]);
          if(smy.size() == 2)  y  = std::stod(smy[1]);
          if(smpx.size() == 2) px = std::stod(smpx[1]);
          if(smpy.size() == 2) py = std::stod(smpy[1]);
          if(smt.size() == 2)  t  = std::stod(smt[1]);
          if(smpt.size() == 2) pt = std::stod(smpt[1]);
          
#ifdef BDSDEBUG 
          G4cout << __METHOD_NAME__ << "read line " << line << G4endl;
          G4cout << __METHOD_NAME__ << "values    " << x << " " << px << " " << y << " " << py << " " << t << " " << pt << G4endl;   
#endif 
          ptcData.emplace_back(std::array<double, 6>{x, px, y, py, t, pt});
        }
    }
  
  // set number of available rays in options
  nRays = (G4int)ptcData.size();
}

void BDSBunchPtc::SetOptions(const BDSParticleDefinition* beamParticle,
                             const GMAD::Beam& beam,
                             const BDSBunchType& distrType,
                             G4Transform3D beamlineTransformIn,
                             const G4double beamlineSIn)
{
  BDSBunchFileBased::SetOptions(beamParticle, beam, distrType, beamlineTransformIn, beamlineSIn);
  matchDistrFileLength = G4bool(beam.distrFileMatchLength);
  beta = beamParticle->Beta();
  fileName = BDS::GetFullPath(beam.distrFile);
  nlinesSkip = beam.nlinesSkip + beam.nlinesIgnore;
}

void BDSBunchPtc::Initialise()
{
  try
    {LoadPtcFile();}
  catch (BDSException& e)
    {
      std::string msg = "\nError on line " + std::to_string(lineCounter);
      e.AppendToMessage(msg);
      throw e;
    }
  catch (std::exception& e)
    {
      std::string msg = "Error on line " + std::to_string(lineCounter);
      throw BDSException(__METHOD_NAME__, msg);
    }
  
  auto g = BDSGlobalConstants::Instance();
  G4bool nGenerateHasBeenSet = g->NGenerateSet();
  G4int nEventsPerLoop = nRays;
  nEventsInFile = nRays;
  G4int nAvailable = nEventsPerLoop * distrFileLoopNTimes;
  G4int nGenerate = g->NGenerate();
  if (matchDistrFileLength)
    {
      if (!nGenerateHasBeenSet)
        {
          g->SetNumberToGenerate(nAvailable);
          G4cout << "BDSBunchPtc::Initialise> distrFileMatchLength is true -> simulating " << nRays << " events";
          if (distrFileLoopNTimes > 1)
            {G4cout << " " << distrFileLoopNTimes << " times";}
          G4cout << G4endl;
          if (g->Recreate())
            {// have to do this now before the primary generator action is called already in the run
              G4int nEventsRemaining = nAvailable - g->StartFromEvent();
              g->SetNumberToGenerate(nEventsRemaining);
              G4cout << "BDSBunchPtc::Initialise> distrFileMatchLength + recreation -> simulate the "
                     << nEventsRemaining << " lines left given startFromEvent including possible looping" << G4endl;
            }
        }
      else
        {// e.g. if recreating a lower number of events - match is on; but ngenerate is lower - must obey
          G4cout << "BDSBunchPtc::Initialise> matchDistrFileLength has been requested "
                 << "but ngenerate has been specified -> use ngenerate" << G4endl;
          // note we don't need to take care of a recreation offset - this is done later in primary generator action
          if (nGenerate > nAvailable)
            {
              G4String msg = "ngenerate (" + std::to_string(nGenerate) + ") is greater than the number of valid lines (";
              msg += std::to_string(nRays) + ") and distrFileMatchLength is on.\nChange ngenerate to <= # lines";
              msg += ", or don't specifcy ngenerate.\n";
              msg += "This includes nlinesSkip.";
              throw BDSException("BDSBunchUserFile::Initialise>", msg);
            }
        }
    }
  else
    {
      if ( (nGenerate > nRays) && !distrFileLoop )
        {
          G4String msg = "ngenerate (" + std::to_string(nGenerate) + ") is greater than the number of inrays (";
          msg += std::to_string(nRays) + ") but distrFileLoop is false in the beam command";
          throw BDSException(__METHOD_NAME__, msg);
        }
    }
}

BDSParticleCoordsFull BDSBunchPtc::GetNextParticleLocal()
{
  if ( iRay == nRays) // so that we're safe to still read the last entry
    {
      if (distrFileLoop)
        {
          iRay = 0;
          G4cout << __METHOD_NAME__ << "End of file reached. Returning to beginning of file." << G4endl;
        }
      else
        {throw BDSException(__METHOD_NAME__, "unable to read another event as file finished");}
    }
  
  G4double x  = ptcData[iRay][0] * CLHEP::m + X0;
  G4double y  = ptcData[iRay][2] * CLHEP::m + Y0;
  G4double z  = ptcData[iRay][4] * CLHEP::m + Z0;
  G4double xp = ptcData[iRay][1] * CLHEP::rad + Xp0;
  G4double yp = ptcData[iRay][3] * CLHEP::rad + Yp0;
  G4double t  = (z-Z0)*CLHEP::m / CLHEP::c_light + T0 * CLHEP::s;
  G4double zp = CalculateZp(xp,yp,Zp0);

  G4double betaSquared = std::pow(beta,2.0);
  G4double ptcVal = ptcData[iRay][5] * betaSquared;
  G4double E  = E0 * (ptcVal + 1.0);

  BDSParticleCoordsFull result(x,y,z,xp,yp,zp,t,S0+z,E,/*weight=*/1.0);

  iRay++;

  return result;
}

void BDSBunchPtc::RecreateAdvanceToEvent(G4int eventOffset)
{
  G4int nAvailable       = nRays * distrFileLoopNTimes;
  G4int nEventsRemaining = nAvailable - eventOffset;
  if (eventOffset >= nRays)
    {
      if (distrFileLoop)
        {
          G4int nToRoll = eventOffset % nRays;
          eventOffset = nToRoll;
        }
      else
        {
          G4String msg = "eventOffset (" + std::to_string(eventOffset);
          msg += ") is greater than the number of inrays in the PTC file";
          throw BDSException(__METHOD_NAME__, msg);
        }
    }
  
  iRay = eventOffset;
  G4int nGenerate = BDSGlobalConstants::Instance()->NGenerate();
  if (nGenerate > nEventsRemaining && !distrFileLoop)
    {
      G4String msg = "ngenerate (" + std::to_string(nGenerate) + ") requested in recreate mode is greater than number\n";
      msg += "of remaining valid lines in file (" + std::to_string(nEventsRemaining) + ") and distrFileLoop is turned off.";
      throw BDSException("BDSBunchPtc>", msg);
    }
  // note we cannot update ngenerate here as we're already being called from the primary
  // generator action in the start of the event after BeamOn(nEvents) has been called
  // therefore this adjustment for recreation + match is done earlier in this class
}
