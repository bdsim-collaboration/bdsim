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
#include "BDSBH4DBase.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"

#include "TFile.h"

#include <string>

ClassImp(BDSBH4DBase)

BDSBH4DBase::BDSBH4DBase():
  h_nxbins(0),
  h_nybins(0),
  h_nzbins(0),
  h_nebins(0),
  h_xmin(0), h_xmax(0),
  h_ymin(0), h_ymax(0),
  h_zmin(0), h_zmax(0),
  h_emin(0), h_emax(0),
  h_entries(0)
{;}

BDSBH4DBase::BDSBH4DBase(unsigned int nXBinsIn, unsigned int nYBinsIn,
			 unsigned int nZBinsIn, unsigned int nEBinsIn,
			 double xMinIn, double xMaxIn, double yMinIn, double yMaxIn,
			 double zMinIn, double zMaxIn, double eMinIn, double eMaxIn,
			 const std::string& nameIn,
			 const std::string& titleIn,
			 const std::string& escaleIn):
  h_nxbins(nXBinsIn), h_nybins(nYBinsIn), h_nzbins(nZBinsIn), h_nebins(nEBinsIn),
  h_xmin(xMinIn), h_xmax(xMaxIn),
  h_ymin(yMinIn), h_ymax(yMaxIn),
  h_zmin(zMinIn), h_zmax(zMaxIn),
  h_emin(eMinIn), h_emax(eMaxIn),
  h_name(nameIn),
  h_title(titleIn),
  h_escale(escaleIn),
  h_entries(0)
{;}

BDSBH4DBase::BDSBH4DBase(unsigned int nXBinsIn, unsigned int nYBinsIn, unsigned int nZBinsIn,
			 double xMinIn, double xMaxIn,
			 double yMinIn, double yMaxIn,
			 double zMinIn, double zMaxIn,
			 const std::string& nameIn,
			 const std::string& titleIn,
			 const std::string& escaleIn,
			 const std::vector<double>& eBinEdgesIn):
  h_nxbins(nXBinsIn), h_nybins(nYBinsIn), h_nzbins(nZBinsIn),
  h_xmin(xMinIn), h_xmax(xMaxIn),
  h_ymin(yMinIn), h_ymax(yMaxIn),
  h_zmin(zMinIn), h_zmax(zMaxIn),
  h_name(nameIn),
  h_title(titleIn),
  h_escale(escaleIn),
  h_ebinsedges(eBinEdgesIn),
  h_entries(0)
{
  if (eBinEdgesIn.size() < 2)
    {throw BDSException(__METHOD_NAME__, "bin edges vector must be at least 2 numbers");}
  h_nebins = (unsigned int)(eBinEdgesIn.size() - 1);
  h_emin   = eBinEdgesIn[0];
  h_emax   = eBinEdgesIn.back();
}

int BDSBH4DBase::GetNbinsX() const
{
  return static_cast<int>(h_nxbins);
}

int BDSBH4DBase::GetNbinsY() const
{
  return static_cast<int>(h_nybins);
}

int BDSBH4DBase::GetNbinsZ() const
{
  return static_cast<int>(h_nzbins);
}

int BDSBH4DBase::GetNbinsE() const
{
  return static_cast<int>(h_nebins);
}

const char* BDSBH4DBase::GetName() const
{
  return h_name.c_str();
}

const char* BDSBH4DBase::GetTitle() const
{
  return h_title.c_str();
}

unsigned long BDSBH4DBase::GetEntries_BDSBH4D() const
{
  return h_entries;
}

void BDSBH4DBase::SetName(const char* name)
{
  h_name = std::string(name);
}

void BDSBH4DBase::SetTitle(const char* title)
{
  h_title = std::string(title);
}

void BDSBH4DBase::SetEntries_BDSBH4D(double i)
{
  h_entries = static_cast<unsigned long>(i);
}
