# - Finds ROOT installation
# This module sets up ROOT information 
# It defines:
# ROOT_FOUND          If the ROOT is found
# ROOT_INCLUDE_DIR    PATH to the include directory
# ROOT_LIBRARIES      Most common libraries
# ROOT_LIBRARY_DIR    PATH to the library directory
# ROOT_EXECUTABLE     executable for root

MESSAGE(STATUS "Looking for ROOT")

if (DEFINED ENV{ROOTSYS})
  set(ROOTSYS $ENV{ROOTSYS})
  message(STATUS "ROOT search hint from $ROOTSYS: ${ROOTSYS}")
endif()

# look for root-config
if(ROOT_CONFIG_EXECUTABLE AND EXISTS ${ROOT_CONFIG_EXECUTABLE})
  # do nothing
elseif(EXISTS "${ROOTSYS}/bin/root-config")
  set(ROOT_CONFIG_EXECUTABLE "${ROOTSYS}/bin/root-config")
elseif(EXISTS "${ROOTSYS}/bin/root-config6")
  set(ROOT_CONFIG_EXECUTABLE "${ROOTSYS}/bin/root-config6")
else()
  if($ENV{VERBOSE})
    message(STATUS "root-config not found in ROOTSYS ${ROOTSYS}, trying default PATHS")
  endif()
  find_program(ROOT_CONFIG_EXECUTABLE NAMES root-config root-config6)
endif()

if(NOT ROOT_CONFIG_EXECUTABLE OR NOT EXISTS ${ROOT_CONFIG_EXECUTABLE}) # for broken symlinks
  set(ROOT_FOUND FALSE)
  MESSAGE(STATUS "root-config not found in PATH")
else()    
  set(ROOT_FOUND TRUE)

  message(STATUS "Using root-config: ${ROOT_CONFIG_EXECUTABLE}")

  execute_process(
    COMMAND ${ROOT_CONFIG_EXECUTABLE} --prefix 
    OUTPUT_VARIABLE ROOTSYS 
    OUTPUT_STRIP_TRAILING_WHITESPACE)

  execute_process(
    COMMAND ${ROOT_CONFIG_EXECUTABLE} --version 
    OUTPUT_VARIABLE ROOT_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE)

  execute_process(
    COMMAND ${ROOT_CONFIG_EXECUTABLE} --incdir
    OUTPUT_VARIABLE ROOT_INCLUDE_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE)

  execute_process(
    COMMAND ${ROOT_CONFIG_EXECUTABLE} --libs
    OUTPUT_VARIABLE ROOT_LIBRARIES
    OUTPUT_STRIP_TRAILING_WHITESPACE)

  execute_process(
    COMMAND ${ROOT_CONFIG_EXECUTABLE} --evelibs
    OUTPUT_VARIABLE ROOT_EVELIBRARIES
    OUTPUT_STRIP_TRAILING_WHITESPACE)

  set(ROOT_EXECUTABLE ${ROOTSYS}/bin/root)
  message(STATUS "Using root-executable: ${ROOT_EXECUTABLE}")

  # Hack to remove c++11 lib in favour of the one provided already
  if (NOT "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    SET(C11 "-stdlib=libc++")
    STRING(REPLACE ${C11} "" ROOT_LIBRARIES_TEMP ${ROOT_LIBRARIES})
    SET(ROOT_LIBRARIES ${ROOT_LIBRARIES_TEMP})
  endif()

  execute_process(
    COMMAND ${ROOT_CONFIG_EXECUTABLE} --libdir
    OUTPUT_VARIABLE ROOT_LIBRARY_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE)

  execute_process(
          COMMAND ${ROOT_CONFIG_EXECUTABLE} --has-mathmore
          OUTPUT_VARIABLE ROOT_HAS_MATHMORE
          OUTPUT_STRIP_TRAILING_WHITESPACE)
  if (ROOT_HAS_MATHMORE STREQUAL "yes")
    # string append or prepend doesn't seem to work yet again
    # replace a known library name in the middle of the string with that plus the new one
    STRING (REPLACE "-lMathCore" "-lMathCore -lMathMore" ROOT_LIBRARIES ${ROOT_LIBRARIES})
    add_definitions("-DROOT_HAS_MATHMORE")
  endif()

  # Make variables changeble to the advanced user
  mark_as_advanced(ROOT_CONFIG_EXECUTABLE)

  if(NOT ROOT_FIND_QUIETLY)
    message(STATUS "Found ROOT ${ROOT_VERSION} in ${ROOTSYS}")
    #message(STATUS "ROOT_LIBRARIES: ${ROOT_LIBRARIES}")
    #message(STATUS "ROOT_LIBRARY_DIR: ${ROOT_LIBRARY_DIR}")
    #message(STATUS "ROOT_LIBRARIES_GLOB: ${ROOT_LIBRARIES_GLOB}")
  endif()
endif()

if (NOT ROOT_FOUND)
  message(FATAL_ERROR "ROOT 6 not found")
endif()


# VERSION & COMPILER FLAGS

# ROOT doesn't implement the version and subversion number in CMAKE as it should, so
# the above find package doesn't match the version required. Need to decode version ourselves
if (ROOT_VERSION VERSION_LESS "6.00")
  message(FATAL_ERROR "ROOT Version 6 required. Version ${ROOT_MAJOR_VERSION} found")
endif()

# fix the version number from root
# nice regex from CRMC pacakge in their search for ROOT also
STRING (REGEX REPLACE "[ \t\r\n]+" "" ROOT_VERSION "${ROOT_VERSION}")
STRING (REGEX REPLACE "/" "." ROOT_VERSION "${ROOT_VERSION}")

# remove the C++ standard set by ROOT so CMake can handle it correctly for the
# compiler we find
removeCXXStandardFlags("${CMAKE_CXX_FLAGS}" CMAKE_CXX_FLAGS)

# now remove any duplicates we have to keep things tidy
removeDuplicateSubstring("${CMAKE_CXX_FLAGS}" $CMAKE_CXX_FLAGS)

if (NOT USE_BOOST)
  set(BOOSTINCLUDES "")
else()
  set(BOOSTINCLUDES "-I${Boost_INCLUDE_DIRS}")
endif()

# ROOT can be compiled with C++17 (or 14) and therefore BDSIM won't compile if it doesn't have
# at least that standard, so we pick apart ROOT stuff to find out and update the standard
execute_process(COMMAND ${ROOT_CONFIG_EXECUTABLE} --features OUTPUT_VARIABLE ROOT_FEATURES RESULT_VARIABLE ROOT_FEATURES_RESULT)
if (NOT ROOT_FEATURES_RESULT EQUAL 0)
  message(FATAL_ERROR "root-config --features failed")
endif()
list(REMOVE_DUPLICATES ROOT_FEATURES)
if($ENV{VERBOSE})
  message(STATUS "ROOT features: ${ROOT_FEATURES}")
endif()
string(FIND ${ROOT_FEATURES} "cxx14" _CXX14FOUND)
string(FIND ${ROOT_FEATURES} "cxx17" _CXX17FOUND)
string(FIND ${ROOT_FEATURES} "cxx20" _CXX20FOUND)
if (_CXX20FOUND STRGREATER -1)
  message(STATUS "ROOT compiled with cxx20 feature -> changing to C++20 for BDSIM")
  set(CMAKE_CXX_STANDARD 20)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
elseif (_CXX17FOUND STRGREATER -1)
  message(STATUS "ROOT compiled with cxx17 feature -> changing to C++17 for BDSIM")
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
elseif(_CXX14FOUND STRGREATER -1)
  message(STATUS "ROOT compiled with cxx14 feature -> changing to C++14 for BDSIM")
  set(CMAKE_CXX_STANDARD 14)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

# remove C++ standard flags from root library linking flags
string(REPLACE "-stdlib=libc++"  "" ROOT_LIBRARIES ${ROOT_LIBRARIES})


# FUNCTIONS

#include(CMakeMacroParseArguments)
find_program(ROOTCINT_EXECUTABLE rootcint rootcint6 HINTS ${ROOTSYS}/bin)
if(NOT ROOTCINT_EXECUTABLE OR
    NOT EXISTS ${ROOTCINT_EXECUTABLE}) # for broken symlinks
  MESSAGE(FATAL_ERROR "rootcint not found")
endif()



find_program(GENREFLEX_EXECUTABLE genreflex PATHS ${ROOTSYS}/bin)
find_package(GCCXML)
# set as advanced variable
mark_as_advanced(FORCE ROOTCINT_EXECUTABLE)
mark_as_advanced(FORCE GENREFLEX_EXECUTABLE)

#----------------------------------------------------------------------------
# function ROOT_GENERATE_DICTIONARY( dictionary   
#                                    header1 header2 ... 
#                                    LINKDEF linkdef1 ... 
#                                    OPTIONS opt1...)
function(ROOT_GENERATE_DICTIONARY dictionary)
  CMAKE_PARSE_ARGUMENTS(ARG "" "" "LINKDEF;OPTIONS" "" ${ARGN})
  #---Get the list of header files-------------------------
  set(headerfiles)
  foreach(fp ${ARG_UNPARSED_ARGUMENTS})
    file(GLOB files ${fp})
    if(files)
      foreach(f ${files})
        if(NOT f MATCHES LinkDef)
          set(headerfiles ${headerfiles} ${f})
        endif()
      endforeach()
    else()
      set(headerfiles ${headerfiles} ${fp})
    endif()
  endforeach()
  #---Get the list of include directories------------------
  get_directory_property(incdirs INCLUDE_DIRECTORIES)
  set(includedirs) 
  foreach( d ${incdirs})    
   set(includedirs ${includedirs} -I${d})
  endforeach()
  #---Get LinkDef.h file------------------------------------
  set(linkdefs)
  foreach( f ${ARG_LINKDEF})
    if( IS_ABSOLUTE ${f})
      set(linkdefs ${linkdefs} ${f})
    else() 
      if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/inc/${f})
        set(linkdefs ${linkdefs} ${CMAKE_CURRENT_SOURCE_DIR}/inc/${f})
      else()
        set(linkdefs ${linkdefs} ${CMAKE_CURRENT_SOURCE_DIR}/${f})
      endif()
    endif()
  endforeach()
  #---call rootcint------------------------------------------
  add_custom_command(OUTPUT ${dictionary}.cxx ${dictionary}.h
                     COMMAND ${ROOTCINT_EXECUTABLE} -cint -f  ${dictionary}.cxx 
                                          -c ${ARG_OPTIONS} ${includedirs} ${headerfiles} ${linkdefs} 
                     DEPENDS ${headerfiles} ${linkdefs})
endfunction()

#----------------------------------------------------------------------------
# function REFLEX_GENERATE_DICTIONARY(dictionary   
#                                     header1 header2 ... 
#                                     SELECTION selectionfile ... 
#                                     OPTIONS opt1...)
function(REFLEX_GENERATE_DICTIONARY dictionary)  
  CMAKE_PARSE_ARGUMENTS(ARG "" "" "SELECTION;OPTIONS" "" ${ARGN})
  #---Get the list of header files-------------------------
  set(headerfiles)
  foreach(fp ${ARG_UNPARSED_ARGUMENTS})
    file(GLOB files ${fp})
    if(files)
      foreach(f ${files})
        set(headerfiles ${headerfiles} ${f})
      endforeach()
    else()
      set(headerfiles ${headerfiles} ${fp})
    endif()
  endforeach()
  #---Get Selection file------------------------------------
  if(IS_ABSOLUTE ${ARG_SELECTION})
    set(selectionfile ${ARG_SELECTION})
  else() 
    set(selectionfile ${CMAKE_CURRENT_SOURCE_DIR}/${ARG_SELECTION})
  endif()
  #---Get the list of include directories------------------
  get_directory_property(incdirs INCLUDE_DIRECTORIES)
  set(includedirs) 
  foreach( d ${incdirs})    
    set(includedirs ${includedirs} -I${d})
  endforeach()
  #---Get preprocessor definitions--------------------------
  get_directory_property(defs COMPILE_DEFINITIONS)
  foreach( d ${defs})    
   set(definitions ${definitions} -D${d})
  endforeach()
  #---Nanes and others---------------------------------------
  set(gensrcdict ${dictionary}.cpp)
  if(MSVC)
    set(gccxmlopts "--gccxmlopt=\"--gccxml-compiler cl\"")
  else()
    #set(gccxmlopts "--gccxmlopt=\'--gccxml-cxxflags -m64 \'")
    set(gccxmlopts)
  endif()  
  #set(rootmapname ${dictionary}Dict.rootmap)
  #set(rootmapopts --rootmap=${rootmapname} --rootmap-lib=${libprefix}${dictionary}Dict)
  #---Check GCCXML and get path-----------------------------
  if(GCCXML)
    get_filename_component(gccxmlpath ${GCCXML} PATH)
  else()
    message(WARNING "GCCXML not found. Install and setup your environment to find 'gccxml' executable")
  endif()
  #---Actual command----------------------------------------
  add_custom_command(OUTPUT ${gensrcdict} ${rootmapname}     
                     COMMAND ${GENREFLEX_EXECUTABLE} ${headerfiles} -o ${gensrcdict} ${gccxmlopts} ${rootmapopts} --select=${selectionfile}
                             --gccxmlpath=${gccxmlpath} ${ARG_OPTIONS} ${includedirs} ${definitions}
                     DEPENDS ${headerfiles} ${selectionfile})
endfunction()
  
