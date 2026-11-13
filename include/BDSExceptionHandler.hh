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
#ifndef BDSEXCEPTIONHANDLER_H
#define BDSEXCEPTIONHANDLER_H

#include "G4ExceptionHandler.hh"

/**
 * @brief Handler that overrides Geant4's behaviour back to a normal exception.
 * 
 * If left alone, Geant4 will use abort() which won't exit safely. Therefore,
 * we use the exception handler they provide and change the behaviour to throwing
 * one of our exceptions which we generally require to be caught.
 *
 * Overall BDSIM can be expected to throw a BDSException or a std::exception.
 *
 * @author Laurie Nevay
 */

class BDSExceptionHandler: public G4ExceptionHandler
{
public:
  /// When called, the base class will register this object with G4StateManager.
  BDSExceptionHandler();
  virtual ~BDSExceptionHandler();

  /// @{ As per G4 - delete these.
  BDSExceptionHandler(const BDSExceptionHandler&) = delete;
  BDSExceptionHandler& operator=(const BDSExceptionHandler&) = delete;
  /// @}

  /// Virtual method to override behaviour.
  virtual G4bool Notify(const char* originOfException,
			const char* exceptionCode, G4ExceptionSeverity severity,
			const char* description);
};

#endif
