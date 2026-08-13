#!/bin/bash

# Update brew
brew update

# --force-bottle python as other method fails with link error
brew install python pybind11

# install BDSIM deps
brew install wget clhep bison xerces-c root hdf5

# hepmc3
brew tap davidchall/hep
brew trust davidchall/hep
brew install hepmc3