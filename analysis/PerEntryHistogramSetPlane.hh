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
#ifndef PERENTRYHISTOGRAMSETPLANE_H
#define PERENTRYHISTOGRAMSETPLANE_H
#include "PerEntryHistogramSet.hh"

#include "BDSOutputROOTEventSampler.hh"

#include <set>

class Event;
class HistogramDef;
class HistogramDefSet;
class TChain;


/**
 * @brief Histogram over a set of integers not number line.
 *
 * Specialised for a plane sampler.
 *
 * @author L. Nevay
 */

class PerEntryHistogramSetPlane: public PerEntryHistogramSet
{
public:
  PerEntryHistogramSetPlane(const HistogramDefSet* definitionIn,
                            Event*                 eventIn,
                            TChain*                chainIn);  
  virtual ~PerEntryHistogramSetPlane();
  
  /// Ensure sampler is setup even if it wasn't on at the beginning when
  /// we inspected the model tree. We need this to build up unique PDG IDs
  /// by manually looping over the data. A derived class must implement this
  /// for each type of sampler data we have.
  virtual void CheckSampler();

protected:
  /// Derived class should get the partID member and form a set from the specific
  /// type of sampler it is.
  virtual void GetPDGIDSetFromSampler(std::set<long long int>& setIn) const;

#ifdef __ROOTDOUBLE__
  BDSOutputROOTEventSampler<double>* sampler;
#else
  BDSOutputROOTEventSampler<float>*  sampler;
#endif
};

#endif
