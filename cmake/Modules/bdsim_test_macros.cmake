# Note:
# For all tests, you can add additional arguments to the command using the 
# variable TESTING_ARGS. This variable will be unset after each test is ran.

# simple_testing:
#  Tests a script and looks for the expression in standard out. Expression
#  can be set to "" if you do not want to require anything in the output.
#
# simple_testing_long:
#  The same as simple_testing but with the ctest label "LONG".
#
# simple_fail:
#  BDSIM test that expects bdsim to exit with a non-zero return value. Success of the
#  test is bdsim failing.
#
# compare_test: 
#  Run a script and afterwards require that file1 and file2 have equal output.
#  The file comparison is done using the python script cmake/compare_files.py
#  
# unit_test:
#  Run a binary which does unit testing. Only require that the binary returns 0
#  to define success or failure.

macro(_run_test test_name input_args)
  # convert input arguments into a cmake list to separate
  set(args ${input_args})
  separate_arguments(args)
  set(TESTING_ARGS ${TESTING_ARGS})
  separate_arguments(TESTING_ARGS)
  if (BDSIM_GENERATE_REGRESSION_DATA)
    list(APPEND TESTING_ARGS "--outfile=${BDSIM_REGRESSION_PREFIX}-${test_name}")
  else()
    if (NOT TESTING_ARGS MATCHES "--outfile") # outfile might already be defined by the test
      list(APPEND TESTING_ARGS "--outfile=${test_name}")
    endif()
  endif()

  add_test(NAME ${test_name} COMMAND ${bdsimBinary} ${TESTING_PERM_ARGS} ${args} ${TESTING_ARGS})
  # unset TESTING_ARGS so only used for this test
  unset(TESTING_ARGS)
endmacro()

# A simple macro which runs a script and looks for some defined
# string in stdout and fails if found:
macro(simple_testing test_name args expression)
    _run_test(${test_name} ${args})
    if(NOT "${expression}" STREQUAL "")
        set_tests_properties(${test_name} PROPERTIES FAIL_REGULAR_EXPRESSION "${expression}")
    endif()
endmacro()

# same macro but with a label of LONG to reduce syntax in tests
macro(simple_testing_long test_name args expression)
    _run_test(${test_name} ${args})
    if(NOT "${expression}" STREQUAL "")
        set_tests_properties(${test_name} PROPERTIES FAIL_REGULAR_EXPRESSION "${expression}")
    endif()
    set_tests_properties(${test_name} PROPERTIES LABELS LONG)
    set_tests_properties(${test_name} PROPERTIES TIMEOUT 1800)
endmacro()

macro(simple_fail test_name args)
    _run_test(${test_name} ${args})
    set_tests_properties(${test_name} PROPERTIES WILL_FAIL 1)
endmacro()

macro(simple_testing_w_string test_name args expression)
    _run_test(${test_name} ${args})
    set_tests_properties(${test_name} PROPERTIES PASS_REGULAR_EXPRESSION "${expression}")
endmacro()

# a macro that adds a simple test
# and then add a second test that compares two files
# (e.g. an output file and a reference file)
macro(compare_test test_name script file1 file2)
   simple_testing("${test_name}" ${script} "")
   find_package(PythonInterp)
   if (PYTHONINTERP_FOUND)
       add_test(${test_name}_CheckOutput ${PYTHON_EXECUTABLE}
       ${CMAKE_SOURCE_DIR}/cmake/compare_files.py
       ${file1} ${file2})
   else()
       add_test(${test_name}_CheckOutput ${CMAKE_COMMAND} -E compare_files ${file1} ${file2})
   endif()
   # this test depends on the running of the original test
   set_tests_properties(${test_name}_CheckOutput PROPERTIES DEPENDS ${test_name})
endmacro()

macro(unit_test test_name binary)
  add_test(NAME ${test_name} COMMAND ${binary})
endmacro()

# macro for tracking tester
macro(tracking_test test_name args)
  add_test(NAME ${test_name} COMMAND ./run_bdsimMadx.py ${args})
endmacro()


# long generation, analysis and comparison test
macro(generate_analyse_compare_long testName fileName analysisConfig)
  # generate
  set(TESTING_ARGS --outfile=${testName}_data)
  simple_testing_long(${testName}-LONG "--file=${fileName}" "")

  # analyse
  rebdsim_test_manual(${testName}-analysis-LONG ${analysisConfig} ${testName}_data.root ${testName}_ana.root)
  set_tests_properties(${testName}-analysis-LONG PROPERTIES DEPENDS ${testName}-LONG)
  set_tests_properties(${testName}-analysis-LONG PROPERTIES LABELS LONG)

  # compare to reference
  comparator_test(${testName}-comparison-LONG ${testName}_ana.root ${testName}_ref.root)
  set_tests_properties(${testName}-comparison-LONG PROPERTIES DEPENDS ${testName}-analysis)
  set_tests_properties(${testName}-comparison-LONG PROPERTIES LABELS LONG)
endmacro()