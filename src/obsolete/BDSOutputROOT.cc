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
#include "BDSOutputROOT.hh"

#include "BDSDebug.hh"
#include "BDSEnergyCounterHit.hh"
#include "BDSGlobalConstants.hh"
#include "BDSHistogram.hh"
#include "BDSSampler.hh"
#include "BDSSamplerHit.hh"
#include "BDSSamplerRegistry.hh"
#include "BDSTrajectory.hh"
#include "BDSUtilities.hh"

#include "globals.hh" // geant types / globals
#include "G4TwoVector.hh"

#include <string>
#include <vector>

template<>
BDSOutputROOT<float>::BDSOutputROOT():
  type("F")
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "output format ROOT"<<G4endl;
#endif
  theRootOutputFile = nullptr;
  PrecisionRegionEnergyLossTree = nullptr;
  EnergyLossTree                = nullptr;
  PrimaryHitsTree               = nullptr;
  PrimaryLossTree               = nullptr;
  TunnelLossTree                = nullptr;
  tunnelHitsHisto               = nullptr;
}

template<>
BDSOutputROOT<double>::BDSOutputROOT():
  type("D")
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "output format ROOT"<<G4endl;
#endif
  theRootOutputFile = nullptr;
  PrecisionRegionEnergyLossTree = nullptr;
  EnergyLossTree                = nullptr;
  PrimaryHitsTree               = nullptr;
  PrimaryLossTree               = nullptr;
  TunnelLossTree                = nullptr;
  tunnelHitsHisto               = nullptr;
}

template<typename Type>
BDSOutputROOT<Type>::~BDSOutputROOT()
{
  if (theRootOutputFile && theRootOutputFile->IsOpen())
    {
      theRootOutputFile->cd();
      theRootOutputFile->Write(0,TObject::kOverwrite);
    }
}

template<typename Type>
TTree* BDSOutputROOT<Type>::BuildSamplerTree(G4String name)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  theRootOutputFile->cd();
  TTree* SamplerTree = new TTree(name, "Sampler output");
  
  SamplerTree->Branch("E",          &E,          ("E/"  + type).c_str()); // (GeV)
  SamplerTree->Branch("x",          &x,          ("x/"  + type).c_str()); // (m)
  SamplerTree->Branch("y",          &y,          ("y/"  + type).c_str()); // (m)
  SamplerTree->Branch("z",          &z,          ("z/"  + type).c_str()); // (m)
  SamplerTree->Branch("xp",         &xp,         ("xp/" + type).c_str()); // (rad)
  SamplerTree->Branch("yp",         &yp,         ("yp/" + type).c_str()); // (rad)
  SamplerTree->Branch("zp",         &zp,         ("zp/" + type).c_str()); // (rad)
  SamplerTree->Branch("t",          &t,          ("t/"  + type).c_str()); // (ns)
  SamplerTree->Branch("S",          &S,          ("S/"  + type).c_str()); // (m)
  SamplerTree->Branch("weight",     &weight,     ("weight/" + type).c_str());
  SamplerTree->Branch("partID",     &part,       "partID/I");
  SamplerTree->Branch("nEvent",     &eventno,    "nEvent/I");
  SamplerTree->Branch("parentID",   &pID,        "parentID/I");
  SamplerTree->Branch("trackID",    &track_id,   "trackID/I");
  SamplerTree->Branch("turnnumber", &turnnumber, "turnnumber/I");
  SamplerTree->Branch("process",    &process);

  return SamplerTree;
}

template<typename Type>
void BDSOutputROOT<Type>::Initialise()
{
  outputFileNumber++;
  const BDSGlobalConstants* globalConstants = BDSGlobalConstants::Instance();
  // set up the root file
  G4String basefilename = globalConstants->OutputFileName();
  // if more than one file add number (starting at 0)
  if (numberEventPerFile>0 && globalConstants->NGenerate()>numberEventPerFile)
    {basefilename += "_" + std::to_string(outputFileNumber);}
  filename = basefilename + ".root";
  // policy: overwrite if output filename specifically set, otherwise increase number
  // always check in interactive mode
  if (!globalConstants->OutputFileNameSet() || !globalConstants->Batch())
    {
      // check if file exists
      int nTimeAppended = 1;
      while (BDS::FileExists(filename))
	{
	  // if exists remove trailing .root
	  filename = basefilename + "-" + std::to_string(nTimeAppended);
	  filename += ".root";
	  nTimeAppended +=1;
	}
    }
  
  G4cout << __METHOD_NAME__ << "Setting up new file: "<<filename<<G4endl;
  theRootOutputFile = new TFile(filename,"RECREATE", "BDS output file");
  theRootOutputFile->cd();
  // Build sampler trees and store in samplerTrees
  // clear (for the case of multiple output files)
  samplerTrees.clear();
  samplerTrees.reserve(BDSSamplerRegistry::Instance()->size());
  
  G4String primariesSamplerName="Primaries";
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "building sampler tree named: " << primariesSamplerName << G4endl;
#endif
  TTree* sampler = BuildSamplerTree(primariesSamplerName);
  // primaries is the first
  samplerTrees.push_back(sampler);

  for (auto const samplerName : BDSSamplerRegistry::Instance()->GetUniqueNames())
    {
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "named: " << samplerName << G4endl;
#endif
      sampler = BuildSamplerTree(samplerName);
      samplerTrees.push_back(sampler);
    }

  if(globalConstants->StoreTrajectory())
    // create a tree with trajectories
    {
      TTree* TrajTree = new TTree("Trajectories", "Trajectories");
      TrajTree->Branch("x",    &x,    ("x/" + type).c_str());  // (m)
      TrajTree->Branch("y",    &y,    ("y/" + type).c_str());  // (m)
      TrajTree->Branch("z",    &z,    ("z/" + type).c_str());  // (m)
      TrajTree->Branch("part", &part, "part/I");
    }

  // Energy loss tree setup
  EnergyLossTree= new TTree("ElossTree", "Energy Loss");
  EnergyLossTree->Branch("S",      &S,      ("S/" + type).c_str()); // (m)
  EnergyLossTree->Branch("E",      &E,      ("E/" + type).c_str()); // (GeV)
  EnergyLossTree->Branch("weight", &weight, "weight/F"); // weight

  // Primary loss tree setup
  PrimaryLossTree  = new TTree("PlossTree", "Primary Losses");
  PrimaryLossTree->Branch("X",          &X,          ("X/" + type).c_str()); // (m)
  PrimaryLossTree->Branch("Y",          &Y,          ("Y/" + type).c_str()); // (m)
  PrimaryLossTree->Branch("Z",          &Z,          ("Z/" + type).c_str()); // (m)
  PrimaryLossTree->Branch("S",          &S,          ("S/" + type).c_str()); // (m)
  PrimaryLossTree->Branch("x",          &x,          ("x/" + type).c_str()); // (m)
  PrimaryLossTree->Branch("y",          &y,          ("y/" + type).c_str()); // (m)
  PrimaryLossTree->Branch("z",          &z,          ("z/" + type).c_str()); // (m)
  PrimaryLossTree->Branch("E",          &E,          ("s/" + type).c_str()); // (GeV)
  PrimaryLossTree->Branch("weight",     &weight,     ("weight/" + type).c_str());
  PrimaryLossTree->Branch("partID",     &part,       "partID/I");
  PrimaryLossTree->Branch("turnnumber", &turnnumber, "turnnumber/I");
  PrimaryLossTree->Branch("eventNo",    &eventno,    "eventNo/I");

  // Primary hits tree setup
  PrimaryHitsTree  = new TTree("PhitsTree", "Primary Hits");
  PrimaryHitsTree->Branch("X",          &X,          ("X/" + type).c_str()); // (m)
  PrimaryHitsTree->Branch("Y",          &Y,          ("Y/" + type).c_str()); // (m)
  PrimaryHitsTree->Branch("Z",          &Z,          ("Z/" + type).c_str()); // (m)
  PrimaryHitsTree->Branch("S",          &S,          ("S/" + type).c_str()); // (m)
  PrimaryHitsTree->Branch("x",          &x,          ("x/" + type).c_str()); // (m)
  PrimaryHitsTree->Branch("y",          &y,          ("y/" + type).c_str()); // (m)
  PrimaryHitsTree->Branch("z",          &z,          ("z/" + type).c_str()); // (m)
  PrimaryHitsTree->Branch("E",          &E,          ("E/" + type).c_str()); // (GeV)
  PrimaryHitsTree->Branch("weight",     &weight,     ("weight/" + type).c_str());
  PrimaryHitsTree->Branch("partID",     &part,       "partID/I");
  PrimaryHitsTree->Branch("turnnumber", &turnnumber, "turnnumber/I");
  PrimaryHitsTree->Branch("eventNo",    &eventno,    "eventNo/I");

  // Tunnel hits tree setup
  TunnelLossTree = new TTree("TunnelHitsTree", "Tunnel Hits");
  TunnelLossTree->Branch("E",           &E_tun,      ("E/" + type).c_str());     // (GeV)
  TunnelLossTree->Branch("S",           &S_tun,      ("S/" + type).c_str());     // (m)
  TunnelLossTree->Branch("r",           &r_tun,      ("r/" + type).c_str());     // (m)
  TunnelLossTree->Branch("theta",       &angle_tun,  ("theta/" + type).c_str()); // (rad)

  // Tunnel hits histogram
  G4double smin     = 0.0;
  G4double smax     = BDSGlobalConstants::Instance()->SMax() / CLHEP::m;
  G4double binwidth = BDSGlobalConstants::Instance()->ElossHistoBinWidth();
  G4int    nbins    = (int) ceil((smax-smin)/binwidth); // rounding up so last bin definitely covers smax
  smax              = smin + (nbins*binwidth);          // redefine smax
  // x then y -> x is angle, y is s position
  tunnelHitsHisto = new TH2F("tunnel_hits_histo","Tunnel Enegy Deposition",50,-CLHEP::pi-0.01,CLHEP::pi+0.01,nbins,0,smax);
  
  // Precision region energy loss tree setup
  PrecisionRegionEnergyLossTree= new TTree("PrecisionRegionElossTree", "Energy Loss");
  PrecisionRegionEnergyLossTree->Branch("X",          &X,          ("X/" + type).c_str()); // (m)
  PrecisionRegionEnergyLossTree->Branch("Y",          &Y,          ("Y/" + type).c_str()); // (m)
  PrecisionRegionEnergyLossTree->Branch("Z",          &Z,          ("Z/" + type).c_str()); // (m)
  PrecisionRegionEnergyLossTree->Branch("S",          &S,          ("S/" + type).c_str()); // (m)
  PrecisionRegionEnergyLossTree->Branch("x",          &x,          ("x/" + type).c_str()); // (m)
  PrecisionRegionEnergyLossTree->Branch("y",          &y,          ("y/" + type).c_str()); // (m)
  PrecisionRegionEnergyLossTree->Branch("z",          &z,          ("z/" + type).c_str()); // (m)
  PrecisionRegionEnergyLossTree->Branch("E",          &E,          ("E/" + type).c_str()); // (GeV)
  PrecisionRegionEnergyLossTree->Branch("stepLength", &stepLength, ("stepLength/" + type).c_str()); // (m)
  PrecisionRegionEnergyLossTree->Branch("weight",     &weight,     ("weight/" + type).c_str());
  PrecisionRegionEnergyLossTree->Branch("partID",     &part,       "partID/I");
  PrecisionRegionEnergyLossTree->Branch("volumeName", &volumeName, "volumeName/C");
  PrecisionRegionEnergyLossTree->Branch("turnnumber", &turnnumber, "turnnumber/I");
  PrecisionRegionEnergyLossTree->Branch("eventNo",    &eventno,    "eventNo/I");
}

template<typename Type>
void BDSOutputROOT<Type>::WriteRootHit(TTree*   tree,
				       G4double totalEnergy,
				       G4double xIn,
				       G4double yIn,
				       G4double zIn,
				       G4double xPrime,
				       G4double yPrime,
				       G4double zPrime,
				       G4double tIn,
				       G4double sIn,
				       G4double weightIn,
				       G4int    PDGtype,
				       G4int    eventNoIn,
				       G4int    parentID,
				       G4int    trackIDIn,
				       G4int    turnsTakenIn,
				       G4String processIn,
				       G4bool   fillTree)
{
  E           = totalEnergy / CLHEP::GeV;
  x           = xIn         / CLHEP::m;
  y           = yIn         / CLHEP::m;
  z           = zIn         / CLHEP::m;
  xp          = xPrime      / CLHEP::radian;
  yp          = yPrime      / CLHEP::radian;
  zp          = zPrime      / CLHEP::radian;
  T           = tIn         / CLHEP::ns;
  S           = sIn         / CLHEP::m;
  weight      = weightIn;
  part        = PDGtype; 
  eventno     = eventNoIn; 
  pID         = parentID; 
  track_id    = trackIDIn;
  turnnumber  = turnsTakenIn;
  process     = processIn;

  if (fillTree)
    {
      theRootOutputFile->cd();
      tree->Fill();
    }
}

template<typename Type>
void BDSOutputROOT<Type>::WriteRootHit(TTree*         tree,
				       BDSSamplerHit* hit,
				       G4bool         fillTree)
{
  E           = hit->GetTotalEnergy() / CLHEP::GeV;
  x           = hit->GetX()           / CLHEP::m;
  y           = hit->GetY()           / CLHEP::m;
  z           = hit->GetZ()           / CLHEP::m;
  xp          = hit->GetXPrime()      / CLHEP::radian;
  yp          = hit->GetYPrime()      / CLHEP::radian;
  zp          = hit->GetZPrime()      / CLHEP::radian;
  T           = hit->GetT()           / CLHEP::ns;
  S           = hit->GetS()           / CLHEP::m;
  weight      = hit->GetWeight();
  part        = hit->GetPDGtype();
  eventno     = hit->GetEventNo();
  pID         = hit->GetParentID();
  track_id    = hit->GetTrackID();
  turnnumber  = hit->GetTurnsTaken();
  process     = hit->GetProcess();

  if (fillTree)
    {
      theRootOutputFile->cd();
      tree->Fill();
    }
}

template<typename Type>
void BDSOutputROOT<Type>::WritePrimary(G4double totalEnergy,
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
				       G4int    turnsTaken)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  WriteRootHit(samplerTrees[0], // Primaries is the first Sampler
	       totalEnergy, 
	       x0, y0, z0, 
	       xp, yp, zp, 
	       t,/*s=*/0.0,
	       weight, 
	       PDGType, 
	       nEvent, 
	       0, 
	       1, 
	       turnsTaken,
	       "");
}

template<typename Type>
void BDSOutputROOT<Type>::WriteHits(BDSSamplerHitsCollection *hc)
{
  G4String name;
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "Number of hits " << hc->entries() << G4endl;
#endif
  for (G4int i=0; i<hc->entries(); i++)
    {
      G4int  samplerID = (*hc)[i]->GetSamplerID();
      TTree* tree      = samplerTrees[samplerID+1]; // +1 to account for extra primaries sampler in output
      WriteRootHit(tree, (*hc)[i]);
    }
}

template<typename Type>
void BDSOutputROOT<Type>::WriteTrajectory(std::vector<BDSTrajectory*> &TrajVec)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  G4String name = "Trajectories";

  theRootOutputFile->cd();
  TTree* TrajTree = dynamic_cast<TTree*>(gDirectory->Get(name));
  
  if(!TrajTree)
    {G4cerr<<"TrajTree=nullptr"<<G4endl; return;}
  
  for(BDSTrajectory* Traj : TrajVec)
    {
      for(G4int j=0; j<Traj->GetPointEntries(); j++)
	{
	  G4TrajectoryPoint* TrajPoint = static_cast<G4TrajectoryPoint*>(Traj->GetPoint(j));
	  G4ThreeVector TrajPos=TrajPoint->GetPosition();
	  
	  x = TrajPos.x() / CLHEP::m;
	  y = TrajPos.y() / CLHEP::m;
	  z = TrajPos.z() / CLHEP::m;
	  
	  TrajTree->Fill();
	}
    }
}

template<typename Type>
void BDSOutputROOT<Type>::FillHit(BDSEnergyCounterHit* hit)
{
  //copy variables from hit to root variables
  X          = hit->GetX()/CLHEP::m;
  Y          = hit->GetY()/CLHEP::m;
  Z          = hit->GetZ()/CLHEP::m;
  S          = hit->GetSHit()/CLHEP::m;
  x          = hit->Getx()/CLHEP::m;
  y          = hit->Gety()/CLHEP::m;
  z          = hit->Getz()/CLHEP::m;
  E          = hit->GetEnergy()/CLHEP::GeV;
  stepLength = hit->GetStepLength()/CLHEP::m;
  weight     = hit->GetWeight();
  part       = hit->GetPartID();
  turnnumber = hit->GetTurnsTaken();
  eventno    = hit->GetEventNo();
}

template<typename Type>
void BDSOutputROOT<Type>::FillHit(BDSTrajectoryPoint* hit)
{
    //copy variables from hit to root variables
    const G4ThreeVector pos = hit->GetPosition();
    X          = pos.x()/CLHEP::m;
    Y          = pos.y()/CLHEP::m;
    Z          = pos.z()/CLHEP::m;
    S          = hit->GetPostS()/CLHEP::m;
    const G4ThreeVector posL = hit->GetPostPosLocal();
    x          = posL.x()/CLHEP::m;
    y          = posL.y()/CLHEP::m;
    z          = posL.z()/CLHEP::m;
    E          = hit->GetEnergy()/CLHEP::GeV;
    G4double sL = ((hit->GetPrePosLocal() - hit->GetPostPosLocal()) / CLHEP::m ).mag();
    stepLength = sL;
    weight     = hit->GetPostWeight();
    part       = 0;   // not provided
    turnnumber = hit->GetTurnsTaken();
    eventno    = -1;  // not provided
}

template<typename Type>
void BDSOutputROOT<Type>::WriteEnergyLoss(BDSEnergyCounterHitsCollection* hc)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  G4int n_hit = hc->entries();
  for (G4int i=0;i<n_hit;i++)
    {
      //all regions fill the energy loss tree....
      BDSEnergyCounterHit* hit = (*hc)[i];
      FillHit(hit);
      EnergyLossTree->Fill();
    }
}

template<typename Type>
void BDSOutputROOT<Type>::WritePrimaryLoss(BDSTrajectoryPoint* ploss)
{
  //copy variables from hit to root variables
  FillHit(ploss);
  //write to file
  PrimaryLossTree->Fill();
}

template<typename Type>
void BDSOutputROOT<Type>::WritePrimaryHit(BDSTrajectoryPoint* phit)
{
  //copy variables from hit to root variables
  FillHit(phit);
  //write to file
  PrimaryHitsTree->Fill();
}

template<typename Type>
void BDSOutputROOT<Type>::WriteTunnelHits(BDSEnergyCounterHitsCollection* tunnelHits)
{
  theRootOutputFile->cd();
  for (G4int i = 0; i < tunnelHits->entries(); i++)
    {
      BDSEnergyCounterHit* hit = (*tunnelHits)[i];
      E_tun     = hit->GetEnergy()/CLHEP::GeV;
      S_tun     = hit->GetSHit()/CLHEP::m;
      G4TwoVector xy = G4TwoVector(hit->Getx()/CLHEP::m, hit->Gety()/CLHEP::m);
      r_tun     = xy.r();
      angle_tun = xy.phi();
      TunnelLossTree->Fill();

      // x,y,w -> angle,s,energy
      tunnelHitsHisto->Fill(angle_tun, S_tun, E_tun);
    }
}

template<typename Type>
void BDSOutputROOT<Type>::WriteHistogram(BDSHistogram1D* hIn)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  theRootOutputFile->cd();
  G4String hname = hIn->GetName();
  hname = BDS::PrepareSafeName(hname);

  std::vector<G4double> binLowerEdges = hIn->GetBinLowerEdges();
  if (binLowerEdges.size() < 1)
    {return;} //no bins - don't write the histogram
  binLowerEdges.push_back(hIn->GetLastBin().GetUpperEdge()); //root requires nbins+1

  //always construct the histogram by bin edges - works both with constant
  //and variable bin width histograms
  // &vector[0] gives an array to the contents of the vector - ensured as
  // standard is that the vector's contents are contiguous
  TH1D* h = new TH1D(hname, hIn->GetTitle(), hIn->GetNBins(), &binLowerEdges[0]);
  // set label titles
  h->GetXaxis()->SetTitle(hIn->GetXLabel());
  h->GetYaxis()->SetTitle(hIn->GetYLabel());
  h->GetXaxis()->CenterTitle();
  h->GetYaxis()->CenterTitle();
  G4int i = 1;
  for (auto const& bin : *hIn)
    {
      h->SetBinContent(i, bin.GetValue());
      h->SetBinError(i, bin.GetError());
      i++;
    }
  //over / underflow manually set
  h->SetBinContent(0,hIn->GetUnderflowBin().GetValue()); //underflow
  h->SetBinContent(0,hIn->GetUnderflowBin().GetError());
  h->SetBinContent(h->GetNbinsX()+1,hIn->GetOverflowBin().GetValue()); //overflow
  h->SetBinContent(h->GetNbinsX()+1,hIn->GetOverflowBin().GetError());

  h->SetEntries(hIn->GetNEntries());
  
  h->Write(); // as commit actually closes a file as does write..
  delete h;
}

template<typename Type>
void BDSOutputROOT<Type>::Write(const time_t& /*startTime*/,
				const time_t& /*stopTime*/,
				const G4float& /*duration*/,
				const std::string& /*seedStateAtStart*/)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif

  if(theRootOutputFile && theRootOutputFile->IsOpen())
    {
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << " - ROOT file found and open, writing." << G4endl;
#endif
      //Dump all other quantities to file...
      theRootOutputFile->cd();
      theRootOutputFile->Write();
    }
  G4cout << __METHOD_NAME__ << " ...finished." << G4endl;
}

template<typename Type>
void BDSOutputROOT<Type>::Close()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif

  if(theRootOutputFile && theRootOutputFile->IsOpen())
    {
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << " - ROOT file found and open, closing." << G4endl;
#endif
      theRootOutputFile->cd();
      theRootOutputFile->Close();
      delete theRootOutputFile;
      theRootOutputFile=nullptr;
    }
}

template void BDSOutputROOT<double>::WriteRootHit(TTree*   tree,
						  G4double totalEnergy,
						  G4double xIn,
						  G4double yIn,
						  G4double zIn,
						  G4double xPrime,
						  G4double yPrime,
						  G4double zPrime,
						  G4double tIn,
						  G4double sIn,
						  G4double weightIn,
						  G4int    PDGtype,
						  G4int    eventNoIn,
						  G4int    parentID,
						  G4int    trackIDIn,
						  G4int    turnsTakenIn,
						  G4String processIn,
						  G4bool   fillTree);

template void BDSOutputROOT<float>::WriteRootHit(TTree*   tree,
						 G4double totalEnergy,
						 G4double xIn,
						 G4double yIn,
						 G4double zIn,
						 G4double xPrime,
						 G4double yPrime,
						 G4double zPrime,
						 G4double tIn,
						 G4double sIn,
						 G4double weightIn,
						 G4int    PDGtype,
						 G4int    eventNoIn,
						 G4int    parentID,
						 G4int    trackIDIn,
						 G4int    turnsTakenIn,
						 G4String processIn,
						 G4bool   fillTree);
