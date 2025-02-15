# Script that sets up the installation of targets..
# To add targets/headers, call macro
# bdsim_install_targets(TARGET_LIST)
# bdsim_install_headers(TARGET_LIST)
# Create installation targets. Note that we put each kind of file
# into a different component via COMPONENT. These components will 
# be used to create the installation components.

# this is put here to be consistent with the macro just below that defines the
# the location of installed executables - curently 'bin'
set(BIN_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/bin")

macro(bdsim_install_targets)
    install(TARGETS ${ARGN} EXPORT BDSIMTargets
      BUNDLE
      DESTINATION .
      COMPONENT Runtime
      ARCHIVE
      DESTINATION lib
      LIBRARY
      DESTINATION lib
      COMPONENT Libraries
      RUNTIME
      DESTINATION bin
      COMPONENT Runtime
      )
endmacro()
macro(bdsim_install_headers)
    install(FILES ${ARGN}
      DESTINATION include/bdsim
      COMPONENT Headers)
endmacro()
macro(bdsim_install_libs)
    install(FILES ${ARGN}
      DESTINATION lib
      COMPONENT Libraries)
endmacro()
macro(bdsim_install_doc)
    install(FILES ${ARGN}
        DESTINATION share/bdsim/doc/${DOC_POSTFIX}
        COMPONENT Documentation)
endmacro()
macro(bdsim_install_scripts)
    install(FILES ${ARGN}
        DESTINATION ${SCRIPT_DEST}
        COMPONENT Scripts)
endmacro()
macro(bdsim_install_vis)
    install(FILES ${ARGN}
        DESTINATION share/bdsim/vis/
        COMPONENT Visualisation)
endmacro()
macro(bdsim_install_analysis_headers)
  install(FILES ${ARGN}
    DESTINATION include/bdsim/analysis
    COMPONENT Headers)
endmacro()
macro(bdsim_install_parser_headers)
  install(FILES ${ARGN}
    DESTINATION include/bdsim/parser
    COMPONENT Headers)
endmacro()
macro(bdsim_install_comparator_headers)
  install(FILES ${ARGN}
    DESTINATION include/bdsim/comparator
    COMPONENT Headers)
endmacro()
macro(bdsim_install_interpolator_headers)
  install(FILES ${ARGN}
    DESTINATION include/bdsim/interpolator
    COMPONENT Headers)
endmacro()
macro(bdsim_install_macros)
  install(FILES ${ARGN}
    DESTINATION share/bdsim/analysis
    COMPONENT Scripts)
endmacro()
macro(bdsim_install_gdml)
  install(FILES ${ARGN}
        DESTINATION share/bdsim/gdml/schema
        COMPONENT GDML)
endmacro()
macro(bdsim_install_python)
    install(TARGETS ${ARGN}
            DESTINATION lib/python/bdsim
            COMPONENT python)
endmacro()

# This macro fixes the MACOSX_BUNDLES
# since we do not make a "traditional app"
macro(bdsim_install_bundle)
  if(APPLE)
    foreach(bundle_name ${ARGN})
      set(APPS "\${CMAKE_INSTALL_PREFIX}/${bundle_name}")
      set(DIRS "")
      install(CODE "
      include(BundleUtilities)
      fixup_bundle(\"${APPS}.app\"   \"\"   \"${DIRS}\")
      " COMPONENT Runtime)
    endforeach()
  endif()
endmacro()

# set MACOSX_RPATH explicitly (CMP0042)
set(CMAKE_MACOSX_RPATH ON)

#include(InstallRequiredSystemLibraries)

# CPack boilerplate for this project
set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_VENDOR "Royal Holloway, University of London")
set(CPACK_PACKAGE_HOMEPAGE_URL "http://www.pp.rhul.ac.uk/bdsim")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/README.md")
set(CPACK_RESOURCE_FILE_LICENSE    "${PROJECT_SOURCE_DIR}/LICENCE.txt")
set(CPACK_BUNDLE_NAME "${CPACK_PACKAGE_NAME}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "BDSIM - Beam Delivery Simulation")
set(CPACK_PACKAGE_VERSION ${BDSIM_VERSION})
set(CPACK_PACKAGE_VERSION_MAJOR ${BDSIM_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${BDSIM_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${BDSIM_VERSION_PATCH})

# Name
set(ARCHIVE_NAME ${CMAKE_PROJECT_NAME}-${BDSIM_VERSION})

if (UNIX)
  set(CPACK_GENERATOR "TGZ")
endif()

if (APPLE)
  set(CPACK_GENERATOR "${CPACK_GENERATOR};DragNDrop")
endif()

set(CPACK_PACKAGE_FILE_NAME "${ARCHIVE_NAME}")