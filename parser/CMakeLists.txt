find_package(BISON 2.4 REQUIRED)
find_package(FLEX REQUIRED)

# suppress warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-sign-compare -Wno-unused-value")
# disable clang warnings on bison/flex generated code
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-register -Wno-unneeded-internal-declaration")
endif()

if($ENV{VERBOSE})
  message(STATUS "PARSER CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS}")
endif()

# compile BISON, with debug if in Debug mode
if(${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "DebugOutput")
  bison_target(PARSER parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.cc
    COMPILE_FLAGS "-t")
else ()
  bison_target(PARSER parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.cc)
endif()

flex_target(SCANNER parser.l ${CMAKE_CURRENT_BINARY_DIR}/lex.yy.cc)
add_flex_bison_dependency(SCANNER PARSER)

# glob parser sources
file(GLOB GMAD_PARSER_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)
set_source_files_properties(${GMAD_PARSER_SOURCES} PROPERTIES LABELS "BDSIM")
# remove parser main
list(REMOVE_ITEM GMAD_PARSER_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/gmad.cc)
if($ENV{VERBOSE})
  message(STATUS "GMAD parser sources ${GMAD_PARSER_SOURCES}")
endif()

# Object library, will be used by both static and shared library
add_library(gmad-object-lib OBJECT ${GMAD_PARSER_SOURCES} ${BISON_PARSER_OUTPUTS} ${FLEX_SCANNER_OUTPUTS})
# Shared libraries need PIC
set_target_properties(gmad-object-lib PROPERTIES POSITION_INDEPENDENT_CODE 1)

if (BDSIM_BUILD_STATIC_LIBS)
  add_library(${GMAD_LIB_NAME_STATIC} STATIC $<TARGET_OBJECTS:gmad-object-lib>)
  generate_export_header(${GMAD_LIB_NAME_STATIC})
endif()
add_library(${GMAD_LIB_NAME} SHARED $<TARGET_OBJECTS:gmad-object-lib>)
generate_export_header(${GMAD_LIB_NAME})

if($ENV{VERBOSE})
  message(STATUS "BISON_PARSER_OUTPUTS ${BISON_PARSER_OUTPUTS}") 
  message(STATUS "FLEX_SCANNER_OUTPUTS ${FLEX_SCANNER_OUTPUTS}")
endif()

add_executable(gmadExec gmad.cc)
set_target_properties(gmadExec PROPERTIES OUTPUT_NAME "gmad" VERSION ${BDSIM_VERSION})
target_link_libraries(gmadExec ${GMAD_LIB_NAME})

if (BDSIM_BUILD_STATIC_LIBS)
  set(glibraries ${GMAD_LIB_NAME} ${GMAD_LIB_NAME_STATIC})
else()
  set(glibraries ${GMAD_LIB_NAME})
endif()
bdsim_install_targets(gmadExec ${glibraries})
file(GLOB parser_headers ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
bdsim_install_parser_headers(${parser_headers})

# set binary name for testing usage
get_target_property(binaryname gmadExec OUTPUT_NAME)
set(gmadBinary ${CMAKE_CURRENT_BINARY_DIR}/${binaryname})

# add tests
add_subdirectory(test)
