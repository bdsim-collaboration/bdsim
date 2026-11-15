simple_testing(aperture-circular       "--file=1_circular.gmad"        ${OVERLAP_CHECK})
simple_testing(aperture-rectangular    "--file=2_rectangular.gmad"     ${OVERLAP_CHECK})
simple_testing(aperture-elliptical     "--file=3_elliptical.gmad"      ${OVERLAP_CHECK})
simple_testing(aperture-lhc            "--file=4_lhc.gmad"             ${OVERLAP_CHECK})
simple_testing(aperture-lhcdetailed    "--file=5_lhcdetailed.gmad"     ${OVERLAP_CHECK})
simple_testing(aperture-rectellipse    "--file=6_rectellipse.gmad"     ${OVERLAP_CHECK})
simple_testing(aperture-racetrack      "--file=7_racetrack.gmad"       ${OVERLAP_CHECK})
simple_testing(aperture-octagonal      "--file=8_octagonal.gmad"       ${OVERLAP_CHECK})
simple_testing(aperture-octagonal-2    "--file=8a_octagonal.gmad"      ${OVERLAP_CHECK})
simple_testing(aperture-circularvacuum "--file=9_circular_vacuum.gmad" ${OVERLAP_CHECK})
simple_testing(aperture-clicpcl        "--file=10_clicpcl.gmad"        ${OVERLAP_CHECK})
simple_testing(aperture-angled-faces   "--file=11_angled_faces_check.gmad" ${OVERLAP_CHECK})
simple_testing(aperture-pointsfile     "--file=12_pointsfile.gmad"     ${OVERLAP_CHECK})
simple_testing(aperture-pointsfile-default  "--file=13_pointsfiledefault.gmad"     ${OVERLAP_CHECK})
simple_testing(aperture-pointsfile-triangle "--file=14_pointsfile_triangle.gmad"   ${OVERLAP_CHECK})

simple_testing(beampipe-vacuum-material-colour  "--file=15_vacuum_is_water.gmad"    ${OVERLAP_CHECK})

simple_testing(aperture-rhombus        "--file=16_rhombus.gmad"        ${OVERLAP_CHECK})
simple_testing(aperture-regularpolygon "--file=17_regularpolygon.gmad" ${OVERLAP_CHECK})


add_subdirectory(variable-aperture)