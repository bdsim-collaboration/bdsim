"""All-particles-lost test for the RF-Track/BDSIM interface."""

import unittest as _unittest

from rftrack_test_helpers import (
    AddDump,
    AddPartialLossCollimator,
    CreateDeterministicBunch,
    CreateTracker,
    DUMP_LENGTH_M,
    NP,
    SAMPLER_ARRAY_KEYS,
)


class RFTrackLossTests(_unittest.TestCase):
    """Verify an empty sampler batch and RF-Track loss positions."""

    @classmethod
    def setUpClass(cls):
        """Initialise one tracker and one absorbing element."""
        cls.tracker = CreateTracker()
        cls.link = cls.tracker.GetBDSIMLink()
        cls.dumpIndex = AddDump(cls.link)
        cls.partialLossIndex = AddPartialLossCollimator(cls.link)

    def setUp(self):
        """Start without transient particles or sampler hits."""
        self.tracker.ClearData()

    def tearDown(self):
        """Clear transient data even when an assertion fails."""
        self.tracker.ClearData()

    def test_all_particles_lost_returns_empty_sampler_batch(self):
        """An absorbing element must update loss positions without sampler hits."""
        bunch = CreateDeterministicBunch()
        particleCount = int(bunch.size())
        initialSMetres = 1.75
        expectedLossSMetres = initialSMetres + DUMP_LENGTH_M
        bunch.S = initialSMetres

        result = self.tracker.TrackRFTrack(self.dumpIndex, bunch)
        self.assertIsNone(result)
        self.assertEqual(int(bunch.size()), particleCount)
        self.assertEqual(int(self.link.SamplerHits().entries()), 0)

        samplerBatch = self.tracker._GetSamplerParticleBatch()
        missingKeys = set(SAMPLER_ARRAY_KEYS) - set(samplerBatch)
        self.assertFalse(missingKeys, f"Missing sampler fields: {sorted(missingKeys)}")
        for key in SAMPLER_ARRAY_KEYS:
            self.assertEqual(NP.asarray(samplerBatch[key]).size, 0, key)

        lossPositions = NP.array(
            [
                float(bunch.get_particle(index).S_lost)
                for index in range(particleCount)
            ],
            dtype=float,
        )
        self.assertTrue(NP.all(NP.isfinite(lossPositions)))
        NP.testing.assert_allclose(
            lossPositions,
            expectedLossSMetres,
            rtol=0.0,
            atol=1.0e-12,
        )
        NP.testing.assert_allclose(
            float(bunch.S),
            expectedLossSMetres,
            rtol=0.0,
            atol=1.0e-12,
        )


    def test_partial_particle_loss_preserves_survivor_mapping(self):
        """Non-consecutive losses must not corrupt the surviving RF-Track particles."""

        bunch = CreateDeterministicBunch()
        particleCount = int(bunch.size())

        initialSMetres = 1.75
        bunch.S = initialSMetres

        # Use non-sequential IDs and lose particles interleaved with survivors.
        particleIDs = 20000 + 43 * NP.arange(particleCount, dtype=NP.int64)
        lostIndices = {1, 3, 6}

        initialXByID = {}

        for index, particleID in enumerate(particleIDs):
            particle = bunch.get_particle(index)
            particle.id = int(particleID)

            if index in lostIndices:
                particle.x = 12.0  # mm: outside the +/-3 mm aperture

            initialXByID[int(particleID)] = float(particle.x)

        expectedSurvivorIDs = {
            int(particleIDs[index])
            for index in range(particleCount)
            if index not in lostIndices
        }

        expectedLostIDs = {
            int(particleIDs[index]) for index in lostIndices
        }

        # Track through the collimator.
        result = self.tracker.TrackRFTrack(self.partialLossIndex, bunch)

        self.assertIsNone(result)
        self.assertEqual(int(bunch.size()), particleCount)

        # Retrieve BDSIM sampler data.
        samplerBatch = self.tracker._GetSamplerParticleBatch()

        missingKeys = set(SAMPLER_ARRAY_KEYS) - set(samplerBatch)
        self.assertFalse(
            missingKeys,
            f"Missing sampler fields: {sorted(missingKeys)}",
        )

        output = {
            key: NP.asarray(samplerBatch[key])
            for key in SAMPLER_ARRAY_KEYS
        }

        for key, values in output.items():
            self.assertEqual(
                values.shape,
                (len(expectedSurvivorIDs),),
                key,
            )

        # Check surviving particle IDs.
        hitIDs = [int(value) for value in output["parent_ids"]]

        self.assertEqual(len(hitIDs), len(set(hitIDs)))
        self.assertEqual(set(hitIDs), expectedSurvivorIDs)

        NP.testing.assert_array_equal(
            output["particle_ids"],
            output["parent_ids"],
        )

        # The sampler order need not match the original bunch order.
        samplerIndexByID = {
            particleID: index
            for index, particleID in enumerate(hitIDs)
        }

        actualLostIDs = set()

        for index, particleID in enumerate(particleIDs):
            particleID = int(particleID)
            particle = bunch.get_particle(index)

            self.assertEqual(int(particle.id), particleID)

            lossS = float(particle.S_lost)

            if NP.isfinite(lossS):
                actualLostIDs.add(particleID)

                # S_lost currently represents the exit sampler position.
                NP.testing.assert_allclose(
                    lossS,
                    float(bunch.S),
                    atol=1e-12,
                    rtol=0,
                )

                # Lost particles retain their original transverse coordinates.
                NP.testing.assert_allclose(
                    float(particle.x),
                    initialXByID[particleID],
                    atol=1e-12,
                    rtol=0,
                )

            else:
                # A surviving particle must have S_lost = NaN.
                self.assertTrue(NP.isnan(lossS))
                self.assertIn(particleID, samplerIndexByID)

                hitIndex = samplerIndexByID[particleID]

                # Verify that the correct particle was updated.
                NP.testing.assert_allclose(
                    float(particle.x),
                    output["x_mm"][hitIndex],
                    atol=1e-12,
                    rtol=0,
                )

                NP.testing.assert_allclose(
                    float(particle.y),
                    output["y_mm"][hitIndex],
                    atol=1e-12,
                    rtol=0,
                )

                NP.testing.assert_allclose(
                    float(particle.xp),
                    1e3 * output["px_mevc"][hitIndex]
                    / output["pz_mevc"][hitIndex],
                    atol=1e-12,
                    rtol=1e-12,
                )

        # Exactly the intended three particles must be lost.
        self.assertEqual(actualLostIDs, expectedLostIDs)

        NP.testing.assert_allclose(
            float(bunch.S),
            initialSMetres + 0.05,
            rtol=0,
            atol=1e-12,
        )


if __name__ == "__main__":
    _unittest.main(verbosity=2)
