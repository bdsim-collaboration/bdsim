# quietly detect if we have HepMC library available - not required

option( USE_HEPMC3 "Include HepMC3 library support." OFF)
option( USE_HEPMC3_ROOTIO "Use ROOT IO support of HEPMC3." OFF)
if (USE_HEPMC3)
  message(STATUS "Using HepMC3")
  find_package(HepMC3 REQUIRED HINTS HepMC3_DIR)
  message(STATUS "HEPMC3 Use File: ${HEPMC3_ROOT_DIR}/share/HepMC3/cmake/HepMC3Config.cmake")
  string(REPLACE "//" "/" HEPMC3_INCLUDE_DIR ${HEPMC3_INCLUDE_DIR})
  include_directories(SYSTEM "${HEPMC3_INCLUDE_DIR}")
  add_definitions(-DUSE_HEPMC3)

  # check if USE_HEPMC3_ROOTIO has been set by the user on initial run
  if (USE_HEPMC3_ROOTIO)
    find_package(HepMC3 REQUIRED COMPONENTS HepMC3 HepMC3fio HepMC3rootIO HITS HepMC3_DIR)
    message(STATUS "HEPMC3 Use File: ${HEPMC3_ROOT_DIR}/share/HepMC3/cmake/HepMC3Config.cmake")
    if (HEPMC3_ROOTIO_LIB STREQUAL "HEPMC3_ROOTIO_LIB-NOTFOUND")
      message(FATAL_ERROR "HepMC3 not compiled with ROOT IO support")
    else()
      add_definitions(-DUSE_HEPMC3_ROOTIO)
    endif()
  endif()

  # generally check if it's available after the general package search
  # hepmc3 doesn't provide any great way of detecting components here
  if (DEFINED HEPMC3_ROOTIO_LIB)
    if (NOT HEPMC3_ROOTIO_LIB STREQUAL "HEPMC3_ROOTIO_LIB-NOTFOUND")
      add_definitions(-DUSE_HEPMC3_ROOTIO)
      set(USE_HEPMC3_ROOTIO ON)	
      message(STATUS "Using HEPMC3 ROOTIO library")
      #message(STATUS ${HEPMC3_ROOTIO_LIB})
    else ()
      set(USE_HEPMC3_ROOTIO OFF)
    endif()
  else ()
    set(USE_HEPMC3_ROOTIO OFF)
  endif()

  mark_as_advanced(HEPMC3_SEARCH_LIB)
  mark_as_advanced(HEPMC3_LIB)
  mark_as_advanced(HEPMC3_ROOTIO_LIB)
endif()
