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
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

#include "TFile.h"
#include "TH1.h"
#include "TTree.h"

class CompRes { 
public :
  std::string name;
  std::string objtype;
  int        iStatus;

  virtual std::string print() const {
    std::stringstream ss;
    ss << name << " " << objtype << " " << iStatus << " " ;
    return ss.str();
  }

  friend std::ostream& operator<<(std::ostream &ostr, const CompRes& rhs) {
    ostr << rhs.print(); 
    return ostr;
  }
};

class HistCompRes : public CompRes { 
public : 
  double h1Entries;
  double h2Entries; 
  int    h1NXBins;
  int    h2NXBins;
  double h1XMean;
  double h2XMean;
  double h1XRms;
  double h2XRms;
  double h1Integral;
  double h2Integral;
  double chi2;

  virtual std::string print() const {
    std::stringstream ss;
    ss << CompRes::print();
    ss << h1Entries << "(" << h2Entries << ") " << h1NXBins << "(" << h2NXBins << ") " << h1XMean << "(" << h2XMean << ") " << h1XRms << "(" << h2XRms << ") " 
       << h1Integral << "(" << h2Integral << ") " << chi2;
    return ss.str();
  }
};

class HistCompRes2D : public HistCompRes { 
public : 
  int    h1NYBins;
  int    h2NYBins;
  double h1YMean;
  double h2YMean;
  double h1YRms;
  double h2YRms;

  virtual std::string print() const {
    std::stringstream ss; 
    ss << HistCompRes::print();
    ss << h1NYBins << " " << h2NYBins << " " << h1YMean << " " << h2YMean << " ";
    return ss.str();
  }
};

class TreeCompRes : public CompRes { 
public :
  int t1NEntries;
  int t2NEntries;

  virtual std::string print() const {
    std::stringstream ss; 
    ss << CompRes::print();
    ss << t1NEntries << "(" << t2NEntries << ") ";
    return ss.str();
  }

};

void                            usage();
std::vector<CompRes*>           compareFiles(TFile *f1, TFile *f2);
HistCompRes*                    compareHistograms(TH1 *h1, TH1 *h2); 
TreeCompRes*                    compareTrees(TTree*, TTree* t2);
void                            printResults(std::vector<CompRes*> res);
void                            printFailure(std::vector<CompRes*> res);
bool                            checkResults(std::vector<CompRes*> res);


#define _EXIT_SUCCESS        0;
#define _EXIT_FAILED         1;  
#define _EXIT_INCORRECT_ARGS 2;
#define _EXIT_FILE_NOT_FOUND 3;

int main(int argc, char* argv[]) {
  if(argc != 3) { 
    usage();
    return _EXIT_INCORRECT_ARGS;    
  }

  // open files 
  
  TFile *f1 = new TFile(argv[1]);
  TFile *f2 = new TFile(argv[2]);

  if(f1->IsZombie()) {
    std::cout << "error : could not open " << argv[1] << std::endl;    
    return _EXIT_FILE_NOT_FOUND;
  }

  if(f2->IsZombie()) {
    std::cout << "error : could not open " << argv[2] << std::endl;    
    return _EXIT_FILE_NOT_FOUND;
  }

  std::vector<CompRes*> res = compareFiles(f1,f2);

  printResults(res);
  //  printFailure(res);

  if(!checkResults(res)) { 
    std::cout << "Tests passed" << std::endl;
  }
  else { 
    std::cout << "Tests failed" << std::endl;
    return _EXIT_FAILED;
  }
  
  return _EXIT_SUCCESS;
}

void usage() { 
  std::cout << "usage : robdsimComp rootFile1 rootFile2 " << std::endl;
  std::cout << "Compares rootFile2 to rootFile1 - ie rootFile1 is the reference." << std::endl;
}

std::vector<CompRes*> compareFiles(TFile *f1, TFile *f2)
{
  std::vector<CompRes*> res;
  
  // get list of keys for file 1 
  TList *f1k = f1->GetListOfKeys();
  
  // loop over keys 
  for(int i = 0; i < f1k->GetEntries(); ++i) {
    TObject *ko = f1k->At(i);
    TObject *f1o = f1->Get(ko->GetName());
    
    if(std::string(f1o->ClassName()) == "TH1D") { 
      //      std::cout << "Compare TH1D" << std::endl;
      TH1 *f1h = (TH1*)f1->Get(ko->GetName());
      TH1 *f2h = (TH1*)f2->Get(ko->GetName());
      if (!f2h)
	{
	  std::cout << "No matching histrogram named \"" << ko->GetName() << "\"" << std::endl;
	  continue;
	}
      HistCompRes *hcr = compareHistograms(f1h,f2h);
      res.push_back(hcr);
    }
    else if(std::string(f1o->ClassName()) == "TTree")
      {
	TTree *f1t = (TTree*)f1->Get(ko->GetName());
	TTree *f2t = (TTree*)f2->Get(ko->GetName());
	if (!f2t)
	  {
	    std::cout << "No matching tree named \"" << ko->GetName() << "\"" << std::endl;
	    continue;
	  }
	TreeCompRes *tcr = compareTrees(f1t,f2t);
	res.push_back(tcr);
      }
  }
   
  return res;
}

HistCompRes* compareHistograms(TH1 *h1, TH1 *h2)
{
  // Take difference between histograms
  HistCompRes *c = new HistCompRes();
  c->name      = h1->GetName();
  c->objtype   = "TH";
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

  c->chi2   = 0.0;
  int ndof = 0; 
  for(int i=0;i < h1->GetNbinsX(); i++) { 
    //    std::cout << h1->GetBinContent(i) << " " << h2->GetBinContent(i) << " " << h1->GetBinError(i) << std::endl;
    if(h1->GetBinError(i) > 0) {
      c->chi2 += std::pow(h1->GetBinContent(i)-h2->GetBinContent(i),2)/(std::pow(h1->GetBinError(i),2)+std::pow(h2->GetBinError(i),2));
      ndof++;
    }
  }
  // chi2 per dof
  c->chi2 /= ndof;

  c->iStatus = 0;
  if(c->chi2 > 40.0)
    {c->iStatus = 1;}

  return c;
}

TreeCompRes* compareTrees(TTree *t1, TTree *t2) { 
  TreeCompRes *c = new TreeCompRes();  
  c->name       = t1->GetName();
  c->objtype   = "TTree";
  c->t1NEntries = t1->GetEntries();
  c->t2NEntries = t2->GetEntries();

  TObjArray *oa1 = t1->GetListOfBranches(); 
  TObjArray *oa2 = t2->GetListOfBranches(); 
  
  for(int j = 0; j<oa1->GetSize(); ++j) {
    for(int i = 0; i<t1->GetEntries(); ++i) {

      double t1v = 0;
      double t2v = 0;

      TBranch *b1 = (TBranch*)((*oa1)[j]);
      TBranch *b2 = (TBranch*)((*oa2)[j]);

      b1->SetAddress(&t1v);
      b2->SetAddress(&t2v);

      t1->GetEntry(i);
      t2->GetEntry(i);

      if(std::abs((t1v - t2v )/t1v) > 0.01) {
	c->iStatus = 1;
      }
      else {
	c->iStatus = 0;
      }
    }
    
  }
  return c;
}

bool checkResults(std::vector<CompRes*> res) {
  for(auto i : res) {   
    if(i->iStatus != 0) {
      return true;
    }
  }
  return false;
}

void printResults(std::vector<CompRes*> res) {   
  for(auto i : res) { 
    //    i->print();    
    std::cout << *i << std::endl;
  }  
}

void printFailure(std::vector<CompRes*> res) {
  for(auto i : res) { 
    if(i->iStatus != 0) {
      std::cout << *i << std::endl;
    }
  }  
}  
