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
#ifndef BDSOUTPUTLOADERSAMPLER_H
#define BDSOUTPUTLOADERSAMPLER_H

#include "BDSOutputLoader.hh"
#include "BDSOutputROOTEventSampler.hh"

#include "G4String.hh"

/**
 * @brief Loader of ROOT Event output for receating events.
 *
 * @author Laurie Nevay
 */

class BDSOutputLoaderSampler: public BDSOutputLoader
{
public:
  BDSOutputLoaderSampler(const G4String& filePath,
			 const G4String& samplerName);
  virtual ~BDSOutputLoaderSampler();

  const BDSOutputROOTEventSampler<float>* SamplerDataFloat(G4long eventNumber = 0);
  const BDSOutputROOTEventSampler<double>* SamplerDataDouble(G4long eventNumber = 0);
  
  inline G4bool DoublePrecision() const {return doublePrecision;}
  inline G4long NEventsInFile() const {return nEvents;}
  
  BDSOutputLoaderSampler() = delete;
  BDSOutputLoaderSampler(const BDSOutputLoaderSampler&) = delete;
  BDSOutputLoaderSampler& operator=(const BDSOutputLoaderSampler&) = delete;
  
private:
  void Common(G4long eventNumber);
  
  G4bool doublePrecision;
  G4long nEvents;
  BDSOutputROOTEventSampler<float>*  localSamplerFloat;
  BDSOutputROOTEventSampler<double>* localSamplerDouble;
};

#endif
