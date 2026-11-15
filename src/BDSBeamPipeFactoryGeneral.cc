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
#include "BDSAperture.hh"
#include "BDSApertureFactory.hh"
#include "BDSBeamPipeFactoryGeneral.hh"
#include "BDSBeamPipe.hh"
#include "BDSBeamPipeInfo.hh"
#include "BDSExtent.hh"

#include "G4String.hh"
#include "G4Types.hh"
#include "G4VSolid.hh"

#include <algorithm>

BDSBeamPipeFactoryGeneral::BDSBeamPipeFactoryGeneral()
{;}

BDSBeamPipe* BDSBeamPipeFactoryGeneral::CreateBeamPipe(const G4String& name,
                                                       G4double fullLength,
                                                       const BDSBeamPipeInfo* bpi)
{
  CleanUp();
  
  BDSAperture* apVacIn  = bpi->aperture;
  // 3 scenarios bpi->apertureOut is unique; nullptr; == bpi->aperture
  G4bool variedAperture = (G4bool)bpi->apertureOut && bpi->aperture != bpi->apertureOut;
  BDSAperture* apVacOut = variedAperture ? bpi->apertureOut : bpi->aperture;
  G4double containerThickness = bpi->vacuumOnly ? lengthSafetyLarge : bpi->beamPipeThickness + 2*lengthSafetyLarge;
  BDSAperture* apBpInnerIn  = apVacIn->Plus(lengthSafetyLarge);
  BDSAperture* apBpInnerOut = variedAperture ? apVacOut->Plus(lengthSafetyLarge) : apBpInnerIn;
  BDSAperture* apContIn     = apVacIn->Plus(containerThickness);
  BDSAperture* apContOut    = variedAperture ? apVacOut->Plus(containerThickness) : apContIn;
  BDSAperture* apContSubIn  = apVacIn->Plus(containerThickness + lengthSafety);
  BDSAperture* apContSubOut = variedAperture ? apVacOut->Plus(containerThickness + lengthSafety) : apContSubIn;
  
  BDSApertureFactory fac;
  auto vacProduct = fac.CreateSolid(name+"_vac", fullLength-2*lengthSafety, apVacIn, apVacOut,
                                    bpi->inputFaceNormal, bpi->outputFaceNormal);
  vacuumSolid = vacProduct.product;
  allSolids.insert(vacProduct.otherSolids.begin(), vacProduct.otherSolids.end());
  // do not delete apVac as it belongs to the beampipe info
  
  if (!bpi->vacuumOnly)
    {
      auto bpProduct = fac.CreateHollowSolid(name+"_pipe", fullLength-2*lengthSafety, apBpInnerIn, apBpInnerOut,
                                                bpi->beamPipeThickness, bpi->inputFaceNormal, bpi->outputFaceNormal);
      beamPipeSolid = bpProduct.product;
      allSolids.insert(bpProduct.otherSolids.begin(), bpProduct.otherSolids.end());
    }
  if (apBpInnerOut != apBpInnerIn)
    {delete apBpInnerOut;}
  delete apBpInnerIn;
  
  auto contProduct = fac.CreateSolid(name + "_cont", fullLength, apContIn, apContOut,
				                             bpi->inputFaceNormal, bpi->outputFaceNormal);
  containerSolid = contProduct.product;
  allSolids.insert(contProduct.otherSolids.begin(), contProduct.otherSolids.end());
  
  auto contSubProduct = fac.CreateSolid(name+"_cont_sub", 1.5*fullLength, apContSubIn, apContSubOut, nullptr, nullptr);
  containerSubtractionSolid = contSubProduct.product;
  allSolids.insert(contSubProduct.otherSolids.begin(), contSubProduct.otherSolids.end());
  
  CommonConstruction(name, bpi->vacuumMaterial, bpi->beamPipeMaterial, fullLength);
  
  BDSExtent et = std::max(apContIn->Extent(), apContOut->Extent());
  BDSExtent ext = BDSExtent(et.XNeg(), et.XPos(), et.YNeg(), et.YPos(), -0.5*fullLength, 0.5*fullLength);
  G4bool simpleCircular = !variedAperture && apVacIn->Circular();
  G4double cr = std::max(apContIn->RadiusToEncompass(), apContOut->RadiusToEncompass());

  delete apContIn;
  if (variedAperture)
    {delete apContOut;}
  delete apContSubIn;
  if (variedAperture)
    {delete apContSubOut;}

  return BuildBeamPipeAndRegisterVolumes(ext, cr, simpleCircular);
}
