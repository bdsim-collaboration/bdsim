message(STATUS "Building test programs")

add_executable(BDSFieldTester BDSFieldTester.cc)
set_target_properties(BDSFieldTester PROPERTIES OUTPUT_NAME "BDSFieldTest" VERSION ${BDSIM_VERSION})
target_link_libraries(BDSFieldTester ${BDSIM_LIB_NAME} ${GMAD_LIB_NAME})
configure_file(purefieldplotter.py purefieldplotter.py COPYONLY)
configure_file(converttobdsim.py converttobdsim.py COPYONLY)
add_test(NAME "tester-fields" COMMAND BDSFieldTester)

add_executable(BDSInterpolatorTester BDSInterpolatorTester.cc)
set_target_properties(BDSInterpolatorTester PROPERTIES OUTPUT_NAME "BDSInterpolatorTester" VERSION ${BDSIM_VERSION})
target_link_libraries(BDSInterpolatorTester ${BDSIM_LIB_NAME} ${GMAD_LIB_NAME})
add_test(NAME "tester-interpolator" COMMAND BDSInterpolatorTester)

add_executable(BDSLinkTester BDSLinkTester.cc)
set_target_properties(BDSLinkTester PROPERTIES OUTPUT_NAME "BDSLinkTester" VERSION ${BDSIM_VERSION})
target_link_libraries(BDSLinkTester ${BDSIM_LIB_NAME} gmad)
#add_test(NAME "tester-link" COMMAND BDSLinkTester)

add_executable(BDSLinkSamplerTester BDSLinkSamplerTester.cc)
target_link_libraries(BDSLinkSamplerTester ${BDSIM_LIB_NAME} gmad)
add_test(NAME "tester-link-sampler-parser-first"
         COMMAND BDSLinkSamplerTester ${CMAKE_CURRENT_SOURCE_DIR}/bdslink/link_sampler.gmad parser probe0)
add_test(NAME "tester-link-sampler-parser-last"
         COMMAND BDSLinkSamplerTester ${CMAKE_CURRENT_SOURCE_DIR}/bdslink/link_sampler.gmad parser probe1)
add_test(NAME "tester-link-sampler-legacy"
         COMMAND BDSLinkSamplerTester ${CMAKE_CURRENT_SOURCE_DIR}/bdslink/link_sampler_empty.gmad legacy probe)

add_executable(BDSLinkFieldTester BDSLinkFieldTester.cc)
target_link_libraries(BDSLinkFieldTester ${BDSIM_LIB_NAME} gmad)
add_test(NAME "tester-link-field-construction"
         COMMAND BDSLinkFieldTester ${CMAKE_CURRENT_SOURCE_DIR}/bdslink/link_field.gmad probe)
foreach(element_name q1 s1 o1 dc1 mul1)
  add_test(NAME "tester-link-field-${element_name}"
           COMMAND BDSLinkFieldTester
                   ${CMAKE_CURRENT_SOURCE_DIR}/bdslink/link_field_elements.gmad
                   ${element_name} field)
endforeach()
add_test(NAME "tester-link-field-rfx1"
         COMMAND BDSLinkFieldTester
                 ${CMAKE_CURRENT_SOURCE_DIR}/bdslink/link_field_elements.gmad
                 rfx1 field-x)
add_test(NAME "tester-link-field-rfy1"
         COMMAND BDSLinkFieldTester
                 ${CMAKE_CURRENT_SOURCE_DIR}/bdslink/link_field_elements.gmad
                 rfy1 field-y)
add_test(NAME "tester-link-field-rfx-finite-reference"
         COMMAND BDSLinkFieldTester
                 ${CMAKE_CURRENT_SOURCE_DIR}/bdslink/link_field_rf_finite.gmad
                 rfxf 0.0100012961775 3.90754303226e-06 4.16973161697)
if (USE_GZSTREAM)
  add_test(NAME "tester-link-field-map"
           COMMAND BDSLinkFieldTester ${CMAKE_CURRENT_SOURCE_DIR}/bdslink/link_field_map.gmad
                   probe_map 0.00962872616947 -0.00148509279825)
endif()

add_executable(BDSLinkStraightElementTester BDSLinkStraightElementTester.cc)
target_link_libraries(BDSLinkStraightElementTester ${BDSIM_LIB_NAME} gmad)
add_test(NAME "tester-link-straight-drift"
         COMMAND BDSLinkStraightElementTester
                 ${CMAKE_CURRENT_SOURCE_DIR}/bdslink/link_straight_elements.gmad
                 d1)
add_test(NAME "tester-link-skip-zero-length-quadrupole"
         COMMAND BDSLinkStraightElementTester
                 ${CMAKE_CURRENT_SOURCE_DIR}/bdslink/link_zero_length_element.gmad
                 q0 missing)

add_executable(BDSLinkUnsupportedElementTester BDSLinkUnsupportedElementTester.cc)
target_link_libraries(BDSLinkUnsupportedElementTester ${BDSIM_LIB_NAME} gmad)
foreach(parameter angle e1 e2)
  add_test(NAME "tester-link-reject-element-${parameter}"
           COMMAND BDSLinkUnsupportedElementTester
                   ${CMAKE_CURRENT_SOURCE_DIR}/bdslink/link_element_${parameter}.gmad)
endforeach()

add_executable(BDSSixTrackTester BDSSixTrackTester.cc)
set_target_properties(BDSSixTrackTester PROPERTIES OUTPUT_NAME "BDSSixTrackTester" VERSION ${BDSIM_VERSION})
target_link_libraries(BDSSixTrackTester ${BDSIM_LIB_NAME} gmad)
add_test(NAME "tester-sixtrack" COMMAND BDSSixTrackTester)
configure_file(somecollimators.dat somecollimators.dat COPYONLY)
configure_file(lhccrystals.gmad lhccrystals.gmad COPYONLY)

# prepare non-tested visualiser copy
add_executable(BDSSixTrackTesterVis BDSSixTrackTester.cc)
set_target_properties(BDSSixTrackTesterVis PROPERTIES OUTPUT_NAME "BDSSixTrackTesterVis" VERSION ${BDSIM_VERSION})
target_compile_definitions(BDSSixTrackTesterVis PUBLIC -DVISLINK)
target_link_libraries(BDSSixTrackTesterVis ${BDSIM_LIB_NAME} gmad)

add_executable(BDSTrajectoryTester BDSTrajectoryTester.cc)
set_target_properties(BDSTrajectoryTester PROPERTIES OUTPUT_NAME "BDSTrajectoryTest" VERSION ${BDSIM_VERSION})
target_link_libraries(BDSTrajectoryTester rebdsim bdsimRootEvent bdsim)
add_test(NAME "tester-trajectories" COMMAND BDSTrajectoryTester "../examples/features/data/trajectory-sample.root")

add_executable(BDSModelTreeTester BDSModelTreeTester.cc)
set_target_properties(BDSModelTreeTester PROPERTIES OUTPUT_NAME "BDSModelTreeTest" VERSION ${BDSIM_VERSION})
target_link_libraries(BDSModelTreeTester rebdsim bdsimRootEvent bdsim)
add_test(NAME "tester-model-tree" COMMAND BDSModelTreeTester "../examples/features/data/sample1.root")

add_executable(TH1SetTest TH1SetTest.cc)
target_link_libraries(TH1SetTest ${BDSIM_LIB_NAME} ${ROOT_LIBRARIES} rebdsim)

add_subdirectory(TrackingTestFiles)
