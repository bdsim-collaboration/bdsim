from .g4particledefinition import *
from .g4particletable import *
from .g4iontable import *
from . import clhep

from .bdsexception import *
from .aperture import *
from .array import *
from .atom import *
from .bdsbunch import *
from .bdslinkbunch import *
from .bdsbunchsixtracklink import *
from .bdsexecoptions import *
from .bdshitsampler import *
from .bdshitsamplerlink import *
from .bdshitscollectionsamplerlink import *
from .bdsimclass import *
from .bdsimlink import *
from .bdsoutputrooteventsampler import *
from .bdsparticlecoords import *
from .bdsparticlecoordsfull import *
from .parser import *
from .bdsparser import *
from .bdsparticledefinition import *
from .bdsiondefinition import *
from .bdslinktrackerinterface import *
from .beam import *
from .blmplacement import *
from .cavitymodel import *
from .crystal import *
from .coolingchannel import *
from .element import *
from .elementtype import *
from .fastlist import *
from .field import *
from .laser import *
from .material import *
from .modulator import *
from .newcolour import *
from .options import *
from .parameters import *
from .physicsbiasing import *
from .placement import *
from .query import *
from .region import *
from .samplerplacement import *
from .scorer import *
from .scorermesh import *
from .sym_table import *
from .tunnel import *
from .bdsim import *
from .convert import *
from .developer import *

try :
    from .ocelot import *
except :
    print("Could not find ocelot")

try :
    from .rftrack import *
except :
    print("could not find rftrack")

try :
    from .xsuite import *
except :
    print("could not find xsuite")

# singleton bdslink for rftrack, ocelot and xsuite
bdslink_singleton = None

classes = ['Aperture', 'Atom', 'Beam', 'BLMPlacement', 'CavityModel', 'CoolingChannel', 'Crystal',
           'Element', 'Field', 'Laser', 'Material', 'Modulator', 'NewColour', 'Options', 'PhysicsBiasing',
           'Placement', 'Query', 'Region', 'SamplerPlacement', 'ScorerMesh', 'Scorer', 'Tunnel']

def install_functions() :

    # function to give dict behaviour
    def dict_getitem(self, name) :
        return self.get_value(name)

    # function to copy data from one bind object to another
    def dict_copy_from(self, other) :
        for n in self.AllNames() :
            self[n] = other[n]


    def dict_eq(self, other) :
        for n in self.AllNames() :
            if self[n] != other[n] :
                return False

        return True

    def dict_repr(self) :
        s = self.__class__.__name__ + "(\n"

        names = list(self.AllNames())
        names.sort()

        for n in names :
            sv = str(self[n])
            if sv == '':
                sv = "''"
            s += n + "=" + sv +"\n"
        s += ")"
        return s

    for c in classes :
        cls = getattr(__import__(__name__), c)
        cls.copy_from = dict_copy_from
        cls.__getitem__ = dict_getitem
        cls.__repr__ = dict_repr
        cls.__eq__ = dict_eq

def install_stl_functions() :
    classes_with_stl_parameters = ['CoolingChannel','Element','Material','PhysicsBiasing','SamplerPlacement']

    def dict_list_setitem(self, name, lst) :
        array = Array(lst)
        self.set_value(key, array, false);

    for c in classes_with_stl_parameters :
        cls = getattr(__import__(__name__), c)

install_functions()
install_stl_functions()
