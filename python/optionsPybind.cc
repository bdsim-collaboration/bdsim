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
//
// Created by Stewart Boogert on 17/03/2023.
//
#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
namespace py = pybind11;

#include "options.h"
#include "optionsBase.h"

PYBIND11_MODULE(options, m) {

  py::class_<GMAD::OptionsBase>(m,"OptionsBase")
    .def(py::init<>())

    .def_readonly("inputFileName",&GMAD::OptionsBase::inputFileName)
    .def_readonly("visMacroFileName",&GMAD::OptionsBase::visMacroFileName)
    .def_readonly("geant4MacroFileName",&GMAD::OptionsBase::geant4MacroFileName)
    .def_readonly("geant4PhysicsMacroFileName",&GMAD::OptionsBase::geant4PhysicsMacroFileName)
    .def_readonly("geant4PhysicsMacroFileNameFromExecOptions",&GMAD::OptionsBase::geant4PhysicsMacroFileNameFromExecOptions)
    .def_readonly("visDebug",&GMAD::OptionsBase::visDebug)

    .def_readonly("outputFileName",&GMAD::OptionsBase::outputFileName)
    .def_readonly("outputFormat",&GMAD::OptionsBase::outputFormat)
    .def_readonly("outputDoublePrecision",&GMAD::OptionsBase::outputDoublePrecision)
    .def_readonly("outputCompressionLevel",&GMAD::OptionsBase::outputCompressionLevel)

    .def_readonly("survey",&GMAD::OptionsBase::survey)
    .def_readonly("surveyFileName",&GMAD::OptionsBase::surveyFileName)

    .def_readonly("batch",&GMAD::OptionsBase::batch)

    .def_readonly("verbose",&GMAD::OptionsBase::verbose)

    .def_readonly("verboseRunLevel",&GMAD::OptionsBase::verboseRunLevel)

    .def_readonly("verboseEventBDSIM",&GMAD::OptionsBase::verboseEventBDSIM)
    .def_readonly("verboseEventLevel",&GMAD::OptionsBase::verboseEventLevel)
    .def_readonly("verboseEventStart",&GMAD::OptionsBase::verboseEventStart)
    .def_readonly("verboseEventContinueFor",&GMAD::OptionsBase::verboseEventContinueFor)

    .def_readonly("verboseTrackingLevel",&GMAD::OptionsBase::verboseTrackingLevel)

    .def_readonly("verboseSteppingBDSIM",&GMAD::OptionsBase::verboseSteppingBDSIM)
    .def_readonly("verboseSteppingLevel",&GMAD::OptionsBase::verboseSteppingLevel)
    .def_readonly("verboseSteppingEventStart",&GMAD::OptionsBase::verboseSteppingEventStart)
    .def_readonly("verboseSteppingEventContinueFor",&GMAD::OptionsBase::verboseSteppingEventContinueFor)
    .def_readonly("verboseSteppingPrimaryOnly",&GMAD::OptionsBase::verboseSteppingPrimaryOnly)

    .def_readonly("verboseImportanceSampling",&GMAD::OptionsBase::verboseImportanceSampling)

    .def_readonly("circular",&GMAD::OptionsBase::circular)
    .def_readonly("seed",&GMAD::OptionsBase::seed)
    .def_readonly("randomEngine",&GMAD::OptionsBase::randomEngine)
    .def_readonly("nGenerate",&GMAD::OptionsBase::nGenerate)
    .def_readonly("recreate",&GMAD::OptionsBase::recreate)
    .def_readonly("recreateFileName",&GMAD::OptionsBase::recreateFileName)
    .def_readonly("startFromEvent",&GMAD::OptionsBase::startFromEvent)
    .def_readonly("writeSeedState",&GMAD::OptionsBase::writeSeedState)
    .def_readonly("useASCIISeedState",&GMAD::OptionsBase::useASCIISeedState)
    .def_readonly("seedStateFileName",&GMAD::OptionsBase::seedStateFileName)

    .def_readonly("generatePrimariesOnly",&GMAD::OptionsBase::generatePrimariesOnly)

    .def_readonly("exportGeometry",&GMAD::OptionsBase::exportGeometry)
    .def_readonly("exportType",&GMAD::OptionsBase::exportType)
    .def_readonly("exportFileName",&GMAD::OptionsBase::exportFileName)

    .def_readonly("bdsimPath",&GMAD::OptionsBase::bdsimPath)

    .def_readonly("physicsList",&GMAD::OptionsBase::physicsList)
    .def_readonly("physicsVerbose",&GMAD::OptionsBase::physicsVerbose)
    .def_readonly("physicsVerbosity",&GMAD::OptionsBase::physicsVerbosity)
    .def_readonly("physicsEnergyLimitLow",&GMAD::OptionsBase::physicsEnergyLimitLow)
    .def_readonly("physicsEnergyLimitHigh",&GMAD::OptionsBase::physicsEnergyLimitHigh)
    .def_readonly("g4PhysicsUseBDSIMRangeCuts",&GMAD::OptionsBase::g4PhysicsUseBDSIMRangeCuts)
    .def_readonly("g4PhysicsUseBDSIMCutsAndLimits",&GMAD::OptionsBase::g4PhysicsUseBDSIMCutsAndLimits)

    .def_readonly("eventOffset",&GMAD::OptionsBase::eventOffset)
    .def_readonly("recreateSeedState",&GMAD::OptionsBase::recreateSeedState)

    .def_readonly("elossHistoBinWidth",&GMAD::OptionsBase::elossHistoBinWidth)

    .def_readonly("ffact",&GMAD::OptionsBase::ffact)

    .def_readonly("beamlineX",&GMAD::OptionsBase::beamlineX)
    .def_readonly("beamlineY",&GMAD::OptionsBase::beamlineY)
    .def_readonly("beamlineZ",&GMAD::OptionsBase::beamlineZ)
    .def_readonly("beamlinePhi",&GMAD::OptionsBase::beamlinePhi)
    .def_readonly("beamlineTheta",&GMAD::OptionsBase::beamlineTheta)
    .def_readonly("beamlinePsi",&GMAD::OptionsBase::beamlinePsi)
    .def_readonly("beamlineAxisX",&GMAD::OptionsBase::beamlineAxisX)
    .def_readonly("beamlineAxisY",&GMAD::OptionsBase::beamlineAxisY)
    .def_readonly("beamlineAxisZ",&GMAD::OptionsBase::beamlineAxisZ)
    .def_readonly("beamlineAngle",&GMAD::OptionsBase::beamlineAngle)
    .def_readonly("beamlineAxisAngle",&GMAD::OptionsBase::beamlineAxisAngle)
    .def_readonly("beamlineS",&GMAD::OptionsBase::beamlineS)

    .def_readonly("eventNumberOffset",&GMAD::OptionsBase::eventNumberOffset)

    .def_readonly("checkOverlaps",&GMAD::OptionsBase::checkOverlaps)

    .def_readonly("xsize",&GMAD::OptionsBase::xsize)
    .def_readonly("ysize",&GMAD::OptionsBase::ysize)

    /// default magnet geometry parameters
    .def_readonly("magnetGeometryType",&GMAD::OptionsBase::magnetGeometryType)
    .def_readonly("outerMaterialName",&GMAD::OptionsBase::outerMaterialName)
    .def_readonly("horizontalWidth",&GMAD::OptionsBase::horizontalWidth)
    .def_readonly("thinElementLength",&GMAD::OptionsBase::thinElementLength)
    .def_readonly("hStyle",&GMAD::OptionsBase::hStyle)
    .def_readonly("vhRatio",&GMAD::OptionsBase::vhRatio)
    .def_readonly("coilWidthFraction",&GMAD::OptionsBase::coilWidthFraction)
    .def_readonly("coilHeightFraction",&GMAD::OptionsBase::coilHeightFraction)
    .def_readonly("ignoreLocalMagnetGeometry",&GMAD::OptionsBase::ignoreLocalMagnetGeometry)
    .def_readonly("buildPoleFaceGeometry",&GMAD::OptionsBase::buildPoleFaceGeometry)

    .def_readonly("preprocessGDML",&GMAD::OptionsBase::preprocessGDML)
    .def_readonly("preprocessGDMLSchema",&GMAD::OptionsBase::preprocessGDMLSchema)

    .def_readonly("dontSplitSBends",&GMAD::OptionsBase::dontSplitSBends)

    .def_readonly("yokeFields",&GMAD::OptionsBase::yokeFields)
    .def_readonly("yokeFieldsMatchLHCGeometry",&GMAD::OptionsBase::yokeFieldsMatchLHCGeometry)
    .def_readonly("useOldMultipoleOuterFields",&GMAD::OptionsBase::useOldMultipoleOuterFields)
    .def_readonly("scalingFieldOuter",&GMAD::OptionsBase::scalingFieldOuter)

    .def_readonly("includeFringeFields",&GMAD::OptionsBase::includeFringeFields)
    .def_readonly("includeFringeFieldsCavities",&GMAD::OptionsBase::includeFringeFieldsCavities)

    .def_readonly("beampipeThickness",&GMAD::OptionsBase::beampipeThickness)
    .def_readonly("apertureType",&GMAD::OptionsBase::apertureType)
    .def_readonly("aper1",&GMAD::OptionsBase::aper1)
    .def_readonly("aper2",&GMAD::OptionsBase::aper2)
    .def_readonly("aper3",&GMAD::OptionsBase::aper3)
    .def_readonly("aper4",&GMAD::OptionsBase::aper4)
    .def_readonly("beampipeMaterial",&GMAD::OptionsBase::beampipeMaterial)
    .def_readonly("ignoreLocalAperture",&GMAD::OptionsBase::ignoreLocalAperture)

    .def_readonly("vacMaterial",&GMAD::OptionsBase::vacMaterial)
    .def_readonly("emptyMaterial",&GMAD::OptionsBase::emptyMaterial)
    .def_readonly("worldMaterial",&GMAD::OptionsBase::worldMaterial)
    .def_readonly("worldGeometryFile",&GMAD::OptionsBase::worldGeometryFile)
    .def_readonly("autoColourWorldGeometryFile",&GMAD::OptionsBase::autoColourWorldGeometryFile)
    .def_readonly("importanceWorldGeometryFile",&GMAD::OptionsBase::importanceWorldGeometryFile)
    .def_readonly("importanceVolumeMap",&GMAD::OptionsBase::importanceVolumeMap)

    .def_readonly("worldVolumeMargin",&GMAD::OptionsBase::worldVolumeMargin)

    .def_readonly("vacuumPressure",&GMAD::OptionsBase::vacuumPressure)

    .def_readonly("buildTunnel",&GMAD::OptionsBase::buildTunnel)
    .def_readonly("buildTunnelStraight",&GMAD::OptionsBase::buildTunnelStraight)
    .def_readonly("tunnelType",&GMAD::OptionsBase::tunnelType)
    .def_readonly("tunnelThickness",&GMAD::OptionsBase::tunnelThickness)
    .def_readonly("tunnelSoilThickness",&GMAD::OptionsBase::tunnelSoilThickness)
    .def_readonly("tunnelMaterial",&GMAD::OptionsBase::tunnelMaterial)
    .def_readonly("soilMaterial",&GMAD::OptionsBase::soilMaterial)
    .def_readonly("buildTunnelFloor",&GMAD::OptionsBase::buildTunnelFloor)
    .def_readonly("tunnelFloorOffset",&GMAD::OptionsBase::tunnelFloorOffset)
    .def_readonly("tunnelAper1",&GMAD::OptionsBase::tunnelAper1)
    .def_readonly("tunnelAper2",&GMAD::OptionsBase::tunnelAper2)
    .def_readonly("tunnelVisible",&GMAD::OptionsBase::tunnelVisible)
    .def_readonly("tunnelOffsetX",&GMAD::OptionsBase::tunnelOffsetX)
    .def_readonly("tunnelOffsetY",&GMAD::OptionsBase::tunnelOffsetY)
    .def_readonly("tunnelMaxSegmentLength",&GMAD::OptionsBase::tunnelMaxSegmentLength)

    .def_readonly("removeTemporaryFiles",&GMAD::OptionsBase::removeTemporaryFiles)
    .def_readonly("temporaryDirectory",&GMAD::OptionsBase::temporaryDirectory)

    .def_readonly("samplerDiameter",&GMAD::OptionsBase::samplerDiameter)

    .def_readonly("turnOnOpticalAbsorption",&GMAD::OptionsBase::turnOnOpticalAbsorption)
    .def_readonly("turnOnMieScattering",&GMAD::OptionsBase::turnOnMieScattering)
    .def_readonly("turnOnRayleighScattering",&GMAD::OptionsBase::turnOnRayleighScattering)
    .def_readonly("turnOnOpticalSurface",&GMAD::OptionsBase::turnOnOpticalSurface)

    .def_readonly("scintYieldFactor",&GMAD::OptionsBase::scintYieldFactor)
    .def_readonly("maximumPhotonsPerStep",&GMAD::OptionsBase::maximumPhotonsPerStep)
    .def_readonly("maximumBetaChangePerStep",&GMAD::OptionsBase::maximumBetaChangePerStep)
    .def_readonly("maximumTracksPerEvent",&GMAD::OptionsBase::maximumTracksPerEvent)
    .def_readonly("minimumKineticEnergy",&GMAD::OptionsBase::minimumKineticEnergy)
    .def_readonly("minimumKineticEnergyTunnel",&GMAD::OptionsBase::minimumKineticEnergyTunnel)
    .def_readonly("minimumRange",&GMAD::OptionsBase::minimumRange)
    .def_readonly("particlesToExcludeFromCuts",&GMAD::OptionsBase::particlesToExcludeFromCuts)
    .def_readonly("defaultRangeCut",&GMAD::OptionsBase::defaultRangeCut)
    .def_readonly("prodCutPhotons",&GMAD::OptionsBase::prodCutPhotons)
    .def_readonly("prodCutElectrons",&GMAD::OptionsBase::prodCutElectrons)
    .def_readonly("prodCutPositrons",&GMAD::OptionsBase::prodCutPositrons)
    .def_readonly("prodCutProtons",&GMAD::OptionsBase::prodCutProtons)
    .def_readonly("neutronTimeLimit",&GMAD::OptionsBase::neutronTimeLimit)
    .def_readonly("neutronKineticEnergyLimit",&GMAD::OptionsBase::neutronKineticEnergyLimit)
    .def_readonly("useLENDGammaNuclear",&GMAD::OptionsBase::useLENDGammaNuclear)
    .def_readonly("useElectroNuclear",&GMAD::OptionsBase::useElectroNuclear)
    .def_readonly("useMuonNuclear",&GMAD::OptionsBase::useMuonNuclear)
    .def_readonly("useGammaToMuMu",&GMAD::OptionsBase::useGammaToMuMu)
    .def_readonly("usePositronToMuMu",&GMAD::OptionsBase::usePositronToMuMu)
    .def_readonly("usePositronToHadrons",&GMAD::OptionsBase::usePositronToHadrons)
    .def_readonly("beamPipeIsInfiniteAbsorber",&GMAD::OptionsBase::beamPipeIsInfiniteAbsorber)
    .def_readonly("collimatorsAreInfiniteAbsorbers",&GMAD::OptionsBase::collimatorsAreInfiniteAbsorbers)
    .def_readonly("tunnelIsInfiniteAbsorber",&GMAD::OptionsBase::tunnelIsInfiniteAbsorber)
    .def_readonly("muonSplittingFactor",&GMAD::OptionsBase::muonSplittingFactor)
    .def_readonly("muonSplittingThresholdParentEk",&GMAD::OptionsBase::muonSplittingThresholdParentEk)
    .def_readonly("muonSplittingFactor2",&GMAD::OptionsBase::muonSplittingFactor2)
    .def_readonly("muonSplittingThresholdParentEk2",&GMAD::OptionsBase::muonSplittingThresholdParentEk2)
    .def_readonly("muonSplittingExcludeWeight1Particles",&GMAD::OptionsBase::muonSplittingExcludeWeight1Particles)
    .def_readonly("muonSplittingExclusionWeight",&GMAD::OptionsBase::muonSplittingExclusionWeight)

    .def_readonly("defaultBiasVacuum",&GMAD::OptionsBase::defaultBiasVacuum)
    .def_readonly("defaultBiasMaterial",&GMAD::OptionsBase::defaultBiasMaterial)
    .def_readonly("biasForWorldVolume",&GMAD::OptionsBase::biasForWorldVolume)
    .def_readonly("biasForWorldContents",&GMAD::OptionsBase::biasForWorldContents)
    .def_readonly("biasForWorldVacuum",&GMAD::OptionsBase::biasForWorldVacuum)
    .def_readonly("worldVacuumVolumeNames",&GMAD::OptionsBase::worldVacuumVolumeNames)

    .def_readonly("integratorSet",&GMAD::OptionsBase::integratorSet)
    .def_readonly("fieldModulator",&GMAD::OptionsBase::fieldModulator)
    .def_readonly("lengthSafety",&GMAD::OptionsBase::lengthSafety)
    .def_readonly("lengthSafetyLarge",&GMAD::OptionsBase::lengthSafetyLarge)
    .def_readonly("maximumTrackingTime",&GMAD::OptionsBase::maximumTrackingTime)
    .def_readonly("maximumStepLength",&GMAD::OptionsBase::maximumStepLength)
    .def_readonly("maximumTrackLength",&GMAD::OptionsBase::maximumTrackLength)
    .def_readonly("chordStepMinimum",&GMAD::OptionsBase::chordStepMinimum)
    .def_readonly("chordStepMinimumYoke",&GMAD::OptionsBase::chordStepMinimumYoke)
    .def_readonly("deltaIntersection",&GMAD::OptionsBase::deltaIntersection)
    .def_readonly("minimumEpsilonStep",&GMAD::OptionsBase::minimumEpsilonStep)
    .def_readonly("maximumEpsilonStep",&GMAD::OptionsBase::maximumEpsilonStep)
    .def_readonly("deltaOneStep",&GMAD::OptionsBase::deltaOneStep)
    .def_readonly("stopSecondaries",&GMAD::OptionsBase::stopSecondaries)
    .def_readonly("killNeutrinos",&GMAD::OptionsBase::killNeutrinos)
    .def_readonly("killedParticlesMassAddedToEloss",&GMAD::OptionsBase::killedParticlesMassAddedToEloss)
    .def_readonly("minimumRadiusOfCurvature",&GMAD::OptionsBase::minimumRadiusOfCurvature)
    .def_readonly("sampleElementsWithPoleface",&GMAD::OptionsBase::sampleElementsWithPoleface)
    .def_readonly("nominalMatrixRelativeMomCut",&GMAD::OptionsBase::nominalMatrixRelativeMomCut)
    .def_readonly("teleporterFullTransform",&GMAD::OptionsBase::teleporterFullTransform)
    .def_readonly("dEThresholdForScattering",&GMAD::OptionsBase::dEThresholdForScattering)
    .def_readonly("backupStepperMomLimit",&GMAD::OptionsBase::backupStepperMomLimit)

    .def_readonly("sensitiveOuter",&GMAD::OptionsBase::sensitiveOuter)
    .def_readonly("sensitiveBeamPipe",&GMAD::OptionsBase::sensitiveBeamPipe)

    .def_readonly("numberOfEventsPerNtuple",&GMAD::OptionsBase::numberOfEventsPerNtuple)

    .def_readonly("storeMinimalData",&GMAD::OptionsBase::storeMinimalData)

    .def_readonly("storeApertureImpacts",&GMAD::OptionsBase::storeApertureImpacts)
    .def_readonly("storeApertureImpactsIons",&GMAD::OptionsBase::storeApertureImpactsIons)
    .def_readonly("storeApertureImpactsAll",&GMAD::OptionsBase::storeApertureImpactsAll)
    .def_readonly("storeApertureImpactsHistograms",&GMAD::OptionsBase::storeApertureImpactsHistograms)
    .def_readonly("apertureImpactsMinimumKE",&GMAD::OptionsBase::apertureImpactsMinimumKE)
    .def_readonly("storeCavityInfo",&GMAD::OptionsBase::storeCavityInfo)
    .def_readonly("storeCollimatorInfo",&GMAD::OptionsBase::storeCollimatorInfo)
    .def_readonly("storeCollimatorHits",&GMAD::OptionsBase::storeCollimatorHits)
    .def_readonly("storeCollimatorHitsLinks",&GMAD::OptionsBase::storeCollimatorHitsLinks)
    .def_readonly("storeCollimatorHitsIons",&GMAD::OptionsBase::storeCollimatorHitsIons)
    .def_readonly("storeCollimatorHitsAll",&GMAD::OptionsBase::storeCollimatorHitsAll)
    .def_readonly("collimatorHitsMinimumKE",&GMAD::OptionsBase::collimatorHitsMinimumKE)
    .def_readonly("storeEloss",&GMAD::OptionsBase::storeEloss)
    .def_readonly("storeElossHistograms",&GMAD::OptionsBase::storeElossHistograms)
    .def_readonly("storeElossVacuum",&GMAD::OptionsBase::storeElossVacuum)
    .def_readonly("storeElossVacuumHistograms",&GMAD::OptionsBase::storeElossVacuumHistograms)
    .def_readonly("storeElossTunnel",&GMAD::OptionsBase::storeElossTunnel)
    .def_readonly("storeElossTunnelHistograms",&GMAD::OptionsBase::storeElossTunnelHistograms)
    .def_readonly("storeElossWorld",&GMAD::OptionsBase::storeElossWorld)
    .def_readonly("storeElossWorldContents",&GMAD::OptionsBase::storeElossWorldContents)
    .def_readonly("storeElossTurn",&GMAD::OptionsBase::storeElossTurn)
    .def_readonly("storeElossLinks",&GMAD::OptionsBase::storeElossLinks)
    .def_readonly("storeElossLocal",&GMAD::OptionsBase::storeElossLocal)
    .def_readonly("storeElossGlobal",&GMAD::OptionsBase::storeElossGlobal)
    .def_readonly("storeElossTime",&GMAD::OptionsBase::storeElossTime)
    .def_readonly("storeElossStepLength",&GMAD::OptionsBase::storeElossStepLength)
    .def_readonly("storeElossPreStepKineticEnergy",&GMAD::OptionsBase::storeElossPreStepKineticEnergy)
    .def_readonly("storeElossModelID",&GMAD::OptionsBase::storeElossModelID)
    .def_readonly("storeElossPhysicsProcesses",&GMAD::OptionsBase::storeElossPhysicsProcesses)
    .def_readonly("storeParticleData",&GMAD::OptionsBase::storeParticleData)
    .def_readonly("storePrimaries",&GMAD::OptionsBase::storePrimaries)
    .def_readonly("storePrimaryHistograms",&GMAD::OptionsBase::storePrimaryHistograms)

    .def_readonly("storeTrajectory",&GMAD::OptionsBase::storeTrajectory)

    .def_readonly("storeTrajectoryDepth",&GMAD::OptionsBase::storeTrajectoryDepth)
    .def_readonly("storeTrajectoryStepPoints",&GMAD::OptionsBase::storeTrajectoryStepPoints)
    .def_readonly("storeTrajectoryStepPointLast",&GMAD::OptionsBase::storeTrajectoryStepPointLast)
    .def_readonly("storeTrajectoryParticle",&GMAD::OptionsBase::storeTrajectoryParticle)
    .def_readonly("storeTrajectoryParticleID",&GMAD::OptionsBase::storeTrajectoryParticleID)
    .def_readonly("storeTrajectorySecondaryParticles",&GMAD::OptionsBase::storeTrajectorySecondaryParticles)
    .def_readonly("storeTrajectoryEnergyThreshold",&GMAD::OptionsBase::storeTrajectoryEnergyThreshold)
    .def_readonly("storeTrajectorySamplerID",&GMAD::OptionsBase::storeTrajectorySamplerID)
    .def_readonly("storeTrajectoryELossSRange",&GMAD::OptionsBase::storeTrajectoryELossSRange)

    .def_readonly("storeTrajectoryTransportationSteps",&GMAD::OptionsBase::storeTrajectoryTransportationSteps)
    .def_readonly("trajNoTransportation",&GMAD::OptionsBase::trajNoTransportation)
    .def_readonly("storeTrajectoryKineticEnergy",&GMAD::OptionsBase::storeTrajectoryKineticEnergy)
    .def_readonly("storeTrajectoryMomentumVector",&GMAD::OptionsBase::storeTrajectoryMomentumVector)
    .def_readonly("storeTrajectoryProcesses",&GMAD::OptionsBase::storeTrajectoryProcesses).def_readonly("storeTrajectoryTime",&GMAD::OptionsBase::storeTrajectoryTime)
    .def_readonly("storeTrajectoryLocal",&GMAD::OptionsBase::storeTrajectoryLocal)
    .def_readonly("storeTrajectoryLinks",&GMAD::OptionsBase::storeTrajectoryLinks)
    .def_readonly("storeTrajectoryIon",&GMAD::OptionsBase::storeTrajectoryIon)
    .def_readonly("storeTrajectoryMaterial",&GMAD::OptionsBase::storeTrajectoryMaterial)
    .def_readonly("storeTrajectoryAllVariables",&GMAD::OptionsBase::storeTrajectoryAllVariables)

    .def_readonly("trajectoryFilterLogicAND",&GMAD::OptionsBase::trajectoryFilterLogicAND)

    .def_readonly("storeSamplerAll",&GMAD::OptionsBase::storeSamplerAll)
    .def_readonly("storeSamplerPolarCoords",&GMAD::OptionsBase::storeSamplerPolarCoords)
    .def_readonly("storeSamplerCharge",&GMAD::OptionsBase::storeSamplerCharge)
    .def_readonly("storeSamplerKineticEnergy",&GMAD::OptionsBase::storeSamplerKineticEnergy)
    .def_readonly("storeSamplerMass",&GMAD::OptionsBase::storeSamplerMass)
    .def_readonly("storeSamplerRigidity",&GMAD::OptionsBase::storeSamplerRigidity)
    .def_readonly("storeSamplerIon",&GMAD::OptionsBase::storeSamplerIon)

    .def_readonly("trajCutGTZ",&GMAD::OptionsBase::trajCutGTZ)
    .def_readonly("trajCutLTR",&GMAD::OptionsBase::trajCutLTR)
    .def_readonly("trajConnect",&GMAD::OptionsBase::trajConnect)

    .def_readonly("storeModel",&GMAD::OptionsBase::storeModel)

    .def_readonly("samplersSplitLevel",&GMAD::OptionsBase::samplersSplitLevel)
    .def_readonly("modelSplitLevel",&GMAD::OptionsBase::modelSplitLevel)
    .def_readonly("uprootCompatible",&GMAD::OptionsBase::uprootCompatible)

    .def_readonly("nturns",&GMAD::OptionsBase::nturns)
    .def_readonly("ptcOneTurnMapFileName",&GMAD::OptionsBase::ptcOneTurnMapFileName)

    .def_readonly("printFractionEvents",&GMAD::OptionsBase::printFractionEvents)
    .def_readonly("printFractionTurns",&GMAD::OptionsBase::printFractionTurns)
    .def_readonly("printPhysicsProcesses",&GMAD::OptionsBase::printPhysicsProcesses)

    .def_readonly("nSegmentsPerCircle",&GMAD::OptionsBase::nSegmentsPerCircle)

    .def_readonly("nbinsx",&GMAD::OptionsBase::nbinsx)
    .def_readonly("nbinsy",&GMAD::OptionsBase::nbinsy)
    .def_readonly("nbinsz",&GMAD::OptionsBase::nbinsz)
    .def_readonly("nbinse",&GMAD::OptionsBase::nbinse)
    .def_readonly("xmin",&GMAD::OptionsBase::xmin)
    .def_readonly("xmax",&GMAD::OptionsBase::xmax)
    .def_readonly("ymin",&GMAD::OptionsBase::ymin)
    .def_readonly("ymax",&GMAD::OptionsBase::ymax)
    .def_readonly("zmin",&GMAD::OptionsBase::zmin)
    .def_readonly("zmax",&GMAD::OptionsBase::zmax)
    .def_readonly("emin",&GMAD::OptionsBase::emin)
    .def_readonly("emax",&GMAD::OptionsBase::emax)
    .def_readonly("useScoringMap",&GMAD::OptionsBase::useScoringMap)

    .def("print",&GMAD::OptionsBase::print);

  py::class_<GMAD::Published<GMAD::OptionsBase>>(m, "PublishedOptions")
    .def("NameExists", &GMAD::Options::NameExists)
    .def("AllNames",&GMAD::Options::AllNames);

  py::class_<GMAD::Options, GMAD::Published<GMAD::OptionsBase>, GMAD::OptionsBase>(m,"Options")
    .def(py::init<>())
    .def(py::init<GMAD::OptionsBase&>())
    .def("Amalgamate", &GMAD::Options::Amalgamate)
    .def("HasBeenSet", &GMAD::Options::HasBeenSet)
    .def("KeysOfSetValues", &GMAD::Options::KeysOfSetValues)

    .def("set_value",[](GMAD::Options &options,std::string name,std::string value) {options.set_value<std::string>(name,value, false);})
    .def("set_value",[](GMAD::Options &options,std::string name,int value) {options.set_value<int>(name,value, false);})
    .def("set_value",[](GMAD::Options &options,std::string name,bool value) {options.set_value<double>(name,static_cast<double>(value), false);})
    .def("set_value",[](GMAD::Options &options,std::string name,long int value) {options.set_value<long int>(name,value, false);})
    .def("set_value",[](GMAD::Options &options,std::string name,double value) {options.set_value<double>(name,value, false);})
    .def("get_value", &GMAD::Options::get_value)

    .def("keys", [](GMAD::Options &self) {return self.AllNames();})
    .def("__len__", [](GMAD::Options &self) {return self.AllNames().size();})
    .def("__setitem__", [](GMAD::Options &self, const std::string& key, bool value) {self.set_value(key,value, false);})
    .def("__setitem__", [](GMAD::Options &self, const std::string& key, int value) {self.set_value(key,value, false);})
    .def("__setitem__", [](GMAD::Options &self, const std::string& key, double value) {self.set_value(key,value, false);})
    .def("__setitem__", [](GMAD::Options &self, const std::string& key, const std::string& value) {self.set_value(key, value, false);})
    .def("_ipython_key_completions_", [](GMAD::Options &self) {return self.AllNames();});
}
