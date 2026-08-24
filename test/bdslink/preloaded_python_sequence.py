"""Track through two independently selected, preloaded Python elements."""

from __future__ import annotations

import numpy as np
import xpart as xp

import bdsim
from preloaded_python_element import (
    KINETIC_ENERGY_MEV,
    REFERENCE_MOMENTUM_EV,
    TOTAL_ENERGY_GEV,
)


parser = bdsim.BDSParser()
beam = parser.GetBeam()
beam.set_value("particle", "proton")
beam.set_value("energy", TOTAL_ENERGY_GEV)

for name, element_type, parameters in (
    ("d1", bdsim.ElementType.DRIFT, {}),
    ("q1", bdsim.ElementType.QUAD, {"k1": 0.1}),
):
    element = bdsim.Element()
    element.name = name
    element.type = element_type
    element.set_value("l", 1.0)
    element.set_value("aper1", 0.05)
    for parameter, value in parameters.items():
        element.set_value(parameter, value)
    parser.GetBeamline().push_back(element, False, "element")

tracker = bdsim.BDSLinkTrackerInterface.GetInstance(
    parser, 2212, KINETIC_ENERGY_MEV, 0.01, 1234, 1, True, True
)
link = tracker.GetBDSIMLink()
indices = [link.GetLinkIndex(name) for name in ("d1", "q1")]
if indices[0] == indices[1]:
    raise RuntimeError(f"elements received duplicate link IDs: {indices}")

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
for index, name in zip(indices, ("d1", "q1"), strict=True):
    tracker.TrackXSuite(index, name, particles, KINETIC_ENERGY_MEV)

coordinates = np.array(
    [particles.x[0], particles.px[0], particles.y[0], particles.py[0]]
)
if particles.state[0] <= 0 or not np.all(np.isfinite(coordinates)):
    raise RuntimeError(f"invalid sequence output: {coordinates}")
if not np.isclose(particles.s[0], 2.0):
    raise RuntimeError(f"wrong accumulated length: {particles.s[0]}")
print(f"sequence: PASS; indices={indices}; output={coordinates}")
