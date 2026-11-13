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
#ifndef BDSLINKRUNMANAGER_H
#define BDSLINKRUNMANAGER_H
#include "G4RunManager.hh"
#include "G4Types.hh"

class BDSExceptionHandler;

/**
 * @brief Wrapper from G4RunManager that provides our exception handler.
 *
 *  Our exception handler prevents Geant4 from calling abort() and instead
 *  throws a BDSException which is expected to be caught.
 */

class BDSLinkRunManager: public G4RunManager
{
public:
  BDSLinkRunManager();
  virtual ~BDSLinkRunManager();

protected:
  BDSExceptionHandler* exceptionHandler;
};
#endif

