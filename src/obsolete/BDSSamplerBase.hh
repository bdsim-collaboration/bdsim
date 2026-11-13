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
#ifndef BDSSAMPLERBASE_H
#define BDSSAMPLERBASE_H 

#include "globals.hh"
#include "BDSAcceleratorComponent.hh"

class BDSSamplerSD;

/** 
 * Abstract base class for output storage elements (Samplers)
 */

class BDSSamplerBase : public BDSAcceleratorComponent
{
public:
  /// constructor with sampler name, length and type
  BDSSamplerBase(G4String name, G4double length, G4String type);
  virtual ~BDSSamplerBase();

  /// Returns current number of samplers
  static int GetNSamplers();

  /// Method to add sampler independent of beamline
  static void AddExternalSampler(G4String outputName);

  /// Names of samplers for output
  static std::vector <G4String> outputNames;

  /// Access for external classes to sensitive detector
  virtual BDSSamplerSD* GetSensitiveDetector() const=0;

  /// Get SamplerId
  G4int GetSamplerNumber()const {return nThisSampler;}

  /// Get OutputName. [nthisSampler-1] since count starts at 1.
  G4String GetOutputName()const {return outputNames[nThisSampler-1];}

protected:
  /// Build container volume
  virtual void BuildContainerLogicalVolume();
private:
  /// Set user limits
  void SetUserLimits();

  /// Set vis attributes and sensitive detector
  void SetVisAttributes();
  
  /// ID of sampler (count starts at 1)
  G4int nThisSampler;

  /// number of total Samplers
  static G4int nSamplers;
};

#endif
