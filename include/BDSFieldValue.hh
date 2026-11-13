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
/// Whatever this typedef defines, it must be called BDSFieldValue!

#include "BDSThreeVector.hh"

#include "G4Types.hh"

#ifdef FIELDDOUBLE
#define FIELDTYPET G4double
typedef BDSThreeVector<G4double> BDSFieldValue;
#else
#define FIELDTYPET G4float
typedef BDSThreeVector<G4float>  BDSFieldValue;
#endif
