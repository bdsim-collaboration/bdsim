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
#ifndef BDSBENDBUILDER_H
#define BDSBENDBUILDER_H

#include "globals.hh" // geant4 globals / types
#include "G4String.hh"
#include "BDSFieldType.hh"
#include "BDSIntegratorSetType.hh"
#include "BDSIntegratorType.hh"

class BDSAcceleratorComponent;
class BDSFieldInfo;
class BDSIntegratorSet;
class BDSLine;
class BDSMagnet;
class BDSMagnetStrength;
class BDSModulatorInfo;

namespace GMAD
{
  struct Element;
}

namespace BDS
{  
  /// This calculates and constructs a BDSLine* of BDSMagnet*. The bend is split
  /// into multiple wedges. If a small or zero poleface angle is specified, the
  /// end wedges will have faces angled as appropriate, the remaining wedges will
  /// re-use a single identical 'central' wedge several times. For a stronger angled
  /// poleface, the faces of each wedge fade in/out from the poleface to the cental
  /// wedge in the middle. Thin fringefield elements are placed at the beginning and
  /// end of the beamline if required.
  BDSAcceleratorComponent* BuildSBendLine(const G4String&         elementName,
					  const GMAD::Element*    element,
					  BDSMagnetStrength*      st,
					  G4double                brho,
					  const BDSIntegratorSet* integratorSet,
					  G4double                incomingFaceAngle,
					  G4double                outgoingFaceAngle,
					  G4bool                  buildFringeFields,
					  const GMAD::Element*    prevElement,
					  const GMAD::Element*    nextElement,
					  BDSModulatorInfo*       fieldModulator = nullptr);
  
  /// Construct beamline for an rbend.  A line is returned with a single
  /// magnet as the main dipole, but can have fringefield magnets placed
  /// either end if specified.
  BDSLine* BuildRBendLine(const G4String&         elementName,
			  const GMAD::Element*    element,
			  const GMAD::Element*    prevElement,
			  const GMAD::Element*    nextElement,
			  G4double                brho,
			  BDSMagnetStrength*      st,
			  const BDSIntegratorSet* integratorSet,
			  G4double                incomingFaceAngle,
			  G4double                outgoingFaceAngle,
			  G4bool                  buildFringeFields,
                          BDSModulatorInfo*       fieldModulator = nullptr);

  /// Utility function to calculate the number of segments an sbend should be split into.
  /// Based on aperture error tolerance - default is 1mm.
  G4int CalculateNSBendSegments(G4double length,
				G4double angle,
				G4double incomingFaceAngle = 0,
				G4double outgoingFaceAngle = 0,
				G4double aperturePrecision = 1.0);

  /// Thin magnet for dipole fringe field.
  /// Is beampipe only, no outer magnet.
  BDSMagnet* BuildDipoleFringe(const GMAD::Element*     element,
			       G4double                 angleIn,
			       G4double                 angleOut,
			       const G4String&          name,
			       BDSMagnetStrength*       st,
			       G4double                 brho,
			       const BDSIntegratorSet*  integratorSet,
			       BDSFieldType             dipoleFieldType,
                               BDSModulatorInfo*        fieldModulator = nullptr);

  /// Function to return a single sector bend section.
  BDSMagnet* BuildSingleSBend(const GMAD::Element*     element,
			      const G4String&          name,
			      G4double                 arcLength,
			      G4double                 angle,
			      G4double                 angleIn,
			      G4double                 angleOut,
			      const BDSMagnetStrength* strength,
			      G4double                 brho,
			      const BDSIntegratorSet*  integratorSet,
			      G4bool                   yokeOnLeft,
			      const BDSFieldInfo*      outerFieldIn,
			      BDSModulatorInfo*        fieldModulator = nullptr);
  
  void UpdateSegmentAngles(G4int     index,
			   G4int     nSBends,
			   G4double  semiAngle,
			   G4double  incomingFaceAngle,
			   G4double  outgoingFaceAngle,
			   G4double& segmentAngleIn,
			   G4double& segmentAngleOut);

  BDSMagnetStrength* GetFringeMagnetStrength(const GMAD::Element* element,
					     const BDSMagnetStrength*  st,
					     G4double            fringeAngle,
					     G4double            e1,
					     G4double            e2,
					     G4double            fintx,
					     G4bool              entranceOrExit);
  
  /// Function to get the integrator type. Test for finite K1 and returns
  /// dipole or dipolequadrupole integrator as appropriate.
  BDSIntegratorType GetDipoleIntegratorType(const BDSIntegratorSet* integratorSet,
					    const GMAD::Element*    element);

  /// Return whether finite angle or field for a dipole.
  G4bool ZeroStrengthDipole(const BDSMagnetStrength* st);
}

#endif
