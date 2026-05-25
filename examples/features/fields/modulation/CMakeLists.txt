simple_testing(field-modulation-sint-rf           "--file=modulation-sint-rfx.gmad"            "")
# (TODO REMOVED OVERLAP) simple_testing(field-modulation-singlobalt-rf     "--file=modulation-singlobalt-rfx.gmad"      "")
simple_testing(field-modulation-emap              "--file=modulation-emap-sint.gmad"              "")
simple_testing(field-modulation-lineart           "--file=modulation-lineart.gmad"              "")


# TBC
#simple_testing(field-modulation-global            "--file=modulation-global.gmad"              "")
simple_fail(field-modulation-global            "--file=modulation-global.gmad"              "")


simple_fail(field-modulation-bad-conflicting   "--file=modulation-bad-conflicting.gmad" "")