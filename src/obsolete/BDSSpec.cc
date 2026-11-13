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
#include "BDSSpec.hh"
#include "BDSDebug.hh"
#include <typeinfo>

BDSSpec::~BDSSpec()
{
}


BDSSpec::BDSSpec()
{
}

void BDSSpec::printAllowed() const{
  for(std::list<G4String>::const_iterator it = _allowedSpecs.begin();
      it != _allowedSpecs.end();
      it++){
  }
}

G4bool BDSSpec::checkAllowed(G4String val) const{
  for(std::list<G4String>::const_iterator it = _allowedSpecs.begin();
      it != _allowedSpecs.end();
      it++){
    if((*it) == val){
      return true;
    }
  }
  G4String exceptionString = (G4String)__METHOD_NAME__ + (G4String)" - invalid specification : " + val;
  G4Exception(exceptionString.c_str(), "-1", FatalException, "");
  return false;
}

void BDSSpec::addAllowedSpec(G4String val){
  _allowedSpecs.push_back(val);
}

void BDSSpec::spec(G4String val){
  if (checkAllowed(val)) _spec = val;
}

G4String BDSSpec::spec(){
  return _spec;
}

G4bool BDSSpec::compare(BDSSpec* val) const{
  return compare(val->spec());
}


G4bool BDSSpec::compare(G4String val) const{
  if(checkAllowed(val)){
    return (_spec == val);
  } else {
    return false;
  }
}

G4bool BDSSpec::compare(const char* val) const{
  return compare((G4String) val);
}


