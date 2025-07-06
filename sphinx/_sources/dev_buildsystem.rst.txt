.. _dev-buildandtesting:

Build System & Testing
**********************

Build System
============

The build system is based on CMake. The build options and variables are described in the user's
manual. Some additional options for developers:

**Deprecated**

 * make dist: This archives the git repository in a file bdsim-0.9.develop.tar.bz2
 * make copy-deps: This copies all dependent libraries to the build/devs/ directory
 * BUILD_MACOSX_APP: CMake variable that builds an MacOS application at installation time

   
Test System
===========

The test system is based on CTest, which works in combination with CMake. Tests are added to the
:code:`CMakeLists.txt` file beside each example declaring the main gmad file as well as any
optional command line arguments.

Useful ctest commands (can all be combined):

* ctest -N : print tests without execution
* ctest -R <regexp> : execute tests that match regexp
* ctest -LE LONG : execute tests that are not LONG (more than about a minute)
* ctest -I 5,5 : only execute test 5
* ctest -VV : print output to screen

For example, to find the test command for the sextupole component test::

  ctest -R Component-Sextupole -VV -N

  
Testing and Continuous Integration
==================================

Testing is done via continuous integration on the Github website. It is initiated by making a pull
request, at which point the website will automatically run all tests for a variety of operating
systems and Geant4 versions and report back. Tests must pass for a pull request to be accepted.


Package System
==============

The package system is based on CPack, which works in combination with CMake.
One can package the binaries and libraries with :code:`cpack`. A packed .tar.gz and a Mac OS .dmg is created.
The source can be packed with :code:`make dist`.
It is recommended to pack the libraries after each release version and put the binaries and libraries on the `BDSIM Download page <https://twiki.ph.rhul.ac.uk/twiki/bin/view/PP/JAI/BDsimDownload>`_.


Static Code Analysis
====================

The code of BDSIM is analysed by the static analysis tool Coverity. Its analysis is available with CERN credentials at `CERN's Coverity site <https://coverity.cern.ch>`_.
