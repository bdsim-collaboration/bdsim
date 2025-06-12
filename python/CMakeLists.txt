# -DPython_FIND_VIRTUALENV=ONLY
find_package(Python 3.12 COMPONENTS Interpreter Development.Module REQUIRED)
message(STATUS "Found python ${Python_VERSION}")
message(STATUS "Python executable ${Python_EXECUTABLE}")

find_package(pybind11 CONFIG REQUIRED)
message(STATUS "Found pybind11 ${pybind11_VERSION}")


# glob python binding sources
file(GLOB GMAD_PARSER_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)

list(APPEND pythonBindingSrcs
        aperture array atom beam blmplacement cavitymodel
        crystal element elementtype fastlist field material modulator
        newcolour options parameters parser physicsbiasing placement
        query region samplerplacement scorer scorermesh
        sym_table tunnel)

list(APPEND rootBindingSrcs
        bdsimclass bdsimlink bdsexecoptions bdsparser bdsoutputrooteventsampler
        bdsbunch bdsbunchsixtracklink bdsparticledefinition bdsiondefinition
        bdsexception bdsparticlecoords bdsparticlecoordsfull
        g4particledefinition g4particletable g4iontable clhep
        bdshitsampler bdshitsamplerlink bdshitscollectionsamplerlink)

foreach(pythonBindingSrc IN ZIP_LISTS pythonBindingSrcs)
    # MESSAGE(STATUS ${pythonBindingSrc_0})
    pybind11_add_module(${pythonBindingSrc_0} ${CMAKE_CURRENT_SOURCE_DIR}/${pythonBindingSrc_0}Pybind.cc)
    target_link_libraries(${pythonBindingSrc_0} PUBLIC ${GMAD_LIB_NAME} Python::Module)
    set_property(TARGET ${pythonBindingSrc_0} PROPERTY CXX_VISIBILITY_PRESET "default")
    bdsim_install_python(${pythonBindingSrc_0})
endforeach()

foreach(rootBindingSrc IN ZIP_LISTS rootBindingSrcs)
    # MESSAGE(STATUS ${rootBindingSrc_0})
    pybind11_add_module(${rootBindingSrc_0} ${CMAKE_CURRENT_SOURCE_DIR}/${rootBindingSrc_0}Pybind.cc)
    target_link_libraries(${rootBindingSrc_0} PUBLIC ${GMAD_LIB_NAME} ${BDSIM_LIB_NAME} Python::Module)
    target_include_directories(${rootBindingSrc_0} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include/)
    bdsim_install_python(${rootBindingSrc_0})
endforeach()

install(FILES __init__.py bdsim.py
        DESTINATION lib/python/bdsim
        COMPONENT python)

