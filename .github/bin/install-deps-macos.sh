#!/bin/bash

brew update

rm -rf /usr/local/bin/idle3
brew install --overwrite python3.13
brew install root wget clhep root bison xerces-c
