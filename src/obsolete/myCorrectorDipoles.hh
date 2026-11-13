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
 * granted without fee, provided that the above copyright notice
 * appear in all copies and that both that copyright notice and
 * this permission notice appear in supporting documentation.
 * No representations about the suitability of this software for
 * any purpose is made. It is provided "as is" without express
 * or implied warranty.
 */

#ifndef myCorrectorDipoles_h
#define myCorrectorDipoles_h 1
#include<vector>

// gab:
//#include "merlin_config.h"

// RectMultipole
//#include "AcceleratorModel/StdComponent/RectMultipole.h"
#include "myRectMultipole.hh"
#include "globals.hh"

//class Tracker;

//	A horizontal corrector dipole.
class XCor : public myRectMultipole  
{
  public:
      //	Constructor taking the identifier for the corrector, the
      //	length and the field (in Tesla).
      XCor (const G4String& id, G4double len, G4double B = 0);

      //	Virtual constructor.
  //      virtual ModelElement* Copy () const;

      //	Returns the unique index for this class of accelerator
      //	components.
  //      virtual IndexType GetIndex () const;

      //	Returns the type G4String for this component.
      virtual const G4String& GetType () const;

      //	Primary tracking interface. Prepares the specified
      //	Tracker object for tracking this component.
  //      virtual void PrepareTracker (Tracker& aTracker);

    // Data Members for Class Attributes

  //      static const IndexType ID;

  protected:
  private:
  private:
};

//	A vertical corrector dipole.

class YCor : public myRectMultipole  
{
  public:
      //	Constructor taking the identifier for the corrector, the
      //	length and the field (in Tesla).
      YCor (const G4String& id, G4double len, G4double B = 0);


      //	Virtual constructor.
  //      virtual ModelElement* Copy () const;

      //	Returns the unique index for this class of accelerator
      //	components.

  //      virtual IndexType GetIndex () const;

      //	Returns the type G4String for this component.
      virtual const G4String& GetType () const;

      //	Primary tracking interface. Prepares the specified
      //	Tracker object for tracking this component.
  //      virtual void PrepareTracker (Tracker& aTracker);

    // Data Members for Class Attributes

  //      static const IndexType ID;

  protected:
  private:
  private:
};

// Class XCor 

inline XCor::XCor (const G4String& id, G4double len, G4double B)
  : myRectMultipole(id,len,0,B){}


// Class YCor 

inline YCor::YCor (const G4String& id, G4double len, G4double B)
  : myRectMultipole(id,len,0,B,true)
{}

#endif
