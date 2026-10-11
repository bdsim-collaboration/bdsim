# RF-Track interface tests

This directory contains a test suite for the BDSIM
RF-Track Python interface.

The suite uses eight deterministic protons and is split into independent
processes:

- `test_rftrack_conversion.py`: standalone `Bunch6d` and `Bunch6dT`
  conversion, metadata, and invalid converter inputs. It does not initialise
  Geant4.
- `test_rftrack_validation.py`: bulk loading, clearing, empty bunches, PDGs,
  and non-finite particle data.
- `test_rftrack_tracking.py`: direct loading, vacuum-drift transport, sampler
  metadata, non-sequential IDs, PDG fallback, in-place reinjection, and the
  longitudinal `S` coordinate.
- `test_rftrack_loss.py`: all-particles-lost behaviour, empty sampler arrays,
  and `S_lost` propagation.

## Run locally

Use the Python interpreter that can import both the installed BDSIM bindings
and RF-Track:

```bash
cd python/tests/rftrack_interface
python test_rftrack_conversion.py
python test_rftrack_validation.py
python test_rftrack_tracking.py
python test_rftrack_loss.py
```

Run the files as separate commands. Each process gets a fresh BDSIM tracker
singleton and therefore cannot inherit beam-line or Geant4 state from another
test group.

To exercise bindings directly from a BDSIM build tree instead of an installed
package:

```bash
export BDSIM_PYTHON_BUILD_DIR=/path/to/bdsim-build/python
python test_rftrack_conversion.py
python test_rftrack_validation.py
python test_rftrack_tracking.py
python test_rftrack_loss.py
```

## CMake/CTest integration

`CMakeLists.txt` is an optional candidate for inclusion from a BDSIM source
tree. It registers four CTest tests only when Python bindings are enabled and
the configured Python interpreter can import `RF_Track` and `numpy`.

It expects the BDSIM binding targets to be built in `${CMAKE_BINARY_DIR}/python`.
After inclusion and configuration, run:

```bash
ctest -R interface-rftrack -VV
```

No generated output or reference-data files are required. These are interface
integration tests in BDSIM terminology, rather than reference-file regression
comparisons.
