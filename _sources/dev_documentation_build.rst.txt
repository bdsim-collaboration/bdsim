Documentation Builds
********************

Making the Manual
=================

The manual is available online at http://www.pp.rhul.ac.uk/bdsim/manual and included
as a pdf in the source directory, but if desired the user can compile the manual
in both HTML and pdflatex from the build directory using the following command
to make the HTML manual in the folder ``manual/html``::

  make manual

Similarly::

  make manual-pdf

will make the pdf Manual in the folder ``manual/latex``.

.. note:: This requires the sphinx documentation system to be installed and all utility
	  python packages to be available in python from any directory. The latexpdf build
	  requires a full installation of pdflatex to be available as well.


Making Doxygen Code Documentation
=================================

Doxygen code documentation is available online at
http://www.pp.rhul.ac.uk/bdsim/doxygen/

If desired the user can create this from the build directory using the following command
to make the Doxygen documentation in a folder called ``Doxygen``.::

  make doc

.. note:: This requires the Doxygen documentation system to be installed.

BDSIM is open source software and the group highly encourages people to get in contact,
contribute and add to the code. We are more than happy to provide guidance, get feedback
and collaborate together.

Users may either get in touch and get write access to the repository after discussion
with the developers or fork the repository and make "pull requests" through git.
