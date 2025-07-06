.. macro for non breaking white space useful or units:
.. |nbsp| unicode:: 0xA0
   :trim:

.. _input-syntax:

============
Input Syntax
============

BDSIM is controlled by input text (ASCII) files that define the model, physics and
options in its own GMAD syntax.

* :ref:`gmad-syntax`
* :ref:`mathematical-functions`
* :ref:`coordinates-and-units`

.. _gmad-syntax:

GMAD Syntax
-----------

GMAD is a language specifically for BDSIM that is made to be human readable.
The name comes from the design intention of MAD-X syntax and extensions for Geant4.
While GMAD is very similar to MAD-X, not all MAD-X commands are supported.

* S.I. units are used except where explicitly specified
* Variables can be defined using :code:`name = value;` syntax
* Arithmetic expressions can be defined
* Binary operators +, -, \*, /, ^, are valid
* Unary operators +, -, are valid
* Boolean operators <, >, <=, >=, <> (not equal), ==, are valid
* Every expression **must** end with a semi-colon;
* No variable name can begin with a number
* !Comments start with an exclamation mark "!"
* A variable may inherit values (via copy) from another variable using :code:`newvariable : existingvariable;`

Extending Objects
^^^^^^^^^^^^^^^^^

If an object (or beam line element) is already defined, a parameter can be updated in it. We call this 'extending'
the object. Examples can be found in :code:`bdsim/parser/test/extend*`. E.g. ::

  d1: drift, l=1*m;
  print, d1;
  ! now we update the d1 definition
  d1: aper1=5*cm;


.. note:: Beam line elements can only effectively be updated **before** the :code:`use` command
	  is called as this command will copy the current definitions to a new place in memory.

.. warning:: When extending an object the parameter "type" must not be used first after the semi-colon.
	     This is a limitation of the parser that understands the input syntax. Any other parameter
	     can be updated first then (if appropriate) "type" can be set.


.. _mathematical-functions:
  
Mathematical Functions
^^^^^^^^^^^^^^^^^^^^^^

The following mathematical functions are provided:

* sqrt
* cos
* sin
* tan
* exp
* log
* acos
* asin
* atan
* abs

Other Commands
^^^^^^^^^^^^^^

* :code:`print;` Prints all elements
* :code:`print, line;` Prints all elements that are in the beam line defined by :code:`use`. See also :ref:`the-use-command`.
* :code:`print, option;` Prints the value of some options
* :code:`print, variable;` Prints the value of a numerical variable, which could be your own defined variable
* :code:`length = d1["l"];` A way to access properties of elements, in this case, length of element d1.
* :code:`stop;` or :code:`return;` Exists parser
* :code:`if () {};` 'if' construct
* :code:`if () {} else {};` 'if-else' construct
* :code:`include ../some/other/file.gmad;` Includes another file to be parsed. Note that the path provided must be relative, not absolute.

Examples
^^^^^^^^

Examples: ::

   x = 1;
   y = 2.5-x;
   z = sin(x) + log(y) - 8e5;
   mat = "copper";


Floating Point and Integer Numbers
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The result of a calculation may be a floating point number. If assigned to what should
be an integer value, such as the number of bins in a scoring mesh, the number is rounded
to the nearest integer.

   
Common Pitfalls
^^^^^^^^^^^^^^^

The following is an example of a common mistake that's not easy to spot: ::

  beam, particle="e-",
        energy=3*TeV;
	E0=1*TeV;

With this syntax we expect to create a design beam of 3 TeV electrons but the central energy of 1 TeV for
the bunch. 3 TeV is used to calculate the magnet strengths and it's expected to fire a 1 TeV electron.
However, the E0 parameter here just defines a variable called E0 that isn't used. The indentation (white-space)
is ignored. The error is the semi-colon at the end of the second line. This is the correct version: ::

  beam, particle="e-",
        energy=3*TeV,
	E0=1*TeV;

   
.. _coordinates-and-units:
   
Coordinates & Units
-------------------

In Geant4, global Euclidean coordinates are used for tracking purposes. However,
in describing a lattice with BDSIM, curvilinear coordinates are used, as is common with
accelerators (X,Y,S).

**GMAD uses SI units**

==============================  =========================
Name                            Units
==============================  =========================
length                          [m] (metres)
time                            [s] (seconds)
angle                           [rad] (radians)
quadrupole coefficient          [m :math:`^{-2}` ]
multipole coefficient 2n poles  [m :math:`^{-n}` ]
electric voltage                [V] (Volts)
electric field strength         [V/m]
particle energy                 [GeV]
particle mass                   [GeV/c :math:`^2` ]
particle momentum               [GeV/c :math:`^2` ]
beam current                    [A] (Amperes)
particle charge                 [e] (elementary charges)
emittance                       [pi m mrad]
density                         [g/cm :math:`^{3}` ]
temperature                     [K] (Kelvin)
pressure                        [atm] (atmosphere)
frequency                       [Hz] (Hertz)
mass number                     [g/mol]
==============================  =========================

Some useful predefined values / units are:

==========  =================================
Name        Value
==========  =================================
pi          3.14159265358979
twopi       2 * pi
halfpi      0.5 * pi
degrees     :math:`\pi` / 180
GeV         1
eV          :math:`10^{-9}`
keV         :math:`10^{-6}`
MeV         :math:`10^{-3}`
TeV         :math:`10^{3}`
PeV         :math:`10^{6}`
J           1
PJ          :math:`10^{12}`
GJ          :math:`10^{9}`
MJ          :math:`10^{6}`
kJ          :math:`10^{3}`
mJ          :math:`10^{-3}`
uJ          :math:`10^{-6}`
nJ          :math:`10^{-9}`
pJ          :math:`10^{-12}`
V           1
mV          :math:`10^{-3}`
kV          :math:`10^{3}`
MV          :math:`10^{6}`
GV          :math:`10^{9}`
Tesla       1
T           1
rad         1
mrad        :math:`10^{-3}`
urad        :math:`10^{-6}`
nrad        :math:`10^{-9}`
clight      :math:`2.99792458 \times 10^{8}`
km          :math:`10^{3}`
m           1
cm          :math:`10^{-2}`
mm          :math:`10^{-3}`
um          :math:`10^{-6}`
mum         :math:`10^{-6}`
nm          :math:`10^{-9}`
ang         :math:`10^{-10}`
pm          :math:`10^{-12}`
s           1
ms          :math:`10^{-3}`
us          :math:`10^{-6}`
ns          :math:`10^{-9}`
ps          :math:`10^{-12}`
Hz          1
kHz         :math:`10^{3}`
MHz         :math:`10^{6}`
GHz         :math:`10^{9}`
THz         :math:`10^{12}`
==========  =================================

As an example, one can write either :code:`100*eV` or :code:`0.1*keV` to specify an energy value in GMAD.
Both are equivalent.
