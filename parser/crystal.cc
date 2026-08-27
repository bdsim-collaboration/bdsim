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
#include "crystal.h"

using namespace GMAD;

Crystal::Crystal()
{
  clear();
  PublishMembers();
}

void Crystal::clear()
{
  name              = "";
  model             = "legacy";
  material          = "";
  data              = "";
  lattice           = "";
  fastSimDataPath   = "";
  shape             = "";
  lengthX           = 0;
  lengthY           = 0;
  lengthZ           = 0;
  sizeA             = 0;
  sizeB             = 0;
  sizeC             = 0;
  alpha             = 0;
  beta              = 0;
  gamma             = 0;
  spaceGroup        = 0;
  bendingAngleYAxis = 0;
  bendingAngleZAxis = 0;
  miscutAngleY      = 0;

  fastSimCUAmplitude = 0;
  fastSimCUPeriod = 0;
  fastSimCUPhase = 0;
  fastSimCUGeometryFile = "";

  fastSimParticles.clear();
  fastSimDefaultLowKineticEnergy = 0.2;
  fastSimDefaultLindhardAngleHighLimit = 100;
  fastSimDefaultHighAngleLimit = 0;
  fastSimLowEnergyParticles.clear();
  fastSimLowEnergyLimits.clear();
  fastSimLindhardAngleParticles.clear();
  fastSimLindhardAngleLimits.clear();
  fastSimHighAngleParticles.clear();
  fastSimHighAngleLimits.clear();
  fastSimMaxPhotonsPerStep = 1000;

  radiation = false;
  radiationSinglePhotonProbabilityLimit = 0.25;
  radiationSmallTrajectorySteps = 10000;
  radiationSamplingPhotons = 150;
  radiationAngleFactor = 4;
  // G4BaierKatkov default in Geant4 11.4.
  radiationMinPhotonEnergy = 0.0001;
  radiationMaxPhotonEnergy = 1;
  radiationSpectrumBins = 110;
  radiationStatisticsMinEnergy.clear();
  radiationStatisticsMaxEnergy.clear();
  radiationStatisticsMultiple.clear();
  radiationVirtualCollimator = "none";
  radiationCollimatorHalfWidthX = 0;
  radiationCollimatorHalfWidthY = 0;
  radiationCollimatorCentreX = 0;
  radiationCollimatorCentreY = 0;

  coherentPairProduction = false;
  coherentPairProductionIncoherent = false;
  coherentPairProductionLowEnergyLimit = 1;
  coherentPairProductionHighAngleLimit = 0.05;
  coherentPairProductionKineticEnergyCut = 0.001;
  coherentPairProductionSamplingPairs = 150;
  coherentPairProductionAngleFactor = 4;
  coherentPairProductionTrajectorySteps = 250;
}

void Crystal::PublishMembers()
{
  publish("name",       &Crystal::name);
  publish("model",      &Crystal::model);
  publish("material",   &Crystal::material);
  publish("data",       &Crystal::data);
  publish("lattice",    &Crystal::lattice);
  publish("fastSimDataPath", &Crystal::fastSimDataPath);
  publish("shape",      &Crystal::shape);
  publish("lengthX",    &Crystal::lengthX);
  publish("lengthY",    &Crystal::lengthY);
  publish("lengthZ",    &Crystal::lengthZ);
  publish("sizeA",      &Crystal::sizeA);
  publish("sizeB",      &Crystal::sizeB);
  publish("sizeC",      &Crystal::sizeC);
  publish("alpha",      &Crystal::alpha);
  publish("beta",       &Crystal::beta);
  publish("gamma",      &Crystal::gamma);
  publish("spaceGroup", &Crystal::spaceGroup);
  publish("bendingAngleYAxis", &Crystal::bendingAngleYAxis);
  publish("bendingAngleZAxis", &Crystal::bendingAngleZAxis);
  publish("miscutAngleY",      &Crystal::miscutAngleY);

  publish("fastSimCUAmplitude",    &Crystal::fastSimCUAmplitude);
  publish("fastSimCUPeriod",       &Crystal::fastSimCUPeriod);
  publish("fastSimCUPhase",        &Crystal::fastSimCUPhase);
  publish("fastSimCUGeometryFile", &Crystal::fastSimCUGeometryFile);

  publish("fastSimParticles", &Crystal::fastSimParticles);
  publish("fastSimDefaultLowKineticEnergy", &Crystal::fastSimDefaultLowKineticEnergy);
  publish("fastSimDefaultLindhardAngleHighLimit", &Crystal::fastSimDefaultLindhardAngleHighLimit);
  publish("fastSimDefaultHighAngleLimit", &Crystal::fastSimDefaultHighAngleLimit);
  publish("fastSimLowEnergyParticles", &Crystal::fastSimLowEnergyParticles);
  publish("fastSimLowEnergyLimits", &Crystal::fastSimLowEnergyLimits);
  publish("fastSimLindhardAngleParticles", &Crystal::fastSimLindhardAngleParticles);
  publish("fastSimLindhardAngleLimits", &Crystal::fastSimLindhardAngleLimits);
  publish("fastSimHighAngleParticles", &Crystal::fastSimHighAngleParticles);
  publish("fastSimHighAngleLimits", &Crystal::fastSimHighAngleLimits);
  publish("fastSimMaxPhotonsPerStep", &Crystal::fastSimMaxPhotonsPerStep);

  publish("radiation", &Crystal::radiation);
  publish("radiationSinglePhotonProbabilityLimit", &Crystal::radiationSinglePhotonProbabilityLimit);
  publish("radiationSmallTrajectorySteps", &Crystal::radiationSmallTrajectorySteps);
  publish("radiationSamplingPhotons", &Crystal::radiationSamplingPhotons);
  publish("radiationAngleFactor", &Crystal::radiationAngleFactor);
  publish("radiationMinPhotonEnergy", &Crystal::radiationMinPhotonEnergy);
  publish("radiationMaxPhotonEnergy", &Crystal::radiationMaxPhotonEnergy);
  publish("radiationSpectrumBins", &Crystal::radiationSpectrumBins);
  publish("radiationStatisticsMinEnergy", &Crystal::radiationStatisticsMinEnergy);
  publish("radiationStatisticsMaxEnergy", &Crystal::radiationStatisticsMaxEnergy);
  publish("radiationStatisticsMultiple", &Crystal::radiationStatisticsMultiple);
  publish("radiationVirtualCollimator", &Crystal::radiationVirtualCollimator);
  publish("radiationCollimatorHalfWidthX", &Crystal::radiationCollimatorHalfWidthX);
  publish("radiationCollimatorHalfWidthY", &Crystal::radiationCollimatorHalfWidthY);
  publish("radiationCollimatorCentreX", &Crystal::radiationCollimatorCentreX);
  publish("radiationCollimatorCentreY", &Crystal::radiationCollimatorCentreY);

  publish("coherentPairProduction", &Crystal::coherentPairProduction);
  publish("coherentPairProductionIncoherent", &Crystal::coherentPairProductionIncoherent);
  publish("coherentPairProductionLowEnergyLimit", &Crystal::coherentPairProductionLowEnergyLimit);
  publish("coherentPairProductionHighAngleLimit", &Crystal::coherentPairProductionHighAngleLimit);
  publish("coherentPairProductionKineticEnergyCut", &Crystal::coherentPairProductionKineticEnergyCut);
  publish("coherentPairProductionSamplingPairs", &Crystal::coherentPairProductionSamplingPairs);
  publish("coherentPairProductionAngleFactor", &Crystal::coherentPairProductionAngleFactor);
  publish("coherentPairProductionTrajectorySteps", &Crystal::coherentPairProductionTrajectorySteps);
}

void Crystal::print()const
{
  std::cout << "crystal: "
	    << "name "       << name       << std::endl
	    << "model "      << model      << std::endl
	    << "material "   << material   << std::endl
	    << "data "       << data       << std::endl
	    << "lattice "    << lattice    << std::endl
	    << "shape "      << shape      << std::endl
	    << "lengthX "    << lengthX    << std::endl
      << "lengthY "    << lengthY    << std::endl
      << "lengthZ "    << lengthZ    << std::endl
	    << "sizeA "      << sizeA      << std::endl
      << "sizeB "      << sizeB      << std::endl
      << "sizeC "      << sizeC      << std::endl
	    << "alpha "      << alpha      << std::endl
	    << "beta "       << beta       << std::endl
	    << "gamma "      << gamma      << std::endl
	    << "spaceGroup " << spaceGroup << std::endl
	    << "bendingAngleYAxis " << bendingAngleYAxis << std::endl
	    << "bendingAngleZAxis " << bendingAngleZAxis << std::endl
	    << "miscutAngleY "      << miscutAngleY      << std::endl;
}
