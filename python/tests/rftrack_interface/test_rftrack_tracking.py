"""Deterministic vacuum-drift tests for the RF-Track/BDSIM interface."""

import unittest as _unittest

from rftrack_test_helpers import (
    AddDrift,
    ConvertBunch,
    CreateDeterministicBunch,
    CreateTracker,
    DRIFT_LENGTH_M,
    NP,
    PROTON_PDG_ID,
    RFTrack,
    SAMPLER_ARRAY_KEYS,
)


class RFTrackTrackingTests(_unittest.TestCase):
    """Exercise direct loading and in-place RF-Track drift tracking."""

    @classmethod
    def setUpClass(cls):
        """Initialise one tracker and one drift for this test process."""
        cls.tracker = CreateTracker()
        cls.link = cls.tracker.GetBDSIMLink()
        cls.driftIndex = AddDrift(cls.link)

    def setUp(self):
        """Start every test without transient particles or sampler hits."""
        self.tracker.ClearData()

    def tearDown(self):
        """Ensure a failing test cannot contaminate the next test."""
        self.tracker.ClearData()

    def test_direct_load_and_beam_on(self):
        """The low-level bulk-load path must deliver every primary to the sampler."""
        bunch = CreateDeterministicBunch()
        expectedCount = int(bunch.size())

        loadedCount = self.tracker._LoadRFTrackBunch(
            bunch,
            pdg_id=PROTON_PDG_ID,
        )
        self.link.SelectLinkElement(self.driftIndex)
        self.link.BeamOn(loadedCount)

        self.assertEqual(loadedCount, expectedCount)
        self.assertEqual(int(self.link.SamplerHits().entries()), expectedCount)

    def test_bunch6dt_without_s_is_rejected_by_tracking(self):
        """TrackRFTrack requires the longitudinal S coordinate of a Bunch6d."""
        bunch6dt = RFTrack.Bunch6dT(CreateDeterministicBunch())
        self.assertFalse(hasattr(bunch6dt, "S"))

        with self.assertRaisesRegex(
            TypeError,
            "requires an RF-Track Bunch6d.*S coordinate",
        ):
            self.tracker.TrackRFTrack(self.driftIndex, bunch6dt)

    def test_drift_batch_mapping_and_reinjection(self):
        """Validate drift physics, metadata, IDs, PDGs, and in-place reinjection."""
        bunch = CreateDeterministicBunch()
        particleCount = int(bunch.size())
        initialSMetres = 1.25
        bunch.S = initialSMetres

        testIDs = 10000 + 37 * NP.arange(particleCount, dtype=NP.int64)
        for index, particleID in enumerate(testIDs):
            particle = bunch.get_particle(index)
            particle.id = int(particleID)
            particle.pdg_id = 0

        inputBatch = ConvertBunch(bunch)
        inputArrays = {
            key: NP.asarray(value)
            for key, value in inputBatch.items()
            if key != "rftrack_index_by_particle_id"
        }

        result = self.tracker.TrackRFTrack(self.driftIndex, bunch)
        self.assertIsNone(result)
        self.assertEqual(int(bunch.size()), particleCount)

        samplerBatch = self.tracker._GetSamplerParticleBatch()
        missingKeys = set(SAMPLER_ARRAY_KEYS) - set(samplerBatch)
        self.assertFalse(missingKeys, f"Missing sampler fields: {sorted(missingKeys)}")
        output = {
            key: NP.asarray(samplerBatch[key])
            for key in SAMPLER_ARRAY_KEYS
        }

        for key, values in output.items():
            self.assertEqual(values.shape, (particleCount,), key)
        for key in (
            "x_mm",
            "y_mm",
            "px_mevc",
            "py_mevc",
            "pz_mevc",
            "t_ns",
            "s_local_mm",
            "weights",
        ):
            self.assertTrue(NP.all(NP.isfinite(output[key])), key)

        NP.testing.assert_array_equal(output["particle_ids"], testIDs)
        NP.testing.assert_array_equal(output["parent_ids"], testIDs)
        NP.testing.assert_array_equal(output["pdg_ids"], PROTON_PDG_ID)
        NP.testing.assert_array_equal(output["track_ids"], 1)
        NP.testing.assert_array_equal(
            output["event_ids"],
            NP.arange(particleCount, dtype=output["event_ids"].dtype),
        )
        NP.testing.assert_allclose(output["weights"], 1.0, rtol=0.0, atol=0.0)

        for component in ("px_mevc", "py_mevc", "pz_mevc"):
            NP.testing.assert_allclose(
                output[component],
                inputArrays[component],
                rtol=1.0e-10,
                atol=1.0e-10,
            )

        driftLengthMm = 1000.0 * DRIFT_LENGTH_M
        expectedXMm = (
            inputArrays["x_mm"]
            + driftLengthMm * inputArrays["px_mevc"] / inputArrays["pz_mevc"]
        )
        expectedYMm = (
            inputArrays["y_mm"]
            + driftLengthMm * inputArrays["py_mevc"] / inputArrays["pz_mevc"]
        )
        NP.testing.assert_allclose(
            output["x_mm"], expectedXMm, rtol=1.0e-9, atol=1.0e-5
        )
        NP.testing.assert_allclose(
            output["y_mm"], expectedYMm, rtol=1.0e-9, atol=1.0e-5
        )

        indexByParticleID = {
            int(particleID): index
            for index, particleID in enumerate(testIDs)
        }
        for hitIndex, parentID in enumerate(output["parent_ids"]):
            bunchIndex = indexByParticleID[int(parentID)]
            particle = bunch.get_particle(bunchIndex)
            expectedXpMrad = (
                1.0e3 * output["px_mevc"][hitIndex] / output["pz_mevc"][hitIndex]
            )
            expectedYpMrad = (
                1.0e3 * output["py_mevc"][hitIndex] / output["pz_mevc"][hitIndex]
            )
            expectedMomentumMeVc = NP.sqrt(
                output["px_mevc"][hitIndex] ** 2
                + output["py_mevc"][hitIndex] ** 2
                + output["pz_mevc"][hitIndex] ** 2
            )

            self.assertEqual(int(particle.id), int(parentID))
            self.assertEqual(int(particle.pdg_id), PROTON_PDG_ID)
            self.assertTrue(NP.isnan(float(particle.S_lost)))
            NP.testing.assert_allclose(
                float(particle.x), output["x_mm"][hitIndex], rtol=0.0, atol=1.0e-12
            )
            NP.testing.assert_allclose(
                float(particle.y), output["y_mm"][hitIndex], rtol=0.0, atol=1.0e-12
            )
            NP.testing.assert_allclose(
                float(particle.xp), expectedXpMrad, rtol=1.0e-12, atol=1.0e-12
            )
            NP.testing.assert_allclose(
                float(particle.yp), expectedYpMrad, rtol=1.0e-12, atol=1.0e-12
            )
            NP.testing.assert_allclose(
                float(particle.Pc),
                expectedMomentumMeVc,
                rtol=1.0e-12,
                atol=1.0e-10,
            )

        NP.testing.assert_allclose(
            float(bunch.S),
            initialSMetres + DRIFT_LENGTH_M,
            rtol=0.0,
            atol=1.0e-12,
        )


if __name__ == "__main__":
    _unittest.main(verbosity=2)
