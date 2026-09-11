import bdsim


def test_element_type_bindings():
    expected_values = {
        "BMCOL": 12,
        "LASERWIREOLD": 15,
        "GASCAP": 18,
        "GASJET": 19,
        "MUONCOOLER": 79,
        "JCOLTIP": 80,
        "GABORLENS": 81,
        "LASERWIRE": 82,
        "LASER": 83,
    }

    for name, expected_value in expected_values.items():
        element_type = getattr(bdsim.ElementType, name)
        assert element_type.name == name
        assert element_type.value == expected_value

    assert bdsim.ElementType.LASERWIREOLD != bdsim.ElementType.LASERWIRE
    assert bdsim.ElementType.LASERWIRE != bdsim.ElementType.LASER
