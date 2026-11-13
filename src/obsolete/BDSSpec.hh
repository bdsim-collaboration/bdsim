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
#ifndef __BDSSPEC_HH
#define __BDSSPEC_HH

#include "BDSGlobalConstants.hh"
#include <list>

//Defines a specification class which contains a G4String that can only contain certain values (to be added to allowed specs
//in the initialise method). The class can be compared to other BDSSpec objects of the same subclass or to a G4String.
//Used to specify, for example, geometry format, or field map format.

class BDSSpec{
public:
  BDSSpec();
  ~BDSSpec();
  G4bool compare(BDSSpec* spec) const; //Compare to another spec object 
  G4bool compare(G4String val) const; //Compare to string
  G4bool compare(const char* val) const; //Compare to string
  //  G4bool compare(G4String spec) const; 
  void spec(G4String val);
  G4String spec();
  void printAllowed() const;
  
protected:
  BDSSpec(G4String spec, std::list<G4String> allowedSpecs);
  void addAllowedSpec(G4String val);

private:
  virtual void allowedSpecs() = 0;
  G4String _spec;
  std::list<G4String> _allowedSpecs;
  G4bool checkAllowed(G4String val) const;
};
#endif


