/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2024.

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
#ifndef GMAD_H
#define GMAD_H

namespace GMAD
{
  /** Python interface **/
  extern "C" {
    int    GmadParser_c(char *name);

    /* Interface to extern beamline_list */
    int          GetNElements();            ///< Length of list
    int          GetType(int);              ///< Type of element
    const char*  GetName(int);              ///< Name of element
    double       GetLength(int);            ///< Length of element
    double       GetAngle(int);             ///< Angle of element
    double*      GetKs(int);                ///< All magnetic ks
    double       GetAper1(int);             ///< Aperture 1
    double       GetAper2(int);             ///< Aperture 2
    double       GetAper3(int);             ///< Aperture 3
    double       GetAper4(int);             ///< Aperture 4
    const char*  GetApertureType(int);      ///< Aperture Type
    double       GetBeampipeThickness(int); ///< Beam Pipe Thickness

  }
}
#endif
