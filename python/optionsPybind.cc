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

    .def_readwrite("inputFileName",&GMAD::OptionsBase::inputFileName)
    .def_readwrite("visMacroFileName",&GMAD::OptionsBase::visMacroFileName)
    .def_readwrite("geant4MacroFileName",&GMAD::OptionsBase::geant4MacroFileName)
    .def_readwrite("geant4PhysicsMacroFileName",&GMAD::OptionsBase::geant4PhysicsMacroFileName)
    .def_readwrite("geant4PhysicsMacroFileNameFromExecOptions",&GMAD::OptionsBase::geant4PhysicsMacroFileNameFromExecOptions)
    .def_readwrite("visDebug",&GMAD::OptionsBase::visDebug)

    .def_readwrite("outputFileName",&GMAD::OptionsBase::outputFileName)
    .def_readwrite("outputFormat",&GMAD::OptionsBase::outputFormat)
    .def_readwrite("outputDoublePrecision",&GMAD::OptionsBase::outputDoublePrecision)
    .def_readwrite("outputCompressionLevel",&GMAD::OptionsBase::outputCompressionLevel)

    .def_readwrite("survey",&GMAD::OptionsBase::survey)
    .def_readwrite("surveyFileName",&GMAD::OptionsBase::surveyFileName)

    .def_readwrite("batch",&GMAD::OptionsBase::batch)

    .def_readwrite("verbose",&GMAD::OptionsBase::verbose)

    .def_readwrite("verboseRunLevel",&GMAD::OptionsBase::verboseRunLevel)

    .def_readwrite("verboseEventBDSIM",&GMAD::OptionsBase::verboseEventBDSIM)
    .def_readwrite("verboseEventLevel",&GMAD::OptionsBase::verboseEventLevel)
    .def_readwrite("verboseEventStart",&GMAD::OptionsBase::verboseEventStart)
    .def_readwrite("verboseEventContinueFor",&GMAD::OptionsBase::verboseEventContinueFor)

    .def_readwrite("verboseTrackingLevel",&GMAD::OptionsBase::verboseTrackingLevel)

    .def_readwrite("verboseSteppingBDSIM",&GMAD::OptionsBase::verboseSteppingBDSIM)
    .def_readwrite("verboseSteppingLevel",&GMAD::OptionsBase::verboseSteppingLevel)
    .def_readwrite("verboseSteppingEventStart",&GMAD::OptionsBase::verboseSteppingEventStart)
    .def_readwrite("verboseSteppingEventContinueFor",&GMAD::OptionsBase::verboseSteppingEventContinueFor)
    .def_readwrite("verboseSteppingPrimaryOnly",&GMAD::OptionsBase::verboseSteppingPrimaryOnly)

    .def_readwrite("verboseImportanceSampling",&GMAD::OptionsBase::verboseImportanceSampling)

    .def_readwrite("circular",&GMAD::OptionsBase::circular)
    .def_readwrite("seed",&GMAD::OptionsBase::seed)
    .def_readwrite("randomEngine",&GMAD::OptionsBase::randomEngine)
    .def_readwrite("nGenerate",&GMAD::OptionsBase::nGenerate)
    .def_readwrite("recreate",&GMAD::OptionsBase::recreate)
    .def_readwrite("recreateFileName",&GMAD::OptionsBase::recreateFileName)
    .def_readwrite("startFromEvent",&GMAD::OptionsBase::startFromEvent)
    .def_readwrite("writeSeedState",&GMAD::OptionsBase::writeSeedState)
    .def_readwrite("useASCIISeedState",&GMAD::OptionsBase::useASCIISeedState)
    .def_readwrite("seedStateFileName",&GMAD::OptionsBase::seedStateFileName)

    .def_readwrite("generatePrimariesOnly",&GMAD::OptionsBase::generatePrimariesOnly)

    .def_readwrite("exportGeometry",&GMAD::OptionsBase::exportGeometry)
    .def_readwrite("exportType",&GMAD::OptionsBase::exportType)
    .def_readwrite("exportFileName",&GMAD::OptionsBase::exportFileName)

    .def_readwrite("bdsimPath",&GMAD::OptionsBase::bdsimPath)

    .def_readwrite("physicsList",&GMAD::OptionsBase::physicsList)
    .def_readwrite("physicsVerbose",&GMAD::OptionsBase::physicsVerbose)
    .def_readwrite("physicsVerbosity",&GMAD::OptionsBase::physicsVerbosity)
    .def_readwrite("physicsEnergyLimitLow",&GMAD::OptionsBase::physicsEnergyLimitLow)
    .def_readwrite("physicsEnergyLimitHigh",&GMAD::OptionsBase::physicsEnergyLimitHigh)
    .def_readwrite("g4PhysicsUseBDSIMRangeCuts",&GMAD::OptionsBase::g4PhysicsUseBDSIMRangeCuts)
    .def_readwrite("g4PhysicsUseBDSIMCutsAndLimits",&GMAD::OptionsBase::g4PhysicsUseBDSIMCutsAndLimits)

    .def_readwrite("eventOffset",&GMAD::OptionsBase::eventOffset)
    .def_readwrite("recreateSeedState",&GMAD::OptionsBase::recreateSeedState)

    .def_readwrite("elossHistoBinWidth",&GMAD::OptionsBase::elossHistoBinWidth)

    .def_readwrite("ffact",&GMAD::OptionsBase::ffact)

    .def_readwrite("beamlineX",&GMAD::OptionsBase::beamlineX)
    .def_readwrite("beamlineY",&GMAD::OptionsBase::beamlineY)
    .def_readwrite("beamlineZ",&GMAD::OptionsBase::beamlineZ)
    .def_readwrite("beamlinePhi",&GMAD::OptionsBase::beamlinePhi)
    .def_readwrite("beamlineTheta",&GMAD::OptionsBase::beamlineTheta)
    .def_readwrite("beamlinePsi",&GMAD::OptionsBase::beamlinePsi)
    .def_readwrite("beamlineAxisX",&GMAD::OptionsBase::beamlineAxisX)
    .def_readwrite("beamlineAxisY",&GMAD::OptionsBase::beamlineAxisY)
    .def_readwrite("beamlineAxisZ",&GMAD::OptionsBase::beamlineAxisZ)
    .def_readwrite("beamlineAngle",&GMAD::OptionsBase::beamlineAngle)
    .def_readwrite("beamlineAxisAngle",&GMAD::OptionsBase::beamlineAxisAngle)
    .def_readwrite("beamlineS",&GMAD::OptionsBase::beamlineS)

    .def_readwrite("eventNumberOffset",&GMAD::OptionsBase::eventNumberOffset)

    .def_readwrite("checkOverlaps",&GMAD::OptionsBase::checkOverlaps)

    .def_readwrite("xsize",&GMAD::OptionsBase::xsize)
    .def_readwrite("ysize",&GMAD::OptionsBase::ysize)

    /// default magnet geometry parameters
    .def_readwrite("magnetGeometryType",&GMAD::OptionsBase::magnetGeometryType)
    .def_readwrite("outerMaterialName",&GMAD::OptionsBase::outerMaterialName)
    .def_readwrite("horizontalWidth",&GMAD::OptionsBase::horizontalWidth)
    .def_readwrite("thinElementLength",&GMAD::OptionsBase::thinElementLength)
    .def_readwrite("hStyle",&GMAD::OptionsBase::hStyle)
    .def_readwrite("vhRatio",&GMAD::OptionsBase::vhRatio)
    .def_readwrite("coilWidthFraction",&GMAD::OptionsBase::coilWidthFraction)
    .def_readwrite("coilHeightFraction",&GMAD::OptionsBase::coilHeightFraction)
    .def_readwrite("ignoreLocalMagnetGeometry",&GMAD::OptionsBase::ignoreLocalMagnetGeometry)
    .def_readwrite("buildPoleFaceGeometry",&GMAD::OptionsBase::buildPoleFaceGeometry)

    .def_readwrite("preprocessGDML",&GMAD::OptionsBase::preprocessGDML)
    .def_readwrite("preprocessGDMLSchema",&GMAD::OptionsBase::preprocessGDMLSchema)

    .def_readwrite("dontSplitSBends",&GMAD::OptionsBase::dontSplitSBends)

    .def_readwrite("yokeFields",&GMAD::OptionsBase::yokeFields)
    .def_readwrite("yokeFieldsMatchLHCGeometry",&GMAD::OptionsBase::yokeFieldsMatchLHCGeometry)
    .def_readwrite("useOldMultipoleOuterFields",&GMAD::OptionsBase::useOldMultipoleOuterFields)
    .def_readwrite("scalingFieldOuter",&GMAD::OptionsBase::scalingFieldOuter)

    .def_readwrite("includeFringeFields",&GMAD::OptionsBase::includeFringeFields)
    .def_readwrite("includeFringeFieldsCavities",&GMAD::OptionsBase::includeFringeFieldsCavities)

    .def_readwrite("beampipeThickness",&GMAD::OptionsBase::beampipeThickness)
    .def_readwrite("apertureType",&GMAD::OptionsBase::apertureType)
    .def_readwrite("aper1",&GMAD::OptionsBase::aper1)
    .def_readwrite("aper2",&GMAD::OptionsBase::aper2)
    .def_readwrite("aper3",&GMAD::OptionsBase::aper3)
    .def_readwrite("aper4",&GMAD::OptionsBase::aper4)
    .def_readwrite("beampipeMaterial",&GMAD::OptionsBase::beampipeMaterial)
    .def_readwrite("ignoreLocalAperture",&GMAD::OptionsBase::ignoreLocalAperture)

    .def_readwrite("vacMaterial",&GMAD::OptionsBase::vacMaterial)
    .def_readwrite("emptyMaterial",&GMAD::OptionsBase::emptyMaterial)
    .def_readwrite("worldMaterial",&GMAD::OptionsBase::worldMaterial)
    .def_readwrite("worldGeometryFile",&GMAD::OptionsBase::worldGeometryFile)
    .def_readwrite("autoColourWorldGeometryFile",&GMAD::OptionsBase::autoColourWorldGeometryFile)
    .def_readwrite("importanceWorldGeometryFile",&GMAD::OptionsBase::importanceWorldGeometryFile)
    .def_readwrite("importanceVolumeMap",&GMAD::OptionsBase::importanceVolumeMap)

    .def_readwrite("worldVolumeMargin",&GMAD::OptionsBase::worldVolumeMargin)

    .def_readwrite("vacuumPressure",&GMAD::OptionsBase::vacuumPressure)

    .def_readwrite("buildTunnel",&GMAD::OptionsBase::buildTunnel)
    .def_readwrite("buildTunnelStraight",&GMAD::OptionsBase::buildTunnelStraight)
    .def_readwrite("tunnelType",&GMAD::OptionsBase::tunnelType)
    .def_readwrite("tunnelThickness",&GMAD::OptionsBase::tunnelThickness)
    .def_readwrite("tunnelSoilThickness",&GMAD::OptionsBase::tunnelSoilThickness)
    .def_readwrite("tunnelMaterial",&GMAD::OptionsBase::tunnelMaterial)
    .def_readwrite("soilMaterial",&GMAD::OptionsBase::soilMaterial)
    .def_readwrite("buildTunnelFloor",&GMAD::OptionsBase::buildTunnelFloor)
    .def_readwrite("tunnelFloorOffset",&GMAD::OptionsBase::tunnelFloorOffset)
    .def_readwrite("tunnelAper1",&GMAD::OptionsBase::tunnelAper1)
    .def_readwrite("tunnelAper2",&GMAD::OptionsBase::tunnelAper2)
    .def_readwrite("tunnelVisible",&GMAD::OptionsBase::tunnelVisible)
    .def_readwrite("tunnelOffsetX",&GMAD::OptionsBase::tunnelOffsetX)
    .def_readwrite("tunnelOffsetY",&GMAD::OptionsBase::tunnelOffsetY)
    .def_readwrite("tunnelMaxSegmentLength",&GMAD::OptionsBase::tunnelMaxSegmentLength)

    .def_readwrite("removeTemporaryFiles",&GMAD::OptionsBase::removeTemporaryFiles)
    .def_readwrite("temporaryDirectory",&GMAD::OptionsBase::temporaryDirectory)

    .def_readwrite("samplerDiameter",&GMAD::OptionsBase::samplerDiameter)

    .def_readwrite("turnOnOpticalAbsorption",&GMAD::OptionsBase::turnOnOpticalAbsorption)
    .def_readwrite("turnOnMieScattering",&GMAD::OptionsBase::turnOnMieScattering)
    .def_readwrite("turnOnRayleighScattering",&GMAD::OptionsBase::turnOnRayleighScattering)
    .def_readwrite("turnOnOpticalSurface",&GMAD::OptionsBase::turnOnOpticalSurface)

    .def_readwrite("scintYieldFactor",&GMAD::OptionsBase::scintYieldFactor)
    .def_readwrite("maximumPhotonsPerStep",&GMAD::OptionsBase::maximumPhotonsPerStep)
    .def_readwrite("maximumBetaChangePerStep",&GMAD::OptionsBase::maximumBetaChangePerStep)
    .def_readwrite("maximumTracksPerEvent",&GMAD::OptionsBase::maximumTracksPerEvent)
    .def_readwrite("minimumKineticEnergy",&GMAD::OptionsBase::minimumKineticEnergy)
    .def_readwrite("minimumKineticEnergyTunnel",&GMAD::OptionsBase::minimumKineticEnergyTunnel)
    .def_readwrite("minimumRange",&GMAD::OptionsBase::minimumRange)
    .def_readwrite("particlesToExcludeFromCuts",&GMAD::OptionsBase::particlesToExcludeFromCuts)
    .def_readwrite("defaultRangeCut",&GMAD::OptionsBase::defaultRangeCut)
    .def_readwrite("prodCutPhotons",&GMAD::OptionsBase::prodCutPhotons)
    .def_readwrite("prodCutElectrons",&GMAD::OptionsBase::prodCutElectrons)
    .def_readwrite("prodCutPositrons",&GMAD::OptionsBase::prodCutPositrons)
    .def_readwrite("prodCutProtons",&GMAD::OptionsBase::prodCutProtons)
    .def_readwrite("neutronTimeLimit",&GMAD::OptionsBase::neutronTimeLimit)
    .def_readwrite("neutronKineticEnergyLimit",&GMAD::OptionsBase::neutronKineticEnergyLimit)
    .def_readwrite("useLENDGammaNuclear",&GMAD::OptionsBase::useLENDGammaNuclear)
    .def_readwrite("useElectroNuclear",&GMAD::OptionsBase::useElectroNuclear)
    .def_readwrite("useMuonNuclear",&GMAD::OptionsBase::useMuonNuclear)
    .def_readwrite("useGammaToMuMu",&GMAD::OptionsBase::useGammaToMuMu)
    .def_readwrite("usePositronToMuMu",&GMAD::OptionsBase::usePositronToMuMu)
    .def_readwrite("usePositronToHadrons",&GMAD::OptionsBase::usePositronToHadrons)
    .def_readwrite("beamPipeIsInfiniteAbsorber",&GMAD::OptionsBase::beamPipeIsInfiniteAbsorber)
    .def_readwrite("collimatorsAreInfiniteAbsorbers",&GMAD::OptionsBase::collimatorsAreInfiniteAbsorbers)
    .def_readwrite("tunnelIsInfiniteAbsorber",&GMAD::OptionsBase::tunnelIsInfiniteAbsorber)
    .def_readwrite("muonSplittingFactor",&GMAD::OptionsBase::muonSplittingFactor)
    .def_readwrite("muonSplittingThresholdParentEk",&GMAD::OptionsBase::muonSplittingThresholdParentEk)
    .def_readwrite("muonSplittingFactor2",&GMAD::OptionsBase::muonSplittingFactor2)
    .def_readwrite("muonSplittingThresholdParentEk2",&GMAD::OptionsBase::muonSplittingThresholdParentEk2)
    .def_readwrite("muonSplittingExcludeWeight1Particles",&GMAD::OptionsBase::muonSplittingExcludeWeight1Particles)
    .def_readwrite("muonSplittingExclusionWeight",&GMAD::OptionsBase::muonSplittingExclusionWeight)

    .def_readwrite("defaultBiasVacuum",&GMAD::OptionsBase::defaultBiasVacuum)
    .def_readwrite("defaultBiasMaterial",&GMAD::OptionsBase::defaultBiasMaterial)
    .def_readwrite("biasForWorldVolume",&GMAD::OptionsBase::biasForWorldVolume)
    .def_readwrite("biasForWorldContents",&GMAD::OptionsBase::biasForWorldContents)
    .def_readwrite("biasForWorldVacuum",&GMAD::OptionsBase::biasForWorldVacuum)
    .def_readwrite("worldVacuumVolumeNames",&GMAD::OptionsBase::worldVacuumVolumeNames)

    .def_readwrite("integratorSet",&GMAD::OptionsBase::integratorSet)
    .def_readwrite("fieldModulator",&GMAD::OptionsBase::fieldModulator)
    .def_readwrite("lengthSafety",&GMAD::OptionsBase::lengthSafety)
    .def_readwrite("lengthSafetyLarge",&GMAD::OptionsBase::lengthSafetyLarge)
    .def_readwrite("maximumTrackingTime",&GMAD::OptionsBase::maximumTrackingTime)
    .def_readwrite("maximumStepLength",&GMAD::OptionsBase::maximumStepLength)
    .def_readwrite("maximumTrackLength",&GMAD::OptionsBase::maximumTrackLength)
    .def_readwrite("chordStepMinimum",&GMAD::OptionsBase::chordStepMinimum)
    .def_readwrite("chordStepMinimumYoke",&GMAD::OptionsBase::chordStepMinimumYoke)
    .def_readwrite("deltaIntersection",&GMAD::OptionsBase::deltaIntersection)
    .def_readwrite("minimumEpsilonStep",&GMAD::OptionsBase::minimumEpsilonStep)
    .def_readwrite("maximumEpsilonStep",&GMAD::OptionsBase::maximumEpsilonStep)
    .def_readwrite("deltaOneStep",&GMAD::OptionsBase::deltaOneStep)
    .def_readwrite("stopSecondaries",&GMAD::OptionsBase::stopSecondaries)
    .def_readwrite("killNeutrinos",&GMAD::OptionsBase::killNeutrinos)
    .def_readwrite("killedParticlesMassAddedToEloss",&GMAD::OptionsBase::killedParticlesMassAddedToEloss)
    .def_readwrite("minimumRadiusOfCurvature",&GMAD::OptionsBase::minimumRadiusOfCurvature)
    .def_readwrite("sampleElementsWithPoleface",&GMAD::OptionsBase::sampleElementsWithPoleface)
    .def_readwrite("nominalMatrixRelativeMomCut",&GMAD::OptionsBase::nominalMatrixRelativeMomCut)
    .def_readwrite("teleporterFullTransform",&GMAD::OptionsBase::teleporterFullTransform)
    .def_readwrite("dEThresholdForScattering",&GMAD::OptionsBase::dEThresholdForScattering)
    .def_readwrite("backupStepperMomLimit",&GMAD::OptionsBase::backupStepperMomLimit)

    .def_readwrite("sensitiveOuter",&GMAD::OptionsBase::sensitiveOuter)
    .def_readwrite("sensitiveBeamPipe",&GMAD::OptionsBase::sensitiveBeamPipe)

    .def_readwrite("numberOfEventsPerNtuple",&GMAD::OptionsBase::numberOfEventsPerNtuple)

    .def_readwrite("storeMinimalData",&GMAD::OptionsBase::storeMinimalData)

    .def_readwrite("storeApertureImpacts",&GMAD::OptionsBase::storeApertureImpacts)
    .def_readwrite("storeApertureImpactsIons",&GMAD::OptionsBase::storeApertureImpactsIons)
    .def_readwrite("storeApertureImpactsAll",&GMAD::OptionsBase::storeApertureImpactsAll)
    .def_readwrite("storeApertureImpactsHistograms",&GMAD::OptionsBase::storeApertureImpactsHistograms)
    .def_readwrite("apertureImpactsMinimumKE",&GMAD::OptionsBase::apertureImpactsMinimumKE)
    .def_readwrite("storeCavityInfo",&GMAD::OptionsBase::storeCavityInfo)
    .def_readwrite("storeCollimatorInfo",&GMAD::OptionsBase::storeCollimatorInfo)
    .def_readwrite("storeCollimatorHits",&GMAD::OptionsBase::storeCollimatorHits)
    .def_readwrite("storeCollimatorHitsLinks",&GMAD::OptionsBase::storeCollimatorHitsLinks)
    .def_readwrite("storeCollimatorHitsIons",&GMAD::OptionsBase::storeCollimatorHitsIons)
    .def_readwrite("storeCollimatorHitsAll",&GMAD::OptionsBase::storeCollimatorHitsAll)
    .def_readwrite("collimatorHitsMinimumKE",&GMAD::OptionsBase::collimatorHitsMinimumKE)
    .def_readwrite("storeEloss",&GMAD::OptionsBase::storeEloss)
    .def_readwrite("storeElossHistograms",&GMAD::OptionsBase::storeElossHistograms)
    .def_readwrite("storeElossVacuum",&GMAD::OptionsBase::storeElossVacuum)
    .def_readwrite("storeElossVacuumHistograms",&GMAD::OptionsBase::storeElossVacuumHistograms)
    .def_readwrite("storeElossTunnel",&GMAD::OptionsBase::storeElossTunnel)
    .def_readwrite("storeElossTunnelHistograms",&GMAD::OptionsBase::storeElossTunnelHistograms)
    .def_readwrite("storeElossWorld",&GMAD::OptionsBase::storeElossWorld)
    .def_readwrite("storeElossWorldContents",&GMAD::OptionsBase::storeElossWorldContents)
    .def_readwrite("storeElossTurn",&GMAD::OptionsBase::storeElossTurn)
    .def_readwrite("storeElossLinks",&GMAD::OptionsBase::storeElossLinks)
    .def_readwrite("storeElossLocal",&GMAD::OptionsBase::storeElossLocal)
    .def_readwrite("storeElossGlobal",&GMAD::OptionsBase::storeElossGlobal)
    .def_readwrite("storeElossTime",&GMAD::OptionsBase::storeElossTime)
    .def_readwrite("storeElossStepLength",&GMAD::OptionsBase::storeElossStepLength)
    .def_readwrite("storeElossPreStepKineticEnergy",&GMAD::OptionsBase::storeElossPreStepKineticEnergy)
    .def_readwrite("storeElossModelID",&GMAD::OptionsBase::storeElossModelID)
    .def_readwrite("storeElossPhysicsProcesses",&GMAD::OptionsBase::storeElossPhysicsProcesses)
    .def_readwrite("storeParticleData",&GMAD::OptionsBase::storeParticleData)
    .def_readwrite("storePrimaries",&GMAD::OptionsBase::storePrimaries)
    .def_readwrite("storePrimaryHistograms",&GMAD::OptionsBase::storePrimaryHistograms)

    .def_readwrite("storeTrajectory",&GMAD::OptionsBase::storeTrajectory)

    .def_readwrite("storeTrajectoryDepth",&GMAD::OptionsBase::storeTrajectoryDepth)
    .def_readwrite("storeTrajectoryStepPoints",&GMAD::OptionsBase::storeTrajectoryStepPoints)
    .def_readwrite("storeTrajectoryStepPointLast",&GMAD::OptionsBase::storeTrajectoryStepPointLast)
    .def_readwrite("storeTrajectoryParticle",&GMAD::OptionsBase::storeTrajectoryParticle)
    .def_readwrite("storeTrajectoryParticleID",&GMAD::OptionsBase::storeTrajectoryParticleID)
    .def_readwrite("storeTrajectorySecondaryParticles",&GMAD::OptionsBase::storeTrajectorySecondaryParticles)
    .def_readwrite("storeTrajectoryEnergyThreshold",&GMAD::OptionsBase::storeTrajectoryEnergyThreshold)
    .def_readwrite("storeTrajectorySamplerID",&GMAD::OptionsBase::storeTrajectorySamplerID)
    .def_readwrite("storeTrajectoryELossSRange",&GMAD::OptionsBase::storeTrajectoryELossSRange)

    .def_readwrite("storeTrajectoryTransportationSteps",&GMAD::OptionsBase::storeTrajectoryTransportationSteps)
    .def_readwrite("trajNoTransportation",&GMAD::OptionsBase::trajNoTransportation)
    .def_readwrite("storeTrajectoryKineticEnergy",&GMAD::OptionsBase::storeTrajectoryKineticEnergy)
    .def_readwrite("storeTrajectoryMomentumVector",&GMAD::OptionsBase::storeTrajectoryMomentumVector)
    .def_readwrite("storeTrajectoryProcesses",&GMAD::OptionsBase::storeTrajectoryProcesses).def_readwrite("storeTrajectoryTime",&GMAD::OptionsBase::storeTrajectoryTime)
    .def_readwrite("storeTrajectoryLocal",&GMAD::OptionsBase::storeTrajectoryLocal)
    .def_readwrite("storeTrajectoryLinks",&GMAD::OptionsBase::storeTrajectoryLinks)
    .def_readwrite("storeTrajectoryIon",&GMAD::OptionsBase::storeTrajectoryIon)
    .def_readwrite("storeTrajectoryMaterial",&GMAD::OptionsBase::storeTrajectoryMaterial)
    .def_readwrite("storeTrajectoryAllVariables",&GMAD::OptionsBase::storeTrajectoryAllVariables)

    .def_readwrite("trajectoryFilterLogicAND",&GMAD::OptionsBase::trajectoryFilterLogicAND)

    .def_readwrite("storeSamplerAll",&GMAD::OptionsBase::storeSamplerAll)
    .def_readwrite("storeSamplerPolarCoords",&GMAD::OptionsBase::storeSamplerPolarCoords)
    .def_readwrite("storeSamplerCharge",&GMAD::OptionsBase::storeSamplerCharge)
    .def_readwrite("storeSamplerKineticEnergy",&GMAD::OptionsBase::storeSamplerKineticEnergy)
    .def_readwrite("storeSamplerMass",&GMAD::OptionsBase::storeSamplerMass)
    .def_readwrite("storeSamplerRigidity",&GMAD::OptionsBase::storeSamplerRigidity)
    .def_readwrite("storeSamplerIon",&GMAD::OptionsBase::storeSamplerIon)

    .def_readwrite("trajCutGTZ",&GMAD::OptionsBase::trajCutGTZ)
    .def_readwrite("trajCutLTR",&GMAD::OptionsBase::trajCutLTR)
    .def_readwrite("trajConnect",&GMAD::OptionsBase::trajConnect)

    .def_readwrite("storeModel",&GMAD::OptionsBase::storeModel)

    .def_readwrite("samplersSplitLevel",&GMAD::OptionsBase::samplersSplitLevel)
    .def_readwrite("modelSplitLevel",&GMAD::OptionsBase::modelSplitLevel)
    .def_readwrite("uprootCompatible",&GMAD::OptionsBase::uprootCompatible)

    .def_readwrite("nturns",&GMAD::OptionsBase::nturns)
    .def_readwrite("ptcOneTurnMapFileName",&GMAD::OptionsBase::ptcOneTurnMapFileName)

    .def_readwrite("printFractionEvents",&GMAD::OptionsBase::printFractionEvents)
    .def_readwrite("printFractionTurns",&GMAD::OptionsBase::printFractionTurns)
    .def_readwrite("printPhysicsProcesses",&GMAD::OptionsBase::printPhysicsProcesses)

    .def_readwrite("nSegmentsPerCircle",&GMAD::OptionsBase::nSegmentsPerCircle)

    .def_readwrite("nbinsx",&GMAD::OptionsBase::nbinsx)
    .def_readwrite("nbinsy",&GMAD::OptionsBase::nbinsy)
    .def_readwrite("nbinsz",&GMAD::OptionsBase::nbinsz)
    .def_readwrite("nbinse",&GMAD::OptionsBase::nbinse)
    .def_readwrite("xmin",&GMAD::OptionsBase::xmin)
    .def_readwrite("xmax",&GMAD::OptionsBase::xmax)
    .def_readwrite("ymin",&GMAD::OptionsBase::ymin)
    .def_readwrite("ymax",&GMAD::OptionsBase::ymax)
    .def_readwrite("zmin",&GMAD::OptionsBase::zmin)
    .def_readwrite("zmax",&GMAD::OptionsBase::zmax)
    .def_readwrite("emin",&GMAD::OptionsBase::emin)
    .def_readwrite("emax",&GMAD::OptionsBase::emax)
    .def_readwrite("useScoringMap",&GMAD::OptionsBase::useScoringMap)

    .def("print",&GMAD::OptionsBase::print);

  py::class_<GMAD::Published<GMAD::OptionsBase>>(m, "PublishedOptions")
    .def("NameExists",&GMAD::Options::NameExists);

  py::class_<GMAD::Options, GMAD::Published<GMAD::OptionsBase>, GMAD::OptionsBase>(m,"Options")
    .def(py::init<>())
    .def(py::init<GMAD::OptionsBase&>())
    .def("set_value",[](GMAD::Options &options,std::string name,std::string value) {options.set_value<std::string>(name,value, false);})
    .def("set_value",[](GMAD::Options &options,std::string name,int value) {options.set_value<int>(name,value, false);})
    .def("set_value",[](GMAD::Options &options,std::string name,bool value) {options.set_value<bool>(name,value, false);})
    .def("set_value",[](GMAD::Options &options,std::string name,long int value) {options.set_value<long int>(name,value, false);})
    .def("set_value",[](GMAD::Options &options,std::string name,double value) {options.set_value<double>(name,value, false);});
}
