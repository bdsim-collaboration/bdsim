Target
======

* Based on :code:`bdsim/examples/target/target-p-cu.gmad`

A simple example of a BDSIM model. It has a drift followed by a closed
collimator which forms a solid block of material, which is copper. This
example is used in the worked example in detail - see
:ref:`example-worked-target`.

How to run::
  
  bdsim --file=target-p-cu.gmad --outfile=data

.. figure:: ../worked_example_target/target-p-cu-10events.png
	    :width: 100%
	    :align: center


When the visualiser starts, the following command can be used: ::

  /run/beamOn 5

As a lot of secondary particles are produced it is recommneded to
only run a small number of events.
