# AGENTS.md

Guidance for AI coding agents working in this repository. For human
contributors, see [CONTRIBUTING.md](CONTRIBUTING.md).

## Project overview

BDSIM is a C++ package for simulating accelerators using Geant4. Most of the source code lives in
`src/`, `analysis/`, `convert/`, `interpolator/`, `parser/`, `python/` . Tests in `examples/` and `test/`, 
Documentation source in `manual/`.

| Contents                                        | Source directory  |
|-------------------------------------------------|-------------------|
| Main Geant4 dependent C++ code                  | `src/`            |
| Main Geant4 dependent C++ headers               | `include/`        |
| ROOT analysis code                              | `analysis/`       |
| Comparisons between output C++ code             | `comparator/`     |
| Interpolator C++ code                           | `comparator/`     |
| Converter utilities C++ code                    | `convert/`        |
| Examples and ctest tests                        | `examples/`       |
| Documentation source sphinx                     | `manual/`         |
| External or specific extension modules C++ code | `ext/`            |
| Python bindings with pybind11                   | `python/`         |
| Parser code to read gmad files                  | `parser/`         |
| External dependency source                      | `src-external`    |
| Assorted utilities                              | `utils/`          |

## Build

```bash
mkdir agent_build
mkdir agent_install/
cd agent_build
cmake -DCMAKE_INSTALL_PREFIX=../agent_install
make -j$(nproc) install
```

## Commands

| Task | Command  |
| --- |----------|
| Run all tests | `ctest -j$(nproc)` |

## Code style

- Detailed style rules are found [here](https://bdsim-collaboration.github.io/bdsim/sphinx/dev_styleguide.html)
- Prefer small, focused functions. Match the patterns of surrounding code.

## Testing

- Most of the tests can be found in `examples` and `test`
- Every behaviour change needs a test in `examples` using a gmad file and corresponding ctest
- Only tests which link against BDSIM should be put in `test`
- Do not write tests which test the pybind11 binding code

## Boundaries

**Always**
- Keep changes minimal and scoped to the task.
- Update manual for user-visible changes.
- Document changes in `manual/source/version_history.rst`
- Add to the manual if a new feature is added or a new option is introduced.

**Ask first**
- Adding or upgrading dependencies
- Changing public APIs or file formats
- Large refactors or file moves

**Never**
- Commit secrets, credentials, or large data files
- Force-push, rewrite history, or push directly to `master`

## Gotchas

- 

