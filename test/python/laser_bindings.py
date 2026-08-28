import fastlist  # Register FastList specialisations before returning one.
import parser    # Register the GMAD::Parser base class before BDSParser.
from bdsparser import BDSParser
from laser import Laser


laser = Laser()
laser["name"] = "laser_binding_test"
laser["wavelength"] = 532e-9
laser["pulseEnergy"] = 1.5
laser["ignoreRayleighRange"] = True

assert laser.name == "laser_binding_test"
assert laser.get_value("wavelength") == 532e-9
assert laser.get_value("pulseEnergy") == 1.5
assert laser.get_value("ignoreRayleighRange") is True

bds_parser = BDSParser()
parser_laser = bds_parser.GetGlobal_Laser()
parser_laser.clear()
parser_laser["name"] = "registered_laser"
parser_laser["wavelength"] = 1064e-9
bds_parser.Add_Laser()

lasers = list(bds_parser.GetList_Laser())
registered = next(item for item in lasers if item.name == "registered_laser")
assert registered.wavelength == 1064e-9
