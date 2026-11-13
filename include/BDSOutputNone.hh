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
#ifndef BDSOutputNone_h
#define BDSOutputNone_h 

#include "BDSOutput.hh"

/**
 * @brief No output class
 * 
 * Write no output
 *
 * @author Jochem Snuverink
 */

class BDSOutputNone: public BDSOutput
{
public: 
  BDSOutputNone(): BDSOutput("", "", -1){;} ///< Default constructor.
  virtual ~BDSOutputNone(){;}

  /// @{ No action.
  virtual void NewFile(){;}
  virtual void CloseFile(){;}
private:
  virtual void WriteHeader(){;}
  virtual void WriteHeaderEndOfFile(){;}
  virtual void WriteParticleData(){;}
  virtual void WriteBeam(){;}
  virtual void WriteOptions(){;}
  virtual void WriteModel(){;}
  virtual void WriteFileEventLevel(){;}
  virtual void WriteFileRunLevel(){;}
  /// @}
};

#endif
