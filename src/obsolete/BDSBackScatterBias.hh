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
#ifndef BACKSCATTERBIAS_H
#define BACKSCATTERBIAS_H

#include "G4WrapperProcess.hh"

class BDSBackScatterBias: public G4WrapperProcess
{
public: 
  //  destructor 
  virtual ~BDSBackScatterBias();

public: // with description
  //  constructor requires the process name and type
  BDSBackScatterBias(const G4String& aName =  "X-",
                 G4ProcessType   aType = fNotDefined );

  //  copy constructor copys the name but does not copy the 
  //  physics table (0 pointer is assigned)
  BDSBackScatterBias(const BDSBackScatterBias &right);

  //  Set/Get methods for the enhancement factor
  void SetEnhanceFactor( G4double ) ;
  G4double GetEnhanceFactor ( ) const; 
  //

  ////////////////////////////
  // DoIt    /////////////////
  ///////////////////////////
  virtual G4VParticleChange* PostStepDoIt(
				  const G4Track& track,
				  const G4Step&  stepData
				  );
  
private:
  /// assignment constructor not implemented nor used
  BDSBackScatterBias& operator=(const BDSBackScatterBias&);

  G4double eFactor; // enhancement factor to the cross-setion

};

inline G4double BDSBackScatterBias::GetEnhanceFactor () const
  { return eFactor;}
  
inline  void  BDSBackScatterBias::SetEnhanceFactor (G4double dval)
  { eFactor = dval;}

#endif













