Histograms in Python
====================

Topics Covered
--------------

* TH1 histograms in Python
* pybdsim histograms in Python
* Weighted combinations of histograms

* Based on example data files in :code:`bdsim/examples/features/data`.

Contents
--------

* `Preparation`_
* `Output Histograms`_
* `Loading Histograms`_
* `Plotting Histograms`_
* `Adding Histograms`_
* `Multiplying a Histogram`_
* `ROOT Histogram Operations`_
* `Weighted Sum Of Histograms`_


Preparation
-----------

* BDSIM has been compiled and installed.
* The (DY)LD_LIBRARY_PATH and ROOT_INCLUDE_PATH environmental variables are set as
  described in :ref:`output-analysis-setup`.
* ROOT can be imported in Python
* `pybdsim` has been installed.

IPython is preferred to work interactively with Python. Using this any function
or class can be queried to see how to use it. Therefore, we can explore with
tab complete and use of the question mark. ::

  ipython
  >>> import pybdsim
  >>> pybdsim.
             pybdsim.Beam            pybdsim.Convert         pybdsim.ModelProcessing  
             pybdsim.Builder         pybdsim.Data            pybdsim.Options          
             pybdsim.Compare         pybdsim.Field           pybdsim.Plot            >
             pybdsim.Constants       pybdsim.Gmad            pybdsim.Run
  >>> pybdsim.Data.TH1?
  Init signature: pybdsim.Data.TH1(hist, extractData=True)
  Docstring:     
  Wrapper for a ROOT TH1 instance. Converts to numpy data.
  
  >>> h = file.Get("histogramName")
  >>> hpy = TH1(h)
  File:           ~/physics/reps/pybdsim/pybdsim/Data.py
  Type:           type
  Subclasses:     TH2


Output Histograms
-----------------

Histograms can be produced 'online' by BDSIM during a simulation (e.g energy loss),
by rebdsim from analysing raw output, or indepently using ROOT. Ultimately, we assume
for this tutorial we have a ROOT histogram (TH1, TH2, TH3 classes) in a ROOT file.

ROOT histograms, once loaded, can be plotted easily with ROOT, however, a common
package is matplotlib in Python to make more modern plots. ROOT histograms cannot
be easily plotted using matplotlib without either writing some code to get the
necessary information or by using some software package.

pybdsim provides some simple classes that can be used to get the information
from a ROOT histogram into that class, which has numpy arrays. It is worth
noting though that ROOT histograms have some very nice functionality that
can be used before plotting.

Loading Histograms
------------------

pybdsim
*******

We recommend using pybdsim for this. ::

  ipython
  >>> import pybdsim
  >>> d = pybdsim.Data.Load("bdsim/examples/features/data/combined-ana.root")

Here, :code:`d` is an instance of a pybdsim class that holds the data. The Load
function returns different types depending on whether a raw BDSIM file is loaded
or a rebdsim histogram file is loaded. We can look inside at the histograms by
typing :code:`d.histograms` and pressing the TAB key (in IPython only). ::
  
  >>> d.histograms
                     d.histograms     d.histograms2d   d.histograms3dpy
                     d.histograms1d   d.histograms2dpy d.histogramspy  
                     d.histograms1dpy d.histograms3d                   

Each of these objects is a dictionary (or 'map') from a string to a histogram.
The string is the path of the histogram inside the file. We can have a look
by simply typing :code:`d.histogram`. ::

  >>> d.histograms
  {'Event/PerEntryHistograms/PrimaryX': <cppyy.gbl.TH1D object at 0x7fb4566d69c0>,
   'Event/PerEntryHistograms/EventDuration': <cppyy.gbl.TH1D object at 0x7fb44cb98810>,
   'Event/PerEntryHistograms/EnergySpectrum': <cppyy.gbl.TH1D object at 0x7fb44cb98fd0>,
   'Event/PerEntryHistograms/EnergyLossManual': <cppyy.gbl.TH1D object at 0x7fb44cb999b0>,
   'Event/PerEntryHistograms/TunnelLossManual': <cppyy.gbl.TH1D object at 0x7fb44cb9a360>,
   'Event/PerEntryHistograms/AperImpactXInRange': <cppyy.gbl.TH1D object at 0x7fb44cb9ac60>,
   'Event/PerEntryHistograms/PrimaryXY': <cppyy.gbl.TH2D object at 0x7fb44e8c2800>,
   'Event/PerEntryHistograms/D2XYEnergy': <cppyy.gbl.TH3D object at 0x7fb44e8c4e00>,
   'Event/SimpleHistograms/PrimaryYSimple': <cppyy.gbl.TH1D object at 0x7fb459a19c50>,
   'Event/SimpleHistograms/PrimaryXYSimple': <cppyy.gbl.TH2D object at 0x7fb44b11fe00>,
   'Event/MergedHistograms/PhitsHisto': <cppyy.gbl.TH1D object at 0x7fb459a1b920>,
   'Event/MergedHistograms/PlossHisto': <cppyy.gbl.TH1D object at 0x7fb459a1c120>,
   'Event/MergedHistograms/ElossHisto': <cppyy.gbl.TH1D object at 0x7fb459a1c6c0>,
   'Event/MergedHistograms/PhitsPEHisto': <cppyy.gbl.TH1D object at 0x7fb459a1d0b0>,
   'Event/MergedHistograms/PlossPEHisto': <cppyy.gbl.TH1D object at 0x7fb459a1d970>,
   'Event/MergedHistograms/ElossPEHisto': <cppyy.gbl.TH1D object at 0x7fb459a1e390>,
   'Event/MergedHistograms/PFirstAIHisto': <cppyy.gbl.TH1D object at 0x7fb459a1e780>,
   'Event/MergedHistograms/ElossTunnelHisto': <cppyy.gbl.TH1D object at 0x7fb459a1ee30>,
   'Event/MergedHistograms/ElossTunnelPEHisto': <cppyy.gbl.TH1D object at 0x7fb459a1f4d0>}

Here, we have loaded the example file :code:`bdsim/examples/features/data/combined-ana.root`,
which is the sum of two analysis files of 10 events each.

We see the path of the file for each entry in the dictionary and the the 'value' is
a ROOT histogram object through ROOT's Python interface.

pybdsim also provides a python version of each of these in similar dictionaries, but
with the suffix "py". ::

  >>> d.histogramspy
  {'Event/PerEntryHistograms/PrimaryX': <pybdsim.Data.TH1 at 0x15cb04e20>,
   'Event/PerEntryHistograms/EventDuration': <pybdsim.Data.TH1 at 0x15cb04d60>,
   'Event/PerEntryHistograms/EnergySpectrum': <pybdsim.Data.TH1 at 0x15cb170d0>,
   'Event/PerEntryHistograms/EnergyLossManual': <pybdsim.Data.TH1 at 0x15cb180a0>,
   'Event/PerEntryHistograms/TunnelLossManual': <pybdsim.Data.TH1 at 0x15cb180d0>,
   'Event/PerEntryHistograms/AperImpactXInRange': <pybdsim.Data.TH1 at 0x15cb18100>,
   'Event/SimpleHistograms/PrimaryYSimple': <pybdsim.Data.TH1 at 0x15cb18130>,
   'Event/MergedHistograms/PhitsHisto': <pybdsim.Data.TH1 at 0x15cb18160>,
   'Event/MergedHistograms/PlossHisto': <pybdsim.Data.TH1 at 0x15cb18190>,
   'Event/MergedHistograms/ElossHisto': <pybdsim.Data.TH1 at 0x15cb181c0>,
   'Event/MergedHistograms/PhitsPEHisto': <pybdsim.Data.TH1 at 0x15cb181f0>,
   'Event/MergedHistograms/PlossPEHisto': <pybdsim.Data.TH1 at 0x15cb18220>,
   'Event/MergedHistograms/ElossPEHisto': <pybdsim.Data.TH1 at 0x15cb18250>,
   'Event/MergedHistograms/PFirstAIHisto': <pybdsim.Data.TH1 at 0x15cb18280>,
   'Event/MergedHistograms/ElossTunnelHisto': <pybdsim.Data.TH1 at 0x15cb182b0>,
   'Event/MergedHistograms/ElossTunnelPEHisto': <pybdsim.Data.TH1 at 0x15cb182e0>,
   'Event/PerEntryHistograms/PrimaryXY': <pybdsim.Data.TH2 at 0x15cb18310>,
   'Event/SimpleHistograms/PrimaryXYSimple': <pybdsim.Data.TH2 at 0x15cb18340>,
   'Event/PerEntryHistograms/D2XYEnergy': <pybdsim.Data.TH3 at 0x15cb18370>}

Here, all the names are the same but we can see the objects are instances of pybdsim
classes.

This is the full set of histograms. For convenience, the same histograms are also included
in a dictionary for each dimensionality.

+----------------------+-----------------------------------------+
| **Dictionary Name**  | **Contents**                            |
+======================+=========================================+
| histograms           | ROOT instances of all histograms        |
+----------------------+-----------------------------------------+
| histogramspy         | pybdsim instances of all histograms     |
+----------------------+-----------------------------------------+
| histograms1d         | ROOT instances of all 1D histograms     |
+----------------------+-----------------------------------------+
| histograms1dpy       | pybdsim instances of all 1D histograms  |
+----------------------+-----------------------------------------+
| histograms2d         | ROOT instances of all 2D histograms     |
+----------------------+-----------------------------------------+
| histograms2dpy       | pybdsim instances of all 2D histograms  |
+----------------------+-----------------------------------------+
| histograms3d         | ROOT instances of all 3D histograms     |
+----------------------+-----------------------------------------+
| histograms3dpy       | pybdsim instances of all 3D histograms  |
+----------------------+-----------------------------------------+

Each histogram can be accessed from the dictionary by its name: ::

  >>> h = d.histograms['Event/PerEntryHistograms/PrimaryX']

The pybdsim ones contain numpy arrays and only Python types.

ROOT
****

In Python, we can also load ROOT histograms using ROOT's Python interface. ::

  ipython
  >>> import ROOT
  >>> f = ROOT.TFile("bdsim/examples/features/data/combined-ana.root")
  >>> h = f.Get("'Event/PerEntryHistograms/PrimaryX")

Plotting Histograms
-------------------

pybdsim provides severl plotting functions for pybdsim histograms for convenience. The
user may of course, write their own in Python or ROOT. ::

  ipython
  >>> import pybdsim
  >>> d = pybdsim.Data.Load("bdsim/examples/features/data/cmobined-ana.root")
  >>> h = d.histogramspy['Event/PerEntryHistograms/PrimaryX']
  >>> pybdsim.Plot.Histogram1D(h)

Several functions are available:

* Histogram1D
* Histogram1DMultiple
* Histogram2D
* Histogram2DErrors

See the pybdsim documentation: :ref:`python-utilities-documentation`.
  
Adding Histograms
-----------------

To add histograms, it is recommended to use the ROOT version of them in Python, then
convert to a pybdsim one for plotting. ::

  ipython
  >>> import pybdsim
  >>> d1 = pybdsim.Data.Load("bdsim/examples/features/data/ana1.root")
  >>> d2 = pybdsim.Data.Load("bdsim/examples/features/data/ana1.root")
  >>> h1 = d.histograms['Event/PerEntryHistograms/PrimaryX']
  >>> h2 = d.histograms['Event/PerEntryHistograms/PrimaryX']
  >>> htotal = h1.Add(h2)

Note, this will also calculate the errors correctly (even if there are different numbers of
events in each histogram).

Multiplying a Histogram
-----------------------

Multiplication in ROOT is called "scale": ::
  
  ipython
  >>> import pybdsim
  >>> d1 = pybdsim.Data.Load("bdsim/examples/features/data/ana1.root")
  >>> d1.Scale(3.0)


ROOT Histogram Operations
-------------------------

Some useful functions in a TH1 in ROOT:

* h.Add(otherHistogram) - add two histograms
* h.Scale(number) - multiply by a number
* h.Divide(otherHistogram) - the ratio of two histograms
* h.Integral()

ROOT has extensive documentation on the histograms: https://root.cern.ch/doc/master/classTH1.html#normalizing .

Weighted Sum Of Histograms
--------------------------

To make a weighted sum of histograms we can use combinations of the above methods. We assume
each histogram has the same shape (i.e. number of dimension and also number of bins and ranges).

If we know all histograms are the same, then we should copy ("clone") the first one and empty it,
then add each to that one. ::

  >>> histograms = [h1,h2,h3,h4]  # list of ROOT histograms
  >>> weights    = [0.25,0.25,0.1,0.4]
  >>> htotal = histograms[0].Clone().Reset()
  >>> for h,w in zip(histograms, weights):
          htotal.Add(h,w)
  >>> htotalpy = pybdsim.Data.TH1(htotal)
  >>> pybdsim.Plot.Histogram1D(htotalpy)


