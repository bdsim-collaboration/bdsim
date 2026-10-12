# Contributing to BDSIM

Thanks for your interest in contributing! This guide explains how to report
problems, propose changes, and get your work merged.

By participating, you agree to follow our [Code of Conduct](CONDUCT.md).

## Ways to contribute

- **Report a bug** or unexpected behaviour
- **Request a feature** or suggest an improvement
- **Improve the docs**, examples, or tutorials
- **Fix an issue** (look for the `good first issue` and `help wanted` labels)
- **Help others** by answering questions in Discussions

You don't need to write code to contribute; documentation fixes are very welcome.

## Reporting bugs

Search [existing issues](https://github.com/bdsim-collaboration/bdsim/issues) first. If you can't find a match, open a
new issue and include:

1. What you expected to happen and what actually happened
2. A **minimal reproducible example** (input gmad file or analysis command)
3. Your environment: OS, BDSIM version, compiler version (python version if relevant), and how
   you installed it (conda, source, etc)
4. Full error output or logs

## Suggesting features

Open an issue describing the use case first, before writing code. Explain the
problem you're trying to solve, not just the solution. This avoids wasted effort
on changes that don't fit the project's direction.

## Development setup

```bash
git clone https://github.com/YOUR_GITBUB_USERNAME/bdsim.git
mkdir bdsim-build
mkdir bdsim-install
cd bdsim-build
cmake -DCMAKE_INSTALL_PREFIX=../bdsim-install ../bdsim  
make -j$(nproc) install 
```

Run the tests to confirm everything works:

```bash
ctest -j$(nproc)
```

## Making a change

1. **Fork** the repo and create a branch from `develop`:
   `git checkout -b fix-issue-123-description origin/develop`
2. Make your changes in small, focused commits.
3. Add or update **tests** found in `examples/`for any behaviour you change.
4. Update **documentation** and the `manual/source/version_history.rst` if the change is user-visible.
5. Run the checks locally (see above).
6. Push your branch and open a pull request.

## Pull request checklist

Before requesting review, make sure that:

- [ ] Tests pass locally and in CI
- [ ] New code is covered by tests (both in `examples/` and [regression](https://github.com/bdsim-collaboration/regression))
- [ ] Docs (`manual/source/`) and changelog (`manual/source/version_history.rst`) are updated
- [ ] The manual has been updated
- [ ] The PR description explains *what* changed and *why*, and links the issue
- [ ] The PR is focused on one thing (split unrelated changes)

### Code style

- The code style briefly is 
  - UpperCamelCase for class names and member functions 
  - lowerCamelCase for member variables
  - GNU curly braces for control structures
  - More details can be found in the online [developer documentation](https://bdsim-collaboration.github.io/bdsim/sphinx/dev_styleguide.html)

## Using AI tools and agents

We welcome contributions made with the help of AI coding tools (Claude Code,
Copilot, Cursor, and similar). You are fully responsible for anything you
submit, however it was produced. The rules below apply either way.

### Ground rules

- **Create an issue first.** Describe the problem, feature, update, the expected behaviour, and a minimal
  example. Don't submit a PR for a problem that hasn't been discussed.
- **You own the contribution.** Read and understand every line before opening a
  PR. If you can't explain a change in review, it isn't ready.
- **Test it yourself.** Run the build and test suite locally. Don't submit code
  that has only been "verified" by the agent saying it works.
- **Keep PRs small and focused.** Agents tend to over-reach with unrelated
  refactors, reformatting, and extra files. Trim the diff to what the issue
  needs.
- **Don't submit unreviewed bulk output.** Large, auto-generated PRs and
  drive-by "fixes" that nobody has checked will be closed.
- **No fabricated content.** Verify that APIs, function names, citations, and
  links an agent produced actually exist.

### Disclosure

If a substantial part of a PR was AI-generated, say so in the description
(e.g. "Drafted with Claude Code, reviewed and tested by me"). This isn't a
penalty. It helps reviewers know where to look more carefully. You can also add
an `Assisted-by:` trailer to commits:

Fix off-by-one error in beam line parser

Assisted-by: Claude Code


Routine autocomplete and spell-checking don't need to be disclosed.

### Configuring your agent

This repository includes an [AGENTS.md](AGENTS.md) with build commands, code
style, and boundaries. Point your tool at it before starting. If your agent
repeatedly makes the same mistake, improve `AGENTS.md` in a PR so everyone
benefits.

When running an agent locally:

- Work on a **branch in your fork**, never directly on `master` or `develop`.
- Use the **out-of-source build** described in `AGENTS.md`; don't let the agent
  configure CMake inside the source tree.
- **Review commands before approving them**, especially anything that deletes
  files, installs packages, or touches the network.
- Keep secrets and credentials out of the agent's reach.
- Don't let agents force-push or rewrite shared history.

### Licensing and provenance

By submitting a PR you confirm that you have the right to contribute it under
the project's [GPL-3.0-or-later](LICENCE.txt) license. AI tools can reproduce
existing code, so:

- Don't paste in code you suspect was copied from an incompatible or unknown
  source.
- Check that generated code doesn't include copyright headers or license
  text from other projects.

### Issues and discussions

- Don't post raw AI-generated bug reports. Reproduce the problem yourself and
  include a minimal example.
- Don't use agents to post comments or reviews on your behalf without reading
  them first.

### What maintainers may do

Maintainers may ask how a change was produced, request a simpler or smaller
version, or close PRs that appear to be unreviewed agent output. This is about
keeping review time sustainable, not about discouraging tool use.