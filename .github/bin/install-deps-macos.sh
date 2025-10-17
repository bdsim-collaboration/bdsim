#!/bin/bash

# Update brew
brew update

# --force-bottle python as other method fails with link error
brew install python pybind11 

# install BDSIM deps
brew install wget clhep bison xerces-c root hdf5

# install pybind11
pip install pybind11
