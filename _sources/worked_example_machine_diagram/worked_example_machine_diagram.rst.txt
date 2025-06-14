Machine Diagram
===============

Topics Covered
--------------

* Adding a machine diagram to a plot

Plotting
--------

BDSIM by default stores a record of the "model" in the Model tree in
each output file. This can be used to add a machine diagram to a plot
using `pybdsim`.

* Using data from :code:`bdsim/examples/features/data/`.

::

   cd bdsim/examples/feautres/data
   ipython
   >>> import pybdsim
   >>> import matplotlib.pyplot as plt
   >>> d = pybdsim.Data.Load("ana1.root")
   >>> h = d.histograms1dpy["Event/PerEntryHistograms/EnergyLossManual"]
   >>> bw =  "{:.2f}".format(h.xwidths[0])
   >>> pybdsim.Plot.Histogram1D(h, "S (m)", "Energy Deposition (GeV / "+bw+" m / Event)")
   >>> pybdsim.Plot.AddMachineLatticeFromSurveyToFigure(plt.gcf(), d.model)


.. figure:: worked_example_machine_diagram.pdf
	    :align: center
	    :width: 100%

