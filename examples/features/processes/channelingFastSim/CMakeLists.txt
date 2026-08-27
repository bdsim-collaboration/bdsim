simple_testing(channeling-ch0-legacy "--file=ch0.gmad --output=none" "")
simple_testing(channeling-ch1-fastsim "--file=ch1.gmad --output=none" "")
simple_testing(channeling-ch2-fastsim-controls "--file=ch2.gmad --output=none" "")
if ((G4_MAJOR_VERSION GREATER 11) OR
    ((G4_MAJOR_VERSION EQUAL 11) AND (G4_MINOR_VERSION GREATER 3)))
  simple_testing(channeling-ch3-coherent-pair "--file=ch3.gmad --output=none" "")
endif()
simple_testing(channeling-ch5-positron-source "--file=ch5.gmad --output=none" "")
