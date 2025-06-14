.. _dev-geantusage:

Geant4 User Action Classes
**************************

Geant4 provides several base classes the developer can inherit, instantiate and register with
the Geant4 run manager as a way of taking various actions defined by the developer at different
stages of the simulation. These are described at `Geant4 user actions <http://geant4.web.cern.ch/geant4/G4UsersDocuments/UsersGuides/ForApplicationDeveloper/html/UserActions/OptionalActions.html>`_.
They provide actions at each level of granularity in the simulation. The actions BDSIM takes in each of these are described
in the following sections.



G4UserRunAction
===============

A 'Run' is the largest unit of simulation in Geant4 parlance. It is a simulation where all the
geometry, fields and physics are fixed and the same throughout. It typically contains many 'Events'.

BDSRunAction::GenerateRun
-------------------------

This is not used, as BDSIM does not define its own Run object and uses the G4Run one.

BDSRunAction::BeginOfRunAction
------------------------------

* This histograms for the simulation (Run) are prepared and registered with the :code:`BDSAnalysisManager`.
* The start time is printed out.


BDSRunAction::EndOfRunAction
----------------------------

* The stop time is printed out.
* All histograms are written to the output.
* Clean up


G4UserEventAction
=================

.. warning:: Note, the Primary Generator action is called before this method at the start of each event!

BDSEventAction::BeginOfEventAction
----------------------------------

* Sampler hits collections are created and registered.

BDSEventAction::EndOfEventAction
--------------------------------

* Primary vertex coordinates are recorded.
* Sampler, Cylindrical Sampler, Energy Deposition, Tunnel Energy Deposition hits are recorded in that order.
* Primary impact points and loss points are recorded.
* Output is written.
* Trajectories matching filtering criteria are prepared and written to output.

G4UserStackingAction
====================

This allows the developer to fiddle the priority of particles (primary and secondary) based on their properties.

BDSStackingAction::ClassifyNewTrack
-----------------------------------

* All neutrinos are killed by default to save time on tracking.
* If specified in :code:`GMAD::Options` stop tracks, the following secondary particles are killed:

  * electrons, photons, positrons and protons/antiprotons

* If stop secondaries is specified, all secondaries are killed.
* If the track is beyond the limit of tracks per event, it is killed.
* If a particle is flagged as killed and it's in a volume that has sensitivity an
  attempt is made to process the sensitivity by dynamically casting the sensitive
  detector to a variety of BDSIM ones.


G4UserTrackingAction
====================

The tracking action happens when a track is created (a new particle) and when it is finished
tracking.

BDSUserTrackingAction::PreUserTrackingAction
--------------------------------------------

* Trajectories are stored if requested or as required for visualisation.
* If no trajectories are required (turned off and in batch mode) a special reduced
  trajectory is created for the primary particle only that marks its first impact
  and its loss location.

BDSUserTrackingAction::PostUserTrackingAction
---------------------------------------------

* The primary particle loss point is flagged as in a collimator or not.

G4UserSteppingAction
====================

Only :code:`UserSteppingAction` is implemented and only to provide verbose output if required.


G4VUserPrimaryGeneratorAction
=============================

BDSPrimaryGeneratorAction::GeneratePrimaries
--------------------------------------------

This uses a single particle gun and uses BDSBunch derived classes to generate various distributions
of single primary particles.

**Strong Reproducibility**

Note, the primary generator action is the first thing to be called per event that advances the random
number generator.  Therefore, the seed state is saved and restored here.  If strong reproduction is
requested using the executable flag :code:`--recreate=<file>`, the options will be loaded from the
output file, including the bunch distribution, and all will be recreated.
