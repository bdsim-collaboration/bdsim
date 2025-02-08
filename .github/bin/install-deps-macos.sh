#!/bin/bash

brew update
brew upgrade

brew uninstall python@3.12
brew uninstall python@3.13

# install non ROOT deps
brew install wget clhep bison xerces-c root

# explicitly remove all conflicting python3.13 files
# rm /usr/local/bin/idle3
# rm /usr/local/bin/pydoc3.13
# rm /usr/local/bin/python3
# rm /usr/local/bin/python3-config
#rm /usr/local/bin/python3.13
# rm /usr/local/bin/python3.13-config

# install root
# brew install root