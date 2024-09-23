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
#include "BDSOutputROOTEventHistograms.hh"

#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"
#include "BDSBH4DBase.hh"
#include "BDSBH4D.hh"

#ifndef USE_BOOST
#include "BDSBH4DTypeDefs.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#endif

ClassImp(BDSOutputROOTEventHistograms)

BDSOutputROOTEventHistograms::BDSOutputROOTEventHistograms()
{
  TH1D::AddDirectory(kFALSE);
  TH2D::AddDirectory(kFALSE);
  TH3D::AddDirectory(kFALSE);
}

BDSOutputROOTEventHistograms::BDSOutputROOTEventHistograms(const BDSOutputROOTEventHistograms& rhs):
  TObject(rhs)
{
  Fill(&rhs);
}

BDSOutputROOTEventHistograms::BDSOutputROOTEventHistograms(std::vector<TH1D*>& histograms1DIn,
                                                           std::vector<TH2D*>& histograms2DIn,
                                                           std::vector<TH3D*>& histograms3DIn,
                                                           std::vector<BDSBH4DBase*>& histograms4DIn):
  histograms1D(histograms1DIn),
  histograms2D(histograms2DIn),
  histograms3D(histograms3DIn),
  histograms4D(histograms4DIn)
{;}

BDSOutputROOTEventHistograms::~BDSOutputROOTEventHistograms()
{;}

void BDSOutputROOTEventHistograms::FillSimple(const BDSOutputROOTEventHistograms* rhs)
{
  if (!rhs)
    {return;}

  histograms1D = rhs->histograms1D;
  histograms2D = rhs->histograms2D;
  histograms3D = rhs->histograms3D;
  histograms4D = rhs->histograms4D;
}

void BDSOutputROOTEventHistograms::Fill(const BDSOutputROOTEventHistograms* rhs)
{
  if (!rhs)
    {return;}

  // for each histogram, clone (ie copy) it into this object
  for (auto h : rhs->histograms1D)
    {histograms1D.push_back(static_cast<TH1D*>(h->Clone()));}
  for (auto h : rhs->histograms2D)
    {histograms2D.push_back(static_cast<TH2D*>(h->Clone()));}
  for (auto h : rhs->histograms3D)
    {histograms3D.push_back(static_cast<TH3D*>(h->Clone()));}
#ifdef USE_BOOST
  for (auto h : rhs->histograms4D)
    {histograms4D.push_back(static_cast<BDSBH4DBase*>(h->Clone("")));}
#endif

}

int BDSOutputROOTEventHistograms::Create1DHistogramSTD(const std::string& name, const std::string& title,
                                                       int nbins, double xmin, double xmax)
{
  histograms1D.push_back(new TH1D(name.c_str(),title.c_str(), nbins, xmin, xmax));
  return (int)histograms1D.size() - 1;
}

#ifndef __ROOTBUILD__

G4int BDSOutputROOTEventHistograms::Create1DHistogram(const G4String& name, const G4String& title,
                                                      G4int nbins, G4double xmin, G4double xmax)
{
  histograms1D.push_back(new TH1D(name,title, nbins, xmin, xmax));
  return (G4int)histograms1D.size() - 1;
}

G4int BDSOutputROOTEventHistograms::Create1DHistogram(const G4String& name,
                                                      const G4String& title,
                                                      const std::vector<double>& edges)
{
  histograms1D.push_back(new TH1D(name, title, (Int_t)edges.size()-1, edges.data()));
  return (G4int)histograms1D.size() - 1;
}

G4int BDSOutputROOTEventHistograms::Create2DHistogram(const G4String& name, const G4String& title,
                                                      G4int nxbins, G4double xmin, G4double xmax,
                                                      G4int nybins, G4double ymin, G4double ymax)
{
  histograms2D.push_back(new TH2D(name,title, nxbins, xmin, xmax, nybins, ymin, ymax));
  return (G4int)histograms2D.size() - 1;
}

G4int BDSOutputROOTEventHistograms::Create2DHistogram(const G4String& name, const G4String& title,
                                                      const std::vector<double>& xedges,
                                                      const std::vector<double>& yedges)
{
  histograms2D.push_back(new TH2D(name.data(),title.data(),
                                  (Int_t)xedges.size()-1, xedges.data(),
                                  (Int_t)yedges.size()-1, yedges.data()));
  return (G4int)histograms2D.size() - 1;
}

G4int BDSOutputROOTEventHistograms::Create3DHistogram(const G4String& name, const G4String& title,
                                                      G4int nxbins, G4double xmin, G4double xmax,
                                                      G4int nybins, G4double ymin, G4double ymax,
                                                      G4int nzbins, G4double zmin, G4double zmax)
{
  histograms3D.push_back(new TH3D(name, title,
                                  nxbins, xmin, xmax,
                                  nybins, ymin, ymax,
                                  nzbins, zmin, zmax));
  return (G4int)histograms3D.size() - 1;
}

G4int BDSOutputROOTEventHistograms::Create3DHistogram(const G4String& name, const G4String& title,
                                                      const std::vector<double>& xedges,
                                                      const std::vector<double>& yedges,
                                                      const std::vector<double>& zedges)
{
  histograms3D.push_back(new TH3D(name.data(),title.data(),
                                  (Int_t)xedges.size()-1, xedges.data(),
                                  (Int_t)yedges.size()-1, yedges.data(),
                                  (Int_t)zedges.size()-1, zedges.data()));
  return (G4int)histograms3D.size() - 1;
}

#ifdef USE_BOOST
G4int BDSOutputROOTEventHistograms::Create4DHistogram(const G4String& name,
                                                      const G4String& title,
                                                      const G4String& eScale,
                                                      const std::vector<double>& eBinsEdges,
                                                      unsigned int nxbins, G4double xmin, G4double xmax,
                                                      unsigned int nybins, G4double ymin, G4double ymax,
                                                      unsigned int nzbins, G4double zmin, G4double zmax,
                                                      unsigned int nebins, G4double emin, G4double emax)
{
  std::string nameC   = (std::string)name;
  std::string titleC  = (std::string)title;
  std::string eScaleC = (std::string)eScale;
  
  if(eScale == "linear")
    {
      histograms4D.push_back(new BDSBH4D<boost_histogram_linear>(nameC, titleC, eScaleC,
                                                                 nxbins, xmin, xmax,
                                                                 nybins, ymin, ymax,
                                                                 nzbins, zmin, zmax,
                                                                 nebins, emin, emax));
    }
  else if(eScale == "log")
    {
      histograms4D.push_back(new BDSBH4D<boost_histogram_log>(nameC, titleC, eScaleC,
                                                              nxbins, xmin, xmax,
                                                              nybins, ymin, ymax,
                                                              nzbins, zmin, zmax,
                                                              nebins, emin, emax));
    }
  else if(eScale == "user")
    {
      histograms4D.push_back(new BDSBH4D<boost_histogram_variable>(nameC, titleC, eScaleC, eBinsEdges,
                                                                   nxbins, xmin, xmax,
                                                                   nybins, ymin, ymax,
                                                                   nzbins, zmin, zmax));
    }

  return (G4int)histograms4D.size() - 1;
}
#else
G4int BDSOutputROOTEventHistograms::Create4DHistogram(const G4String&, const G4String&, const G4String&,
                                                      const std::vector<double>&,
                                                      unsigned int, G4double, G4double,
                                                      unsigned int, G4double, G4double,
                                                      unsigned int, G4double, G4double,
                                                      unsigned int, G4double, G4double)
{
  throw BDSException(__METHOD_NAME__, "BDSIM compiled without BOOST support -> no 4D histograms.");
}
#endif

void BDSOutputROOTEventHistograms::Fill1DHistogram(G4int    histoId,
                                                   G4double value,
                                                   G4double weight)
{
  histograms1D[histoId]->Fill(value,weight);
}

void BDSOutputROOTEventHistograms::Fill2DHistogram(G4int    histoId,
                                                   G4double xValue,
                                                   G4double yValue,
                                                   G4double weight)
{
  histograms2D[histoId]->Fill(xValue,yValue,weight);
}

void BDSOutputROOTEventHistograms::Fill3DHistogram(G4int    histoId,
                                                   G4double xValue,
                                                   G4double yValue,
                                                   G4double zValue,
                                                   G4double weight)
{
  histograms3D[histoId]->Fill(xValue,yValue,zValue,weight);
}

#ifdef USE_BOOST
void BDSOutputROOTEventHistograms::Fill4DHistogram(G4int    histoId,
                                                   G4double xValue,
                                                   G4double yValue,
                                                   G4double zValue,
                                                   G4double eValue)
{
  histograms4D[histoId]->Fill_BDSBH4D(xValue, yValue, zValue, eValue);
}
#else
void BDSOutputROOTEventHistograms::Fill4DHistogram(G4int,
                                                   G4double,
                                                   G4double,
                                                   G4double,
                                                   G4double)
{
  throw BDSException(__METHOD_NAME__, "BDSIM compiled without BOOST support -> no 4D histograms.");
}
#endif

void BDSOutputROOTEventHistograms::Set3DHistogramBinContent(G4int histoId,
                                                            G4int globalBinID,
                                                            G4double value)
{
  histograms3D[histoId]->SetBinContent(globalBinID, value);
}

#ifdef USE_BOOST
void BDSOutputROOTEventHistograms::Set4DHistogramBinContent(G4int histoId,
                                                            G4int x,
                                                            G4int y,
                                                            G4int z,
                                                            G4int e,
                                                            G4double value)
{
  histograms4D[histoId]->Set_BDSBH4D(x, y, z, e, value);
}
#else
void BDSOutputROOTEventHistograms::Set4DHistogramBinContent(G4int, G4int, G4int, G4int, G4int, G4double)
{
  throw BDSException(__METHOD_NAME__, "BDSIM compiled without BOOST support -> no 4D histograms.");
}
#endif

void BDSOutputROOTEventHistograms::AccumulateHistogram3D(G4int histoId,
                                                         TH3D* otherHistogram)
{
  histograms3D[histoId]->Add(otherHistogram);
}

void BDSOutputROOTEventHistograms::AccumulateHistogram4D(G4int histoId,
                                                         BDSBH4DBase* otherHistogram)
{
  *histograms4D[histoId] += *otherHistogram;
}

#endif

void BDSOutputROOTEventHistograms::Flush()
{
  for (auto h : histograms1D)
    {h->Reset();}
  for (auto h : histograms2D)
    {h->Reset();}
  for (auto h : histograms3D)
    {h->Reset();}
#ifdef USE_BOOST
  for (auto h : histograms4D)
    {h->Reset_BDSBH4D();}
#endif
}
