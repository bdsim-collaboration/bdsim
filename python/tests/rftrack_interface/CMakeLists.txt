# Optional CTest registration for the RF-Track Python interface tests.
# This file is intended to be included from a BDSIM build with
# USE_PYTHON_BINDINGS enabled.

if(NOT USE_PYTHON_BINDINGS)
  message(STATUS "Skipping RF-Track interface tests: USE_PYTHON_BINDINGS is OFF")
  return()
endif()

if(NOT TARGET bdslinktrackerinterface)
  message(STATUS "Skipping RF-Track interface tests: binding target is unavailable")
  return()
endif()

if(NOT Python3_EXECUTABLE)
  find_package(Python3 COMPONENTS Interpreter REQUIRED)
endif()

execute_process(
  COMMAND "${Python3_EXECUTABLE}" -c "import RF_Track, numpy"
  RESULT_VARIABLE RFTRACK_TEST_DEPENDENCIES_RESULT
  OUTPUT_QUIET
  ERROR_QUIET
)

if(NOT RFTRACK_TEST_DEPENDENCIES_RESULT EQUAL 0)
  message(STATUS
    "Skipping RF-Track interface tests: RF_Track or numpy is unavailable "
    "to ${Python3_EXECUTABLE}"
  )
  return()
endif()

set(RFTRACK_TEST_FILES
  rftrack_test_helpers.py
  test_rftrack_conversion.py
  test_rftrack_validation.py
  test_rftrack_tracking.py
  test_rftrack_loss.py
  tracker-interface.gmad
)

foreach(RFTRACK_TEST_FILE IN LISTS RFTRACK_TEST_FILES)
  configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/${RFTRACK_TEST_FILE}"
    "${CMAKE_CURRENT_BINARY_DIR}/${RFTRACK_TEST_FILE}"
    COPYONLY
  )
endforeach()

set(RFTRACK_TEST_ENVIRONMENT
  "BDSIM_PYTHON_BUILD_DIR=${CMAKE_BINARY_DIR}/python"
  "PYTHONPATH=${CMAKE_CURRENT_BINARY_DIR}:$ENV{PYTHONPATH}"
)

foreach(RFTRACK_TEST_GROUP IN ITEMS conversion validation tracking loss)
  set(RFTRACK_TEST_NAME "interface-rftrack-${RFTRACK_TEST_GROUP}")
  add_test(
    NAME "${RFTRACK_TEST_NAME}"
    COMMAND "${Python3_EXECUTABLE}"
            "${CMAKE_CURRENT_BINARY_DIR}/test_rftrack_${RFTRACK_TEST_GROUP}.py"
  )
  set_tests_properties(
    "${RFTRACK_TEST_NAME}"
    PROPERTIES
      ENVIRONMENT "${RFTRACK_TEST_ENVIRONMENT}"
      LABELS "PYTHON;RFTRACK"
      TIMEOUT 120
      WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
  )
endforeach()
