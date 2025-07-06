Program Layout
**************

BDSIM progresses in the following general steps:

1. Parser reads and parses input text files that prepare structures representing input - all in GMAD namespace.
2. A Geant4 model (geometry, fields, sensitivity) is created based on information from the parser structures.
3. Special Geant4 user actions are registered.
4. The simulation proceeds either interactively or automatically for a given number of events.
5. Some analysis is performed at run time, output is written and post-run analysis can be performed.


Parser
======

The parser uses Flex and Bison to automatically generate the C++ code required
given a scripted syntax. This defines a set of rules for the syntax that can be
matched.

The parser is described in much more detail in :ref:`dev-parser`.

Geant4 Model
============

The use of Geant4 falls into two general parts: the model and the user action classes.  The model
is built using the parser beam line structure through many *factories*. The details are described
in :ref:`dev-geometry`, :ref:`dev-fields`, :ref:`dev-tracking`, and :ref:`dev-analysisoutput`.

Geant4 User Action Classes
==========================

Apart from the geometry and the fields, the Geant4 managed simulation is largely controlled through
user action classes. Geant4 provides base classes (some virtual, others not) that the developer can
register that allow actions to be taken at specific points, such as the beginning and end of event
actions.

These are discussed in detail in :ref:`dev-geantusage`.

Analysis
========

Run time analysis is also discussed in the aforementioned :ref:`dev-analysisoutput`.

Post run time (outside BDSIM) analysis is discussed in :ref:`dev-analysissuite`.

Specific Run Time Order
=======================

This is a rough description of the first few actions in BDSIM (bdsim.cc) that have to be in this
order.

1. :code:`BDSExecOptions`
2. Check if Geant4 is available in the current environment (env variables checked) - cannot proceed without this.
3. Construct parser and parser input.
4. Construct required Geant4 materials, as they're required by :code:`BDSGlobalConstants`.
5. Force instantiation of :code:`BDSGlobalConstants` singleton based on :code:`GMAD::Options`.
6. Initialise the pseudo-random number generator engine.
