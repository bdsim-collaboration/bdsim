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
#ifndef BDSXSBIAS_H
#define BDSXSBIAS_H

#include "G4WrapperProcess.hh"


class BDSXSBias: public G4WrapperProcess
{
public: 
  //  destructor 
  virtual ~BDSXSBias();

public: // with description
  //  constructor requires the process name and type
  BDSXSBias(const G4String& aName =  "X-",
                 G4ProcessType   aType = fNotDefined );

  //  copy constructor copies the name but does not copy the 
  //  physics table (0 pointer is assigned)
  BDSXSBias(const BDSXSBias &right);
  
  virtual G4VParticleChange* PostStepDoIt(
				  const G4Track& track,
				  const G4Step&  stepData
				  );
 
  inline G4double eFactor() const {return _eFactor;}
  inline void eFactor(G4double val){_eFactor = val;}
 
private:
  /// assignment constructor not implemented nor used
  BDSXSBias& operator=(const BDSXSBias&);
  G4double _eFactor;
  
};


#endif













