# Beam Delivery Simulation (BDSIM) Copyright (c) Royal Holloway, University of London 2001 - 2024

## General Information

BDSIM version 1.7.7.develop program for radiation transport in accelerators

The official web site is:
	https://bdsim-collaboration.github.io/web/

Mailing list is:
https://www.jiscmail.ac.uk/cgi-bin/webadmin?A0=BDSIM

Currently maintained and developed by:

  * Laurie Nevay          <laurie.nevay@cern.ch> (main contact)
  * Will Shields          <william.shields@rhul.ac.uk>
  * Stewart Boogert       <stewart.boogert@cockcroft.ac.uk>
  * Jochem Snuverink      <jochem.snuverink@psi.ch>

Originally started by G.A. Blair. See manual for full authorship.

## Installation

Regularly tested on:
  * OS: Linux (RHEL9, Alma9, Ubuntu 20,22, 24), Mac OS 13 onwards
  * Architecture: x86_64, ARM64
  * Compiler: GCC 8,9,11,13, Clang 12-15 (fully C++ 11 compliant)


## System requirements:
  * compiler with C++11 support
  * CMake 3.5 or higher
  * BISON
  * CLHEP (recommended version > 2.3.3.0 - check with Geant4 version)
  * FLEX
  * Geant4 versions Geant4.10 or greater (10.4.3, 10.7.2, 11.2.0 recommended) or latest patch version recommended of any 10.X version. 
  * ROOT 6

Optional: 
  * OpenGL / QT5 (for interactive visualiser)
  * XercesC3 (for GDML)

To compile, see http://www.pp.rhul.ac.uk/bdsim/manual/installation.html

```
   %  mkdir build
   %  cd build
   %  cmake ../bdsim
   %  make
   %  make install
```

## Getting Started


To run:
```
    bdsim [options]
```

for example: 
```
    bdsim --file=line.gmad
```

to see the full list of options:
```
    bdsim --help
```

Some input examples are available in ./examples/ directory. 
For more details consult documentation on http://www.pp.rhul.ac.uk/bdsim/manual/

## Directories
```
analysis/        : analysis tools
building         : various build systems
cmake/           : cmake configuration files
comparator/      : tool for comparison of results
configuration/   : software version information for compilation
convert/         : conversion of coordinates
examples/        : example models and examples for features
include/         : header files
interpolator/    : tool for field map validation
manual/          : documentation
modules/         : extra modules for user code
parser/          : gmad parser
src/             : source files
src-external/    : third party source code
test/            : various test programs
utils/           : various useful scripts
vis/             : default visualisation files
```

## History

```
v1.7.7  29.10.23
v1.7.6  18.10.23
v1.7.5  03.10.23
v1.7.4  25.08.23
v1.7.3  11.08.23
v1.7.2  11.08.23
v1.7.1  20.07.23
v1.7.0  11.07.23
v1.6.0  16.12.21
v1.5.1  21.12.21
v1.5.0  16.12.20
v1.4.1  09.06.20
v1.4.0: 08.06.20
v1.3.3: 21.05.19
v1.3.2: 20.04.19
v1.3.1: 05.03.19
v1.3:   27.02.19
v1.2:   26.08.18
v1.1:   23.05.18
v1.0:   10.04.18
v0.993: 11.12.17
v0.992: 26.07.17
v0.99:  09.06.17
v0.95:  07.11.16
v0.94:  14.09.16
v0.93:  24.08.16
v0.92:  29.03.16
v0.91:  17.12.15
v0.9 :  10.11.15
v0.8 :  10.08.15
v0.7 :  30.06.15
v0.65:  10.04.15
v0.64:  16.02.15
v0.63:  06.02.15
v0.62:  07.08.14
v0.61:  05.08.14
v0.6 :  02.12.13
v0.5 :  30.05.08
v0.4 :  25.02.08
v0.3 :  26.01.07
v0.2 :  18.05.06
v0.1 :  15.11.05 
Beta :  05.05.05
```