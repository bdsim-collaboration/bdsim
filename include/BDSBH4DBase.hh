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
#ifndef BDSBH4DBASE_H
#define BDSBH4DBASE_H

#include "Rtypes.h"
#include "TH1D.h"
#include "TTree.h"
#include "TFile.h"
#include "TObject.h"

#include <string>
#include <vector>

/** @brief Base class for the 4D histogram classes.
 *
 * @author Eliott Ramoisiaux
 */

class BDSBH4DBase: public TH1D
{
public:
  BDSBH4DBase();
  /// Constructor where 'e' dimension specifications are given.
  BDSBH4DBase(unsigned int h_nxbinsIn, unsigned int h_nybinsIn,
	      unsigned int h_nzbinsIn, unsigned int h_nebinsIn,
	      double xminIn, double xmaxIn, double yminIn, double ymaxIn,
	      double zminIn, double zmaxIn, double eminIn, double emaxIn,
	      const std::string& nameIn,
	      const std::string& titleIn,
	      const std::string& escaleIn);
  /// Alternative where the 'e' dimension specifications are determined from a supplied vector of edges.
  BDSBH4DBase(unsigned int h_nxbinsIn, unsigned int h_nybinsIn, unsigned int h_nzbinsIn,
	      double xminIn, double xmaxIn,
	      double yminIn, double ymaxIn,
	      double zminIn, double zmaxIn,
	      const std::string& nameIn,
	      const std::string& titleIn,
	      const std::string& escaleIn,
	      const std::vector<double>& eBinEdgesIn);
  virtual ~BDSBH4DBase() override = default;

  int GetNbinsX() const final;
  int GetNbinsY() const final;
  int GetNbinsZ() const final;
  int GetNbinsE() const;
  const char* GetName() const override;
  const char* GetTitle() const override;
  unsigned long GetEntries_BDSBH4D() const;
  
  void SetName(const char*) override;
  void SetTitle(const char*) override;
  void SetEntries_BDSBH4D(double);
  
  virtual BDSBH4DBase& operator+=(const BDSBH4DBase& other) = 0;
  virtual void Reset_BDSBH4D() = 0;
  BDSBH4DBase* Clone(const char*) const override = 0;
  virtual int Fill_BDSBH4D(double, double, double, double) = 0;
  virtual void Set_BDSBH4D(int, int, int, int, double) = 0;
  virtual void Set_BDSBH4D(int global, double value) = 0;
  virtual void SetError_BDSBH4D(int, int, int, int, double) = 0;
  virtual void SetError_BDSBH4D(int global, double value) = 0;
  virtual void Add_BDSBH4D(BDSBH4DBase*) = 0;
  virtual double At(int, int, int, int) = 0;
  virtual double At(int) = 0; ///< Global index access.
  virtual double AtError(int, int, int, int) = 0;
  virtual double LowBinEdgeAt(int, int, int, int) = 0;
  virtual double HighBinEdgeAt(int, int, int, int) = 0;
  virtual void Print_BDSBH4D(bool with_zero_values = true) = 0;
  virtual void Print_BDSBH4D(int, int, int, int) = 0;
  
  unsigned int h_nxbins;
  unsigned int h_nybins;
  unsigned int h_nzbins;
  unsigned int h_nebins;
  double h_xmin;
  double h_xmax;
  double h_ymin;
  double h_ymax;
  double h_zmin;
  double h_zmax;
  double h_emin;
  double h_emax;
  std::string h_name;
  std::string h_title;
  std::string h_escale;
  std::vector<double> h_ebinsedges;
  unsigned long h_entries;
  
  ClassDefOverride(BDSBH4DBase,1);
};

#endif
