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
#include "BDSBH4D.hh"
#include "BDSHistBinMapper.hh"

#include "TFile.h"

#ifdef USE_BOOST
#include "BDSBH4DTypeDefs.hh"
#include <boost/format.hpp>
#include <boost/histogram.hpp>
#endif

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#ifdef USE_BOOST
templateClassImp(BDSBH4D)

template <>
BDSBH4D<boost_histogram_linear>::BDSBH4D():
  BDSBH4DBase(3,3,3,3, 0,1, 0,1, 0,1, 0.1,0.230, "BDSBH4D","BDSBH4D","linear")
#ifndef __ROOTBUILD__
  ,mapper(3,3,3,3)
#endif
{
  h = boost::histogram::make_histogram_with(std::vector<double>(),
					    boost::histogram::axis::regular<double> {3, 0.0, 1.0, "x"},
					    boost::histogram::axis::regular<double> {3, 0.0, 1.0, "y"},
					    boost::histogram::axis::regular<double> {3, 0.0, 1.0, "z"},
					    boost::histogram::axis::regular<double> {3, 1.0, 230.0, "energy"});
  
  h_err = boost::histogram::make_histogram_with(std::vector<double>(),
						boost::histogram::axis::regular<double> {3, 0.0, 1.0, "x"},
						boost::histogram::axis::regular<double> {3, 0.0, 1.0, "y"},
						boost::histogram::axis::regular<double> {3, 0.0, 1.0, "z"},
						boost::histogram::axis::regular<double> {3, 1.0, 230.0, "energy"});
}

template <>
BDSBH4D<boost_histogram_log>::BDSBH4D():
  BDSBH4DBase(3,3,3,3, 0,1, 0,1, 0,1, 0.1,0.230, "BDSBH4D","BDSBH4D","log")
#ifndef __ROOTBUILD__
  ,mapper(3,3,3,3)
#endif
{
  h = boost::histogram::make_histogram_with(std::vector<double>(),
					    boost::histogram::axis::regular<double> {3, 0.0, 1.0, "x"},
					    boost::histogram::axis::regular<double> {3, 0.0, 1.0, "y"},
					    boost::histogram::axis::regular<double> {3, 0.0, 1.0, "z"},
					    boost::histogram::axis::regular<double, boost::histogram::axis::transform::log> {3, 1.0, 230.0, "energy"});
  
  h_err = boost::histogram::make_histogram_with(std::vector<double>(),
						boost::histogram::axis::regular<double> {3, 0.0, 1.0, "x"},
						boost::histogram::axis::regular<double> {3, 0.0, 1.0, "y"},
						boost::histogram::axis::regular<double> {3, 0.0, 1.0, "z"},
						boost::histogram::axis::regular<double, boost::histogram::axis::transform::log> {3, 1.0, 230.0, "energy"});
}

template <>
BDSBH4D<boost_histogram_variable>::BDSBH4D():
  BDSBH4DBase(3,3,3,0,1, 0,1, 0,1, "BDSBH4D","BDSBH4D","user",std::vector<double>{0.001,0.1,0.230})
#ifndef __ROOTBUILD__
  ,mapper(3,3,3,0)
#endif
{
  h = boost::histogram::make_histogram_with(std::vector<double>(),
					    boost::histogram::axis::regular<double> {3, 0.0, 1.0, "x"},
					    boost::histogram::axis::regular<double> {3, 0.0, 1.0, "y"},
					    boost::histogram::axis::regular<double> {3, 0.0, 1.0, "z"},
					    boost::histogram::axis::variable<double> {std::vector<double>{0.001,0.230}, "energy"});
  
  h_err = boost::histogram::make_histogram_with(std::vector<double>(),
						boost::histogram::axis::regular<double> {3, 0.0, 1.0, "x"},
						boost::histogram::axis::regular<double> {3, 0.0, 1.0, "y"},
						boost::histogram::axis::regular<double> {3, 0.0, 1.0, "z"},
						boost::histogram::axis::variable<double> {std::vector<double>{0.001,0.1,0.230}, "energy"});
}

template <>
BDSBH4D<boost_histogram_linear>::BDSBH4D(std::string& name, std::string& title, const std::string& eScale,
					 unsigned int nxbins, double xmin, double xmax,
					 unsigned int nybins, double ymin, double ymax,
					 unsigned int nzbins, double zmin, double zmax,
					 unsigned int nebins, double emin, double emax):
  BDSBH4DBase(nxbins, nybins, nzbins, nebins,
	      xmin, xmax, ymin, ymax, zmin, zmax, emin, emax,
	      name, title, eScale)
#ifndef __ROOTBUILD__
  ,mapper(nxbins, nybins, nzbins, nebins)
#endif
{
  h = boost::histogram::make_histogram_with(std::vector<double>(),
					    boost::histogram::axis::regular<double> {nxbins, xmin, xmax, "x"},
					    boost::histogram::axis::regular<double> {nybins, ymin, ymax, "y"},
					    boost::histogram::axis::regular<double> {nzbins, zmin, zmax, "z"},
					    boost::histogram::axis::regular<double> {nebins, emin, emax, "energy"});
  
  h_err = boost::histogram::make_histogram_with(std::vector<double>(),
						boost::histogram::axis::regular<double> {nxbins, xmin, xmax, "x"},
						boost::histogram::axis::regular<double> {nybins, ymin, ymax, "y"},
						boost::histogram::axis::regular<double> {nzbins, zmin, zmax, "z"},
						boost::histogram::axis::regular<double> {nebins, emin, emax, "energy"});
}

template <>
BDSBH4D<boost_histogram_log>::BDSBH4D(std::string& name, std::string& title, const std::string& eScale,
				      unsigned int nxbins, double xmin, double xmax,
				      unsigned int nybins, double ymin, double ymax,
				      unsigned int nzbins, double zmin, double zmax,
				      unsigned int nebins, double emin, double emax):
  BDSBH4DBase(nxbins, nybins, nzbins, nebins,
	      xmin, xmax, ymin, ymax, zmin, zmax, emin, emax,
	      name, title, eScale)
#ifndef __ROOTBUILD__
  ,mapper(nxbins, nybins, nzbins, nebins)
#endif
{
  h = boost::histogram::make_histogram_with(std::vector<double>(),
					    boost::histogram::axis::regular<double> {nxbins, xmin, xmax, "x"},
					    boost::histogram::axis::regular<double> {nybins, ymin, ymax, "y"},
					    boost::histogram::axis::regular<double> {nzbins, zmin, zmax, "z"},
					    boost::histogram::axis::regular<double, boost::histogram::axis::transform::log> {nebins, emin, emax, "energy"});
  
  h_err = boost::histogram::make_histogram_with(std::vector<double>(),
						boost::histogram::axis::regular<double> {nxbins, xmin, xmax, "x"},
						boost::histogram::axis::regular<double> {nybins, ymin, ymax, "y"},
						boost::histogram::axis::regular<double> {nzbins, zmin, zmax, "z"},
						boost::histogram::axis::regular<double, boost::histogram::axis::transform::log> {nebins, emin, emax, "energy"});
}

template <>
BDSBH4D<boost_histogram_variable>::BDSBH4D(std::string& name, std::string& title, const std::string& eScale,
					   const std::vector<double>& eBinEdgesIn,
					   unsigned int nxbins, double xmin, double xmax,
					   unsigned int nybins, double ymin, double ymax,
					   unsigned int nzbins, double zmin, double zmax):
  BDSBH4DBase(nxbins, nybins, nzbins, xmin, xmax, ymin, ymax, zmin, zmax, name, title, eScale, eBinEdgesIn)
#ifndef __ROOTBUILD__
  ,mapper(nxbins, nybins, nzbins, eBinEdgesIn.size())
#endif
{
  h = boost::histogram::make_histogram_with(std::vector<double>(),
					    boost::histogram::axis::regular<double> {nxbins, xmin, xmax, "x"},
					    boost::histogram::axis::regular<double> {nybins, ymin, ymax, "y"},
					    boost::histogram::axis::regular<double> {nzbins, zmin, zmax, "z"},
					    boost::histogram::axis::variable<double> {eBinEdgesIn, "energy"});
  
  h_err = boost::histogram::make_histogram_with(std::vector<double>(),
						boost::histogram::axis::regular<double> {nxbins, xmin, xmax, "x"},
						boost::histogram::axis::regular<double> {nybins, ymin, ymax, "y"},
						boost::histogram::axis::regular<double> {nzbins, zmin, zmax, "z"},
						boost::histogram::axis::variable<double> {eBinEdgesIn, "energy"});
}

template<class T>
BDSBH4D<T>::~BDSBH4D()
{;}

template <class T>
void BDSBH4D<T>::Reset_BDSBH4D()
{
  h.reset();
  h_entries = 0;
}

template <class T>
BDSBH4D<T>* BDSBH4D<T>::Clone(const char* newname) const
{
  auto clone = new BDSBH4D<T>(*this);
  clone->SetName(newname);
  return clone;
}

template <class T>
int BDSBH4D<T>::Fill_BDSBH4D(double xValue,
			      double yValue,
			      double zValue,
			      double eValue)
{
  h(xValue, yValue, zValue, eValue);
#ifndef __ROOTBUILD__
  int i = h.axis(0).index(xValue);
  int j = h.axis(1).index(yValue);
  int k = h.axis(2).index(zValue);
  int l = h.axis(3).index(eValue);
  return mapper.GlobalFromIJKLIndex(i, j, k, l);
#else
  return 0;
#endif
}

template <class T>
void BDSBH4D<T>::Set_BDSBH4D(int x,
			     int y,
			     int z,
			     int e,
			     double value)
{
  h.at(x, y, z, e) = value;
}


template <class T>
#ifndef __ROOTBUILD__
void BDSBH4D<T>::Set_BDSBH4D(int global,
                             double value)
{
  int x,y,z,e;
  mapper.IJKLFromGlobal(global, x, y, z, e);
  h.at(x, y, z, e) = value;
#else
void BDSBH4D<T>::Set_BDSBH4D(int, double){
#endif
}

template <class T>
void BDSBH4D<T>::SetError_BDSBH4D(int x,
				  int y,
				  int z,
				  int e,
				  double value)
{
  h_err.at(x, y, z, e) = value;
}

template <class T>
#ifndef __ROOTBUILD__
void BDSBH4D<T>::SetError_BDSBH4D(int global,
                                  double value)
{
  int x,y,z,e;
  mapper.IJKLFromGlobal(global, x, y, z, e);
  h_err.at(x, y, z, e) = value;
#else
  void BDSBH4D<T>::SetError_BDSBH4D(int, double){
#endif
}

template <class T>
void BDSBH4D<T>::Add_BDSBH4D(BDSBH4DBase* otherHistogram)
{
  auto tmp = dynamic_cast<BDSBH4D<T>*>(otherHistogram);
  h += tmp->h;
}

template <class T>
double BDSBH4D<T>::At(int x, int y, int z, int e)
{
  return h.at(x, y, z, e);
}

template <class T>
#ifndef __ROOTBUILD__
double BDSBH4D<T>::At(int global)
{
  int x,y,z,e;
  mapper.IJKLFromGlobal(global, x, y, z, e);
  return h.at(x, y, z, e);
#else
double BDSBH4D<T>::At(int){return 0;
#endif
}

template <class T>
double BDSBH4D<T>::LowBinEdgeAt(int x, int y, int z, int e)
{
  std::ostringstream os4;
  for (auto&& i : indexed(h))
    {
      if (i.index(0) == x and i.index(1) == y and i.index(2) == z and i.index(3) == e)
	{return i.bin(3).lower();}
    }
  return 0;
}

template <class T>
double BDSBH4D<T>::HighBinEdgeAt(int x, int y, int z, int e)
{
  std::ostringstream os4;
  for (auto&& i : indexed(h))
    {
      if (i.index(0) == x and i.index(1) == y and i.index(2) == z and i.index(3) == e)
	{return i.bin(3).upper();}
    }
  return 0;
}

template <class T>
double BDSBH4D<T>::AtError(int x, int y, int z, int e)
{
  return h_err.at(x, y, z, e);
}

template <class T>
void BDSBH4D<T>::Print_BDSBH4D(bool with_zero_values)
{
  std::ostringstream os4;
  for (auto&& x : indexed(this->h))
    {
      if (!with_zero_values and *x != 0)
	{
	  os4 << boost::format("(%i, %i, %i, %i) [%.3f, %.3f) [%.3f, %.3f) [%.3f, %.3f) [%.3f, %.3f) %i\n")
	    % x.index(0) % x.index(1) % x.index(2) % x.index(3)
	    % x.bin(0).lower() % x.bin(0).upper()
	    % x.bin(1).lower() % x.bin(1).upper()
	    % x.bin(2).lower() % x.bin(2).upper()
	    % x.bin(3).lower() % x.bin(3).upper()
	    % *x;
        }
      else if (with_zero_values)
	{
	  os4 << boost::format("(%i, %i, %i, %i) [%.3f, %.3f) [%.3f, %.3f) [%.3f, %.3f) [%.3f, %.3f) %i\n")
	    % x.index(0) % x.index(1) % x.index(2) % x.index(3)
	    % x.bin(0).lower() % x.bin(0).upper()
	    % x.bin(1).lower() % x.bin(1).upper()
	    % x.bin(2).lower() % x.bin(2).upper()
	    % x.bin(3).lower() % x.bin(3).upper()
	    % *x;	  
        }
    }
    std::cout << os4.str() << std::flush;
}

template <class T>
void BDSBH4D<T>::Print_BDSBH4D(int x, int y, int z, int e)
{
  std::ostringstream os4;
  for (auto&& i : indexed(this->h))
    {
      if (i.index(0) == x and  i.index(1) == y and i.index(2) ==z and i.index(3) ==e)
	{
	  os4 << boost::format("(%i, %i, %i, %i) [%.3f, %.3f) [%.3f, %.3f) [%.3f, %.3f) [%.3f, %.3f) %i\n")
	    % i.index(0) % i.index(1) % i.index(2) % i.index(3)
	    % i.bin(0).lower() % i.bin(0).upper()
	    % i.bin(1).lower() % i.bin(1).upper()
	    % i.bin(2).lower() % i.bin(2).upper()
	    % i.bin(3).lower() % i.bin(3).upper()
	    % *i;
        }
    }
  std::cout << os4.str() << std::flush;
}

template class BDSBH4D<boost_histogram_linear>;
template class BDSBH4D<boost_histogram_log>;
template class BDSBH4D<boost_histogram_variable>;

#endif
