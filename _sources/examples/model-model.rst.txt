Model-Model
===========

* Based on several models in :code:`/bdsim/examples/model-model`
* Specific example in :code:`/bdsim/examples/model-model/bdsim/halo/h-main-h.gmad`

The model-model is the fictional storage ring that could be used in a high energy
proton collider. The machine is described in the primary BDSIM paper as an example.

The ring is racetrack shaped with two straight sections. One is used to create a
small beam size that might be suitable for a collision and the other for large beam
sizes to allow collimation. The model is prepared in MADX and custom collimator opening
settings and materials are supplied in hand written text files. Example Python scripts
are included to show how to merge this information together into a BDSIM model.

Several conversions of the model are made for different purposes. Linear optics,
full non-linear optics and for a halo simulation.

* MADX models and input can be found in :code:`bdsim/examples/model-model/madx`.
* BDSIM models can be found in :code:`bdsim/examples/model-model/bdsim`.

Assuming the user is located in the :code:`bdsim/examples/model-model` directory. ::
  
  bdsim --file=bdsim/halo/h-main-h.gmad --outfile=t1 --batch

This will generate 20 events of up to 100 turns. The job is designed to simulate
a halo beam impacting the first horizontal collimator.
