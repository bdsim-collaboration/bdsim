.. _dev-release:

Release Checklist
*****************

Workflow to make a release from develop. Assume the previous version was X.Y.Z and the current status is
X.Y.Z.develop and the new version will be A.B.C.

#. Make a vX.Y.Z-rc release candidate branch off of develop. Must end in 'rc'. Check it out.
#. README.txt
   * ensure the copyright year year1 - year2 -> year2 is this year.
   * update in the General Information section the version number. (Normally, X.Y.Z.develop -> A.B.C)
#. CMakeLists.txt - change major, minor and patch version at the very top.
#. Edit manual/source/version_history.rst for this release. At the end of the release notes, write
   the version number of each of pybdsim, pymadx, pymad8, pytransport (from pypi.org) under "Utilities".
#. Check data class versions are correct:
   * Git compare each class BDSOutputROOTEvent*.hh between the last tag and master and the code in the current rc branch.
   * Specifically for BDSOutputROOTEventBeam and BDSOutputROOTEventOptions, compare the corresponding files
     of the parent classes parser/beamBase.h and parser/optionsBase.h have changed.
   * If the data members or order has changed since the last tag in master, increment by 1 the data version in
     each output class header in the ClassDef() at the bottom from the last number in the master version. This
     may have been partially completed by the developer during development.
   * Generate a table in manual/source/version_history.rst at the end of this version notes with the columns
     Class Name, Changed, Old Version, New Version with an entry for each output class (BDSOutputROOTEvent*).
     This can be automated with python manual/data_version_table.py and copying the print out into the manual.
#. Update the main data version number if any data class versions have been incremented. If the data is incremented,
   we must update the example data files.
   * If needed, increment the data version in the source tree :code:`configuration/BDSVersionData.hh`.
   * Make a commit and ensure the git working directory is clean (otherwise, we will get the 'dirty' flag in git).
   * Make a temporary build and install directory.
   * Configure a build of bdsim and set the install directory.
   * In the build tree, edit :code:`<bdsim-build-dir>/configuration/BDSVersion.hh` BDSIM_GIT_VERSION to
     be the new version (e.g. v1.8.0). Note, any use of cmake after this point will overwrite this back to
     the original text.
   * Compile and install the build.
   * Source :code:`<bdsim-install-dir>/bin/bdsim.sh` and then use the build in the following steps.
   * In the source tree:
     - Regenerate data samples in :code:`examples/features/data/` using regenerateSamples.sh
     - Regenerate data sample :code:`examples/features/beam/userfile/userfile-sample.root` using regenerateUserFileSample.sh.
     - Regenerate data sample :code:`examples/features/beam/ptc/ptc-sample.root` using regeneratePtcSample.sh.
     - Commit these new root files.
#. Update the colour list table in model_customisation.rst using :code:`bdsim --colours` output.
#. Merge release candidate branch back into develop.
   * From the rc branch create a develop-return branch and check it out.
   * In this branch, edit the version number in README to be A.B.C.develop.
   * In CMakeLists.txt edit the program version number to be A B C.develop.
   * Create a pull request from this branch into develop on github.
#. Merge release candidate branch into master then delete. (:code:`git checkout master; git merge --no-ff v1.X.0-rc`)
#. Check all tests complete locally given merge before pushing.
#. Tag master branch for version number.


Change Of Year or Licence
*************************

#. Update LICENCE.txt in bdsim root directory.
#. From BDSIM root directory, :code:`source utils/updatelicence.sh`
#. Reset :code:`BDSEmStandardPhysicsOp4Channelling.hh` and :code:`BDSEmStandardPhysicsOp4Channelling.cc`.

Then it should be safe to commit the hundreds of file changes in one go.
