"""Smoke test for a BDSLink element preloaded through the Python parser.

Run this test in a fresh process: Geant4 and the BDSLink tracker are global
singletons and cannot be reliably reinitialised for another case in-process.
"""

from __future__ import annotations

import argparse
import math
from pathlib import Path
import subprocess
import sys

import bdsim
import numpy as np
import xpart as xp


PROTON_MASS_MEV = float(xp.PROTON_MASS_EV) * 1e-6
KINETIC_ENERGY_MEV = 100_000.0
TOTAL_ENERGY_GEV = (KINETIC_ENERGY_MEV + PROTON_MASS_MEV) * 1e-3
REFERENCE_MOMENTUM_EV = (
    math.sqrt(
        (KINETIC_ENERGY_MEV + PROTON_MASS_MEV) ** 2 - PROTON_MASS_MEV**2
    )
    * 1e6
)

NATIVE_CASES = {
    "drift": (bdsim.ElementType.DRIFT, {}),
    "quadrupole": (bdsim.ElementType.QUAD, {"k1": 0.1}),
    "sextupole": (bdsim.ElementType.SEXTUPOLE, {"k2": 0.1}),
    "octupole": (bdsim.ElementType.OCTUPOLE, {"k3": 0.1}),
    "decapole": (bdsim.ElementType.DECAPOLE, {"k4": 0.1}),
    "hkicker": (bdsim.ElementType.HKICKER, {"hkick": 1e-3}),
    "vkicker": (bdsim.ElementType.VKICKER, {"vkick": -2e-3}),
    "kicker": (
        bdsim.ElementType.KICKER,
        {"hkick": 1e-3, "vkick": -2e-3},
    ),
    "sbend": (bdsim.ElementType.SBEND, {"angle": 1e-2}),
    "rbend": (bdsim.ElementType.RBEND, {"angle": 1e-2}),
    "laserwire": (
        bdsim.ElementType.LASERWIRE,
        {"laserBeam": "probe_laser", "wireLength": 0.1},
    ),
}


def make_parser(
    case: str,
    field_map: Path | None,
    geometry: Path | None,
    field_type: str = "bmap2d",
):
    parser = bdsim.BDSParser()
    beam = parser.GetBeam()
    beam.set_value("particle", "proton")
    beam.set_value("energy", TOTAL_ENERGY_GEV)

    element = bdsim.Element()
    element.name = "probe"
    if case in NATIVE_CASES:
        element.type, parameters = NATIVE_CASES[case]
        element.set_value("l", 1.0)
        element.set_value("aper1", 0.05)
        for name, value in parameters.items():
            element.set_value(name, value)
        if case == "laserwire":
            laser = parser.GetGlobal_Laser()
            for name, value in {
                "name": "probe_laser",
                "wavelength": 633e-9,
                "m2": 1.3,
                "pulseEnergy": 0.013,
                "pulseDuration": 1.3e-9,
                "w0": 23.5e-6,
                "sigma0": 12e-6,
            }.items():
                laser.set_value(name, value)
            parser.Add_Laser(False, "laser")
    else:
        if field_map is None or geometry is None:
            raise ValueError("field-map case requires --field-map and --geometry")
        field = parser.GetGlobal_Field()
        field.set_value("name", "probe_map")
        field.set_value("type", field_type)
        field.set_value("integrator", "g4classicalrk4")
        dimension = "3d" if field_type == "bmap3d" else "2d"
        field.set_value("magneticFile", f"bdsim{dimension}:{field_map.resolve()}")
        field.set_value("magneticInterpolator", "linear")
        if field_type == "bmap2d":
            field.set_value("angle", -math.pi / 2)
            field.set_value("axisAngle", True)
            field.set_value("axisZ", 1.0)
            field.set_value("bScaling", 1.6)
            field.set_value("magneticReflection", "reflectxyquadrupole")
        parser.Add_Field(False, "field")

        element.type = bdsim.ElementType.ELEMENT
        element.set_value("l", 5.0)
        element.set_value("aper1", math.sqrt(0.5))
        element.set_value("tilt", -math.pi / 2)
        element.set_value("geometryFile", f"gdml:{geometry.resolve()}")
        element.set_value("fieldAll", "probe_map")

    # pybind11 does not expose the C++ defaults for this overload correctly.
    parser.GetBeamline().push_back(element, False, "element")
    return parser


def main():
    argument_parser = argparse.ArgumentParser()
    cases = (*NATIVE_CASES, "field-map", "all")
    argument_parser.add_argument("case", choices=cases)
    argument_parser.add_argument("--field-map", type=Path)
    argument_parser.add_argument("--geometry", type=Path)
    argument_parser.add_argument(
        "--field-type", choices=("bmap2d", "bmap3d"), default="bmap2d"
    )
    arguments = argument_parser.parse_args()

    if arguments.case == "all":
        failures = 0
        for case in (*NATIVE_CASES, "field-map"):
            command = [sys.executable, str(Path(__file__).resolve()), case]
            if case == "field-map":
                if arguments.field_map is None or arguments.geometry is None:
                    argument_parser.error(
                        "all requires --field-map and --geometry for the field-map case"
                    )
                command.extend(
                    [
                        "--field-map",
                        str(arguments.field_map),
                        "--geometry",
                        str(arguments.geometry),
                    ]
                )
            failures += subprocess.run(command, check=False).returncode != 0
        raise SystemExit(1 if failures else 0)

    parser = make_parser(
        arguments.case,
        arguments.field_map,
        arguments.geometry,
        arguments.field_type,
    )
    tracker = bdsim.BDSLinkTrackerInterface.GetInstance(
        parser,
        2212,
        KINETIC_ENERGY_MEV,
        0.01,
        1234,
        1,
        True,
        True,
    )
    link = tracker.GetBDSIMLink()
    index = link.GetLinkIndex("probe")

    particles = xp.Particles(
        _capacity=8,
        p0c=REFERENCE_MOMENTUM_EV,
        mass0=xp.PROTON_MASS_EV,
        q0=1,
        pdg_id=2212,
        x=[1e-3],
        px=[2e-4],
        y=[-2e-3],
        py=[-1e-4],
    )
    before = np.array(
        [particles.x[0], particles.px[0], particles.y[0], particles.py[0]]
    )
    tracker.TrackXSuite(index, "probe", particles, KINETIC_ENERGY_MEV)
    after = np.array(
        [particles.x[0], particles.px[0], particles.y[0], particles.py[0]]
    )

    if particles.state[0] <= 0 or not np.all(np.isfinite(after)):
        raise RuntimeError(f"invalid tracked particle: {after}, state={particles.state[0]}")
    if np.allclose(after, before, rtol=0, atol=1e-14):
        raise RuntimeError(f"{arguments.case} did not transport the particle")
    print(f"{arguments.case}: PASS; index={index}; before={before}; after={after}")


if __name__ == "__main__":
    main()
