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
#include <iostream>


#include "TCanvas.h"
#include "TChain.h"
#include "TString.h"
#include "TLine.h"

#include "BDSOutputROOTEventOptions.hh"
#include "parser/optionsBase.h"

#include "Event.hh"
#include "Utility.hh"

void DrawGaussTwissEllipse(TString fileName)
{
  // TBC - does this need to be called? Commented out return variable
  // to prevent unused variable warning, but does this load something useful
  // 
  /*BDSOutputROOTEventOptions *o = */GetOptions(fileName);
}

void DrawHalo(TString fileName)
{
  BDSOutputROOTEventOptions *o = GetOptions(fileName);
  GMAD::OptionsBase *ob = (GMAD::OptionsBase*)o;

#if 0
  std::cout << ob->envelopeX << " " << ob->envelopeXp << std::endl;
  std::cout << ob->envelopeY << " " << ob->envelopeYp << std::endl;
  std::cout << ob->haloEnvelopeCollMinX << " " << ob->haloEnvelopeCollMaxX << std::endl;
  std::cout << ob->haloEnvelopeCollMinY << " " << ob->haloEnvelopeCollMaxY << std::endl;
  std::cout << ob->haloEnvelopeCollMinXp << " " << ob->haloEnvelopeCollMaxXp << std::endl;
  std::cout << ob->haloEnvelopeCollMinYp << " " << ob->haloEnvelopeCollMaxYp << std::endl;
#endif

  TChain *c = new TChain("Event");
  Event  *e = new Event();

  GetEvent(fileName,c,e);

  TCanvas *can = new TCanvas("c");
  can->Divide(2,2);
  can->cd(1);

  ///////////////////////////////////////////////////////////////////////
  // X
  ///////////////////////////////////////////////////////////////////////
  c->Draw("Primary.xp:Primary.x","","");

  TLine *xPos = new TLine(ob->haloEnvelopeCollMaxX,-ob->envelopeXp,
                          ob->haloEnvelopeCollMaxX, ob->envelopeXp);
  xPos->Draw();

  TLine *xNeg = new TLine(ob->haloEnvelopeCollMinX,-ob->envelopeXp,
                          ob->haloEnvelopeCollMinX, ob->envelopeXp);
  xNeg->Draw();


  TLine *xpPos = new TLine(-ob->envelopeX,ob->haloEnvelopeCollMaxXp,
                            ob->envelopeX,ob->haloEnvelopeCollMaxXp);
  xpPos->Draw();

  TLine *xpNeg = new TLine(-ob->envelopeX,ob->haloEnvelopeCollMinXp,
                           ob->envelopeX,ob->haloEnvelopeCollMinXp);
  xpNeg->Draw();

  ///////////////////////////////////////////////////////////////////////
  // Y
  ///////////////////////////////////////////////////////////////////////
  can->cd(2);
  c->Draw("Primary.yp:Primary.y","","");


  TLine *yPos = new TLine(ob->haloEnvelopeCollMaxY,-ob->envelopeYp,
                          ob->haloEnvelopeCollMaxY, ob->envelopeYp);
  yPos->Draw();

  TLine *yNeg = new TLine(ob->haloEnvelopeCollMinY,-ob->envelopeYp,
                          ob->haloEnvelopeCollMinY, ob->envelopeYp);
  yNeg->Draw();


  TLine *ypPos = new TLine(-ob->envelopeY,ob->haloEnvelopeCollMaxYp,
                            ob->envelopeY,ob->haloEnvelopeCollMaxYp);
  ypPos->Draw();

  TLine *ypNeg = new TLine(-ob->envelopeY,ob->haloEnvelopeCollMinYp,
                            ob->envelopeY,ob->haloEnvelopeCollMinYp);
  ypNeg->Draw();

  ///////////////////////////////////////////////////////////////////////
  // Y vs X
  ///////////////////////////////////////////////////////////////////////
  can->cd(3);
  c->Draw("Primary.y:Primary.x","","");

  ///////////////////////////////////////////////////////////////////////
  // Yp vs Xp
  ///////////////////////////////////////////////////////////////////////
  can->cd(4);
  c->Draw("Primary.yp:Primary.xp","","");

}
