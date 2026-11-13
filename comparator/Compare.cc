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
#include "Compare.hh"
#include "Result.hh"
#include "ResultEvent.hh"
#include "ResultEventTree.hh"
#include "ResultHistogram.hh"
#include "ResultHistogram2D.hh"
#include "ResultSampler.hh"
#include "ResultTree.hh"

#include "analysis/Event.hh"
#include "analysis/Model.hh"
#include "analysis/Options.hh"

#include "BDSDebug.hh"
#include "BDSOutputROOTEventOptions.hh"
#include "BDSOutputROOTEventSampler.hh"
#include "BDSVersionData.hh"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "TBranch.h"
#include "TChain.h"
#include "TDirectory.h"
#include "TFile.h"
#include "TH1.h"
#include "TObjArray.h"
#include "TTree.h"

std::vector<Result*> Compare::Files(TFile* f1, TFile* f2)
{
  std::vector<Result*> results; 
  // A TFile inherits TDirectory, so we simply use the TDirectory function.
  Compare::Directories((TDirectory*)f1, (TDirectory*)f2, results);
  return results;
}

void Compare::Directories(TDirectory* d1,
			  TDirectory* d2,
			  std::vector<Result*>& results)
{
  // record original directory in file.
  TDirectory* originalDirectory = TDirectory::CurrentDirectory();
  
  d1->cd(); // change into the directory
  
  // get list of keys for file 1 & loop over
  TList* d1k = d1->GetListOfKeys();
  for (int i = 0; i < d1k->GetEntries(); ++i)
    {
      TObject* keyObject = d1k->At(i); // key object in list of keys
      TObject* d1o       = d1->Get(keyObject->GetName()); // object in file

      std::string objectName = std::string(keyObject->GetName());
      std::string className  = std::string(d1o->ClassName());

      // get storePrimaries from options tree.
      if (objectName == "Options" && className == "TTree")
        {
          std::vector<const char*> names;
          TTree* options = (TTree*) d1->Get(objectName.c_str());
          Options* optLocal = new Options();
          optLocal->SetBranchAddress(options);
          options->GetEntry(0);
          hasPrimaries = optLocal->options->storePrimaries;
          delete optLocal;  // delete - no need to store in memory.
        }

      if (className == "TDirectory" || className == "TDirectoryFile") // recursion!
	{
	  TDirectory* subD1 = (TDirectory*)d1->Get(objectName.c_str());
	  TDirectory* subD2 = (TDirectory*)d2->Get(objectName.c_str());
	  if (!subD2)
	    {
	      Compare::PrintNoMatching(className, objectName);
	      continue;
	    }
	  Compare::Directories(subD1, subD2, results);
	}
      else if(className == "TH1D")
	{
	  TH1* d1h = (TH1*)d1->Get(objectName.c_str());
	  TH1* d2h = (TH1*)d2->Get(objectName.c_str());
	  if (!d2h)
	    {
	      Compare::PrintNoMatching(className, objectName);
	      continue;
	    }
	  Compare::Histograms(d1h, d2h, results);
	}
      else if(className == "TTree")
	{
	  TTree* d1t = (TTree*)d1->Get(objectName.c_str());
	  TTree* d2t = (TTree*)d2->Get(objectName.c_str());
	  if (!d2t)
	    {
	      Compare::PrintNoMatching(className, objectName);
	      continue;
	    }
	  Compare::Trees(d1t, d2t, results);
	}
      else
	{
	  std::cout << "No comparison written for object named " << objectName
		    << " of type " << className << std::endl;
	}
    }
  originalDirectory->cd();
}

void Compare::Histograms(TH1* h1, TH1* h2, std::vector<Result*>& results)
{
  // Take difference between histograms
  ResultHistogram* c = new ResultHistogram();
  c->name      = h1->GetName();
  c->tolerance = Compare::Chi2Tolerance;
  c->objtype   = "TH1";
  c->h1Entries = h1->GetEntries();
  c->h2Entries = h2->GetEntries();
  c->h1NXBins  = h1->GetNbinsX();
  c->h2NXBins  = h2->GetNbinsX();
  c->h1XMean   = h1->GetMean();
  c->h2XMean   = h2->GetMean();
  c->h1XRms    = h1->GetRMS();
  c->h2XRms    = h2->GetRMS();
  c->h1Integral= h1->Integral();
  c->h2Integral= h2->Integral();

  // check for a bad comparison
  if (c->h1NXBins != c->h2NXBins)
    {
      c->passed = false;
      results.push_back(c);
      return;
    }

  c->chi2   = 0.0;
  int ndof = 0; 
  for (int i=0;i < h1->GetNbinsX(); i++)
    { 
      //    std::cout << h1->GetBinContent(i) << " " << h2->GetBinContent(i) << " " << h1->GetBinError(i) << std::endl;
      if (h1->GetBinError(i) > 0)
	{
	  c->chi2 += std::pow(h1->GetBinContent(i)-h2->GetBinContent(i),2)/(std::pow(h1->GetBinError(i),2)+std::pow(h2->GetBinError(i),2));
	  ndof++;
	}
    }
  // chi2 per dof
  if (!std::isnormal(ndof))
    {ndof = 1;}
  c->chi2 /= ndof;
  
  c->passed = true;
  if(c->chi2 > Compare::Chi2Tolerance)
    {c->passed = false;}

  results.push_back(c);
}

void Compare::Trees(TTree* t1, TTree* t2, std::vector<Result*>& results)
{
  std::vector<std::string> treesToIgnore = {"Header", "Model", "Options", "Run", "Beam", "ParticleData"};

  // skip some trees
  std::string treeName = t1->GetName();
  if (std::find(treesToIgnore.begin(), treesToIgnore.end(), treeName) != treesToIgnore.end())
    {return;}
  else if (!strcmp(treeName.c_str(), "Optics"))
    {
      Compare::Optics(t1, t2, results);
      return;
    }
  else if (!strcmp(treeName.c_str(), "Event"))
    {
      // We need the sampler names which are in the Model tree. If we have an
      // event tree, we must have a Model tree too!
      TDirectory* dir = t1->GetDirectory();
  
      std::vector<std::string> samplerNames;
      std::vector<std::string> samplerCNames;
      std::vector<std::string> samplerSNames;
      
      TTree* modTree = dynamic_cast<TTree*>(dir->Get("Model"));
      bool warn = false;
      if (!modTree)
	{warn = true;}  // shouldn't really happen, but we can't compare the samplers
      else if (modTree->GetEntries() == 0)
	{warn = true;}
      else
	{
	  Model* mod = new Model();
	  mod->SetBranchAddress(modTree);
	  modTree->GetEntry(0);
	  samplerNames = mod->SamplerNames();
	  samplerCNames = mod->SamplerCNames();
	  samplerSNames = mod->SamplerSNames();
	  delete mod;
	}
      if (warn)
	{std::cout << "Model not stored so can't compare sampler branches." << std::endl;}
      Compare::EventTree(t1, t2, results, samplerNames, samplerCNames, samplerSNames);
      return;
    }
  
  ResultTree* c = new ResultTree();
  c->name       = treeName;
  c->objtype    = "TTree";
  c->t1NEntries = (int)t1->GetEntries();
  c->t2NEntries = (int)t2->GetEntries();

  TObjArray* oa1 = t1->GetListOfBranches(); 
  TObjArray* oa2 = t2->GetListOfBranches();
  
  for(int j = 0; j<oa1->GetSize(); ++j)
    {// loop over branches
      bool branchFailed = false;
      TBranch* b1 = (TBranch*)((*oa1)[j]);
      TBranch* b2 = (TBranch*)((*oa2)[j]);
      double  t1v = 0;
      double  t2v = 0;
      b1->SetAddress(&t1v);
      b2->SetAddress(&t2v);
        for(int i = 0; i<t1->GetEntries(); ++i)
	{// loop over entries	  
	  t1->GetEntry(i);
	  t2->GetEntry(i);
	  
	  if(std::abs((t1v - t2v )/t1v) > Compare::TreeTolerance)
	    {
	      c->passed    = false;
	      branchFailed = true;
	    }
	  else
	    {c->passed = true;}
	}
      if (branchFailed)
	{c->offendingBranches.emplace_back(std::string(b2->GetName()));}
  }
  results.push_back(c);
}

void Compare::Optics(TTree* t1, TTree* t2, std::vector<Result*>& results)
{
  ResultTree* c = new ResultTree();
  c->name       = t1->GetName();
  c->passed     = true; // set default to pass
  c->objtype    = "TTree(Optics)";
  c->t1NEntries = (int)t1->GetEntries();
  c->t2NEntries = (int)t2->GetEntries();

  // whether the value of a variable should be >= 0
  std::map<std::string, bool> positiveValues = {
    {"Emitt_x"    , true},
    {"Emitt_y"    , true},
    {"Alpha_x"    , false},
    {"Alpha_y"    , false},
    {"Beta_x"     , true},
    {"Beta_x"     , true},
    {"Gamma_x"    , true},
    {"Gamma_y"    , true},
    {"Disp_x"     , false},
    {"Disp_y"     , false},
    {"Disp_xp"    , false},
    {"Disp_yp"    , false},
    {"Mean_x"     , false},
    {"Mean_y"     , false},
    {"Mean_xp"    , false},
    {"Mean_yp"    , false},
    {"S"          , false},
    {"Npart"      , true},
    {"Mean_E"     , false},
    {"Mean_t"     , false}
  };

  // branches we won't compare as they're error branches
  std::vector<std::string> errorBranches = {
    "Sigma_Emitt_x",  "Sigma_Emitt_y",
    "Sigma_Alpha_x",  "Sigma_Alpha_y",
    "Sigma_Beta_x",   "Sigma_Beta_y",
    "Sigma_Gamma_x",  "Sigma_Gamma_y",
    "Sigma_Disp_x",   "Sigma_Disp_y",
    "Sigma_Disp_xp",  "Sigma_Disp_yp",
    "Sigma_Mean_x",   "Sigma_Mean_y",
    "Sigma_Sigma_x",  "Sigma_Sigma_y",
    "Sigma_Sigma_xp", "Sigma_Sigma_yp",
    "Sigma_Mean_E",   "Sigma_Sigma_E",
    "Sigma_Mean_t",   "Sigma_Sigma_t"
  };
  
  TObjArray *oa1 = t1->GetListOfBranches(); 

  // loop over branches
  // for each branch loop over all entries and compare to reference file
  for (int j = 0; j< oa1->GetSize(); ++j)
    {
      TBranch* b1 = (TBranch*)(*oa1)[j];
      std::string branchName = std::string(b1->GetName());
      
      bool branchFailed    = false;
      bool shouldBeGTEZero = false;
      
      // Don't compare an error branch to that in the reference file
      // as it'll be different (likely reference is at higher statistics).
      // We use a specific set of vector names because we still want to compare
      // sigma for some values.
      // However, we should still check they have +ve, not nan values.
      if (Compare::IsInVector(branchName, errorBranches))
	{
	  double t1v = 0;
	  b1->SetAddress(&t1v);
	  for(int i = 0; i<t1->GetEntries(); ++i)
	    {// loop over entries
	      t1->GetEntry(i);
	      // errors (sigmas) should be +ve and finite
	      if (LTZero(t1v) || NanOrInf(t1v))
		{
		  branchFailed = true;
		  break;
		}
	    }
	}
      else
	{// not an error branch - compare with reference file
	  // whether this variable should always be >= 0
	  shouldBeGTEZero = positiveValues[branchName];
	    
	  std::string errBranchName = "Sigma_" + branchName;
	  
	  TBranch* b1err = t1->GetBranch(errBranchName.c_str());
	  TBranch* b2err = t2->GetBranch(errBranchName.c_str());     
	  if (!b1err || !b2err)
	    {continue;} // There's no appropriate error branch - don't compare
	  
	  TBranch* b2 = t2->GetBranch(branchName.c_str());

	  // setup local variables and link to root file
	  double t1v = 0;
	  double t1e = 0;
	  double t2v = 0;
	  double t2e = 0;
	  b1->SetAddress(&t1v);
	  b1err->SetAddress(&t1e);
	  b2->SetAddress(&t2v);
	  b2err->SetAddress(&t2e);

	  // loop over all entries and compare
	  for(int i = 0; i<t1->GetEntries(); ++i)
	    {
	      t1->GetEntry(i);
	      t2->GetEntry(i);
	      
	      if (!std::isnormal(t1e) || !std::isnormal(t2e))
		{break;} // skip test when errors are 0

	      // check for nans or negative values that shouldn't be there
	      // only check if greater than zero if they should be, otherwise no need to check.
	      bool valueIsGood = true;
	      if (shouldBeGTEZero)
	        {
	      	  if (!GTEZero(t1v) || !GTEZero(t2v))
	            {valueIsGood = false;}
	        }
	      if (NanOrInf(t1v) || NanOrInf(t2v) || !valueIsGood)
		{
		  branchFailed = true;
		  std::cout << "Invalid value found for branch \"" << branchName << "\"" << G4endl;
		  std::cout << t1v << " " << t2v << " " << t1e << " " << t2e << " " << std::endl;
		  break; // skip testing rest of branch entries
		}
		  
	      // Here only one entry so ndof = 1
	      double chi2 = std::pow(t1v - t2v, 2) / (std::pow(t1e,2) + std::pow(t2e,2));
	      
	      branchFailed = chi2 > Compare::OpticsSimgaTolerance;
	      
	      if (branchFailed)
		{
		  std::cout << t1v << " " << t2v << " " << t1e << " "
			    << t2e << " " << chi2 << std::endl;
		  break; // skip testing rest of branch entries
		}
	    }
	}

      // record result
      if (branchFailed)
	{
	  std::cout << "Branch was " << branchName << std::endl << std::endl;
	  c->offendingBranches.push_back(branchName);
	  c->passed = false;
	}
    }
  results.push_back(c);
}

void Compare::EventTree(TTree* t1, TTree* t2, std::vector<Result*>& results,
			const std::vector<std::string>& samplerNames,
			const std::vector<std::string>& samplerCNames,
			const std::vector<std::string>& samplerSNames)
{
  ResultEventTree* ret = new ResultEventTree();
  ret->name            = t1->GetName();
  ret->passed          = true; // set default to pass
  ret->objtype         = "TTree(Event)";
  ret->t1NEntries      = (int)t1->GetEntries();
  ret->t2NEntries      = (int)t2->GetEntries();

  // Don't proceed if uneven number of entries of the even tree
  // ie different number of events
  if (ret->t1NEntries != ret->t2NEntries)
    {
      ret->passed = false;
      results.push_back(ret);
      return;
    }

  // Need to tell Event to process samplers at construction time.
  G4bool processSamplers = !samplerNames.empty();
  Event* evtLocal1 = new Event(/*debug=*/false, processSamplers, BDSIM_DATA_VERSION);
  Event* evtLocal2 = new Event(/*debug=*/false, processSamplers, BDSIM_DATA_VERSION);
  evtLocal1->SetBranchAddress(t1, &samplerNames, false, nullptr, nullptr, &samplerCNames, &samplerSNames);
  evtLocal2->SetBranchAddress(t2, &samplerNames, false, nullptr, nullptr, &samplerCNames, &samplerSNames);

  for (long int i = 0; i < (long int)t1->GetEntries(); i++)
    {
      ResultEvent re = ResultEvent();
      re.name    = std::to_string(i);
      re.passed  = true; // default true
      re.objtype = "Event of Event Tree";

#ifdef DEBUGOUTPUT
      Int_t bytesLoaded1 = t1->GetEntry(i);
      Int_t bytesLoaded2 = t2->GetEntry(i);
      std::cout << "Bytes loaded (1) " << bytesLoaded1 << std::endl;
      std::cout << "Bytes loaded (2) " << bytesLoaded2 << std::endl;
      std::cout << evtLocal1->GetPrimaries()->n << std::endl;
      std::cout << evtLocal2->GetPrimaries()->n << std::endl;
      std::cout << evtLocal1 << " " << evtLocal2 << std::endl;
      std::cout << evtLocal1->GetPrimaries() << " " << evtLocal2->GetPrimaries() << std::endl;
#else
      t1->GetEntry(i);
      t2->GetEntry(i);
#endif
      
      if (hasPrimaries)
        {Compare::Sampler(evtLocal1->GetPrimaries(), evtLocal2->GetPrimaries(), &re);}
      for (auto j = 0; j < (int)evtLocal1->Samplers.size(); j++)
	{
	  Compare::Sampler(evtLocal1->Samplers[j], evtLocal2->Samplers[j], &re);
	}

      ret->eventResults.push_back(re);
      if (!re.passed)
	{
	  ret->passed = false;
	  break;
	}
    }
  results.push_back(ret);

  delete evtLocal1;
  delete evtLocal2;
}

#ifdef __ROOTDOUBLE__
void Compare::Sampler(BDSOutputROOTEventSampler<double>* e1,
		      BDSOutputROOTEventSampler<double>* e2,
		      ResultEvent* re)
#else
void Compare::Sampler(BDSOutputROOTEventSampler<float>* e1,
		      BDSOutputROOTEventSampler<float>* e2,
		      ResultEvent* re)
#endif
{
  std::string samplerName = e1->samplerName;
  if (samplerName == "sampler") // the default name in BDSOutputROOTEventSampler.cc
    {samplerName = e2->samplerName;} // could still be the default value but worth a try
  ResultSampler rs(samplerName);
  
  if (e1->n != e2->n)
    {rs.passed = false; rs.offendingLeaves.emplace_back("n");}
  else
    {
      // only one z / S entry, so only check once
      if (Diff(e1->z, e2->z))
        {rs.passed = false; rs.offendingLeaves.emplace_back("z");}
      if (Diff(e1->S, e2->S))
        {rs.passed = false; rs.offendingLeaves.emplace_back("S");}
      
      for (int i = 0; i < e1->n; i++)
	{
	  if (Diff(e1->energy, e2->energy, i))
	    {rs.passed = false; rs.offendingLeaves.emplace_back("energy");}
	  if (Diff(e1->x, e2->x, i))
	    {rs.passed = false; rs.offendingLeaves.emplace_back("x");}
	  if (Diff(e1->y, e2->y, i))
	    {rs.passed = false; rs.offendingLeaves.emplace_back("y");}
	  if (Diff(e1->xp, e2->xp, i))
	    {rs.passed = false; rs.offendingLeaves.emplace_back("xp");}
	  if (Diff(e1->yp, e2->yp, i))
	    {rs.passed = false; rs.offendingLeaves.emplace_back("yp");}
	  if (Diff(e1->zp, e2->zp, i))
	    {rs.passed = false; rs.offendingLeaves.emplace_back("zp");}
	  if (Diff(e1->p, e2->p, i))
	    {rs.passed = false; rs.offendingLeaves.emplace_back("p");}
	  if (Diff(e1->T, e2->T, i))
	    {rs.passed = false; rs.offendingLeaves.emplace_back("T");}
	  if (Diff(e1->partID, e2->partID, i))
	    {rs.passed = false; rs.offendingLeaves.emplace_back("partID");}
	  if (Diff(e1->charge, e2->charge, i))
	    {rs.passed = false; rs.offendingLeaves.emplace_back("charge");}
	  if (Diff(e1->isIon, e2->isIon, i))
	    {rs.passed = false; rs.offendingLeaves.emplace_back("isIon");}
	  if (Diff(e1->ionA, e2->ionA, i))
	    {rs.passed = false; rs.offendingLeaves.emplace_back("ionA");}
	  if (Diff(e1->ionZ, e2->ionZ, i))
	    {rs.passed = false; rs.offendingLeaves.emplace_back("ionZ");}
	  if (Diff(e1->nElectrons, e2->nElectrons, i))
	    {rs.passed = false; rs.offendingLeaves.emplace_back("nElectrons");}
	}
    }
  
  // update parent result status
  if (!rs.passed)
    {re->passed = false; re->offendingBranches.push_back(samplerName);}
  re->samplerResults.push_back(rs);
}

bool Compare::Summarise(const std::vector<Result*>& results)
{
  bool allPassed = true;
  const int titleWidth = 20;
  const int fullWidth  = titleWidth + 22;
  std::cout << std::endl;
  std::cout << "N results: " << results.size() << std::endl;
  std::cout << "Comparison: " << std::setw(titleWidth) << "Object Name" << "   "
	    << "Result" << std::endl;
  std::cout << std::setfill('-') << std::setw(fullWidth) << " " << std::endl;
  std::cout << std::setfill(' ');
  for (const auto& result : results)
    {
      if (!(result->passed))
	{
	  allPassed = false;
	  std::cout << *result << std::endl;
	}
      else
	{std::cout << "Comparison: " << std::setw(20) << result->name << " : Passed" << std::endl;}
    }
  return allPassed;
}

void Compare::PrintNoMatching(std::string className, std::string objectName)
{
  std::cout << "Comparison file has no " << className << " called " << objectName << ". Skipping" << std::endl;
}

bool Compare::StringStartsWith(std::string aString, std::string prefix)
{
  try
    {
      if (aString.compare(0, prefix.length(), prefix) == 0)
	{return true;}
      else
	{return false;}
    }
  catch(const std::out_of_range&)
    {return false;} // if string isn't as long as prefix
  return false; // for static analysis warning
}

bool Compare::IsInVector(std::string key, const std::vector<std::string>& vec)
{
  return std::find(vec.begin(), vec.end(), key) != vec.end();
}
