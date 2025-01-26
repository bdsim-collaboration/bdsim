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
#include "EventAnalysisOrbit.hh"
#include "RBDSException.hh"
#include "SamplerAnalysis.hh"

#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "TChain.h"
#include "TFile.h"
#include "TString.h"
#include "TTree.h"

ClassImp(EventAnalysisOrbit)

EventAnalysisOrbit::EventAnalysisOrbit():
  EventAnalysis()
{;}

EventAnalysisOrbit::EventAnalysisOrbit(Event*   eventIn,
                                       TChain*  chainIn,
                                       bool     perEntryAnalysis,
                                       bool     processSamplersIn,
                                       bool     debugIn,
                                       bool     printOutIn,
                                       double   printModuloFraction,
                                       bool     emittanceOnTheFlyIn,
                                       long int eventStartIn,
                                       long int eventEndIn):
  EventAnalysis(eventIn, chainIn, perEntryAnalysis, processSamplersIn, false,
                debugIn, printOutIn, printModuloFraction, emittanceOnTheFlyIn,
                eventStartIn, eventEndIn)
{;}

void EventAnalysisOrbit::Clear()
{
  ss.clear();
  x.clear();
  xp.clear();
  y.clear();
  yp.clear();
  elementName.clear();
}

void EventAnalysisOrbit::ExtractOrbit(int index)
{
  if (index > entries-1)
    {
      std::string errString = "Orbit index: " + std::to_string(index) +
	" greater than number of events: " + std::to_string(entries) + " in file minus one";
      throw RBDSException(errString);
    }

  int nSamplers = (int)samplerAnalyses.size();

  std::cout << "Getting orbit " << index << std::endl;
  chain->GetEntry(index);
  std::cout << "Loaded" << std::endl;
  
  int counter = 0;
  for (auto s : samplerAnalyses)
    {
      std::cout << "\rSampler #" << std::setw(6) << counter << " of " << nSamplers;
      std::cout.flush();
      if (s->s->n > 0)
        {// valid entry on that sampler
          ss.push_back(s->s->S);
          x.push_back(s->s->x[0]);
          xp.push_back(s->s->xp[0]);
          y.push_back(s->s->y[0]);
          yp.push_back(s->s->yp[0]);
          elementName.push_back(s->s->samplerName);
        }
      counter++;
    }
  std::cout << std::endl;
}

void EventAnalysisOrbit::WriteOrbit(TFile* f)
{
  f->cd("/");
  TTree* orbitTree = new TTree("Orbit", "Orbit");

  double dss;
  double dx;
  double dxp;
  double dy;
  double dyp;
  std::string delementName;

  orbitTree->Branch("s",  &dss, "s/D");
  orbitTree->Branch("x",  &dx,  "x/D");
  orbitTree->Branch("xp", &dxp, "xp/D");
  orbitTree->Branch("y",  &dy,  "y/D");
  orbitTree->Branch("yp", &dyp, "yp/D");
  orbitTree->Branch("elementName", &delementName);
  
  for (int i = 0; i < (int)ss.size(); ++i)
    {
      dss = ss.at(i);
      dx  = x.at(i);
      dxp = xp.at(i);
      dy  = y.at(i);
      dyp = yp.at(i);
      delementName = elementName.at(i);
      orbitTree->Fill();
    }

  orbitTree->Write();
  f->Close();
}
