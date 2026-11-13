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
#ifndef BDSCOLOURSCALEMAGMA_H
#define BDSCOLOURSCALEMAGMA_H

#include "BDSColourScale.hh"

#include "G4Types.hh"

#include <array>
#include <vector>

/**
 * @brief Colour scale based on magma colour map.
 * 
 * This class linearly interpolates between values.
 * 
 * The colour map is "magma" created by Stefan van der Walt 
 * ([@stefanv](https://github.com/stefanv)) and Nathaniel Smith 
 * ([@njsmith](https://github.com/njsmith)). Full credit goes to them for
 * their excellent colour scale.
 * Details can be found here: https://cran.r-project.org/web/packages/viridis/vignettes/intro-to-viridis.html
 * The values hard-coded in this class come from a json file on
 * [@stefanv](https://github.com/stefanv), in the package "scale-color-perceptual",
 * specifically https://github.com/stefanv/scale-color-perceptual/blob/master/rgb/magma.json
 * 
 * This file has 256 values and has been cut by a fraction of 1/8 to 32 values as
 * we will linearly interpolate anyway and want to save memory.
 *
 * @author Laurie Nevay
 */

class BDSColourScaleMagma: public BDSColourScale
{
public:
  BDSColourScaleMagma();
  virtual ~BDSColourScaleMagma();

  virtual G4Colour GetValue(G4double numberFromZeroToOne) const;

private:

  static std::vector<std::array<G4double,3>> data;
  
  G4double dataStep;
};

#endif
