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
#ifndef BDSCOMPONENTCONSTRUCTOR_H
#define BDSCOMPONENTCONSTRUCTOR_H

class BDSAcceleratorComponent;
class BDSBeamlineIntegral;
namespace GMAD
{
  struct Element;
}

/**
 * @brief Interface class the developer should derive to construct their element.
 * 
 * One BDSComponentConstructor per one class that inherits BDSAcceleratorComponent.
 *
 * When creating a custom beam line component that inherits BDSAcceleratorComponent,
 * the developer should also create a class that inherits this one. This will implement
 * the one pure virtual method Construct() that will specify how to construct their
 * component from the parser element class (GMAD::Element) provided by the parser.
 * 
 * @author Laurie Nevay
 */

class BDSComponentConstructor
{
public:
  BDSComponentConstructor(){;}
  virtual ~BDSComponentConstructor(){;}

  /// Construct an instance of the user implement component. You get access to the previous
  /// current and next elements from the parser in case of angled faces etc. The current cumulative
  /// arc length allows calculation of the phase with respect to the beginning of the beam line.
  /// The developer need not use all of the supplied parameters in their implementation.
  virtual BDSAcceleratorComponent* Construct(GMAD::Element const* elementIn,
					     GMAD::Element const* prevElementIn,
					     GMAD::Element const* nextElementIn,
					     const BDSBeamlineIntegral& integral) = 0;
};

#endif
