# long tests for em shower with combined analysis and comparison
# (TODO REMOVED) generate_analyse_compare_long(processes-em-e-250GeV-copper    electron_250GeV_copper.gmad    processes-em-e-250GeV-analysis.txt)
# (TODO REMOVED) generate_analyse_compare_long(processes-em-e-250GeV-stainless electron_250GeV_stainless.gmad processes-em-e-250GeV-analysis.txt)
# (TODO REMOVED) generate_analyse_compare_long(processes-em-e-250GeV-titanium  electron_250GeV_titanium.gmad  processes-em-e-250GeV-analysis.txt)
# (TODO REMOVED) generate_analyse_compare_long(processes-em-e-250GeV-tungsten  electron_250GeV_tungsten.gmad  processes-em-e-250GeV-analysis.txt)

# short versions - generation only
set(TESTING_ARGS --ngenerate=1)
simple_testing(processes-em-e-250GeV-copper "--file=electron_250GeV_copper.gmad" "")
set(TESTING_ARGS --ngenerate=1)
simple_testing(processes-em-e-250GeV-stainless "--file=electron_250GeV_stainless.gmad" "")
set(TESTING_ARGS  --ngenerate=1)
simple_testing(processes-em-e-250GeV-titanium "--file=electron_250GeV_titanium.gmad" "")
set(TESTING_ARGS  --ngenerate=1)
simple_testing(processes-em-e-250GeV-tungsten "--file=electron_250GeV_tungsten.gmad" "")
