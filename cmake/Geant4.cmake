option(USE_GEANT4_EMD_ID "Support custom build of Geant4 with EMD process sub type set discretely"  OFF )
if(USE_GEANT4_EMD_ID)
  add_definitions("-DG4EMDPROCESSID")
  message(STATUS "USE_GEANT4_EMD_ID ON - using custom build of Geant4 with EMD process ID customised.")
endif()

# with all available gui and visualisation options available
# Try first with Geant4 built-in Config (versions 9.5 and higher should have it):
if ($ENV{VERBOSE})
   set(Geant4_CONFIG_DEBUG TRUE)
endif()

# Search for Geant4 CONFIG mode, if prefix given search that path only.
# Add to GUI
set(Geant4_PREFIX "" CACHE PATH "Location of Geant4 (optional)")
if (NOT ${Geant4_PREFIX} STREQUAL "")
   if($ENV{VERBOSE})
      message(STATUS "Geant4 prefix: ${Geant4_PREFIX}")
   endif()
   find_package(Geant4 QUIET COMPONENTS ui_all vis_all CONFIG PATHS ${Geant4_PREFIX} NO_DEFAULT_PATH)
else()
   if($ENV{VERBOSE})
      message(STATUS "No Geant4 prefix, searching default paths")
   endif()
   find_package(Geant4 QUIET COMPONENTS ui_all vis_all CONFIG)
endif()

if (Geant4_FOUND)
      message(STATUS "Geant4 Use File: ${Geant4_USE_FILE}")
      include(${Geant4_USE_FILE})
      message(STATUS "Geant4 Definitions: ${Geant4_DEFINITIONS}")

      # We require geant4 be built against the system clhep (as bdsim will be)
      # to ensure strong reproducibility
      # the geant4 config search bit is taken from their cmake script as they don't
      # export the variable for use by others
      find_program(GEANT4_CONFIG NAMES geant4-config
             PATHS $ENV{GEANT4_INSTALL}/bin
                   ${GEANT4_INSTALL}/bin
                   /usr/local/bin /opt/local/bin)

      # check geant4 was built with external clhep so we only have 1 clhep behind the scenes
      # and therefore 1 random number generator. BDSIM requires to be compiled with the full
      # clhep so if Geant4 has its own internal one, then we won't be reproducible.
      execute_process(COMMAND ${GEANT4_CONFIG} --has-feature clhep OUTPUT_VARIABLE _TMP)
      if($ENV{VERBOSE})
         message(STATUS "Geant4 config executable:  ${GEANT4_CONFIG}")
         message(STATUS "Geant4 uses its own clhep: ${_TMP}")
      endif()
      if (_TMP MATCHES "yes")
      	 message(FATAL_ERROR "BDSIM requires Geant4 to be compiled using the system CLHEP so it's the same as BDSIM for strong reproducibility - please reconfigure and reinstall Geant4")
      endif()

      # check if Geant4 has GDML if we have that feature turned on in BDSIM
      if(USE_GDML)
        execute_process(COMMAND ${GEANT4_CONFIG} --has-feature gdml OUTPUT_VARIABLE _TMP3)
        if($ENV{VERBOSE})
          message(STATUS "Geant4 config executable:  ${GEANT4_CONFIG}")
          message(STATUS "Geant4 uses gdml: ${_TMP3}")
        endif()
        if (_TMP3 MATCHES "no")
          message(FATAL_ERROR "BDSIM has USE_GDML on but Geant4 was not compiled with GDML support - please reconfigure and reinstall Geant4")
        endif()
      endif()

      execute_process(COMMAND ${GEANT4_CONFIG} --prefix  OUTPUT_VARIABLE _TMP2)
      string(REGEX REPLACE "\n$" "" _TMP2 "${_TMP2}")
      set(Geant4_LIBRARY_DIR ${_TMP2}/lib)

      # We don't support multithreading for now
      if ("${Geant4_DEFINITIONS}" MATCHES "G4MULTITHREADED")
	    message(FATAL_ERROR "Currently Geant4 builds with multithreading are not supported at the moment! Please build Geant4 with multithreading off. Exiting")
      endif()
      
      if($ENV{VERBOSE})
        message(STATUS "Geant4_INCLUDE_DIRS: ${Geant4_INCLUDE_DIRS}")
        message(STATUS "Geant4_LIBRARY_DIR: ${Geant4_LIBRARY_DIR}")
        message(STATUS "Geant4_LIBRARIES: ${Geant4_LIBRARIES}")
      endif()
      # Mark Geant4_DIR as advanced since can't be set in GUI and this turned out to be confusing
      # Mark other Geant4 things as advanced to hide from bdsim's cmake configuration
      mark_as_advanced(Geant4_DIR)
      mark_as_advanced(Geant4_PREFIX)
      mark_as_advanced(PTL_DIR)
      mark_as_advanced(Qt5Core_DIR)
      mark_as_advanced(Qt5Gui_DIR)
      mark_as_advanced(Qt5OpenGL_DIR)
      mark_as_advanced(Qt5PrintSupport_DIR)
      mark_as_advanced(Qt5Widgets_DIR)
      mark_as_advanced(Qt5Gamepad_DIR)
      mark_as_advanced(Qt5Network_DIR)
      mark_as_advanced(Qt53DCore_DIR)
      mark_as_advanced(Qt53DExtras_DIR)	
      mark_as_advanced(Qt53DInput_DIR)
      mark_as_advanced(Qt53DLogic_DIR)
      mark_as_advanced(Qt53DRender_DIR)
      mark_as_advanced(X11_xcb_icccm_INCLUDE_PATH)
      mark_as_advanced(X11_xcb_icccm_LIB)
      mark_as_advanced(X11_xcb_util_INCLUDE_PATH)
      mark_as_advanced(X11_xcb_util_LIB)
      mark_as_advanced(X11_xcb_xfixes_INCLUDE_PATH)
      mark_as_advanced(X11_xcb_xfixes_LIB)
      
else(Geant4_FOUND)
      message(FATAL_ERROR "Geant4 not found. Exiting. You can help cmake by adding -DGeant4_PREFIX=<PATH>")
endif()

if (Geant4_VERSION AND NOT "${Geant4_VERSION}" MATCHES "UNKNOWN")
  string(SUBSTRING ${Geant4_VERSION} 0 1 G4_MAJOR_VERSION)
  if(${G4_MAJOR_VERSION} STREQUAL "1")
    string(SUBSTRING ${Geant4_VERSION} 0 2 G4_MAJOR_VERSION)
    string(SUBSTRING ${Geant4_VERSION} 3 1 G4_MINOR_VERSION) 
    string(SUBSTRING ${Geant4_VERSION} 5 1 G4_PATCH_LEVEL)
  else()
    string(SUBSTRING ${Geant4_VERSION} 2 1 G4_MINOR_VERSION) 
    string(SUBSTRING ${Geant4_VERSION} 4 1 G4_PATCH_LEVEL)
  endif()

  message(STATUS "G4_VERSION: ${Geant4_VERSION}")
  if($ENV{VERBOSE})
    message(STATUS "G4_MAJOR_VERSION: ${G4_MAJOR_VERSION}")
    message(STATUS "G4_MINOR_VERSION: ${G4_MINOR_VERSION}")
    message(STATUS "G4_PATCH_LEVEL: ${G4_PATCH_LEVEL}")
  endif()
  if(10 GREATER ${G4_MAJOR_VERSION})
    message(FATAL_ERROR "Geant4 version 10 is needed!")
  endif()
else()
  set(G4_VERSION 10.9.9)
  set(G4_MAJOR_VERSION 10)
  set(G4_MINOR_VERSION 9)
  set(G4_PATCH_LEVEL 9)
endif()

if(${G4_MINOR_VERSION} GREATER 5 OR ${G4_MAJOR_VERSION} GREATER 10)
  add_definitions("-DG4VIS_USE")
  add_definitions("-DG4UI_USE")
endif()

# might be empty depending on geant4 version - must avoid error in empty argument to function call
if (NOT ${CMAKE_CXX_FLAGS} STREQUAL "")
  # remove the C++ standard set by geant4 so we can enforce our own
  removeCXXStandardFlags(${CMAKE_CXX_FLAGS} CMAKE_CXX_FLAGS)

  # now remove any duplicates we have to keep things tidy
  removeDuplicateSubstring(${CMAKE_CXX_FLAGS} CMAKE_CXX_FLAGS)
endif()

string(FIND ${Geant4_CXX_FLAGS} "c++14" _G4CXX14FOUND)
string(FIND ${Geant4_CXX_FLAGS} "c++17" _G4CXX17FOUND)
string(FIND ${Geant4_CXX_FLAGS} "c++1z" _G4CXX17FOUND2)
string(FIND ${Geant4_CXX_FLAGS} "c++20" _G4CXX20FOUND)
string(FIND ${Geant4_CXX_FLAGS} "c++23" _G4CXX23FOUND)
if (_G4CXX23FOUND STRGREATER -1)
  message(STATUS "Geant4 compiled with cxx23 -> changing to C++23 for BDSIM")
  set(CMAKE_CXX_STANDARD 23)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
elseif (_G4CXX20FOUND STRGREATER -1)
  message(STATUS "Geant4 compiled with cxx20 -> changing to C++20 for BDSIM")
  set(CMAKE_CXX_STANDARD 20)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
elseif (_G4CXX17FOUND STRGREATER -1 OR _G4CXX17FOUND2 STRGREATER -1)
  if (${CMAKE_CXX_STANDARD} STRLESS "17")
    message(STATUS "Geant4 compiled with cxx17 -> changing to C++17 for BDSIM")
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
  endif()
elseif (_G4CXX14FOUND STRGREATER -1)
  if (${CMAKE_CXX_STANDARD} STRLESS "14")
    message(STATUS "Geant4 compiled with cxx14 -> changing to C++14 for BDSIM")
    set(CMAKE_CXX_STANDARD 14)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
  endif()
endif()