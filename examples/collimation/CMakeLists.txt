simple_testing(example-collimation                 "--file=collimation.gmad"                ${OVERLAP_CHECK})
simple_testing(example-collimation-hits            "--file=collimation-hits.gmad"           ${OVERLAP_CHECK})
simple_testing(example-collimation-hits-ion        "--file=collimation-hits-ion.gmad"       ${OVERLAP_CHECK})
simple_testing(example-collimation-hits-all        "--file=collimation-hits-all.gmad"       ${OVERLAP_CHECK})
simple_testing(example-collimation-hits-minke      "--file=collimation-hits-minke.gmad"     ${OVERLAP_CHECK})
simple_testing(example-collimation-hits-ion-minke  "--file=collimation-hits-ion-minke.gmad" ${OVERLAP_CHECK})
simple_testing(example-collimation-hits-all-minke  "--file=collimation-hits-all-minke.gmad" ${OVERLAP_CHECK})

simple_testing(example-collimation-scatter         "--file=scatter.gmad"                    ${OVERLAP_CHECK})

