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
#ifndef TH1SET_H
#define TH1SET_H

#include "HistSparse1D.hh"

#include "TH1.h"
#include "TH1D.h"

#include <map>

class TList;
class TPad;

/**
 * @brief TH1D but with a category axis.
 * 
 * @author Laurie Nevay
 */

class TH1Set: public TH1D
{
public:
  typedef HistSparse1D<long long int> HistSparse;
  TH1Set();
  TH1Set(const char* name, const char* title);
  ~TH1Set() override;

  const HistSparse& GetHistSparse1D() const {return data;}
  
  Int_t Fill(const char*, Double_t)  override {return 0;}
  Int_t Fill(Double_t x)             override {return this->Fill(x,1.0);}
  Int_t Fill(Double_t x, Double_t w) override;
  void  DoFillN(Int_t ntimes, const Double_t* x, const Double_t* w, Int_t stride=1) override;
  
  Int_t AddNewBin(long long int x);
  
  // Have to implement these to prevent a warning in older GCC.
  Bool_t Add(TF1*, Double_t, Option_t*) override {return false;}
  Bool_t Add(const TH1*, const TH1*, Double_t, Double_t) override {return false;}
  // The one we implement.
  Bool_t Add(const TH1*, Double_t) override;
  
  Double_t GetBinContentByAbscissa(long long int x) const;
  Double_t GetBinErrorByAbscissa(long long int x) const;
  void     SetBinContentByAbscissa(long long int x, Double_t newValue);
  void     SetBinErrorByAbscissa(long long int x, Double_t newError);
  
  HistSparse data;
  std::map<long long int, Int_t> abscissaToBinIndex;
  
  /*
virtual Bool_t Add(TF1 *h1, Double_t c1=1, Option_t *option="");
TObject* Clone(const char* newname = "") const;

virtual Int_t FindBin(Double_t x, Double_t y = 0, Double_t z = 0);
virtual Double_t GetBinContent(Int_t bin) const;
virtual Double_t GetBinError(Int_t bin) const;
//Double_t GetMaximum() const;
virtual Double_t GetMaximum(Double_t maxval) const;
//  Double_t     GetMinimum() const;
virtual Double_t GetMinimum(Double_t minval) const;

virtual Double_t     Integral(Option_t* option = "") const;
virtual Long64_t     Merge(TCollection *);

virtual void SavePrimitive(std::ostream& out, Option_t* option = "");
virtual void Reset(Option_t *option);
virtual void Scale(Double_t c1 = 1, Option_t* option = "");
virtual void SetBinContent(Int_t bin, Double_t content);
virtual void SetBinError(Int_t bin, Double_t error);
virtual void GetStats(Double_t *stats) const;
*/
  ClassDefOverride(TH1Set,1)
};

#endif
