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
