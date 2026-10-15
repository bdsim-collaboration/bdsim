"""Shared deterministic helpers for the RF-Track/BDSIM interface tests."""

import os as _os
import sys as _sys
from pathlib import Path as _Path

import numpy as _np


_buildPythonDirectory = _os.environ.get("BDSIM_PYTHON_BUILD_DIR")
if _buildPythonDirectory:
    _sys.path.insert(0, _buildPythonDirectory)
    # Import return-value types before the interface that exposes them.
    import bdshitsampler as _bdshitsampler
    import bdshitsamplerlink as _bdshitsamplerlink
    import bdshitscollectionsamplerlink as _bdshitscollectionsamplerlink
    import bdsimlink as _bdsimlink
    import element as _element
    import elementtype as _elementtype
    import bdslinktrackerinterface as _bdslink
else:
    import bdsim as _bdsim

    _bdslink = _bdsim.bdslinktrackerinterface
    _element = _bdsim.element
    _elementtype = _bdsim.elementtype

import RF_Track as _rftrack


C_MM_PER_NS = 299.792458
PROTON_PDG_ID = 2212
REFERENCE_KINETIC_ENERGY_MEV = 15.0
DRIFT_LENGTH_M = 0.10
DUMP_LENGTH_M = 0.01
TEST_DIRECTORY = _Path(__file__).resolve().parent
TRACKER_CONFIGURATION = TEST_DIRECTORY / "tracker-interface.gmad"

CONVERSION_FLOAT_KEYS = (
    "x_mm",
    "y_mm",
    "px_mevc",
    "py_mevc",
    "pz_mevc",
    "t_ns",
)
CONVERSION_ARRAY_KEYS = CONVERSION_FLOAT_KEYS + (
    "s_mm",
    "particle_ids",
    "pdg_ids",
)
CONVERSION_MAPPING_KEY = "rftrack_index_by_particle_id"
SAMPLER_ARRAY_KEYS = (
    "x_mm",
    "y_mm",
    "px_mevc",
    "py_mevc",
    "pz_mevc",
    "t_ns",
    "s_local_mm",
    "particle_ids",
    "pdg_ids",
    "parent_ids",
    "track_ids",
    "event_ids",
    "weights",
)


def MomentumFromKineticEnergy(kineticEnergyMeV):
    """Return momentum in MeV/c for a proton kinetic energy in MeV."""
    massMeVc2 = float(_rftrack.protonmass)
    totalEnergyMeV = _np.asarray(kineticEnergyMeV, dtype=float) + massMeVc2
    return _np.sqrt(
        _np.maximum(totalEnergyMeV**2 - massMeVc2**2, 0.0)
    )


REFERENCE_MOMENTUM_MEV_C = float(
    MomentumFromKineticEnergy(REFERENCE_KINETIC_ENERGY_MEV)
)


def CreateDeterministicBunch():
    """Create a small Bunch6d with non-trivial deterministic coordinates."""
    momentumScale = _np.array(
        [1.0, 1.0005, 0.9995, 1.0008, 0.9992, 1.0002, 0.9998, 1.0],
        dtype=float,
    )
    coordinates = _np.column_stack(
        (
            _np.array([0.0, 0.2, -0.3, 0.4, -0.5, 0.1, -0.2, 0.3]),
            _np.array([0.0, 1.0, -0.7, 0.4, -1.2, 0.8, -0.3, 0.2]),
            _np.array([0.0, -0.1, 0.4, -0.3, 0.2, 0.5, -0.4, 0.1]),
            _np.array([0.0, -0.5, 0.8, -1.0, 0.6, -0.2, 0.3, -0.7]),
            _np.array([0.0, 100.0, -100.0, 50.0, -50.0, 200.0, -200.0, 25.0]),
            REFERENCE_MOMENTUM_MEV_C * momentumScale,
        )
    )
    return _rftrack.Bunch6d(
        float(_rftrack.protonmass),
        1.0e9,
        1.0,
        coordinates,
    )


def CreateEmptyBunch():
    """Create an empty Bunch6d for load-validation tests."""
    return _rftrack.Bunch6d(
        float(_rftrack.protonmass),
        1.0e9,
        1.0,
        _np.empty((0, 6), dtype=float),
    )


def ReferenceBDSIMCoordinates(bunch):
    """Convert native RF-Track coordinates to BDSIM interface units."""
    bunchType = type(bunch).__name__

    if bunchType == "Bunch6d":
        phase = _np.asarray(
            bunch.get_phase_space("%x %xp %y %yp %t %P", "all"),
            dtype=float,
        ).reshape(-1, 6)
        xMm, xpMrad, yMm, ypMrad, tMmC, pMeVc = phase.T
        xpRad = xpMrad * 1.0e-3
        ypRad = ypMrad * 1.0e-3
        pzMeVc = pMeVc / _np.sqrt(1.0 + xpRad**2 + ypRad**2)
        pxMeVc = xpRad * pzMeVc
        pyMeVc = ypRad * pzMeVc

    elif bunchType == "Bunch6dT":
        phase = _np.asarray(
            bunch.get_phase_space(),
            dtype=float,
        ).reshape(-1, 6)
        xMm, pxMeVc, yMm, pyMeVc, zMm, pzMeVc = phase.T
        t0MmC = _np.asarray(
            bunch.get_phase_space("%t0", "all"),
            dtype=float,
        ).reshape(-1)
        massesMeVc2 = _np.asarray(
            bunch.get_phase_space("%m", "all"),
            dtype=float,
        ).reshape(-1)
        pMeVc = _np.sqrt(pxMeVc**2 + pyMeVc**2 + pzMeVc**2)
        beta = pMeVc / _np.sqrt(pMeVc**2 + massesMeVc2**2)
        tMmC = t0MmC - zMm / beta

    else:
        raise TypeError(f"Unsupported RF-Track bunch type: {bunchType}")

    return {
        "x_mm": xMm,
        "y_mm": yMm,
        "px_mevc": pxMeVc,
        "py_mevc": pyMeVc,
        "pz_mevc": pzMeVc,
        "t_ns": tMmC / C_MM_PER_NS,
    }


def ConvertBunch(bunch, pdgID=PROTON_PDG_ID):
    """Call the standalone C++ RF-Track-to-BDSIM converter."""
    return _bdslink._ConvertRFTrackBunchToBDSLinkBatch(
        bunch,
        pdg_id=int(pdgID),
    )


def AssertConversionStructure(testCase, bunch, result):
    """Check the stable public structure and particle-ID mapping."""
    requiredKeys = set(CONVERSION_ARRAY_KEYS) | {CONVERSION_MAPPING_KEY}
    missingKeys = requiredKeys - set(result)
    testCase.assertFalse(missingKeys, f"Missing result fields: {sorted(missingKeys)}")

    arrays = {
        key: _np.asarray(result[key])
        for key in CONVERSION_ARRAY_KEYS
    }
    particleCount = int(bunch.size())

    for key, array in arrays.items():
        testCase.assertEqual(
            array.shape,
            (particleCount,),
            f"{key} has shape {array.shape}",
        )

    for key in CONVERSION_FLOAT_KEYS + ("s_mm",):
        testCase.assertTrue(
            _np.all(_np.isfinite(arrays[key])),
            f"{key} contains NaN or infinity",
        )

    receivedMapping = {
        int(particleID): int(index)
        for particleID, index in result[CONVERSION_MAPPING_KEY].items()
    }
    expectedMapping = {
        int(particleID): index
        for index, particleID in enumerate(arrays["particle_ids"])
    }
    testCase.assertEqual(receivedMapping, expectedMapping)
    _np.testing.assert_array_equal(
        arrays["pdg_ids"],
        _np.full(particleCount, PROTON_PDG_ID, dtype=_np.int64),
    )


def AssertConversionMatchesReference(testCase, bunch, result):
    """Compare the C++ conversion with independent NumPy calculations."""
    expected = ReferenceBDSIMCoordinates(bunch)
    for key in CONVERSION_FLOAT_KEYS:
        with testCase.subTest(coordinate=key):
            _np.testing.assert_allclose(
                _np.asarray(result[key], dtype=float),
                expected[key],
                rtol=1.0e-12,
                atol=1.0e-12,
            )


def CreateTracker():
    """Initialise the singleton tracker using the colocated GMAD input."""
    trackerClass = _bdslink.BDSLinkTrackerInterface
    tracker = trackerClass.GetInstance(
        bdsimConfigFileIn=str(TRACKER_CONFIGURATION),
        referenceParticlePDG=PROTON_PDG_ID,
        referenceKineticEnergy=REFERENCE_KINETIC_ENERGY_MEV,
        relativeEnergyCut=0.01,
        seed=1234,
        referenceIonCharge=1,
        batchMode=True,
        no_neutral_particles=True,
    )
    if tracker is None:
        raise RuntimeError("BDSLinkTrackerInterface.GetInstance returned None")
    return tracker


def AddDrift(link):
    """Add and return the index of the deterministic vacuum drift."""
    drift = _element.Element()
    drift.name = "rftrack_test_drift"
    drift.type = _elementtype.ElementType.DRIFT
    drift.set_value("l", DRIFT_LENGTH_M)
    index = int(link.AddLinkElement(drift))
    if index < 0:
        raise RuntimeError("Could not add RF-Track interface test drift")
    return index


def AddDump(link):
    """Add and return the index of an element that absorbs every particle."""
    dump = _element.Element()
    dump.name = "rftrack_test_dump"
    dump.type = _elementtype.ElementType.DUMP
    dump.set_value("l", DUMP_LENGTH_M)
    dump.set_value("horizontalWidth", 0.20)
    index = int(link.AddLinkElement(dump))
    if index < 0:
        raise RuntimeError("Could not add RF-Track interface test dump")
    return index


def AddPartialLossCollimator(link):
    """Add a rectangular tungsten collimator for deterministic partial loss."""
    collimator = _element.Element()
    collimator.name = "rftrack_test_partial_loss_rcol"
    collimator.type = _elementtype.ElementType.RCOL

    collimator.set_value("l", 0.05)              # m
    collimator.set_value("xsize", 0.003)         # horizontal half aperture, m
    collimator.set_value("ysize", 0.003)         # vertical half aperture, m
    collimator.set_value("horizontalWidth", 0.10)
    collimator.set_value("material", "W")

    index = int(link.AddLinkElement(collimator))
    if index < 0:
        raise RuntimeError("Could not add RF-Track partial-loss collimator")

    return index

RFTrack = _rftrack
BDSLink = _bdslink
NP = _np
