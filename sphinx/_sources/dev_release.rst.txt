.. _dev-release:

Release Checklist
*****************

Things to update immediately before a release  (i.e. from a release candidate branch):

#. README - update at the top and the version history.
#. CMakeLists.txt - change major, minor and patch version at the very top.
#. For each submodule:

    * Follow release procedure for PyPi: http://www.pp.rhul.ac.uk/bdsim/pybdsim/developer.html#release-checklist
    * Upload to testpypi.
    * Upload to pypi.
    * Update html manual on website.

#. Update version number of each Python package in version history only.
#. Check data versions and update samples:

   * If the data format has changed increment the data version in each output class header in the ClassDef() at the bottom.
   * Check data version in :code:`configuration/BDSVersionData.hh`.
   * After updating the build, go to :code:`<bdsim-build-dir>/configuration` and manually edit BDSVersion.hh to
     be the new version (e.g. v1.7.0) then make install. You may have to touch
     :code:`<bdsim-dir>/src/BDSOutputROOTEventHeader.cc`. Then the data samples will have the right
     version number in their header.bdsimVersion (rather than previous-version-dirty). If ccmake
     is run though, this will be overwritten.
   * Also check :code:`<bdsim-build-dir>/src/BDSExecOptions.cc` for the "versionGit" string.
   * Regenerate data samples in :code:`examples/features/data/` using regenerateSamples.sh
   * Regenerate data sample :code:`examples/features/beam/userfile/userfile-sample.root` using regenerateUserFileSample.sh.
   * Regenerate data sample :code:`examples/features/beam/ptc/ptc-sample.root` using regeneratePtcSample.sh.


#. Update version history (including Python utilities and data versions) in
   manual source.

   * Python utility versions.
   * Data version.
   * Data class versions.


#. If BDSColours has changed, run BDSIM with DEBUGOUTPUT build and copy print out of
   colours to manual (already in correct format) - model_customisation.rst : Colours.
#. Regenerate BDSIM manual (pdf and html) and check the version number then commit the new
   pdf version. Upload html version to website. May have to re-run cmake to update version number.
   If the manual won't copy to the build directory it's because there is a temporary file starting
   with a `#` is present in the source directory.
#. Merge release candidate branch back into develop.
#. In develop, put back README, CMakeLists.txt to new version.develop.
#. Merge release candidate branch into master then delete. (:code:`git checkout master; git merge --no-ff v1.X.0-rc`)
#. Check all tests complete locally given merge before pushing.
#. Tag master branch for version number.


Change Of Year or Licence
*************************

#. Update LICENCE.txt in bdsim root directory.
#. From BDSIM root directory, :code:`source utils/updatelicence.sh`
#. Reset :code:`BDSEmStandardPhysicsOp4Channelling.hh` and :code:`BDSEmStandardPhysicsOp4Channelling.cc`.

Then it should be safe to commit the hundreds of file changes in one go.
