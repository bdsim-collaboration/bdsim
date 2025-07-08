#!/bin/bash

# Update brew
brew update

# --force-bottle python as other method fails with link error
brew install python@3.13 --force-bottle

# upgrade packages
brew upgrade

# install BDSIM deps
brew install wget clhep bison xerces-c root