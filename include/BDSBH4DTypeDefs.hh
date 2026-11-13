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
#ifndef BDSBH4DTYPEDEFS_H
#define BDSBH4DTYPEDEFS_H

#ifdef USE_BOOST
#include <boost/histogram.hpp>
#include <boost/variant.hpp>

#ifndef __ROOTDOUBLE__
typedef double boost_histogram_storage_type;
#else
typedef double boost_histogram_storage_type;
#endif

typedef boost::histogram::axis::regular<double, boost::use_default, boost::use_default, boost::use_default> boost_histogram_linear_axis;
typedef boost::histogram::axis::regular<double, boost::histogram::axis::transform::log, boost::use_default, boost::use_default> boost_histogram_log_axis;
typedef boost::histogram::axis::variable<double, boost::use_default, boost::use_default, std::allocator<double> > boost_histogram_variable_axis;

typedef boost::variant<boost_histogram_linear_axis*, boost_histogram_log_axis*, boost_histogram_variable_axis*> boost_histogram_axes_variant;

typedef boost::histogram::histogram<std::tuple<boost_histogram_linear_axis, boost_histogram_linear_axis, boost_histogram_linear_axis, boost_histogram_linear_axis   >, boost::histogram::storage_adaptor<std::vector<boost_histogram_storage_type, std::allocator<boost_histogram_storage_type> > > > boost_histogram_linear;
typedef boost::histogram::histogram<std::tuple<boost_histogram_linear_axis, boost_histogram_linear_axis, boost_histogram_linear_axis, boost_histogram_log_axis      >, boost::histogram::storage_adaptor<std::vector<boost_histogram_storage_type, std::allocator<boost_histogram_storage_type> > > > boost_histogram_log;
typedef boost::histogram::histogram<std::tuple<boost_histogram_linear_axis, boost_histogram_linear_axis, boost_histogram_linear_axis, boost_histogram_variable_axis >, boost::histogram::storage_adaptor<std::vector<boost_histogram_storage_type, std::allocator<boost_histogram_storage_type> > > > boost_histogram_variable;

#endif

#endif
