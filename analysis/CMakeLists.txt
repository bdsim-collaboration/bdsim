# option to turn event display, and therefore dependency on ROOT EVE libraries off or on.
option( USE_EVENT_DISPLAY "Use event display - requires ROOT EVE libraries." OFF )

file(GLOB rebdsimLibSources ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)
file(GLOB rebdsimLibLinkDefs ${CMAKE_CURRENT_SOURCE_DIR}/*LinkDef.hh)

# Remove mains from dictionary
list(REMOVE_ITEM rebdsimLibSources ${CMAKE_CURRENT_SOURCE_DIR}/rebdsim.cc)
list(REMOVE_ITEM rebdsimLibSources ${CMAKE_CURRENT_SOURCE_DIR}/edbdsim.cc)
list(REMOVE_ITEM rebdsimLibSources ${CMAKE_CURRENT_SOURCE_DIR}/rebdsimHistoMerge.cc)
list(REMOVE_ITEM rebdsimLibSources ${CMAKE_CURRENT_SOURCE_DIR}/rebdsimOptics.cc)
list(REMOVE_ITEM rebdsimLibSources ${CMAKE_CURRENT_SOURCE_DIR}/rebdsimOrbit.cc)
list(REMOVE_ITEM rebdsimLibSources ${CMAKE_CURRENT_SOURCE_DIR}/rebdsimCombine.cc)
list(REMOVE_ITEM rebdsimLibSources ${CMAKE_CURRENT_SOURCE_DIR}/bdskim.cc)
list(REMOVE_ITEM rebdsimLibSources ${CMAKE_CURRENT_SOURCE_DIR}/bdsimCombine.cc)

# Histogram accumulators used by both libbdsim and librebdsim. These are built into their
# own library that only depends on ROOT so that libbdsim does not have to link librebdsim
# (and therefore libbdsimRootEvent, which has different builds of the same output classes).
set(accumulatorClasses HistogramAccumulator HistogramAccumulatorFast)
set(accumulatorLibSources "")
foreach(className ${accumulatorClasses})
  list(REMOVE_ITEM rebdsimLibSources ${CMAKE_CURRENT_SOURCE_DIR}/${className}.cc)
  list(APPEND accumulatorLibSources ${CMAKE_CURRENT_SOURCE_DIR}/${className}.cc)
endforeach()

if (NOT USE_EVENT_DISPLAY)
  list(REMOVE_ITEM rebdsimLibSources ${CMAKE_CURRENT_SOURCE_DIR}/EventDisplay.cc)
  list(REMOVE_ITEM rebdsimLibLinkDefs ${CMAKE_CURRENT_SOURCE_DIR}/EventDisplayLinkDef.hh)	
endif()

#message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}")
#message(STATUS "ROOT DICTS : ${rebdsimLibLinkDefs}")

if(USE_ROOT_DOUBLE_OUTPUT)
  set(PREPROCESSOR_DEFS "-D__ROOTBUILD__;-D__ROOTDOUBLE__")
else()
  set(PREPROCESSOR_DEFS "-D__ROOTBUILD__")
endif()

if (NOT USE_BOOST)
  set(BOOSTINCLUDES "")
else()
  set(BOOSTINCLUDES "-I${Boost_INCLUDE_DIRS}")
endif()

# Add BDSIM label to original files
set_source_files_properties(${rebdsimLibSources} ${rebdsimLibLinkDefs} PROPERTIES LABELS "BDSIM")

foreach(header ${rebdsimLibLinkDefs})
    # remove LinkDef.hh
    string(FIND ${header} "LinkDef.h" pos REVERSE)
    string(FIND ${header} "/" dir REVERSE)
    MATH(EXPR beginpos "${dir}+1")
    MATH(EXPR length "${pos}-${beginpos}")
    string(SUBSTRING ${header} ${beginpos} ${length} className)

    # add preprocessor flag __ROOTBUILD__ as used in BDSIM.
    # root6 .pcm file needs to be at library level
    # for discussion see e.g. this forum topic: https://root.cern.ch/phpBB3/viewtopic.php?t=17654
    add_custom_command(
            OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${className}Dict.cc ${CMAKE_CURRENT_BINARY_DIR}/${className}Dict_rdict.pcm
            COMMAND ${ROOTCINT_EXECUTABLE}
            ARGS -f ${CMAKE_CURRENT_BINARY_DIR}/${className}Dict.cc -noIncludePaths -inlineInputHeader
            ${PREPROCESSOR_DEFS} -I${CMAKE_CURRENT_SOURCE_DIR} -I${CMAKE_SOURCE_DIR}/include -I${Boost_INCLUDE_DIRS}
            -I${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/${className}.hh ${BOOSTINCLUDES} ${header}
            DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${className}.hh ${header}
            IMPLICIT_DEPENDS CXX ${CMAKE_CURRENT_SOURCE_DIR}/${className}.hh ${header}
            COMMENT "Generate ROOT Dictionary for ${className}"
    )
    # add arbitrary dependence to make it run at compile time (since DEPENDS at custom_command seems not to work)
    set_property(
            SOURCE ${CMAKE_CURRENT_BINARY_DIR}/${className}Dict.cc
            APPEND PROPERTY OBJECT_DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${className}Dict_rdict.pcm
    )
     # keep list of ROOT pcms
    set(rebdsim_pcms ${rebdsim_pcms} ${CMAKE_CURRENT_BINARY_DIR}/${className}Dict_rdict.pcm)

    if (className IN_LIST accumulatorClasses)
      list(APPEND accumulatorLibSources ${CMAKE_CURRENT_BINARY_DIR}/${className}Dict.cc)
    else()
      set(rebdsimLibSources ${rebdsimLibSources} ${CMAKE_CURRENT_BINARY_DIR}/${className}Dict.cc)
    endif()
endforeach()

add_library(bdsimAccumulator SHARED ${accumulatorLibSources})
target_link_libraries(bdsimAccumulator ${ROOT_LIBRARIES})
bdsim_install_targets(bdsimAccumulator)

add_library(rebdsim SHARED ${rebdsimLibSources})
target_link_libraries(rebdsim bdsimAccumulator bdsimRootEvent)
if (USE_EVENT_DISPLAY)
    target_link_libraries(rebdsim ${ROOT_EVELIBRARIES})
endif()
bdsim_install_targets(rebdsim)

add_executable(rebdsimExec rebdsim.cc)
set_target_properties(rebdsimExec PROPERTIES OUTPUT_NAME "rebdsim" VERSION ${BDSIM_VERSION})
target_link_libraries(rebdsimExec rebdsim bdsimRootEvent)
bdsim_install_targets(rebdsimExec)

if (USE_EVENT_DISPLAY)
    add_executable(edbdsimExec edbdsim.cc)
    set_target_properties(edbdsimExec PROPERTIES OUTPUT_NAME "edbdsim" VERSION ${BDSIM_VERSION})
    target_link_libraries(edbdsimExec rebdsim ${ROOT_EVELIBRARIES})
    bdsim_install_targets(edbdsimExec)
endif()

add_executable(rebdsimHistoMergeExec rebdsimHistoMerge.cc)
set_target_properties(rebdsimHistoMergeExec PROPERTIES OUTPUT_NAME "rebdsimHistoMerge" VERSION ${BDSIM_VERSION})
target_link_libraries(rebdsimHistoMergeExec rebdsim bdsimRootEvent)
bdsim_install_targets(rebdsimHistoMergeExec)

add_executable(rebdsimCombineExec rebdsimCombine.cc)
set_target_properties(rebdsimCombineExec PROPERTIES OUTPUT_NAME "rebdsimCombine" VERSION ${BDSIM_VERSION})
target_link_libraries(rebdsimCombineExec rebdsim bdsimRootEvent)
bdsim_install_targets(rebdsimCombineExec)

add_executable(rebdsimOpticsExec rebdsimOptics.cc)
set_target_properties(rebdsimOpticsExec PROPERTIES OUTPUT_NAME "rebdsimOptics" VERSION ${BDSIM_VERSION})
target_link_libraries(rebdsimOpticsExec rebdsim bdsimRootEvent)
bdsim_install_targets(rebdsimOpticsExec)

add_executable(rebdsimOrbitExec rebdsimOrbit.cc)
set_target_properties(rebdsimOrbitExec PROPERTIES OUTPUT_NAME "rebdsimOrbit" VERSION ${BDSIM_VERSION})
target_link_libraries(rebdsimOrbitExec rebdsim bdsimRootEvent)
bdsim_install_targets(rebdsimOrbitExec)

add_executable(bdskimExec bdskim.cc)
set_target_properties(bdskimExec PROPERTIES OUTPUT_NAME "bdskim" VERSION ${BDSIM_VERSION})
target_link_libraries(bdskimExec rebdsim bdsimRootEvent)
bdsim_install_targets(bdskimExec)

add_executable(bdsimCombineExec bdsimCombine.cc)
set_target_properties(bdsimCombineExec PROPERTIES OUTPUT_NAME "bdsimCombine" VERSION ${BDSIM_VERSION})
target_link_libraries(bdsimCombineExec rebdsim bdsimRootEvent)
bdsim_install_targets(bdsimCombineExec)

# Install pcms
bdsim_install_libs(${rebdsim_pcms})

file(GLOB rebdsimMacros ${CMAKE_CURRENT_SOURCE_DIR}/*.C ${CMAKE_CURRENT_SOURCE_DIR}/*.H)
bdsim_install_macros(${rebdsimMacros})
#message(STATUS "rebdsimMacros ${rebdsimMacros}")

file(GLOB rebdsimHeaders ${CMAKE_CURRENT_SOURCE_DIR}/*.hh)
# remove linkdefs
foreach(header ${rebdsimLibLinkDefs})
  list(REMOVE_ITEM rebdsimHeaders ${header})
endforeach()
if (NOT USE_EVENT_DISPLAY)
  list(REMOVE_ITEM rebdsimHeaders ${CMAKE_CURRENT_SOURCE_DIR}/EventDisplay.hh)
  list(REMOVE_ITEM rebdsimHeaders ${CMAKE_CURRENT_SOURCE_DIR}/EventDisplayLinkDef.hh)	
endif()
bdsim_install_analysis_headers(${rebdsimHeaders})
#message(STATUS "rebdsimHeaders ${rebdsimHeaders}")

# load test macros
include(rebdsim_test_macros)
