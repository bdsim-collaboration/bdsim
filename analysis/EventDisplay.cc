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
#include "EventDisplay.hh"

#include <iostream>
#include <string>

#include "TFile.h"
#include "TVector3.h"

#include "TEveManager.h"
#include "TGeoManager.h"
#include "TEveGeoNode.h"
#include "TEvePointSet.h"
#include "TEveBoxSet.h"
#include "TEveLine.h"
#include "TEveScene.h"
#include "TEveEventManager.h"

#include "BDSOutputROOTEventLoss.hh"
#include "BDSOutputROOTEventModel.hh"
#include "BDSOutputROOTEventOptions.hh"
#include "BDSOutputROOTEventTrajectory.hh"
#include "Model.hh"
#include "Options.hh"


ClassImp(EventDisplay)

EventDisplay* EventDisplay::instance = nullptr;


EventDisplay* EventDisplay::Instance()
{
  if (!instance)
    {std::cout << "EventDisplay> WARNING - no file supplied" << std::endl;}
  return instance;
}

EventDisplay* EventDisplay::Instance(TString geoFileName,
				     TString dataFileName)
{
  instance = new EventDisplay(geoFileName, dataFileName);
  return instance;
}

EventDisplay::EventDisplay(TString geoFileNameIn,
			   TString dataFileNameIn):
  geoFileName(geoFileNameIn),
  dataFileName(dataFileNameIn)
{
  std::cout << "EventDisplay::EventDisplay(" << geoFileName << ")" << std::endl;
  TEveManager::Create();
  dataLoader = new DataLoader(std::string(dataFileName.Data()));
  event      = dataLoader->GetEvent();
  eventTree  = dataLoader->GetEventTree();

  options    = dataLoader->GetOptions();
  optionsTree= dataLoader->GetOptionsTree();

  model      = dataLoader->GetModel();
  modelTree  = dataLoader->GetModelTree();

  LoadGeometry();
  LoadOptions(0);
  LoadModel(0);
  LoadData(0);
}

EventDisplay::~EventDisplay()
{
  delete dataLoader;
  instance = nullptr;
}

void EventDisplay::LoadGeometry()
{
  std::cout << "EventDisplay::LoadGeometry>" << std::endl;
  TFile::SetCacheFileDir(".");
  std::cout << geoFileName.Data() << std::endl;
  gGeoManager = gEve->GetGeometry(geoFileName.Data());
  gGeoManager->DefaultColors();

  TEveGeoTopNode* tn = new TEveGeoTopNode(gGeoManager, gGeoManager->GetTopNode());
  gEve->AddGlobalElement(tn);
  gEve->FullRedraw3D(kTRUE);
}

void EventDisplay::LoadModel(int iMod)
{
  std::cout << "EventDisplay::LoadModel>" << std::endl;
  modelTree->GetEntry(iMod);
}

void EventDisplay::LoadOptions(int iOpt)
{
  std::cout << "EventDisplay::LoadOptions>" << std::endl;
  optionsTree->GetEntry(iOpt);
}

void EventDisplay::LoadData(int i)
{
  std::cout << "EventDisplay::LoadData>" << std::endl;
  eventTree->GetEntry(i);
}

void EventDisplay::ClearEvent()
{
  std::cout << "EventDisplay::ClearEvent>" << std::endl;
  gEve->GetCurrentEvent()->DestroyElements();
}

void EventDisplay::Draw()
{
  DrawModel();
  DrawElossHits();
  DrawTunnelHits();
  DrawSamplers();
  DrawTrajectories();
}

void EventDisplay::DrawModel()
{
  std::cout << "EventDisplay::DrawModel>" << std::endl;
  BDSOutputROOTEventOptions* ob = options->options;
  std::cout << "EventDisplay::DrawModel> horizontalWidth " << ob->horizontalWidth << std::endl;
  double horizontalWidth = ob->horizontalWidth; // need cm


  TEveBoxSet* bs = new TEveBoxSet("model");
  bs->Reset(TEveBoxSet::kBT_FreeBox, kFALSE,64);

  int i = 0;
  float vert[8*3];
  for(auto mp : model->model->midPos)
  {
    double modelLength      = model->model->length[i];
    TRotation modelRotation = model->model->midRot[i];

    // 100 as need cm not m
    TVector3 v1 = 100*(mp + modelRotation*TVector3(-horizontalWidth / 2, -horizontalWidth / 2, -modelLength / 2));
    TVector3 v2 = 100*(mp + modelRotation*TVector3(-horizontalWidth / 2,  horizontalWidth / 2, -modelLength / 2));
    TVector3 v3 = 100*(mp + modelRotation*TVector3( horizontalWidth / 2,  horizontalWidth / 2, -modelLength / 2));
    TVector3 v4 = 100*(mp + modelRotation*TVector3( horizontalWidth / 2, -horizontalWidth / 2, -modelLength / 2));

    TVector3 v5 = 100*(mp + modelRotation*TVector3(-horizontalWidth / 2, -horizontalWidth / 2,  modelLength / 2));
    TVector3 v6 = 100*(mp + modelRotation*TVector3(-horizontalWidth / 2,  horizontalWidth / 2,  modelLength / 2));
    TVector3 v7 = 100*(mp + modelRotation*TVector3( horizontalWidth / 2,  horizontalWidth / 2,  modelLength / 2));
    TVector3 v8 = 100*(mp + modelRotation*TVector3( horizontalWidth / 2, -horizontalWidth / 2,  modelLength / 2));


    vert[0] = (float) v1.x();
    vert[1] = (float) v1.y();
    vert[2] = (float) v1.z();

    vert[3] = (float) v2.x();
    vert[4] = (float) v2.y();
    vert[5] = (float) v2.z();

    vert[6] = (float) v3.x();
    vert[7] = (float) v3.y();
    vert[8] = (float) v3.z();

    vert[9] = (float)  v4.x();
    vert[10] = (float) v4.y();
    vert[11] = (float) v4.z();

    vert[12] = (float) v5.x();
    vert[13] = (float) v5.y();
    vert[14] = (float) v5.z();

    vert[15] = (float) v6.x();
    vert[16] = (float) v6.y();
    vert[17] = (float) v6.z();

    vert[18] = (float) v7.x();
    vert[19] = (float) v7.y();
    vert[20] = (float) v7.z();

    vert[21] = (float) v8.x();
    vert[22] = (float) v8.y();
    vert[23] = (float) v8.z();

    bs->AddBox(vert);
    ++i;
  }
  bs->SetMainColor(kWhite);
  bs->SetRenderMode(TEveBoxSet::kRM_Line);
  gEve->AddElement(bs);
  gEve->Redraw3D();
}

void EventDisplay::DrawElossHits()
{
  std::cout << "EventDisplay::DrawElossHits>" << std::endl;
  TEvePointSet* ps = new TEvePointSet("Eloss");
  for(int i=0;i<(int)event->Eloss->X.size();++i)
  {
    ps->SetNextPoint(event->Eloss->X[i]*100.0,
                     event->Eloss->Y[i]*100.0,
                     event->Eloss->Z[i]*100.0);
  }
  ps->SetMainColor(kWhite);
  gEve->AddElement(ps);
  gEve->Redraw3D();
}

void EventDisplay::DrawTunnelHits()
{
  std::cout << "EventDisplay::DrawTunnelHits>" << std::endl;

  TEveBoxSet* bs = new TEveBoxSet("TunnelHits");
  bs->Reset(TEveBoxSet::kBT_AABox, kFALSE,64);

  for(int i=0;i<(int)event->TunnelHit->X.size();++i)
    {
      bs->AddBox(event->TunnelHit->X[i]*100.0,
		 event->TunnelHit->Y[i]*100.0,
		 event->TunnelHit->Z[i]*100.0,
		 -event->TunnelHit->energy[i]*5000.0,
		 -event->TunnelHit->energy[i]*5000.0,
		 -event->TunnelHit->energy[i]*5000.0);
    }
  bs->RefitPlex();

  bs->SetMainColor(kWhite);
  gEve->AddElement(bs);
  gEve->Redraw3D();
}

void EventDisplay::DrawSamplers()
{
  std::cout << "EventDisplay::DrawSamplers>" << std::endl;

  // loop over samplers
  for(auto sampler : event->Samplers)
  {
    TEvePointSet* ps = new TEvePointSet((std::string("Sampler_")+sampler->samplerName.c_str()).c_str());

    // Get coordinate tranform for sampler
    TVector3 mpos = model->model->endPos[sampler->modelID];
    TRotation mrot = model->model->endRot[sampler->modelID];

    // loop over sampler hits
    for(int i=0;i<sampler->n;++i)
    {
      TVector3 v = mpos + mrot*TVector3(sampler->x[i],sampler->y[i],0);
      ps->SetNextPoint(v.x()*100.0,
                       v.y()*100.0,
                       v.z()*100.0);
    }

    ps->SetMainColor(kBlue);
    gEve->AddElement(ps);
  }
  gEve->Redraw3D();
}

void EventDisplay::DrawTrajectories()
{
  std::cout << "EventDisplay::DrawTrajectories>" << std::endl;
  std::cout << "EventDisplay::DrawTrajectories> ntraj=" << event->Trajectory->XYZ.size() << std::endl;

  int iTraj = 0;
  for (auto t : event->Trajectory->XYZ)
    {
      std::string trajNameAppend = std::string();
      if (event->Trajectory->trackID[iTraj] == 1)
	{trajNameAppend = "_primary";}
      else
	{trajNameAppend = "_"+std::to_string(event->Trajectory->trackID[iTraj]);}
      
      TEveLine* et = new TEveLine((std::string("Trajectory")+trajNameAppend).c_str());
      
      for (auto tp : t)
	{
	  //std::cout << tp.x() << " " << tp.y() << " " << tp.z() << std::endl;
	  et->SetNextPoint(tp.x()*100,
			   tp.y()*100,
			   tp.z()*100);
	}
      et->SetMainColor(kWhite);
      gEve->AddElement(et);
      gEve->Redraw3D();
      
      iTraj++;
    }
}
