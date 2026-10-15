"""Tracker loading and invalid-input tests for the RF-Track interface."""

import unittest as _unittest

from rftrack_test_helpers import (
    CreateDeterministicBunch,
    CreateEmptyBunch,
    CreateTracker,
    NP,
    PROTON_PDG_ID,
)


class RFTrackValidationTests(_unittest.TestCase):
    """Validate loading, clearing, PDGs, and invalid coordinates."""

    @classmethod
    def setUpClass(cls):
        """Initialise BDSIM once for this test process."""
        cls.tracker = CreateTracker()

    def setUp(self):
        """Start every test without transient particles or sampler hits."""
        self.tracker.ClearData()

    def tearDown(self):
        """Ensure a failing test cannot contaminate the next test."""
        self.tracker.ClearData()

    def test_repeated_load_does_not_accumulate_particles(self):
        """Loading a new bunch must clear particles from the previous load."""
        bunch = CreateDeterministicBunch()
        expectedCount = int(bunch.size())

        firstCount = self.tracker._LoadRFTrackBunch(
            bunch,
            pdg_id=PROTON_PDG_ID,
        )
        secondCount = self.tracker._LoadRFTrackBunch(
            bunch,
            pdg_id=PROTON_PDG_ID,
        )

        self.assertEqual(firstCount, expectedCount)
        self.assertEqual(secondCount, expectedCount)

    def test_empty_bunch_is_rejected(self):
        """Loading zero particles must fail before AddParticles is called."""
        with self.assertRaisesRegex(
            (ValueError, RuntimeError),
            "empty RF-Track bunch|phase space.*shape",
        ):
            self.tracker._LoadRFTrackBunch(
                CreateEmptyBunch(),
                pdg_id=PROTON_PDG_ID,
            )

    def test_unknown_explicit_and_fallback_pdgs_are_rejected(self):
        """Neither an explicit nor a fallback unknown PDG may be loaded."""
        unknownPDG = 123456789

        explicitBunch = CreateDeterministicBunch()
        for index in range(int(explicitBunch.size())):
            explicitBunch.get_particle(index).pdg_id = PROTON_PDG_ID
        explicitBunch.get_particle(0).pdg_id = unknownPDG

        with self.assertRaisesRegex(ValueError, str(unknownPDG)):
            self.tracker._LoadRFTrackBunch(
                explicitBunch,
                pdg_id=PROTON_PDG_ID,
            )

        fallbackBunch = CreateDeterministicBunch()
        for index in range(int(fallbackBunch.size())):
            fallbackBunch.get_particle(index).pdg_id = 0

        with self.assertRaisesRegex(ValueError, str(unknownPDG)):
            self.tracker._LoadRFTrackBunch(
                fallbackBunch,
                pdg_id=unknownPDG,
            )

    def test_non_finite_coordinate_and_momentum_are_rejected(self):
        """NaN coordinates and momenta must be rejected before tracking."""
        coordinateBunch = CreateDeterministicBunch()
        coordinateBunch.get_particle(0).x = NP.nan
        with self.assertRaisesRegex(ValueError, "non-finite x"):
            self.tracker._LoadRFTrackBunch(
                coordinateBunch,
                pdg_id=PROTON_PDG_ID,
            )

        momentumBunch = CreateDeterministicBunch()
        momentumBunch.get_particle(0).Pc = NP.nan
        with self.assertRaisesRegex((ValueError, RuntimeError), "momentum"):
            self.tracker._LoadRFTrackBunch(
                momentumBunch,
                pdg_id=PROTON_PDG_ID,
            )


if __name__ == "__main__":
    _unittest.main(verbosity=2)
