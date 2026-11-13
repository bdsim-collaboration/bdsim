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
#ifndef BDSRUNMANAGER_H
#define BDSRUNMANAGER_H
#include "G4RunManager.hh"
#include "G4Types.hh"

class BDSExceptionHandler;

/**
 * @brief Wrapper from G4RunManager that provides more output.
 *
 */

class BDSRunManager: public G4RunManager
{
public:
  BDSRunManager();

  virtual ~BDSRunManager();

  /// Run G4RunManager::Initialise() and update BDSPrimaryGeneratorAction with knowledge
  /// of the world extent for coordinate checking.
  virtual void Initialize();

  /// For additional output.
  virtual void DoEventLoop(G4int n_event,const char* macroFile=nullptr,G4int n_select=-1);
  
  /// Altered BeamOn function to account for Placet synchronisation.
  virtual void BeamOn(G4int n_event,const char* macroFile=nullptr,G4int n_select=-1);
  
  /// For additional output.
  virtual void ProcessOneEvent(G4int i_event);

  /// Run G4RunManager:AbortRun(), but give some print out feedback for the user.
  virtual void AbortRun(G4bool);

protected:
  BDSExceptionHandler* exceptionHandler;
};
#endif

