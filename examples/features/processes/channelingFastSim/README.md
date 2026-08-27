# Geant4 channeling validation cases

These five BDSIM inputs reproduce the physics configurations of the Geant4
extended examples `channeling/ch0`, `ch1`, `ch2`, `ch3`, and `ch5`.

- `ch0.gmad` exercises the existing (legacy) `G4Channeling` implementation.
- `ch1.gmad` exercises FastSim channeling and Baier-Katkov radiation.
- `ch2.gmad` explicitly sets the FastSim, radiation, spectrum, and virtual
  collimator controls exposed by the parser.
- `ch3.gmad` exercises coherent pair production in an oriented W `<111>`
  crystal (Geant4 11.4 or newer).
- `ch5.gmad` is the hybrid positron-source configuration: an oriented
  W `<111>` radiator followed by an amorphous tungsten converter.

The checked-in values match the corresponding Geant4 examples.  Event counts
are deliberately one for continuous test-suite smoke testing; use the
command-line `--ngenerate` override for statistical comparisons.
