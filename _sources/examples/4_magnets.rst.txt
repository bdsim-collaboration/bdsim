Magnet Geometry
===============

one_of_each_base.gmad
---------------------

A base file that's used (:code:`include one_of_each_base.gmad`) by
many examples in this directory. It's loosely based on the one of each
example in layout and contains one of each of the magnetic elements in
BDSIM. Each example demonstrates a different geometry possible.

1_cylindrical.gmad
------------------

Key code::

  magnetGeometryType="cylindrical";

This example uses the sequence of magnets defined in one_of_each_base.gmad
and ensures that they have cylindrical geometry. This is a little redundant
as the default is cylindrical.

How to run::
  
  bdsim --file=1_cylindrical.gmad

.. figure:: 1_cylindrical.png
	    :width: 70%

2_poles_circular.gmad
---------------------

Key code::

  magnetGeometryType="polescircular";

As above but with poled geometry and a circular yoke.

How to run::
  
  bdsim --file=2_poles_circular.gmad

.. figure:: 2_poles_circular.png
	    :width: 70%


3_poles_square.gmad
---------------------

Key code::

  magnetGeometryType="polessquare";

As above but with poled geometry and a square yoke.

How to run::
  
  bdsim --file=3_poles_square.gmad

.. figure:: 3_poles_square.png
	    :width: 70%

4_poles_face.gmad
-----------------

Key code::

  magnetGeometryType="polesfacet";

As above but with poled geometry and a square yoke rotated by 45 degrees.

How to run::
  
  bdsim --file=4_poles_facet.gmad

.. figure:: 4_poles_facet.png
	    :width: 70%


5_poles_face_crop.gmad
----------------------

Key code::

  magnetGeometryType="polesfacetcrop";

As above but with poled geometry and a square yoke rotated by 45 degrees.
Additionally, the corner edges are cropped giving the yoke, :math:`n x 2`
edges, where :math:`n` is the number of poles the magnet has.

How to run::
  
  bdsim --file=5_poles_facet_crop.gmad

.. figure:: 5_poles_facet_crop.png
	    :width: 70%


6_lhcleft.gmad & 7_lhcright.gmad
--------------------------------

Key code::

  magnetGeometryType="lhcleft";
  magnetGeometryType="lhcright";

LHC cyrogenic magnet for dipoles and quadrupoles.  Given the two beam nature
of the LHC, the magnet is either offset to the right or two the left dictating
the 'active' beam pipe.

.. figure:: 7_lhcleft.png
	    :width: 70%

8_none.gmad
-----------

Key code::

  magnetGeometryType="none";

No magnet outer geometry is built around the beam pipe - only the beam pipe exists.

.. figure:: 8_none.png
	    :width: 70%

9_coils.gmad
------------

A test with carefully split and spaced quadrupoles to test the automatic choice to place
or not the coils at the end of a magnet.

.. figure:: 9_coils.png
	    :width: 70%

10_size_variation_circ_quad.gmad
--------------------------------

An example / test to show the scaling of the circular outer geometry.

.. figure:: 10_size_variation_circ_quad.png
	    :width: 70%
	    

11_size_variation_circ_sext.gmad
--------------------------------

An example / test to show the scaling of the circular outer geometry.

.. figure:: 11_size_variation_circ_sext.png
	    :width: 70%
	    

12_size_variation_circ_oct.gmad
-------------------------------

An example / test to show the scaling of the circular outer geometry.

.. figure:: 12_size_variation_circ_oct.png
	    :width: 70%

		    
13_size_variation_circ_dec.gmad
-------------------------------

An example / test to show the scaling of the circular outer geometry.

.. figure:: 13_size_variation_circ_dec.png
	    :width: 70%
	    

14_size_variation_sq_quad.gmad
------------------------------

An example / test to show the scaling of the square outer geometry.

.. figure:: 14_size_variation_sq_quad.png
	    :width: 70%

		    
15_size_variation_sq_sext.gmad
------------------------------

An example / test to show the scaling of the square outer geometry.

.. figure:: 15_size_variation_sq_sext.png
	    :width: 70%
		    

16_size_variation_sq_oct.gmad
-----------------------------

An example / test to show the scaling of the square outer geometry.

.. figure:: 16_size_variation_sq_oct.png
	    :width: 70%

		    
17_size_variation_sq_dec.gmad
-----------------------------

An example / test to show the scaling of the square outer geometry.

.. figure:: 17_size_variation_sq_dec.png
	    :width: 70%

		    
18_size_variation_facet_quad.gmad
---------------------------------

An example / test to show the scaling of facet outer geometry.

.. figure:: 18_size_variation_facet_quad.png
	    :width: 70%

		    
19_size_variation_facet_sext.gmad
---------------------------------

An example / test to show the scaling of facet outer geometry.

.. figure:: 19_size_variation_facet_sext.png
	    :width: 70%


20_size_variation_facet_oct.gmad
--------------------------------

An example / test to show the scaling of facet outer geometry.

.. figure:: 20_size_variation_facet_oct.png
	    :width: 70%


21_size_variation_facet_dec.gmad
--------------------------------

An example / test to show the scaling of facet outer geometry.

.. figure:: 21_size_variation_facet_dec.png
	    :width: 70%


22_size_variation_facetcrop_quad.gmad
-------------------------------------

An example / test to show the scaling of facetcrop outer geometry.

.. figure:: 22_size_variation_facetcrop_quad.png
	    :width: 70%


23_size_variation_facetcrop_sext.gmad
-------------------------------------

An example / test to show the scaling of facetcrop outer geometry.

.. figure:: 23_size_variation_facetcrop_sext.png
	    :width: 70%


24_size_variation_facetcrop_oct.gmad
------------------------------------

An example / test to show the scaling of facetcrop outer geometry.

.. figure:: 24_size_variation_facetcrop_oct.png
	    :width: 70%


25_size_variation_facetcrop_dec.gmad
------------------------------------

An example / test to show the scaling of facetcrop outer geometry.

.. figure:: 25_size_variation_facetcrop_dec.png
	    :width: 70%


26_size_variation_sbend_c.gmad
------------------------------

An example / test to show the scaling of sector bend outer geometry.

.. figure:: 26_size_variation_sbend_c.png
	    :width: 70%


27_size_variation_rbend_c.gmad
------------------------------

An example / test to show the scaling of rectangular bend outer geometry.

.. figure:: 27_size_variation_rbend_c.png
	    :width: 70%

		    
28_size_variation_hkicker_c.gmad
--------------------------------

An example / test to show the scaling of horizontal kicker outer geometry.

.. figure:: 28_size_variation_hkicker_c.png
	    :width: 70%

		    
29_size_variation_vkicker_c.gmad
--------------------------------


An example / test to show the scaling of vertical kicker outer geometry.

.. figure:: 29_size_variation_vkicker_c.png
	    :width: 70%

30_dipole_pole_face.gmad
------------------------

An example / test to show sector bend with pole face rotation outer geometry.

.. figure:: 30_dipole_pole_face.png
	    :width: 70%
