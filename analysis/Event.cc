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
#include "Event.hh"
#include "RBDSException.hh"
#include "RebdsimTypes.hh"

#include "BDSOutputROOTEventAperture.hh"
#include "BDSOutputROOTEventCollimator.hh"
#include "BDSOutputROOTEventCoords.hh"
#include "BDSOutputROOTEventHistograms.hh"
#include "BDSOutputROOTEventInfo.hh"
#include "BDSOutputROOTEventLoss.hh"
#include "BDSOutputROOTEventLossWorld.hh"
#include "BDSOutputROOTEventTrajectory.hh"
#include "BDSOutputROOTEventSampler.hh"
#include "BDSOutputROOTEventSamplerC.hh"
#include "BDSOutputROOTEventSamplerS.hh"

#include <set>
#include <vector>

#include "TChain.h"

ClassImp(Event)

Event::Event():
  tree(nullptr),
  debug(false),
  processSamplers(false),
  dataVersion(0),
  usePrimaries(false)
{
  CommonCtor();
}

Event::Event(bool debugIn,
             bool processSamplersIn,
             int  dataVersionIn):
  tree(nullptr),
  debug(debugIn),
  processSamplers(processSamplersIn),
  dataVersion(dataVersionIn),
  usePrimaries(false)
{
  CommonCtor();
}

Event::~Event()
{
  delete Primary;
  delete PrimaryGlobal;
  delete Eloss;
  delete ElossVacuum;
  delete ElossTunnel;
  delete ElossWorld;
  delete ElossWorldExit;
  delete PrimaryFirstHit;
  delete PrimaryLastHit;
  delete TunnelHit;
  delete Trajectory;
  delete Histos;
  delete Summary;
  delete Info;
  delete ApertureImpacts;
  for (auto s : Samplers)
    {delete s;}
  for (auto s : SamplersC)
    {delete s;}
  for (auto s : SamplersS)
    {delete s;}
  for (auto c : collimators)
    {delete c;}
}

void Event::CommonCtor()
{
#ifdef __ROOTDOUBLE__
  Primary         = new BDSOutputROOTEventSampler<double>();
#else
  Primary         = new BDSOutputROOTEventSampler<float>();
#endif
  PrimaryGlobal      = new BDSOutputROOTEventCoords();
  Eloss              = new BDSOutputROOTEventLoss();
  ElossVacuum        = new BDSOutputROOTEventLoss();
  ElossTunnel        = new BDSOutputROOTEventLoss();
  ElossWorld         = new BDSOutputROOTEventLossWorld();
  ElossWorldContents = new BDSOutputROOTEventLossWorld();
  ElossWorldExit     = new BDSOutputROOTEventLossWorld();
  PrimaryFirstHit    = new BDSOutputROOTEventLoss();
  PrimaryLastHit     = new BDSOutputROOTEventLoss();
  TunnelHit          = new BDSOutputROOTEventLoss();
  Trajectory         = new BDSOutputROOTEventTrajectory();
  Histos             = new BDSOutputROOTEventHistograms();
  Summary            = new BDSOutputROOTEventInfo();
  Info               = new BDSOutputROOTEventInfo();
  ApertureImpacts    = new BDSOutputROOTEventAperture();
}

#ifdef __ROOTDOUBLE__
BDSOutputROOTEventSampler<double>* Event::GetSampler(const std::string& name)
#else
BDSOutputROOTEventSampler<float>* Event::GetSampler(const std::string& name)
#endif
{
  auto found = samplerMap.find(name);
  if (found != samplerMap.end())
    {return found->second;}
  else
    {
      if (tree)
        {
          auto branch = tree->GetBranch(name.c_str());
          if (!branch)
            {return nullptr;}
          else
            {
#ifdef __ROOTDOUBLE__
              Samplers.push_back(new BDSOutputROOTEventSampler<double>(name));
#else
              Samplers.push_back(new BDSOutputROOTEventSampler<float>(name));
#endif
              samplerNames.push_back(name);  // cache the name in a vector
              samplerMap[name] = Samplers.back();// cache the sampler in a map
              tree->SetBranchStatus((name+"*").c_str(), true);
              RelinkSamplers();
              return Samplers.back();
            }
        }
      else
        {return nullptr;}
    }
}

#ifdef __ROOTDOUBLE__
BDSOutputROOTEventSampler<double>* Event::GetSampler(int index)
#else
BDSOutputROOTEventSampler<float>* Event::GetSampler(int index)
#endif
{
  return index >= (int) Samplers.size() ? nullptr : Samplers[index];
}

BDSOutputROOTEventSamplerC* Event::GetSamplerC(const std::string& name)
{
  auto found = samplerCMap.find(name);
  return found != samplerCMap.end() ? found->second : nullptr;
}

BDSOutputROOTEventSamplerC* Event::GetSamplerC(int index)
{
  return index >= (int) SamplersC.size() ? nullptr : SamplersC[index];
}

BDSOutputROOTEventSamplerS* Event::GetSamplerS(const std::string& name)
{
  auto found = samplerSMap.find(name);
  return found != samplerSMap.end() ? found->second : nullptr;
}

BDSOutputROOTEventSamplerS* Event::GetSamplerS(int index)
{
  return index >= (int) SamplersS.size() ? nullptr : SamplersS[index];
}

BDSOutputROOTEventCollimator* Event::GetCollimator(const std::string& name)
{
  // help the user out with some variations on the naming that can be created
  // due to our storage format
  std::vector<std::string> variations = {name,
                                         "COLL_" + name,
                                         "COLL_" + name + "_0"};
  for (const auto& var : variations)
    {
      std::cout << (var == collimatorNames[0]) << std::endl;
      auto found = collimatorMap.find(var);
      if (found != collimatorMap.end())
        {return found->second;}
    }
  return nullptr; // wasn't found
}

BDSOutputROOTEventCollimator* Event::GetCollimator(int index)
{
  if (index >= (int) collimators.size())
    {return nullptr;}
  else
    {return collimators[index];}
}

void Event::SetBranchAddress(TTree* t,
                             const RBDS::VectorString* samplerNamesIn,
                             bool                      allBranchesOn,
                             const RBDS::VectorString* branchesToTurnOn,
                             const RBDS::VectorString* collimatorNamesIn,
                             const RBDS::VectorString* samplerCNamesIn,
                             const RBDS::VectorString* samplerSNamesIn)
{
  if (debug)
    {std::cout << "Event::SetBranchAddress" << std::endl;}

  tree = t; // cache tree used in case we update samplers later
  
  // turn off all branches by default and build up by turning things back on
  // loop speed is dependent on how much we load each event -> only what we need
  t->SetBranchStatus("*", false);

  int nCollimatorsToTurnOn = 0;
  int ithCollimator = 0;
  
  RBDS::VectorString bToTurnOn; // local copy as we modify it
  if (branchesToTurnOn)
    {bToTurnOn = RBDS::VectorString(*branchesToTurnOn);}

  // Few very small things on always for loading
  bToTurnOn.emplace_back("Primary");
  bToTurnOn.emplace_back( dataVersion < 4 ? "Info" : "Summary" );
  bToTurnOn.emplace_back("PrimaryFirstHit");
  bToTurnOn.emplace_back("PrimaryLastHit");
  
  if (allBranchesOn)
    {
      t->SetBranchStatus("*", true);
      bToTurnOn.emplace_back("Eloss");
      bToTurnOn.emplace_back("Histos");
      bToTurnOn.emplace_back( dataVersion > 4 ? "ElossTunnel" : "TunnelHit" );
      bToTurnOn.emplace_back("Trajectory");

      if (dataVersion > 3)
        {
          bToTurnOn.emplace_back("PrimaryGlobal");
          bToTurnOn.emplace_back("ElossVacuum");
          bToTurnOn.emplace_back("ElossWorld");
          bToTurnOn.emplace_back("ElossWorldContents");
          bToTurnOn.emplace_back("ElossWorldExit");
          // add all collimators but ensure not duplicate from user supplied branch names
          if (collimatorNamesIn)
            {
              bToTurnOn.insert(bToTurnOn.end(), collimatorNamesIn->begin(), collimatorNamesIn->end());
              bToTurnOn = RemoveDuplicates(bToTurnOn);
            }
        }
      if (dataVersion > 4)
        {bToTurnOn.emplace_back("ApertureImpacts");}
    }
  bToTurnOn = RemoveDuplicates(bToTurnOn);

  // pre-count the number of collimators and create them all at once. Do this so the vector
  // is never resized and therefore the contents copied / moved in memory. SetBranchAddress
  // takes & (object*) so pointer to pointer, which would break if the contents of the vector
  // move. Note, some 
  for (const auto& name : bToTurnOn)
    {
      if (name.substr(0,4) == "COLL")
        {nCollimatorsToTurnOn++;}
    }
  collimators.resize(nCollimatorsToTurnOn);
      
  for (const auto& name : bToTurnOn)
    {
      if (name.empty())
        {std::cerr << "empty string given as argument for branch name"; continue;}
      std::string nameStar = name + "*";
      std::string nameDot  = name.back() != '.' ? name + "." : name; // note emptystr.back() is undefined behaviour
      if (debug)
        {std::cout << "Event::SetBranchAddress> Turning on branch \"" << nameStar << "\"" << std::endl;}

      bool condition1 = ((*t).GetListOfBranches()->FindObject(name.c_str()))    != nullptr;
      bool condition2 = ((*t).GetListOfBranches()->FindObject(nameDot.c_str())) != nullptr;
      // if we don't find the branch name (tolerating "." suffix), so pass by (some branches are optional)
      if (! (condition1 || condition2) )
        {
          if (debug)
            {std::cout << "Unknown branch name \"" + name + "\"" << std::endl;}
          continue;
        }
      
      t->SetBranchStatus(nameStar.c_str(), true); // turn the branch loading on
      
      // we can't automatically do this as SetBranchAddress must use the pointer
      // of the object type and not the base class (say TObject) so there's no
      // way to easily map these -> ifs
      // special case first, then alphabetical as this is how they'll come from a set (optimisation)

      // we record the return result of SetBranchAddress but don't test it - only
      // for debugging purposes
      Int_t addressSetResult = 0;
      if (name == "Primary")
        {// special case
          usePrimaries = true;
          addressSetResult = t->SetBranchAddress("Primary.", &Primary);
        }
      else if (name == "ApertureImpacts")
        {addressSetResult = t->SetBranchAddress("ApertureImpacts.", &ApertureImpacts);}
      else if (name == "Eloss")
        {addressSetResult = t->SetBranchAddress("Eloss.",           &Eloss);}
      else if (name == "ElossVacuum")
        {addressSetResult = t->SetBranchAddress("ElossVacuum.",     &ElossVacuum);}
      else if (name == "ElossTunnel")
        {addressSetResult = t->SetBranchAddress("ElossTunnel.",     &ElossTunnel);}
      else if (name == "ElossWorld")
        {addressSetResult = t->SetBranchAddress("ElossWorld.",      &ElossWorld);}
      else if (name == "ElossWorldContents")
        {addressSetResult = t->SetBranchAddress("ElossWorldContents.", &ElossWorldContents);}
      else if (name == "ElossWorldExit")
        {addressSetResult = t->SetBranchAddress("ElossWorldExit.",  &ElossWorldExit);}
      else if (name == "Histos")
        {addressSetResult = t->SetBranchAddress("Histos.",          &Histos);}
      else if (name == "Info")
        {addressSetResult = t->SetBranchAddress("Info.",            &Info);}
      else if (name == "Summary")
        {addressSetResult = t->SetBranchAddress("Summary.",         &Summary);}
      else if (name == "PrimaryGlobal")
        {addressSetResult = t->SetBranchAddress("PrimaryGlobal.",   &PrimaryGlobal);}
      else if (name == "PrimaryFirstHit")
        {addressSetResult = t->SetBranchAddress("PrimaryFirstHit.", &PrimaryFirstHit);}
      else if (name == "PrimaryLastHit")
        {addressSetResult = t->SetBranchAddress("PrimaryLastHit.",  &PrimaryLastHit);}
      else if (name == "TunnelHit")
        {addressSetResult = t->SetBranchAddress("TunnelHit.",       &TunnelHit);}
      else if (name == "Trajectory")
        {addressSetResult = t->SetBranchAddress("Trajectory.",      &Trajectory);}
      else if (name.substr(0,4) == "COLL")
        {
          addressSetResult = SetBranchAddressCollimatorSingle(t, nameDot, ithCollimator);
          ithCollimator++;
        }
      if (debug) // has to be done inside loop
        {std::cout << "SetBranchAddress result: " << addressSetResult << std::endl;}
    }
  
  if (debug)
    {
      std::cout << "Event::SetBranchAddress> Primary.            " << Primary            << std::endl;
      std::cout << "Event::SetBranchAddress> PrimaryGlobal.      " << PrimaryGlobal      << std::endl;
      std::cout << "Event::SetBranchAddress> Eloss.              " << Eloss              << std::endl;
      std::cout << "Event::SetBranchAddress> ElossTunnel.        " << ElossTunnel        << std::endl;
      std::cout << "Event::SetBranchAddress> ElossVacuum.        " << ElossVacuum        << std::endl;
      std::cout << "Event::SetBranchAddress> ElossWorld.         " << ElossWorld         << std::endl;
      std::cout << "Event::SetBranchAddress> ElossWorldContents. " << ElossWorldContents << std::endl;
      std::cout << "Event::SetBranchAddress> ElossWorldExit.     " << ElossWorldExit     << std::endl;
      std::cout << "Event::SetBranchAddress> PrimaryFirstHit.    " << PrimaryFirstHit    << std::endl;
      std::cout << "Event::SetBranchAddress> PrimaryLastHit.     " << PrimaryLastHit     << std::endl;
      std::cout << "Event::SetBranchAddress> TunnelHit.          " << TunnelHit          << std::endl;
      std::cout << "Event::SetBranchAddress> Trajectory.         " << Trajectory         << std::endl;
      std::cout << "Event::SetBranchAddress> Histos.             " << Histos             << std::endl;
      std::cout << "Event::SetBranchAddress> Info.               " << Info               << std::endl;
    }

  if (processSamplers || samplerNamesIn)
    {
      unsigned int nrSamplers = samplerNamesIn->size();
      Samplers.resize(nrSamplers); // reserve and nominally instantiate instances.
      for (unsigned int i=0; i < nrSamplers; ++i)
        {
          const auto sampName = (*samplerNamesIn)[i];
#ifdef __ROOTDOUBLE__
          Samplers[i] = new BDSOutputROOTEventSampler<double>(sampName);
#else
          Samplers[i] = new BDSOutputROOTEventSampler<float>(sampName);
#endif
          samplerNames.push_back(sampName);  // cache the name in a vector
          samplerMap[sampName] = Samplers[i];// cache the sampler in a map
            
          t->SetBranchAddress(sampName.c_str(), &Samplers[i]);
          t->SetBranchStatus((sampName+"*").c_str(), true);
          if (debug)
            {std::cout << "Event::SetBranchAddress> " << (*samplerNamesIn)[i] << " " << Samplers[i] << std::endl;}
        }
    }
  
  if (processSamplers || samplerCNamesIn)
    {
      unsigned int nrSamplers = samplerCNamesIn->size();
      SamplersC.resize(nrSamplers); // reserve and nominally instantiate instances.
      for (unsigned int i=0; i < nrSamplers; ++i)
        {
          const auto sampName = (*samplerCNamesIn)[i];
          SamplersC[i] = new BDSOutputROOTEventSamplerC(sampName);
          samplerCNames.push_back(sampName);  // cache the name in a vector
          samplerCMap[sampName] = SamplersC[i];// cache the sampler in a map
          
          t->SetBranchAddress(sampName.c_str(), &SamplersC[i]);
          t->SetBranchStatus((sampName+"*").c_str(), true);
          if (debug)
            {std::cout << "Event::SetBranchAddress> " << (*samplerCNamesIn)[i] << " " << SamplersC[i] << std::endl;}
        }
    }
  
  if (processSamplers || samplerSNamesIn)
    {
      unsigned int nrSamplers = samplerSNamesIn->size();
      SamplersS.resize(nrSamplers); // reserve and nominally instantiate instances.
      for (unsigned int i=0; i < nrSamplers; ++i)
        {
          const auto sampName = (*samplerSNamesIn)[i];
          SamplersS[i] = new BDSOutputROOTEventSamplerS(sampName);
          samplerSNames.push_back(sampName);  // cache the name in a vector
          samplerSMap[sampName] = SamplersS[i];// cache the sampler in a map
          
          t->SetBranchAddress(sampName.c_str(), &SamplersS[i]);
          t->SetBranchStatus((sampName+"*").c_str(), true);
          if (debug)
            {std::cout << "Event::SetBranchAddress> " << (*samplerSNamesIn)[i] << " " << SamplersS[i] << std::endl;}
        }
    }
}

void Event::RelinkSamplers()
{
  if (!tree)
    {throw RBDSException("Event::RelinkSamplers>", "no tree from set branch address");}
  for (const auto& item : samplerMap)
    {tree->SetBranchAddress(item.first.c_str(), (void*)&item.second);}
}

RBDS::VectorString Event::RemoveDuplicates(const RBDS::VectorString& namesIn) const
{
  std::set<std::string> namesSet(namesIn.begin(), namesIn.end());
  auto namesUnique = RBDS::VectorString(namesSet.begin(), namesSet.end());
  return namesUnique;
}

void Event::RegisterCollimator(const std::string& collimatorName)
{
  // be careful of push_back to collimators vector as this might invalidate
  // any &pointers used with SetBranchAddress
  BDSOutputROOTEventCollimator* collimator = new BDSOutputROOTEventCollimator();
  collimatorNames.push_back(collimatorName);
  collimators.push_back(collimator);
  collimatorMap[collimatorName] = collimator;
}

void Event::RegisterSampler(const std::string& samplerName)
{
#ifdef __ROOTDOUBLE__
  BDSOutputROOTEventSampler<double>* sampler = new BDSOutputROOTEventSampler<double>();
  samplerNames.push_back(samplerName);
  Samplers.push_back(sampler);
  samplerMap[samplerName] = sampler;
#else
  BDSOutputROOTEventSampler<float>* sampler = new BDSOutputROOTEventSampler<float>();
  samplerNames.push_back(samplerName);
  Samplers.push_back(sampler);
  samplerMap[samplerName] = sampler;
#endif
}

void Event::SetBranchAddressCollimators(TTree* t,
                                        const RBDS::VectorString* collimatorNamesIn)
{
  if (collimatorNamesIn)
    {
      int i = 0;
      for (const auto& name : *collimatorNamesIn)
        {
          collimators.resize((unsigned int)collimatorNamesIn->size());
          SetBranchAddressCollimatorSingle(t, name, i);
          i++;
        }
    }
}

Int_t Event::SetBranchAddressCollimatorSingle(TTree* t,
                                              const std::string& name,
                                              int i)
{
  // we must not push_back to collimators (vector) as this might expand it
  // and invalidate all addresses to pointers in that vector
  collimators[i] = new BDSOutputROOTEventCollimator();
  collimatorNames.push_back(name);
  collimatorMap[name] = collimators[i];

  // record result purely for debugging purposes
  Int_t addressSetResult = t->SetBranchAddress(name.c_str(), &collimators[i]);
  if (debug)
    {std::cout << "Event::SetBranchAddress> " << name << " " << collimators[i] << std::endl;}
  return addressSetResult;
}

void Event::Fill(Event* other)
{
  Primary->Fill(other->Primary);
  PrimaryGlobal->Fill(other->PrimaryGlobal);
  Eloss->Fill(other->Eloss);
  ElossVacuum->Fill(other->ElossVacuum);
  ElossTunnel->Fill(other->ElossTunnel);
  ElossWorld->Fill(other->ElossWorld);
  ElossWorldContents->Fill(other->ElossWorldContents);
  ElossWorldExit->Fill(other->ElossWorldExit);
  PrimaryFirstHit->Fill(other->PrimaryFirstHit);
  PrimaryLastHit->Fill(other->PrimaryLastHit);
  TunnelHit->Fill(other->TunnelHit);
  Trajectory->Fill(other->Trajectory);
  Histos->FillSimple(other->Histos);
  Summary->Fill(other->Summary);
  Info->Fill(other->Info);
  ApertureImpacts->Fill(other->ApertureImpacts);

  for (unsigned long i = 0; i < Samplers.size(); i++)
    {Samplers[i]->Fill(other->Samplers[i]);}
  
  for (unsigned long i = 0; i < SamplersC.size(); i++)
    {SamplersC[i]->Fill(other->SamplersC[i]);}
  
  for (unsigned long i = 0; i < SamplersS.size(); i++)
    {SamplersS[i]->Fill(other->SamplersS[i]);}

  for (unsigned long i = 0; i < collimators.size(); i++)
    {collimators[i]->Fill(other->collimators[i]);}
}

void Event::Flush()
{
  Primary->Flush();
  PrimaryGlobal->Flush();
  Eloss->Flush();
  ElossVacuum->Flush();
  ElossTunnel->Flush();
  ElossWorld->Flush();
  ElossWorldContents->Flush();
  ElossWorldExit->Flush();
  PrimaryFirstHit->Flush();
  PrimaryLastHit->Flush();
  TunnelHit->Flush();
  Trajectory->Flush();
  Histos->Flush();
  Summary->Flush();
  Info->Flush();
  ApertureImpacts->Flush();
  FlushCollimators();
  FlushSamplers();
}

void Event::FlushSamplers()
{
  for (auto s : Samplers)
    {s->Flush();}
  for (auto s : SamplersC)
    {s->Flush();}
  for (auto s : SamplersS)
    {s->Flush();}
}

void Event::FlushCollimators()
{
  for (auto c : collimators)
    {c->Flush();}
}
