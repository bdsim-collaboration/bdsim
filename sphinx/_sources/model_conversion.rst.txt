================
Model Conversion
================

A BDSIM model can be prepared either manually in a hand-written fashion,
or using a provided suite of python tools to automatically convert
the description of an accelerator lattice from other formats, such as
MAD-X, MAD8 or Transport, to that of BDSIM - gmad.

The automatic conversion is typically achieved by preparing a 'rendered'
or 'flat' output description of each element in the accelerator from
whatever optics program you use to design the accelerator, then
converting this using our Python utility `pybdsim` - see :ref:`python-utilities`.

Additionally, the python tools can be used to programmatically create
an accelerator lattice of your own design, which is described in
`Python Builder`_.

Manual Preparation
==================

An input gmad (text) file can easily be prepared manually in your favourite
text editor by defining (in order):

1. Individual elements
2. The order they appear in a :code:`line`
3. Which :code:`period` to use - the above line
4. Options such as the physics list and tracking cuts
5. Input beam distribution

Please see :ref:`model-construction` for a description of the input syntax.

MAD-X Conversion
================

A MAD-X lattice can be easily converted to a BDSIM gmad input file using the supplied
python utilities. This is achieved by:

1. Preparing a TFS file with MAD-X containing all Twiss table information
2. Converting the TFS file to gmad using pybdsim.

The Twiss file can be prepared by appending the following MAD-X syntax to the
end of your MAD-X script::

  select,flag=twiss, clear;
  twiss,sequence=SEQUENCENAME, file=twiss.tfs;

where `SEQUENCENAME` is the name of the sequence in MAD-X. By not specifying the
output columns, a very large file is produced containing all possible columns.
This is required to successfully convert the lattice.  If the TFS file contains
insufficient information, `pybdsim` will not be able to convert the model. Use
`pybdsim` in Python to convert the model::

  >>> import pybdsim
  >>> pybdsim.Convert.MadxTfs2Gmad('inputfile.tfs', 'latticev1')

This is described in more detail in the dedicated `pybdsim` documentation
`<http://www.pp.rhul.ac.uk/bdsim/pybdsim/convert.html>`_.

.. note:: The python utilities require "`.tfs`" suffix as the file type to work properly.

To convert the TFS file, `pybdsim` should be used and can be installed via pip - see
:ref:`python-utilities`.


MAD8 Conversion
===============

This can be prepared in a similar fashion to a MAD-X model. The user must have our
pymad8 and `pybdsim` packages (see :ref:`python-utilities`).

This is described in more detail in the dedicated `pybdsim` documentation
`<http://www.pp.rhul.ac.uk/bdsim/pybdsim/convert.html>`_.

Python Builder
==============

Using the classes in the `pybdsim` package that are used in the conversion process,
the user may create a BDSIM model programmatically, i.e. the user can write a Python
script to create the lattice they want.

This is described in more detail in the dedicated `pybdsim` documentation
`<http://www.pp.rhul.ac.uk/bdsim/pybdsim/builder.html>`_

The pymadx package also has a similar functionality and is documented in its
dedicated documentation `<http://www.pp.rhul.ac.uk/bdsim/pymadx/moduledocs.html#module-pymadx.Builder>`_.

.. _optical-validation:

Optical Validation
==================

Once a model has been prepared or converted, it is often useful to compare the optical functions
of the accelerator to a known reference. This ensures that the model has been prepared correctly.
If, for example, a drift between magnets was too short or magnetic strengths were wrong, the
optical functions would disagree and this would clearly indicate where the problem lies. This
is an important validation step before physics studies begin, where such errors may not be so
clear.

Simulating the passage of several thousand particles validates optical functions with a
distribution according to the core Gaussian distribution. Samplers are attached to
all elements to record the particle distribution after each element. A provided optical
analysis is used to calculate optical functions and moments of the particle coordinates
that can then be compared to a reference.

Recommended Options for Generating Optics
-----------------------------------------

Options in gmad::
  
  option, physicsList="em",
          stopSecondaries=1,
	  aper1=5*cm;

  sample, all;

* If no physics is present, particles will not interact with material and will not stop if lost.
* 'Stop secondaries' will kill any secondary particles - faster and doesn't pollute data.
* A large aperture size should be use throughout to avoid clipping the beam.
* Samplers should be attached to all elements to record the beam distribution after each element.

.. note:: Attaching samplers to all elements vastly increases the output data file size and the
	  user should generally only attach samplers to points of specific interest.

A Gaussian beam according to the Twiss parameters at the start of the beam line should be used. An
emittance should be chosen that is used in the source of optics (i.e. in the MAD-X model and therefore
appears in the header of the TFS Twiss output), but an emittance that ensures the beam size is small
enough throughout the machine to avoid particles clipping.

A target number of particles is 1000 for linear optics with no energy spread and approximately 10000
for a beam with energy spread. The optical analysis calculates the statistical uncertainty associated
with estimating each parameter, given there is a finite number of particles. With approximately 1000
particles, it is very unlikely to have any particles beyond 3:math:`\sigma` for a Gaussian distribution,
therefore as long as most apertures are above this, no particles should be lost.

Generating Optics Data
----------------------

BDSIM should be executed in batch mode running between 1000 and 10000 particles (for example). ::

  bdsim --file=mymodel.gmad --outfile=op1 --batch --ngenerate=2000

Analysing Optics Data
---------------------

The output file can then be analysed with `rebdsimOptics`. `rebdsimOptics` is a simple interface to
`rebdsim` to calculate optical functions only. It is possible to run `rebdsim` with an analysis
configuration text file specifying `CalculateOpticalFunctions` (see :ref:`output-analysis-configuration-file`).
Rebdsim takes arguments as the input file(s) and the desired output file name for the optical functions.
The optical functions are written to a separate ROOT format file only containing the optical functions. ::

  rebdsimOptics op1.root op1_optics.root

To calculate the optical functions, the emittance is calculated from the distribution at each sampler.
In this case, only linear optics should be used, as non-linear optical elements, such as sextupoles
and higher order magnets, couple the emittance in horizontal and vertical directions and the calculation
is invalid for Twiss parameters. The default option in `rebdsimOptics` is to calculate the emittance only
once for the first sampler and assume this for all subsequent samplers. In the case of acceleration, or
where the geometrical emittance is expected to change, the emittance can be calculated at each sampler
freshly. The executable option `-\\-emittanceOnFly` should be used. ::

  rebdsimOptics op1.root op1_optics.root --emittanceOnFly

.. note:: When using rebdsimOptics to analyse multiple files for optics, the input file name should
	  be surrounded with inverted commas - i.e. :code:`rebdsimOptics "*.root" optics.root`.

Loading Optical Data
--------------------

After calculating the optical functions, the data file can be loaded in ROOT manually or
using the provided `pybdsim` utility in Python using ROOT. ::

  > python
  >>> import pybdsim
  >>> d = pybdsim.Data.Load("op1_optics.root")
  >>> d.optics

This provides arrays of all the optical functions in a dictionary.
  
Comparison of Optics
--------------------

For each of the formats BDSIM supports for converting models, there is a comparison plotting script
in `pybdsim` to allow easy comparison. For MAD-X conversion, for example, the Twiss output in a TFS
file can be used. ::

  > python
  >>> import pybdsim
  >>> pybdsim.Compare.MadxVsBDSIM('madxtwiss.tfs', 'op1_optics.root')

This will create a series of plots with both the optical functions from MAD-X and those calcualted by
`rebdsimOptics` on the same plot with a colour machine diagram on top. A few example plots are shown
below.

All plots are also written by default to a single pdf file whose name is by default based on the
the `rebdsimOptics` output file name given to the comparison. In this case, it would be `op1_optics.pdf`.


.. figure:: figures/optics/mean.pdf
	    :width: 100%
	    :align: center

	    Example comparison of beam centroids for ATF2.

.. figure:: figures/optics/sigma.pdf
	    :width: 100%
	    :align: center

	    Example comparison of beam size for ATF2.

.. figure:: figures/optics/sigmap.pdf
	    :width: 100%
	    :align: center

	    Example comparison of :math:`\sigma_{xp, yp}` for ATF2.

.. figure:: figures/optics/beta.pdf
	    :width: 100%
	    :align: center

	    Example comparison of Twiss :math:`\beta` functions.
	       
.. figure:: figures/optics/alpha.pdf
	    :width: 100%
	    :align: center

	    Example comparison of Twiss :math:`\alpha` functions.


Comparison Notes
----------------

It is worth noting that we are drawing a sample of particle coordinates from a probability distribution
and then calculating moments of the sample distribution to estimate the original parameters of the
probability distribution. With a higher number of samples, the estimate improves but is still an estimate.
The optical comparison is useful to validate the model preparation and extremely small differences in
optical functions should not be interpreted as poor tracking.

Advanced Comparison
-------------------

For very precise comparison, there are two further possible options that can be used.

1) BDSIM can be compiled with double precision output as opposed to the default floating-point
   precision. This is a CMake option (see :ref:`configuring-bdsim`). The option `ROOT_DOUBLE_OUTPUT`
   should be turned on and BDSIM recompiled and installed.

2) :code:`beam, offsetSampleMean=1` should be used in the input gmad. This pre-calculates all coordinates
   of the particles and subtracts the small sample mean from each coordinate, removing any small systematic
   offset at the beginning that typically propagates throughout the beam line. This will not work in the
   visualiser and breaks the strong-reproducibility in BDSIM - use with caution.



