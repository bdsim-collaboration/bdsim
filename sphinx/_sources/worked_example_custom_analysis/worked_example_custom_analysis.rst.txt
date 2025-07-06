Custom Analysis
===============

Topics Covered
--------------

* Per-event data structures and loading
* Histogramming data
* Using rebdsim classes in custom analysis

* Based on models in :code:`bdsim/examples/trajectories`.

Contents
--------

* `Preparation`_
* `Generating Data To Analyse`_
* `Simple Data Loop`_
* `Rebdsim Average Histograms`_


Preparation
-----------

* BDSIM has been compiled and installed.
* The (DY)LD_LIBRARY_PATH and ROOT_INCLUDE_PATH environmental variables are set as
  described in :ref:`output-analysis-setup`.
* ROOT can be imported in Python
* `pymadx` and `pybdsim` have been installed.

.. note:: This example model will only work with Geant4.10.4 and upwards.

Generating Data To Analyse
--------------------------

For the purpose of this example we need to generate a data file to analyse. We
use the example described in :ref:`worked-example-trajectory-generation`. We can
generate a data file with: ::

  cd bdsim/examples/trajectories
  bdsim --file=proton-target.gmad --outfile=run1 --batch --ngenerate=200 --seed=123

This will generate a file called :code:`run1.root` that we will use to analyse.

Broadly speaking, the example generates trajectories of muons and neutrinos with
their link back the primary proton (e.g. likely a pion). The model uses biasing so
we must use the weights in the data.

Simple Data Loop
----------------

Using pybdsim, which in turn uses the Python interface to ROOT, we can load the data
and see all of it individually. pybdsim internally uses ROOT to load the bdsim shared
libraries that contain the definitions of all the C++ classes used in the output. This
script is in :code:`bdsim/examples/trajectories/simpleloop.py` and contains: ::

  import pybdsim
  import ROOT

  def Analyse(filename, outputfilename):
  d = pybdsim.Data.Load(filename)

  particles    = [13,-13,14,-14]
  particlesStr = [str(p) for p in particles]
  names        = [r'$\mu^-$',     r'$\mu^+$',
                  r'$\nu_{\mu}$', r'$\overline{\nu}_{\mu}$']
    
  # setup histograms - 1x 2D histogram in X,Z for each particle
  oZX = {}
  for p,ps in zip(particles,particlesStr):
      # x dimension of histogram: 50 bins in Z from 0 to 10m
      # y dimension of histogram: 50 bins in X from -1 to 1 m
      # note the histogram has no concept of units - it's purely just a
      # number so we need to choose ranges appropriately
      h = ROOT.TH2D("Origin_ZX_"+ps, "Origin of "+ps,
                    50, 0, 10,
                    50, -0.5, 0.5)
      oZX[p] = h       

  eventTree = d.GetEventTree()
  # using this python syntax for name in thing, ROOT will make the variable
  # 'name' ('event') here, have the exact same layout as we see in a TBrowser
  # and as is in the file.
  for event in eventTree:
      # now loop over the trajectories stored for this event
      for i in range(event.Trajectory.n):
          # get the particle ID of the trajectory
          partID = event.Trajectory.partID[i]
          if partID not in particles:
              continue # skip ones we don't want to histogram

	  # i-th trajectory and the 0th (ie first) point of the trajectory position vector
          originPos = event.Trajectory.XYZ[i][0]
          x,z = originPos[0],originPos[2]
          weight = event.Trajectory.postWeights[i][0]

          oZX[partID].Fill(z, x, weight)

  # now write histograms out to a new ROOT file
  f = ROOT.TFile(outputfilename, "RECREATE")
  for pid,histogram in oZX.items():
      histogram.AddDirectory(True) # bdsim / rebdsim classes turn off AddDirectory by default
      histogram.Write()
  f.Close()


This loops over each event and inside that event loops over each trajectory. If it matches
one of the desired particle IDs we histogram the X,Z (global) coordinates of its starting
point.

To view the histograms, see :ref:`output-analysis-quick-recipes`.


Rebdsim Average Histograms
--------------------------

The above histograms are simple ROOT histograms. These are a count of a quantity
in each bin. However, often we are interested in the mean or average per-event
rate so that we can easily scale this to a realistic beam or setup. i.e. we
typically never simulate the same number of events as real life particles, but
enough to gain a statistical understanding of the outcome. With the simple histograms
we can of course, normalise to the number of events if we keep track of it. If the quantity
however, is more than 1 thing per event, then the variance and error on the mean will not
be correct. This is discussed in :ref:`analysis-per-entry-histograms-vs-simple`.

Normally, in rebdsim, the histograms are per-event averages. We can reproduce this ourselves
in Python using classes from rebdsim. The following script is included in
:code:`bdsim/examples/trajectories/rebdsimaveragehistograms.py`. The same histograms
are created as in the earlier part of this example but are a per-event average. ::

  import pybdsim
  import ROOT
  
  def Analyse(filename, outputfilename):
      d = pybdsim.Data.Load(filename)

      particles    = [13,-13,14,-14]
      particlesStr = [str(p) for p in particles]
      names        = [r'$\mu^-$',     r'$\mu^+$',
                      r'$\nu_{\mu}$', r'$\overline{\nu}_{\mu}$']
    
      # setup histograms - 1x 2D histogram in X,Z for each particle
      # a base hsitogram is one we will use for each event and empty it afterwards
      baseHistograms = {}
      for p,ps in zip(particles,particlesStr):
          # x dimension of histogram: 50 bins in Z from 0 to 10m
          # y dimension of histogram: 50 bins in X from -1 to 1 m
          # note the histogram has no concept of units - it's purely just a
          # number so we need to choose ranges appropriately
          h = ROOT.TH2D("Origin_ZX_"+ps+"BASE", "Origin of "+ps, 50, 0, 10, 50, -0.5, 0.5)
          baseHistograms[p] = h

      # make accumulators that calculate a rolling average for each histogram
      accumulators = {}
      for p,hist in baseHistograms.items():
          # note we must have a different name for the resultant accumulated histogram, so we strip
          # off the 'BASE' suffix we added (knowingly) to the base histogram
          accumulators[p] = ROOT.HistogramAccumulator(hist, hist.GetName().strip("BASE"), hist.GetTitle())

      eventTree = d.GetEventTree()
      # using this python syntax for name in thing, ROOT will make the variable
      # 'name' ('event') here, have the exact same layout as we see in a TBrowser
      # and as is in the file.
      for event in eventTree:
          for p,h in baseHistograms.items():
              h.Reset()
        
          # now loop over the trajectories stored for this event
	  for i in range(event.Trajectory.n):
	      # get the particle ID of the trajectory
              partID = event.Trajectory.partID[i]
              if partID not in particles:
                  continue # skip ones we don't want to histogram

              # i-th trajectory and the 0th (ie first) point of the trajectory position vector
              originPos = event.Trajectory.XYZ[i][0]
              x,z = originPos[0],originPos[2]
              weight = event.Trajectory.postWeights[i][0]

              baseHistograms[partID].Fill(z, x, weight)

          # at the end of the event we 'accumulate' one event
          for p,h in baseHistograms.items():
	      accumulators[p].Accumulate(h)

      # now we terminate the accumulators - ie normalise to the number of events
      results = {}
      for p,h in accumulators.items():
          results[p] = h.Terminate()

      # now write histograms out to a new ROOT file
      f = ROOT.TFile(outputfilename, "RECREATE")
      for p,h in results.items():
          h.AddDirectory(True) # bdsim / rebdsim classes turn off AddDirectory by default
          h.Write()
      f.Close()


The key concept is we create a regular histogram but also an accumulator based on it.
We fill the histogram possibly several times for a given event, and at the end of the
event we mark this by accumulating that individual histogram. At the start of each event
we use the same base histogram again but we reset it to empty it.

To view the histograms, see :ref:`output-analysis-quick-recipes`.
