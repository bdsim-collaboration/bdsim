.. _dev-analysissuite:

Analysis Suite
==============

Executables
-----------

+-------------------+--------------------------------------------------------+
| **Executable**    | **Description**                                        |
+===================+========================================================+
| rebdsim           | Main analysis tool. Reads histogram definition file.   |
+-------------------+--------------------------------------------------------+
| rebdsimCombine    | Combines output from multiple runs of rebdsim          |
+-------------------+--------------------------------------------------------+
| rebdsimHistoMerge | Loops over events and calculates mean of histograms    |
|                   | already store in output only. A quick way to get a     |
|                   | summary of data.                                       |
+-------------------+--------------------------------------------------------+
| rebdsimOptics     | Calculates the optics only from a BDSIM ROOT file      |
+-------------------+--------------------------------------------------------+
| rebdsimOrbit      | Extracts the primary particle hit in each sampler for  |
|                   | a given event                                          |
+-------------------+--------------------------------------------------------+
| edbdsim           | Event display                                          |
+-------------------+--------------------------------------------------------+


Source Libraries
----------------

The classes stored in the output are compiled into the BDSIM library, however, they
are also copied into a separate library called `libBdsimRootEvent` without any Geant4
types of linkages. These classes are `bdsim/include/BDSOutputROOTEvent*`.

A series of classes for loading the output in the structure as it is in the output files
are included in `bdsim/analysis`. These are compiled into a library called `librebdsim`.

The user must load both libraries to fully inspect the data.

.. note:: No Geant4 types are exposed to the output as this would increase the complexity
	  of the interface with ROOT and ROOT's inspection of the types for reflection.


Dynamic Loading
---------------

Loading all of the data at once results in slow performance and is often unnecessary. For
the event tree where the majority of the data is stored, it is advantageous to only
load the 'branches' required for a particular analysis. Turning off branches means that
when `GetEntry(n)` is called on the tree, nothing is copied out of the file onto the local
objects. We therefore turn 'on' the branches we need for a particular analysis.

The analysis tool `rebdsim` works out the branches required when parsing histogram definitions
and these are stored in a map of branch names for each tree. The `DataLoader` class takes
this as an optional argument and turns on the appropriate branches.

The default arguments for the `DataLoader` class are such that all data is loaded. It is
non-trivial and requires a lot of typing to define the map necessary for efficient loading,
which hinders a user when inspecting data interactively. Therefore, all data is on when
loading interactively.

Interactive Usage
-----------------

The code used for analysis must be maintained in such a way that is both useful for the
compiled analysis tools, but also simple to use interactively.
