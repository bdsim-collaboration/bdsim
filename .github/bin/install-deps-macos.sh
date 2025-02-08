#!/bin/bash

brew update

# explicitly remove all conflicting python3.13 files
rm /usr/local/bin/idle3
rm /usr/local/bin/pydoc3.13
rm /usr/local/bin/python3
rm /usr/local/bin/python3-config
rm /usr/local/bin/python3.13
rm /usr/local/bin/python3.13-config
# brew link --overwrite python3.13

brew install root wget clhep root bison xerces-c
