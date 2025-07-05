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
from .bdsparser import *
from .bdsparticledefinition import *
from .bdsiondefinition import *
from .bdslinktrackerinterface import *
from .beam import *
from .blmplacement import *
from .cavitymodel import *
from .crystal import *
from .element import *
from .elementtype import *
from .fastlist import *
from .field import *
from .material import *
from .modulator import *
from .newcolour import *
from .options import *
from .parameters import *
from .parser import *
from .physicsbiasing import *
from .placement import *
from .query import *
from .region import *
from .samplerplacement import *
from .scorermesh import *
from .sym_table import *
from .tunnel import *
from .bdsim import *

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
