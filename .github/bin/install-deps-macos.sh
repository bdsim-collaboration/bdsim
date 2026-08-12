#!/bin/bash

# Update brew
brew update

# --force-bottle python as other method fails with link error
brew install python pybind11

# install BDSIM deps
brew install wget clhep bison xerces-c root hdf5 boost

# hepmc3 (with root)
wget -q http://hepmc.web.cern.ch/hepmc/releases/HepMC3-3.3.1.tar.gz &&\
tar -xzf HepMC3-3.3.1.tar.gz                   &&\
mkdir hepmc3-build                             &&\
cd hepmc3-build                                &&\
cmake -DHEPMC3_ENABLE_ROOTIO:BOOL=ON             \
      -DHEPMC3_ENABLE_PROTOBUFIO:BOOL=OFF        \
      -DHEPMC3_ENABLE_TEST:BOOL=OFF              \
      -DHEPMC3_INSTALL_INTERFACES:BOOL=ON        \
      -DHEPMC3_BUILD_STATIC_LIBS:BOOL=OFF        \
      -DHEPMC3_BUILD_DOCS:BOOL=OFF               \
      -DHEPMC3_ENABLE_PYTHON:BOOL=OFF            \
      ../HepMC3-3.3.1                          &&\
make -j4                                       &&\
sudo make install