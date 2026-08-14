# find ROOT of at least version 6
# this will use our FindROOT - don't specify hints etc here
find_package(ROOT 6.0 REQUIRED)

# add ROOT include directory
include_directories(SYSTEM ${ROOT_INCLUDE_DIR})

# Make Dictionaries
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/root)
file(GLOB linkHeaders ${CMAKE_CURRENT_SOURCE_DIR}/include/*LinkDef.hh)
# for loop over link definitions
foreach(header ${linkHeaders})
  # The BDSBH4D dictionary only carries Boost-based 4D-histogram classes; its
  # LinkDef is entirely #ifdef USE_BOOST. Without Boost it preprocesses to an
  # empty selection file, which ROOT >= 6.40 rejects ("No selection rules
  # specified"). Skip generating it when Boost support is off.
  if(NOT USE_BOOST AND header MATCHES "BDSBH4DLinkDef")
    continue()
  endif()
  # remove LinkDef.hh
  string(FIND ${header} "LinkDef.hh" pos REVERSE)
  string(FIND ${header} "/" dir REVERSE)
  MATH(EXPR beginpos "${dir}+1")
  MATH(EXPR length "${pos}-${beginpos}")
  string(SUBSTRING ${header} ${beginpos} ${length} className)
  #message(STATUS "${ROOTCINT_EXECUTABLE} -f ${CMAKE_CURRENT_BINARY_DIR}/root/${className}Dict.cc -c ${CMAKE_CURRENT_SOURCE_DIR}/include/${className}.hh ${header}")

  # add preprocessor flag __ROOTBUILD__ as used in BDSIM.
  # root6 .pcm file needs to be at library level
  # for discussion see e.g. this forum topic: https://root.cern.ch/phpBB3/viewtopic.php?t=17654
  add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/root/${className}Dict.cc
    ${CMAKE_CURRENT_BINARY_DIR}/root/${className}Dict_rdict.pcm
    COMMAND ${ROOTCINT_EXECUTABLE}
    ARGS -f ${CMAKE_CURRENT_BINARY_DIR}/root/${className}Dict.cc -noIncludePaths -inlineInputHeader
    ${PREPROCESSOR_DEFS}  -I${CMAKE_CURRENT_SOURCE_DIR} -I${CMAKE_CURRENT_SOURCE_DIR}/include -I${CMAKE_CURRENT_SOURCE_DIR}/parser/
    ${CMAKE_CURRENT_SOURCE_DIR}/include/${className}.hh ${BOOSTINCLUDES} ${header}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/include/${className}.hh ${header}
    IMPLICIT_DEPENDS CXX ${CMAKE_CURRENT_SOURCE_DIR}/include/${className}.hh ${header}
    COMMENT "Generate ROOT Dictionary for ${className}"
    )
  add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${className}Dict_rdict.pcm
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/root/${className}Dict_rdict.pcm
    ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/root/${className}Dict.cc
    )
  # add arbitrary dependence to make it run at compile time (since DEPENDS at custom_command seems not to work)
  set_property(
    SOURCE ${CMAKE_CURRENT_BINARY_DIR}/root/${className}Dict.cc
    APPEND PROPERTY OBJECT_DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${className}Dict_rdict.pcm
    )
  # keep list of ROOT pcms
  set(root_pcms ${root_pcms} ${CMAKE_CURRENT_BINARY_DIR}/${className}Dict_rdict.pcm)

  # keep list of ROOT dictionaries
  set(root_files ${root_files} ${CMAKE_CURRENT_SOURCE_DIR}/src/${className}.cc)
  set(root_dicts ${root_dicts} ${CMAKE_CURRENT_BINARY_DIR}/root/${className}Dict.cc)
endforeach()
