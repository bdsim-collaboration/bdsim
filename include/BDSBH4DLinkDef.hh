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

// #pragma link C++ class BDSBH4D<TH1D>+;

#ifdef USE_BOOST
#ifndef __ROOTDOUBLE__
typedef double boost_histogram_storage_type;
#else
typedef double boost_histogram_storage_type;
#endif

#include "BDSBH4DTypeDefs.hh"

#pragma link C++ class boost::histogram::histogram<std::tuple<boost::histogram::axis::regular<double, boost::use_default, boost::use_default, boost::use_default>, boost::histogram::axis::regular<double, boost::use_default, boost::use_default, boost::use_default>, boost::histogram::axis::regular<double, boost::use_default, boost::use_default, boost::use_default>, boost::histogram::axis::regular<double, boost::use_default, boost::use_default, boost::use_default> >, boost::histogram::storage_adaptor<std::vector<boost_histogram_storage_type, std::allocator<boost_histogram_storage_type> > > >+;
#pragma link C++ class tuple<boost::histogram::axis::regular<double,boost::use_default,boost::use_default,boost::use_default>,boost::histogram::axis::regular<double,boost::use_default,boost::use_default,boost::use_default>,boost::histogram::axis::regular<double,boost::use_default,boost::use_default,boost::use_default>,boost::histogram::axis::regular<double,boost::use_default,boost::use_default,boost::use_default> >+;
#pragma link C++ class boost::histogram::detail::mutex_base<tuple<boost::histogram::axis::regular<double,boost::use_default,boost::use_default,boost::use_default>,boost::histogram::axis::regular<double,boost::use_default,boost::use_default,boost::use_default>,boost::histogram::axis::regular<double,boost::use_default,boost::use_default,boost::use_default>,boost::histogram::axis::regular<double,boost::use_default,boost::use_default,boost::use_default> >,boost::histogram::storage_adaptor<vector<boost_histogram_storage_type> >,boost::histogram::detail::null_mutex>+;
#pragma link C++ class boost::histogram::histogram<std::tuple<boost::histogram::axis::regular<double, boost::use_default, boost::use_default, boost::use_default>, boost::histogram::axis::regular<double, boost::use_default, boost::use_default, boost::use_default>, boost::histogram::axis::regular<double, boost::use_default, boost::use_default, boost::use_default>, boost::histogram::axis::regular<double, boost::histogram::axis::transform::log, boost::use_default, boost::use_default> >, boost::histogram::storage_adaptor<std::vector<boost_histogram_storage_type, std::allocator<boost_histogram_storage_type> > > >+;
#pragma link C++ class boost::empty_::empty_value<boost::histogram::detail::null_mutex,0,true>+;
#pragma link C++ class boost::histogram::detail::null_mutex+;
#pragma link C++ class tuple<boost::histogram::axis::regular<double,boost::use_default,boost::use_default,boost::use_default>,boost::histogram::axis::regular<double,boost::use_default,boost::use_default,boost::use_default>,boost::histogram::axis::regular<double,boost::use_default,boost::use_default,boost::use_default>,boost::histogram::axis::regular<double,boost::histogram::axis::transform::log,boost::use_default,boost::use_default> >+;
#pragma link C++ class boost::histogram::axis::regular<double,boost::histogram::axis::transform::log,boost::use_default,boost::use_default>+;
#pragma link C++ class boost::histogram::axis::iterator_mixin<boost::histogram::axis::regular<double,boost::use_default,boost::use_default,boost::use_default> >+;
#pragma link C++ class boost::histogram::axis::transform::id+;
#pragma link C++ class boost::histogram::axis::iterator_mixin<boost::histogram::axis::regular<double,boost::histogram::axis::transform::log,boost::use_default,boost::use_default> >+;
#pragma link C++ class boost::histogram::axis::transform::log+;
#pragma link C++ class boost::histogram::detail::mutex_base<tuple<boost::histogram::axis::regular<double,boost::use_default,boost::use_default,boost::use_default>,boost::histogram::axis::regular<double,boost::use_default,boost::use_default,boost::use_default>,boost::histogram::axis::regular<double,boost::use_default,boost::use_default,boost::use_default>,boost::histogram::axis::regular<double,boost::histogram::axis::transform::log,boost::use_default,boost::use_default> >,boost::histogram::storage_adaptor<vector<boost_histogram_storage_type> >,boost::histogram::detail::null_mutex>+;
#pragma link C++ class allocator<char>+;
#pragma link C++ class boost::histogram::detail::vector_impl<vector<double> >+;
#pragma link C++ class boost::histogram::storage_adaptor<vector<boost_histogram_storage_type> >+;
#pragma link C++ class boost::empty_::empty_value<string,0,false>+;
#pragma link C++ class boost::histogram::histogram<tuple<boost::histogram::axis::regular<double,boost::use_default,boost::use_default,boost::use_default>,boost::histogram::axis::regular<double,boost::use_default,boost::use_default,boost::use_default>,boost::histogram::axis::regular<double,boost::use_default,boost::use_default,boost::use_default>,boost::histogram::axis::variable<double,boost::use_default,boost::use_default,allocator<double> > >,boost::histogram::storage_adaptor<vector<double> > >+;
#pragma link C++ class tuple<boost::histogram::axis::regular<double,boost::use_default,boost::use_default,boost::use_default>,boost::histogram::axis::regular<double,boost::use_default,boost::use_default,boost::use_default>,boost::histogram::axis::regular<double,boost::use_default,boost::use_default,boost::use_default>,boost::histogram::axis::variable<double,boost::use_default,boost::use_default,allocator<double> > >+;
#pragma link C++ class boost::histogram::detail::mutex_base<tuple<boost::histogram::axis::regular<double,boost::use_default,boost::use_default,boost::use_default>,boost::histogram::axis::regular<double,boost::use_default,boost::use_default,boost::use_default>,boost::histogram::axis::regular<double,boost::use_default,boost::use_default,boost::use_default>,boost::histogram::axis::variable<double,boost::use_default,boost::use_default,allocator<double> > >,boost::histogram::storage_adaptor<vector<double> >,boost::histogram::detail::null_mutex>+;
#pragma link C++ class boost::histogram::axis::variable<double,boost::use_default,boost::use_default,allocator<double> >+;
#pragma link C++ class boost::histogram::axis::iterator_mixin<boost::histogram::axis::variable<double,boost::use_default,boost::use_default,allocator<double> > >+;
#pragma link C++ class boost::histogram::axis::metadata_base<string,false>+;
#pragma link C++ class BDSBH4D<boost_histogram_linear>+;
#pragma link C++ class BDSBH4D<boost_histogram_log>+;
#pragma link C++ class BDSBH4D<boost_histogram_variable>+;


#endif
