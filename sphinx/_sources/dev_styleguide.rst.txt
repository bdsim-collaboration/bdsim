Style Guide
***********

This section describes the agreed style and conventions used when editing
and writing source code in BDSIM.

C++
===

General
-------

* All variables shall be namespaced and not global.
* The asterisk is attached to the object, not the variable name.

::

   BDSClassName* anInstance;


Naming
------

* Underscores are to be avoided, as they are hard to type and read.
* HEADERGUARDS_H are like this
* Classes start with "BDS".
* Classes use UpperCamelCaseForNaming.
* Member functions use UpperCamelCase as well.
* Member variables have **no** prefix (such as :code:`_variable` or :code:`m_variable`).
* Member variables use lowerCamelCase.

* Energy loss is "ELoss" in code and "Eloss" in options and output for backwards compatibility
  even this generally breaks our lower camel case rule.

Indentation & Spacing
---------------------

* Two spaces per level of indentation.
* Tabs should not be used.
* Spaces should be between operators.


Braces
------

* Explicit braces should always be used, even for one line if statements.

::

   if (a > 4)
     {G4cout << a << G4endl;}

* Braces should be on a new line - makes scope easier to determine.

::

   if (a > 4)
     {
       G4cout << a << G4endl;
       G4cout << "This is a test" << G4endl;
     }

* The above style is preferred (indented block), but the following is also fine.

::

   if (a > 4)
     {
     G4cout << a << G4endl;
     G4cout << "This is a test" << G4endl;
     }



In-Code Documentation
---------------------

* Every single class should have doxygen documentation in the header.
* Obviously comments are strongly encouraged, as well as notes in this documentation.
* However, try not to repeat implementation - this makes it harder to maintain as comments
  have to be updated as well as the implementation in the future.
* Avoid documenting the purpose of functions (i.e. outside the function) in the source
  code - document the header as that is the interface that people should use when developing.


Python
======

The Python modules are developed with the intention that they be used and discovered
interactively in ipython, therefore, the naming convention described should make
tab completion easy to understand and docstrings should allow the use of the :code:`?` for
help on any class or object.

General
-------

* Functions or classes in modules should read well. e.g. `package.Plot.Losses`, not `package.Plot.PlotLosses`.
* Docstrings must be provided for all modules, classes and functions.
* General packages such as numpy should be imported in a hidden fashion by renaming.

::

   import numpy as _np



Naming
------

* Classes use UpperCamelCaseForNaming.
* Member functions use UpperCamelCase as well.
* Member variables have no prefix (such as :code:`_variable` or :code:`m_variable`).
* Member variables use lowerCamelCase.
