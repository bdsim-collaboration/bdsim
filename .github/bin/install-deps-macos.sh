#!/bin/bash

#rm /usr/local/bin/idle3
#rm /usr/local/bin/pydoc3.13
#rm /usr/local/bin/python3
#rm /usr/local/bin/python3-config
#rm /usr/local/bin/python3.13
#rm /usr/local/bin/python3.13-config

brew update

# explicitly remove all conflicting python3.13 files
# brew unlink python@3.13
brew install python@3.13 --force-bottle

brew upgrade

# install non ROOT deps
brew install wget clhep bison xerces-c root

# install root
# brew install root