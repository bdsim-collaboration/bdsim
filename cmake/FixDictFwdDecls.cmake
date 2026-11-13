# Post-process a rootcling-generated dictionary source file.
# Usage: cmake -DDICT_FILE=<path/to/XDict.cc> -P FixDictFwdDecls.cmake
#
# Boost.Histogram declares its axis templates with default arguments written as
# 'use_default', which is brought into boost::histogram by 'using boost::use_default;'.
# rootcling writes these into the dictionary forward declarations payload as
# 'boost::histogram::use_default', but the payload doesn't contain the using
# declaration, so cling fails to parse it at library load time ("no type named
# 'use_default' in namespace 'boost::histogram'"). Spell it as the real type.
# This is a no-op for dictionaries that don't contain it.
file(READ ${DICT_FILE} contents)
string(FIND "${contents}" "boost::histogram::use_default" found)
if(NOT found EQUAL -1)
  string(REPLACE "boost::histogram::use_default" "boost::use_default" contents "${contents}")
  file(WRITE ${DICT_FILE} "${contents}")
endif()
