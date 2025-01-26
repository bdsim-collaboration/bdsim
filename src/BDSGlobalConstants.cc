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
#include "BDSBeamPipeInfo.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSGlobalConstants.hh"
#include "BDSFieldInfo.hh"
#include "BDSIntegratorSetType.hh"
#include "BDSOutputType.hh"
#include "BDSPTCOneTurnMap.hh"
#include "BDSParser.hh"
#include "BDSSamplerCustom.hh"
#include "BDSSamplerPlane.hh"
#include "BDSSamplerRegistry.hh"
#include "BDSTrajectoryFilter.hh"
#include "BDSTrajectoryOptions.hh"
#include "BDSTunnelInfo.hh"
#include "BDSUtilities.hh"
#include "BDSWarning.hh"

#include "parser/options.h"

#include "globals.hh"
#include "G4Colour.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"
#include "G4UserLimits.hh"
#include "G4Version.hh"
#include "G4VisAttributes.hh"

#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Vector/EulerAngles.h"

#include <cstdlib>
#include <limits>
#include <sstream>
#include <string>
#include <stdexcept>
#include <utility>

BDSGlobalConstants* BDSGlobalConstants::instance = nullptr;

BDSGlobalConstants* BDSGlobalConstants::Instance()
{
  if (!instance)
    {
      if (BDSParser::IsInitialised())
        {instance = new BDSGlobalConstants(BDSParser::Instance()->GetOptions());}
      else
        {instance = new BDSGlobalConstants(GMAD::Options());}
    }
  return instance;
}

BDSGlobalConstants::BDSGlobalConstants(const GMAD::Options& opt):
  options(opt),
  turnsTaken(0)
{
  ResetTurnNumber();
  outputType = BDS::DetermineOutputType(options.outputFormat);

  numberToGenerate = G4int(options.nGenerate);

  samplerDiameter = G4double(options.samplerDiameter)*CLHEP::m;
  curvilinearDiameter = 5*CLHEP::m;
  curvilinearDiameterShrunkForBends = false;

  // beam pipe
  defaultBeamPipeModel = new BDSBeamPipeInfo(options.apertureType,
					     options.aper1 * CLHEP::m,
					     options.aper2 * CLHEP::m,
					     options.aper3 * CLHEP::m,
					     options.aper4 * CLHEP::m,
					     options.vacMaterial,
					     options.beampipeThickness * CLHEP::m,
					     options.beampipeMaterial);
  
  // magnet geometry
  G4double horizontalWidth = options.horizontalWidth * CLHEP::m;
  if (horizontalWidth < 2*(defaultBeamPipeModel->beamPipeThickness + defaultBeamPipeModel->aper1))
    {
      G4cerr << __METHOD_NAME__ << "Error: option \"horizontalWidth\" " << horizontalWidth
	     << " must be greater than 2x (\"aper1\" + \"beamPipeThickness\") ("
	     << defaultBeamPipeModel->aper1 << " + " << defaultBeamPipeModel->beamPipeThickness << ")" << G4endl;
      throw BDSException(__METHOD_NAME__,"error in beam pipe defaults");
    }
  magnetGeometryType = BDS::DetermineMagnetGeometryType(options.magnetGeometryType);

  // tunnel
  tunnelInfo = new BDSTunnelInfo(options.tunnelType,
				 options.tunnelThickness     * CLHEP::m,
				 options.tunnelSoilThickness * CLHEP::m,
				 options.tunnelMaterial,
				 options.soilMaterial,
				 options.buildTunnelFloor,
				 options.tunnelFloorOffset   * CLHEP::m,
				 options.tunnelAper1         * CLHEP::m,
				 options.tunnelAper2         * CLHEP::m,
				 options.storeElossTunnel,
				 options.tunnelVisible);
  
  // defaults - parameters of the laserwire process
  itsLaserwireWavelength = 0.532 * CLHEP::micrometer;
  itsLaserwireDir = G4ThreeVector(1,0,0);
  itsLaserwireTrackPhotons = true;
  itsLaserwireTrackElectrons = true;

  // initialise the default vis attributes and user limits that
  // can be copied by various bits of geometry
  InitVisAttributes();
  InitDefaultUserLimits();

  integratorSet = BDS::DetermineIntegratorSetType(options.integratorSet);

  InitialiseBeamlineTransform();
  
  if (options.lengthSafetyLarge <= options.lengthSafety)
    {throw BDSException(__METHOD_NAME__, "\"lengthSafetyLarge\" must be > \"lengthSafety\"");}

  BDSSamplerPlane::chordLength  = 10*LengthSafety();
  BDSSamplerCustom::chordLength = 10*BDSSamplerPlane::chordLength;
  
  ProcessTrajectoryELossSRange();

  // mark which trajectory filters have been set
  trajectoryFiltersSet[BDSTrajectoryFilter::depth]           = options.HasBeenSet("storeTrajectoryDepth");
  trajectoryFiltersSet[BDSTrajectoryFilter::particle]        = options.HasBeenSet("storeTrajectoryParticle") ||
    options.HasBeenSet("storeTrajectoryParticleID");
  trajectoryFiltersSet[BDSTrajectoryFilter::energyThreshold] = options.HasBeenSet("storeTrajectoryEnergyThreshold");
  trajectoryFiltersSet[BDSTrajectoryFilter::sampler]         = options.HasBeenSet("storeTrajectorySamplerID");
  trajectoryFiltersSet[BDSTrajectoryFilter::elossSRange]     = options.HasBeenSet("storeTrajectoryElossSRange");
  trajectoryFiltersSet[BDSTrajectoryFilter::minimumZ]        = options.HasBeenSet("trajCutGTZ");
  trajectoryFiltersSet[BDSTrajectoryFilter::maximumR]        = options.HasBeenSet("trajCutLTR");
  trajectoryFiltersSet[BDSTrajectoryFilter::secondary]       = options.HasBeenSet("storeTrajectorySecondaryParticles");

  if (StoreMinimalData())
    {
      G4cout << "\nGlobal option> storing minimal data\n" << G4endl;
      // these options are made with respect to the defaults in parser/optionsBase.cc - i.e. no need ot set false
      // for a default that is false -> saves code
      // the one on the right MUST be the one returned by this class in the getter function in the header
      auto& o = options;
      std::map<std::string, bool*> otc = {
        {"storeApertureImpacts",               &o.storeApertureImpacts},
        {"storeApertureImpactsHistograms",     &o.storeApertureImpactsHistograms},
        {"storeCollimatorInfo",                &o.storeCollimatorInfo},
        {"storeCollimatorHits",                &o.storeCollimatorHits},
        {"storeCollimatorHitsLinks",           &o.storeCollimatorHitsLinks},
        {"storeCollimatorHitsIons",            &o.storeCollimatorHitsIons},
        {"storeCollimatorHitsAll",             &o.storeCollimatorHitsAll},
        {"storePrimaryHistograms",             &o.storePrimaryHistograms},
        {"storeTrajectoryTransportationSteps", &o.storeTrajectoryTransportationSteps},
        {"storeModel",                         &o.storeModel},
        {"storeEventLevelHistograms",          &o.storeEventLevelHistograms},
        {"storeEventLevelMeshes",              &o.storeEventLevelMeshes}
        };
      for (auto& no : otc)
	{
	  if (!options.HasBeenSet(no.first))
	    {*no.second = false;}
	}
      // try again for options that have multiple versions and check if any are set
      // even though there's only one member bool we turn on / off in the options
      std::map<std::set<std::string>, bool*> otcMultiple = {
        {{"storeELoss", "storeEloss"},                     &o.storeEloss},
        {{"storeELoss","storeEloss"},                      &o.storeEloss},
        {{"storeELossHistograms", "storeElossHistograms"}, &o.storeElossHistograms},
        {{"storeTrajectory", "storeTrajectories"},         &o.storeTrajectory},
        {{"storeParticleData","storeGeant4Data"},          &o.storeParticleData},
        {{"storePrimaries", "writePrimaries"},             &o.storePrimaries},
      };
      for (auto& no : otcMultiple)
	{
	  bool hasBeenSet = false;
	  for (auto& op : no.first)
	    {hasBeenSet = hasBeenSet || options.HasBeenSet(op);}
	  if (!hasBeenSet)
	    {*no.second = false;}
	}
    }
  
  // TBC
  if (options.HasBeenSet("fieldModulator"))
    {throw BDSException(__METHOD_NAME__, "the option \"fieldModulator\" cannot be used currently - in development");}
  

  // uproot
  if (options.uprootCompatible == 1)
    {
      options.samplersSplitLevel = 1;
      options.modelSplitLevel = 2;
    }

#if G4VERSION_NUMBER > 1079
  if (options.HasBeenSet("scintYieldFactor"))
    {BDS::Warning("The option \"scintYieldFactor\" has no effect with Geant4 11.0 onwards");}
#endif
}

void BDSGlobalConstants::InitialiseBeamlineTransform()
{  
  G4ThreeVector offset = G4ThreeVector(options.beamlineX * CLHEP::m,
				       options.beamlineY * CLHEP::m,
				       options.beamlineZ * CLHEP::m);
  
  G4RotationMatrix rm;
  if (options.beamlineAxisAngle)
    {
      G4ThreeVector axis = G4ThreeVector(options.beamlineAxisX,
					 options.beamlineAxisY,
					 options.beamlineAxisZ);

      rm = G4RotationMatrix(axis, options.beamlineAngle * CLHEP::rad);
    }
  else
    {
      G4double phi   = options.beamlinePhi   * CLHEP::rad;
      G4double theta = options.beamlineTheta * CLHEP::rad;
      G4double psi   = options.beamlinePsi   * CLHEP::rad;
      CLHEP::HepEulerAngles ang = CLHEP::HepEulerAngles(phi, theta, psi);
      rm = G4RotationMatrix(ang);
    }
  
  beamlineTransform = G4Transform3D(rm, offset);
}

void BDSGlobalConstants::InitVisAttributes()
{
  //for vacuum volumes
  invisibleVisAttr = new G4VisAttributes(G4Colour::Black());
  invisibleVisAttr->SetVisibility(false);
  invisibleVisAttr->SetForceLineSegmentsPerCircle(options.nSegmentsPerCircle);

  //for normally invisible volumes like marker / container volumes in debug mode
  visibleDebugVisAttr = new G4VisAttributes(); //green
  visibleDebugVisAttr->SetColour(0,0.6,0,0.1);
  visibleDebugVisAttr->SetVisibility(true);
  visibleDebugVisAttr->SetForceLineSegmentsPerCircle(options.nSegmentsPerCircle);
}

void BDSGlobalConstants::InitDefaultUserLimits()
{
  auto pteAsVector = BDS::SplitOnWhiteSpace(ParticlesToExcludeFromCuts());
  // construct the set of PDG IDs
  for (G4int i = 0; i < (G4int)pteAsVector.size(); i++)
    {
      try
        {
          G4int pdgID = std::stoi(pteAsVector[i]);
          particlesToExcludeFromCutsAsSet.insert(pdgID);
        }
      catch (std::logic_error& e)
        {
          G4String msg = "Particle ID " + pteAsVector[i] + " at index " + std::to_string(i);
          msg += " in the option particlesToExcludeFromCutsAsSet cannot be converted to an integer";
          throw BDSException(__METHOD_NAME__, msg);
        }
    }
  
  defaultUserLimits = new G4UserLimits("default_cuts");
  const G4double maxTime = MaxTime();
  if (maxTime > 0)
    {
      G4cout << __METHOD_NAME__ << "Setting maximum tracking time to " << maxTime << " ns" << G4endl;
      defaultUserLimits->SetUserMaxTime(maxTime);
    }
  defaultUserLimits->SetMaxAllowedStep(MaxStepLength());
  defaultUserLimits->SetUserMaxTrackLength(MaxTrackLength());
  G4double minEK = MinimumKineticEnergy();
  defaultUserLimits->SetUserMinEkine(minEK);
  if (minEK > 0)
    {G4cout << __METHOD_NAME__ << "Default minimum kinetic energy for model: " << minEK/CLHEP::GeV << " GeV" << G4endl;}
  G4double minR = MinimumRange();
  defaultUserLimits->SetUserMinRange(minR);
  if (minR > 0)
    {G4cout << __METHOD_NAME__ << "Default minimum range for user limits: " << minR/CLHEP::mm << " mm" << G4endl;} 

  
  BDSFieldInfo::defaultUL = defaultUserLimits; // update static member for field definitions

  defaultUserLimitsTunnel = new G4UserLimits(*defaultUserLimits);
  defaultUserLimitsTunnel->SetType("default_cuts_tunnel");
  if (BDS::IsFinite(MinimumKineticEnergyTunnel()))
    {defaultUserLimitsTunnel->SetUserMinEkine(MinimumKineticEnergyTunnel());}
  if (TunnelIsInfiniteAbsorber())
    {defaultUserLimitsTunnel->SetUserMinEkine(std::numeric_limits<double>::max());}
}

G4int BDSGlobalConstants::PrintModuloEvents() const
{
  G4int nGenerate = NGenerate();
  G4double fraction = PrintFractionEvents();
  G4int printModulo = (G4int)ceil(nGenerate * fraction);
  if (printModulo < 0)
    {printModulo = 1;}

  if (!Batch())
    {printModulo = 1;} // interactive -> print every event
  return printModulo;
}

G4int BDSGlobalConstants::PrintModuloTurns() const
{
  G4int nTurns = TurnsToTake();
  G4double fraction = PrintFractionTurns();
  G4int printModulo = (G4int)ceil(nTurns * fraction);
  if (printModulo < 0)
    {printModulo = 1;}

  if (!Batch())
    {printModulo = 1;} // interactive -> print every turn
  return printModulo;
}

BDSGlobalConstants::~BDSGlobalConstants()
{  
  delete defaultBeamPipeModel;
  delete tunnelInfo;
  delete defaultUserLimits;
  delete defaultUserLimitsTunnel;
  delete invisibleVisAttr;
  delete visibleDebugVisAttr;

  instance = nullptr;
}

void BDSGlobalConstants::ProcessTrajectoryELossSRange()
{
  if (options.storeTrajectoryELossSRange.empty())
    {return;}
  std::istringstream is(options.storeTrajectoryELossSRange);
  std::string tok;
  while (is >> tok)
    {
      std::size_t loc = tok.find(':',0);
      if (loc == std::string::npos)
	{throw BDSException(__METHOD_NAME__, "Error: no ':' character found in option storeTrajectoryELossSRange \"" + options.storeTrajectoryELossSRange + "\" - invalid range.");}
      G4double rstart = 0;
      G4double rend   = 0;
      try
	{
	  rstart = std::stod(tok.substr(0, loc));
	  rend   = std::stod(tok.substr(loc+1,tok.size()));
	}
      catch (const std::invalid_argument& e)
	{
	  G4cerr << "Invalid value \"" << tok << "\" in option storeTrajectoryELossSRange." << G4endl;
	  throw BDSException(__METHOD_NAME__, "Error: can't convert string to number for option storeTrajectoryELossSRange.");
	}
      if (rend < rstart)
	{
	  G4String message = "Error in option storeTrajectoryElossSRange - end point "
	    + std::to_string(rend) + " is less than beginning " + std::to_string(rstart) + ".";
	  throw BDSException(__METHOD_NAME__, message);
	}
      elossSRange.emplace_back(rstart*CLHEP::m, rend*CLHEP::m);
    }
}

G4bool BDSGlobalConstants::StoreTrajectoryTransportationSteps() const
{
  // only if true we let this option take precedence
  if (options.trajNoTransportation)
    {return false;}
  else
    {return options.storeTrajectoryTransportationSteps;}
}

BDS::TrajectoryOptions BDSGlobalConstants::StoreTrajectoryOptions() const
{
  BDS::TrajectoryOptions result = {!StoreTrajectoryTransportationSteps(),
				   StoreTrajectoryKineticEnergy(),
				   StoreTrajectoryMomentumVector(),
				   StoreTrajectoryProcesses(),
				   StoreTrajectoryTime(),
				   StoreTrajectoryLocal(),
				   StoreTrajectoryLinks(),
				   StoreTrajectoryIon(),
				   StoreTrajectoryMaterial()};
  
  if (StoreTrajectoryAllVariables())
  {
    result.storeKineticEnergy  = true;
    result.storeMomentumVector = true;
    result.storeProcesses      = true;
    result.storeTime           = true;
    result.storeLocal          = true;
    result.storeLinks          = true;
    result.storeIon            = true;
    result.storeMaterial       = true;
  }
  return result;
}
