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
#include "BDSBeamPipeFactoryBase.hh"
#include "BDSBeamPipeFactoryClicPCL.hh"
#include "BDSBeamPipe.hh"
#include "BDSDebug.hh"
#include "BDSExtent.hh"

#include "globals.hh"                 // geant4 globals / types
#include "G4EllipticalTube.hh"
#include "G4CutTubs.hh"
#include "G4IntersectionSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4ThreeVector.hh"
#include "G4VSolid.hh"

#include <cmath>                           // sin, cos, fabs

BDSBeamPipeFactoryClicPCL::BDSBeamPipeFactoryClicPCL():
  extentYLow(0),
  extentYHigh(0)
{;}

void BDSBeamPipeFactoryClicPCL::GenerateClicPCL(std::vector<G4TwoVector>& vec,
                                                G4double aper1,
                                                G4double aper2,
                                                G4double aper3,
                                                G4double aper4,
                                                G4int    pointsPerTwoPi,
                                                G4double margin)
{
  G4double a1 = aper1 + margin;
  G4double a2 = aper2 + margin;
  G4double a3 = aper3 + margin;
  G4double a4 = aper4;

  G4int nPoints = 0.5*pointsPerTwoPi;
  AppendAngleEllipse(vec, -CLHEP::halfpi, CLHEP::halfpi, a1, a2, nPoints, 0, a4);
  AppendPoint(vec, a1, a4);
  AppendAngleEllipse(vec, CLHEP::halfpi, CLHEP::halfpi + CLHEP::pi, a1, a3, nPoints);
  AppendPoint(vec, -a1, 0);
}

void BDSBeamPipeFactoryClicPCL::GeneratePoints(G4double aper1,
                                               G4double aper2,
                                               G4double aper3,
                                               G4double aper4,
                                               G4double beamPipeThickness,
                                               G4int    pointsPerTwoPi)
{
  G4double bpInMargin  = lengthSafetyLarge;
  G4double bpOutMargin = bpInMargin + beamPipeThickness;
  G4double cont1Margin = bpOutMargin + lengthSafetyLarge;
  G4double cont2Margin = cont1Margin + lengthSafetyLarge;
  GenerateClicPCL(vacuumEdge,               aper1, aper2, aper3, aper4, pointsPerTwoPi);
  GenerateClicPCL(beamPipeInnerEdge,        aper1, aper2, aper3, aper4, pointsPerTwoPi, bpInMargin);
  GenerateClicPCL(beamPipeOuterEdge,        aper1, aper2, aper3, aper4, pointsPerTwoPi, bpOutMargin);
  GenerateClicPCL(containerEdge,            aper1, aper2, aper3, aper4, pointsPerTwoPi, cont1Margin);
  GenerateClicPCL(containerSubtractionEdge, aper1, aper2, aper3, aper4, pointsPerTwoPi, cont2Margin);

  extentX     = aper1 + cont1Margin;
  extentYLow  = -(std::abs(aper3) + cont1Margin);
  extentYHigh = aper2 + aper4 + cont1Margin;
}

G4double BDSBeamPipeFactoryClicPCL::CalculateIntersectionRadius(G4double aper1,
                                                                G4double aper2,
                                                                G4double aper3,
                                                                G4double aper4,
                                                                G4double beamPipeThickness)
{
  G4double bottom = std::max(std::abs(aper3), aper1);
  G4double top    = std::max(aper1, aper2);
  G4double hyp    = std::hypot(aper1, aper4);

  G4double result = std::max(std::max(bottom, top), hyp) + beamPipeThickness;
  result *= 1.2; // 20% margin
  return result;
}

void BDSBeamPipeFactoryClicPCL::CleanUpClicPCL()
{
  extentYLow  = 0;
  extentYHigh = 0;
}

void BDSBeamPipeFactoryClicPCL::CleanUp()
{
  CleanUpClicPCL();
  BDSBeamPipeFactoryPoints::CleanUp();
}

BDSBeamPipe* BDSBeamPipeFactoryClicPCL::CommonFinalConstruction(const G4String& nameIn,
                                                                G4Material* vacuumMaterialIn,
                                                                G4Material* beamPipeMaterialIn,
                                                                G4double    lengthIn)
{
  BDSBeamPipeFactoryBase::CommonConstruction(nameIn, vacuumMaterialIn,
                                             beamPipeMaterialIn, lengthIn);

  // record extents
  BDSExtent ext = BDSExtent(-extentX,     extentX,
                            extentYLow,   extentYHigh,
                            -lengthIn*0.5,lengthIn*0.5);
  
  // calculate radius if a tube were to be place around it
  G4double containerRadius = ext.MaximumAbsTransverse();
  
  BDSBeamPipe* aPipe = BuildBeamPipeAndRegisterVolumes(ext,containerRadius);
  
  return aPipe;
}
