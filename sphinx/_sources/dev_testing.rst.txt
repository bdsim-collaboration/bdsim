Testing
*******

Unit Tests
==========

* Unit tests test that the code runs with the chosen input and that a particular bit of code is executed
  without problem. They do not necessarily guarantee the correct result. e.g. a quadrupole unit test may
  track a particle through a quadrupole successfully (pass) but doesn't guarantee the action on the particle
  was correct.
* The unit tests make sure we have good code 'coverage' where we have an example that is regularly run
  that uses a certain bit of the code.
* Any **new feature** must have a test (or multiple) defined that exploits and demonstrates it fully.

BDSIM makes use of CTest which uses tests defined in the many CMakeLists.txt as part of the CMake build system.
Every example is also a unit test.

Running
-------

Recommended: ::

  ctest -LE LONG -j4

Generally, from a build directory of BDSIM, the following command can be run: ::

  ctest

This will run all available tests in the build. At the time of writing this is approximately 800 unit tests
and takes around 15 minutes on the developers computer to run with 6 threads.

Various additional options can be used: ::

  ctest -j4

Run with 4 threads.  Note, even if you have 4 cores and hyperthreading, empirically it has been found
that the tests will run more slowly than single threaded if close to the maximum of the computers. It
is recommended to only use the number of cores available as a maximum, i.e. ignore hyperthreading. ::

  ctest -LE LONG -j4

LE = label exclude. Exclude any tests with the 'label' (defined in the test macro) as 'LONG' (our BDSIM defined label). ::

  ctest -R mytest

Run tests matching "mytest" in the name (this can be a regular expression). ::

  ctest -R example-collimation-hits-minke -VV

Run tests matching the name "example-collimation-hits-minke" and show (verbosely) all screen out from the test.
In this case it is therefore prudent to not use :code:`-jN` and only run single threaded so if multiple tests
are matched they don't print out over each other at the same time.


Adding New Unit Tests
---------------------

* An input gmad file should be prepared that does the least possible work to show the desired
  feature.
* Extraneous options and number of events should be avoided.
* The simplest physics list should be used.
* The test should be as short as possible.
* Consider only 1 event, use the default options as much as possible in BDSIM (less input syntax).
* Try to make the test naming hierarchical, e.g. :code:`field-magnetic-quadrupole`. This makes testing
  everything to do with a field easy.
* Only use overlap checking if necessary - this is time consuming and we should only check one geometry
  combination once. This is largely covered already in the test suite.
* Adding data files to the repository should be avoided unless strictly necessary as this
  permanently inflates the size of the git repository. Specific BDSIM data examples are included
  in the :code:`examples/features/data` directory that can be used by any test.
* Remember, do not edit the copy of the input or test in the build directory as this will be overwritten
  then next time the build is run. Prefer to edit the original, then update the build.

Why so pedantic about this? With (currently) around 800 tests, being wasteful in individual tests
quickly adds up.  1s extra on each tests would add up to 13 minutes of extra execution time.

To add a test, add a line in the CMakeLists.txt beside that example. The syntax is: ::

  simple_testing(testname   "--file=inputfile.gmad"  "")

The last argument is any string to match in the print out from the test that should cause it to fail,
e.g. "overlap". :code:`${OVERLAP_CHECK}` defines a pre-made set of overlap keywords. It can be left
empty.

To add a purposively failing test, i.e. a test that the program should complain and exit, is: ::

  simple_fail(testname  "--file=inputfile.gmad" "")

.. note:: Test input files must be committed to the git repository to be picked up by the build
	  system and included in the build directory. This is done as the build system uses git
	  to only copy files that are defined in the repository, therefore avoiding copying the
	  typically polluted example directory when developing the code.


To add a test that passes by finding some text explicitly in the print out (i.e. a test that should
provide a warning but maybe not return 1 error code), use: ::

  simple_testing_w_string(testname  "--file=inputfile.gmad"  "regex_expression")


Steps:

1) Define new input file
2) Add to CMakeLists.txt
3) Commit input file(s) and updated CMakeLists.txt
4) Update build (:code:`ccmake .`, then :code:`<c>`, then :code:`<g>`, then :code:`make`)
5) :code:`ctest -R mynewntestname`
6) If test doesn't pass, fix (in source directory, not the build), repeat steps 4 and 5 (3 is not necessary this time)


Extending Test Macros
---------------------

Test macros are small functions that we have defined in our CMake input to not only add a test
to the CMake build system but also set some properties (such as labels). These are defined in: ::

  bdsim/cmake/Modules/bdsim_test_macros.cmake

Please review the already existing ones and extend if necessary.

.. note:: There are different test macros for various executables.



**Deprecated**

.. warning:: The following sections are kept for posterity and will shortly be removed.
          

Regression Testing
==================

Regression testing tests that the code reproduces a result it once did. When developing a new feature
or indeed the code itself, the developer should demonstrate the output (i.e. physics-wise output) is
correct. At this point they should create a reference result that can be used in future for regression
testing.

As the code is developed further, the implementation may change and therefore the regression testing
should be used to prove that the code achieves the same result. If it does not but is now correct, the
reference test result(s) should be updated.

The regression testing work with a separate package in a separate repository. A BDSIM build is made
with the advanced (i.e. hidden by default) CMake option :code:`BDSIM_GENERATE_REGRESSION_DATA` turned
on. This runs all the tests and prefixes all output files with a string as defined by the CMake option
:code:`BDSIM_REGRESSION_PREFIX`. This can be set to something like "ref" or "test".

The output files are gathered from the build after running all tests. Then the regression test package
is configured with respect two directories containing a set of reference files and test files. In this
build a CMake test is defined for every matching pair of files. Each test runs our own program for comparing
the data called the :code:`comparator`.
	  

Automated Testing
=================

At Royal Holloway, the testing is run hourly for short (i.e. non-LONG) tests if any new commits
are made in develop as well as the full test suite on different platforms for different branches
and versions of Geant4 nightly. As this is rather CPU intensive it is run on our own computing
resources rather than on say Bitbucket's continuous integration testing where we would quickly
exhaust the monthly free amount of CPU time.

The scripts to run the tests are kept in a separate repository: `<https://bitbucket.org/jairhul/bdsim-cdash>`_.
These scripts are based on using the EasyBuild set of environment modules for all the dependencies at
Royal Holloway. The tests are run with the following in whomever manages the test's crontab on
linappserv1. ::

  crontab -e

   * * * * /home/accsoft/cdash/bdsim-cdash/continuous.sh >> /scratch5/lnevay/cdash-builds/logs/crontab.log 2>&1
   0 2 * * * /home/accsoft/cdash/bdsim-cdash/nightly.sh >> /scratch5/lnevay/cdash-builds/logs/crontab.log 2>&1
   0 2 * * * /home/accsoft/cdash/bdsim-cdash/coverage.sh >> /scratch5/lnevay/cdash-builds/logs/crontab.log 2>&1

Each script submits one job on the Royal Holloway Faraday cluster per build and each build and test execution
runs single threaded taking typically 4 hours. 4 Gb of RAM has been found to be a safe amount for all testing
purposes.

On occasion if the farm load is high, the scheduler will accumulate these jobs due to the higher than average
memory requirement. In future the continuous jobs could be improved to initially submit with lower memory requirements
and resubmit if there are in fact changes to the repository with the necessary higher memory requirement.

CDash Website
-------------

The test output is reported to a CDash "dashboard" which is a website using a mysql (or free version mariadb) database.
This website is hosted on the (virtual machine) server :code:`jaiserv1.pp.rhul.ac.uk` that is only internally accessible
in Royal Holloway for security reasons.

To access the website it is convenient to use a tunnel. The following is added to the developer's bash profile: ::

  alias jaiserv="ssh -L 8080:jaiserv1.pp.rhul.ac.uk:80 lnevay@linappserv2.pp.rhul.ac.uk"

The command "jaiserv" can then be executed from any terminal. Then the following website can be accessed.

* `<http://localhost:8080/cdash/index.php?project=BDSIM&date=>`_

This shows an overview of all the test status as well as coverage.


.. note:: Due to the selinux security extensions the ability of the website to email people notifying them
	  of failed tests will work but then be broken when settings automatically reset after a week. This
	  has not been pursued to be fixed.


For administration of the jaiserv1 VM, contact the PP system administrators.
