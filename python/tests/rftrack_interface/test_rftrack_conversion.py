"""Fast conversion tests that do not initialise Geant4 or a tracker."""

import unittest as _unittest

from rftrack_test_helpers import (
    AssertConversionMatchesReference,
    AssertConversionStructure,
    ConvertBunch,
    CreateDeterministicBunch,
    NP,
    PROTON_PDG_ID,
    RFTrack,
)


class RFTrackConversionTests(_unittest.TestCase):
    """Exercise the standalone RF-Track bunch conversion API."""

    def test_bunch6d_and_bunch6dt_coordinates(self):
        """Both supported bunch types must match the reference conversion."""
        bunch6d = CreateDeterministicBunch()
        for bunch in (bunch6d, RFTrack.Bunch6dT(bunch6d)):
            with self.subTest(bunchType=type(bunch).__name__):
                result = ConvertBunch(bunch)
                AssertConversionStructure(self, bunch, result)
                AssertConversionMatchesReference(self, bunch, result)

    def test_fallback_pdg_does_not_modify_input_bunch(self):
        """Fallback PDGs belong to the output and must not mutate RF-Track."""
        bunch = CreateDeterministicBunch()
        for index in range(int(bunch.size())):
            bunch.get_particle(index).pdg_id = 0

        result = ConvertBunch(bunch)

        NP.testing.assert_array_equal(
            result["pdg_ids"],
            NP.full(int(bunch.size()), PROTON_PDG_ID, dtype=NP.int64),
        )
        inputPDGs = NP.array(
            [
                int(bunch.get_particle(index).pdg_id)
                for index in range(int(bunch.size()))
            ],
            dtype=NP.int64,
        )
        NP.testing.assert_array_equal(inputPDGs, 0)

    def test_zero_fallback_pdg_is_rejected(self):
        """A zero fallback PDG is invalid even if the bunch is otherwise valid."""
        with self.assertRaisesRegex(ValueError, "pdg_id must be non-zero"):
            ConvertBunch(CreateDeterministicBunch(), pdgID=0)

    def test_duplicate_particle_ids_are_rejected(self):
        """The correspondence table requires unique external particle IDs."""
        bunch = CreateDeterministicBunch()
        duplicateID = int(bunch.get_particle(0).id)
        bunch.get_particle(1).id = duplicateID

        with self.assertRaisesRegex(RuntimeError, "IDs are not unique"):
            ConvertBunch(bunch)

    def test_non_finite_momentum_is_rejected(self):
        """The converter must reject a non-finite RF-Track momentum."""
        bunch = CreateDeterministicBunch()
        bunch.get_particle(0).Pc = NP.nan

        with self.assertRaisesRegex(RuntimeError, "momentum"):
            ConvertBunch(bunch)

    def test_unsupported_python_object_is_rejected(self):
        """Objects outside the two supported RF-Track types must be rejected."""
        with self.assertRaisesRegex(
            (TypeError, ValueError),
            "Expected an RF-Track Bunch6d or Bunch6dT",
        ):
            ConvertBunch(object())


if __name__ == "__main__":
    _unittest.main(verbosity=2)
