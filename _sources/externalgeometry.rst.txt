.. _external-geometry-formats:

=========================
External Geometry Formats
=========================

Currently, three formats are supported by BDSIM. GDML is recommended as this is thoroughly
supported by Geant4 and the geometry extent can be automatically determined.

More can be added in collaboration with the BDSIM development team - please see :ref:`feature-request`.

.. warning:: The Mokka format is not currently developed or maintained in BDSIM.

.. warning:: The GGMAD format has been deprecated and removed from BDSIM as of V1.7.0 as
	     it was not maintained and was very hard to maintain as it was old code.

GDML
----

GDML (Geometry Description Markup Language) is an XML schema for detector description.
To use Geant4 and BDSIM needs to be built with GDML usage on (default true). For more
information please refer to the GDML `website <http://gdml.web.cern.ch/GDML/>`_ and
`manual <http://gdml.web.cern.ch/GDML/doc/GDMLmanual.pdf>`_.

This format is widely supported and other geometry software may be able to export
geometry in GDML format.

See :ref:`geometry-gdml-preprocessing` for a relevant discussion about how BDSIM
handles GDML files.

GDML Preparation
^^^^^^^^^^^^^^^^

A Python package **pyg4ometry** has been created to aid preparation, visualisation and
overlap checking of GDML geometry. Please see :ref:`python-geometry-preparation` for more details.

This package is used for many of the examples included with BDSIM and the Python scripts are
included with the examples.

GDML Colours
^^^^^^^^^^^^

BDSIM can read auxiliary information tags in GDML attached to logical volumes. The following
information is optionally interpreted if available in the :code:`<volume>` tag:  ::

  <auxiliary auxtype="colour" auxvalue="0.39215686274509803 0.5490196078431373 0.596078431372549 1"/>

The :code:`auxvalue` should be a white-space separated list of 4 values from 0 to 1 for
RGBA values.

An example can be found in :code:`examples/features/geometry/9_gdml/14_gdml_colours.gmad`.

.. note:: This is not a Geant4 standard and is custom information the user must supply in their
	  GDML file and therefore it is only convention we agree on for the tag and the contents.

Mokka
-----

.. warning:: The Mokka format is not currently developed or maintained in BDSIM.

This format is currently in the form of a dumped MySQL database format. Note that throughout
any of the Mokka files, a `#` may be used to represent a commented line. There are three key
stages, which are detailed in the following sections, that are required for setting up the
Mokka geometry:

* :ref:`mokka-description`
* :ref:`mokka-list`
* :ref:`mokka-element`

.. _mokka-description:
  
Describing the geometry
^^^^^^^^^^^^^^^^^^^^^^^

An object must be described by creating a MySQL file containing commands that would typically
be used for uploading/creating a database and a corresponding new table into a MySQL database.
BDSIM supports only a few such commands - specifically the CREATE TABLE and INSERT INTO commands.
When writing a table to describe a solid, there are some parameters that are common to all solid
types (such as NAME and MATERIAL) and some that are more specific (such as those relating to radii
for cone objects). A full list of the standard and specific table parameters, as well as some
basic examples, are given below with each solid type. All files containing geometry descriptions
must have the following database creation commands at the top of the file::

  DROP DATABASE IF EXISTS DATABASE_NAME;
  CREATE DATABASE DATABASE_NAME;
  USE DATABASE_NAME;

A table must be created to allow for the insertion of the geometry descriptions. A table is
created using the following  MySQL compliant commands::

  CREATE TABLE TABLE-NAME_GEOMETRY-TYPE (
  TABLE-PARAMETER VARIABLE-TYPE,
  TABLE-PARAMETER VARIABLE-TYPE,
  TABLE-PARAMETER VARIABLE-TYPE
  );

Once a table has been created, values must be entered into it in order to define the solids and
position them. The insertion command must appear after the table creation and must be followed
by the MySQL compliant table insertion command::

  INSERT INTO TABLE-NAME_GEOMETRY-TYPE VALUES(value1, value2, "char-value", ...);

The values must be inserted in the same order, as their corresponding parameter types are described
in the table creation. Note that ALL length types must be specified in mm and that ALL angles
must be in radians.

An example of two simple boxes with no visual attribute set is shown below. The first box is a
simple vacuum cube, whilst the second is an iron box with length x = 10mm, length y = 150mm,
length z = 50mm, positioned at x=1m, y=0, z=0.5m and with zero rotation::

  CREATE TABLE mytable_BOX (
  NAME VARCHAR(32),
  MATERIAL VARCHAR(32),
  LENGTHX DOUBLE(10,3),
  LENGTHY DOUBLE(10,3),
  LENGTHZ DOUBLE(10,3),
  POSX DOUBLE(10,3),
  POSY DOUBLE(10,3),
  POSZ DOUBLE(10,3),
  ROTPSI DOUBLE(10,3),
  ROTTHETA DOUBLE(10,3),
  ROTPHI DOUBLE(10,3)
  );

  INSERT INTO mytable_BOX VALUES("a_box","vacuum", 50.0, 50.0, 50.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
  INSERT INTO mytable_BOX VALUES("another_box","iron", 10.0, 150.0, 50.0, 1000.0, 0.0, 500.0, 0.0, 0.0, 0.0);

Further examples of the Mokka geometry implementation can be found in the examples/features/geometry/Mokka/General directory. See the common table parameters and solid type sections below for more information on the table parameters available for use.

Common Table Parameters
_______________________

The following is a list of table parameters that are common to all solid types, either as an optional or mandatory parameter:

* | NAME
  | Variable type: VARCHAR(32)
  | Optional parameter
  | If supplied, then the Geant4 LogicalVolume associated with the solid will be labelled with this name. The default is set to be the table’s name plus an automatically assigned volume number.
* | MATERIAL
  | Variable type: VARCHAR(32)
  | Optional parameter
  | If supplied, then the volume will be created with this material type - note that the material must be given as a character string inside double quotation marks(“). The default material is set as Vacuum.
* | PARENTNAME
  | Variable type: VARCHAR(32)
  | Optional parameter
  | If supplied, then the volume will be placed as a daughter volume to the object with ID equal to PARENTNAME. The default parent is set to be the Component Volume. Note that if PARENTID is set to the Component Volume, then POSZ will be defined with respect to the start of the object. Else POSZ will be defined with respect to the centre of the parent object.
* | INHERITSTYLE
  | Variable type: VARCHAR(32)
  | Optional parameter to be used with PARENTNAME.
  | If set to “SUBTRACT“ then instead of placing the volume within the parent volume as an inherited object, it will be subtracted from the parent volume in a Boolean solid operation. The default for this value is set to ““ - which sets to the usual mother/daughter volume inheritance.
* | ALIGNIN
  | Variable type: INTEGER(11)
  | Optional parameter
  | If set to 1 then the placement of components will be rotated and translated such that the incoming beamline will pass through the z-axis of this object. The default is set to zero.
* | ALIGNOUT
  | Variable type: INTEGER(11)
  | Optional parameter
  | If set to 1 then the placement of the next beamline component will be rotated and translated such that the outgoing beamline will pass through the z-axis of this object. The default is set to zero.
* | SETSENSITIVE
  | Variable type: INTEGER(11)
  | Optional parameter
  | If set to 1 then the object will be set up to register energy depositions made within it and also record the z-position at which this deposition occurs. This information will be saved in the ELoss Histogram if using ROOT output. The default is set to zero.
* | MAGTYPE
  | Variable type: VARCHAR(32)
  | Optional parameter
  | If supplied, then the object will be set up to produce the appropriate magnetic field using the supplied K1 or K2 table parameter values . Three magnet types are available - “QUAD”, “SEXT” and “OCT”. The default is set to no magnet type. Note that if MAGTYPE is set to a value whilst K1/K2/K3 are not set, then no magnetic field will be implemented.
* | K1
  | Variable type: DOUBLE(10,3)
  | Optional parameter
  | If set to a value other than zero, in conjunction with MAGTYPE set to “QUAD”, then a quadrupole field with this K1 value will be set up within the object. Default is set to zero.
* | K2
  | Variable type: DOUBLE(10,3)
  | Optional parameter
  | If set to a value other than zero, in conjunction with MAGTYPE set to “SEXT”, then a sextupole field with this K2 value will be set up within the object. Default is set to zero.
* | K3
  | Variable type: DOUBLE(10,3)
  | Optional parameter
  | If set to a value other than zero, in conjunction with MAGTYPE set to “OCT”, then a sextupole field with this K3 value will be set up within the object. Default is set to zero.
* | POSX, POSY, POSZ
  | Variable type: DOUBLE(10,3)
  | Required parameters
  | They form the position in mm used to place the object in the component volume. POSX and POSY are defined with respect to the centre of the component volume and with respect to the component volume’s rotation. POSZ is defined with respect to the start of the component volume. Note that if the object is being placed inside another volume using PARENTNAME then the position will refer to the centre of the parent object.
* | ROTPSI, ROTTHETA, ROTPHI
  | Variable type: DOUBLE(10,3)
  | Optional parameters
  | These are the Euler angles in radians used to rotate the object before it is placed. The default is set to zero for each angle.
* | RED, BLUE, GREEN
  | Variable type: DOUBLE(10,3)
  | Optional parameters
  | They are the RGB colour components assigned to the object and should be a value between 0 and 1. The default is set to zero for each colour.
* | VISATT
  | Variable type: VARCHAR(32)
  | Optional parameter
  | This is the visual state setting for the object. Setting this to “W” results in a wireframe display of the object. “S” produces a shaded solid and “I” leaves the object invisible. The default is set to be solid.
* | FIELDX, FIELDY, FIELDZ
  | Variable type: DOUBLE(10,3)
  | Optional parameters
  | They can be used to apply a uniform field to any volume, with default units of Tesla. Note that if there is a solenoid field present throughout the entire element, then this uniform field will act in addition to the solenoid field.

'Box' Solid Types
_________________

Append _BOX to the table name in order to make use of the G4Box solid type. The following table
parameters are specific to the box solid:

* | LENGTHX, LENGTHY, LENGTHZ
  | Variable type: DOUBLE(10,3)
  | Required parameters
  | There values will be used to specify the box’s dimensions.

’Trapezoid’ Solid Types
_______________________

Append _TRAP to the table name in order to make use of the G4Trd solid type - which is defined as a
trapezoid with the X and Y dimensions varying along z-functions. The following table parameters
are specific to the trapezoid solid:

* | LENGTHXPLUS
  | Variable type: DOUBLE(10,3)
  | Required parameter
  | This value will be used to specify the x-extent of the box’s dimensions at the surface positioned at +dz.
* | LENGTHXPMINUS
  | Variable type: DOUBLE(10,3)
  | Required parameter
  | This value will be used to specify the x-extent of the box’s dimensions at the surface positioned at -dz.
* | LENGTHYPLUS
  | Variable type: DOUBLE(10,3)
  | Required parameter
  | This value will be used to specify the y-extent of the box’s dimensions at the surface positioned at +dz.
* | LENGTHYPMINUS
  | Variable type: DOUBLE(10,3)
  | Required parameter
  | This value will be used to specify the y-extent of the box’s dimensions at the surface positioned at -dz.
* | LENGTHZ
  | Variable type: DOUBLE(10,3)
  | Required parameter
  | This value will be used to specify the z-extent of the box’s dimensions.

’Cone’ Solid Types
__________________

Append _CONE to the table name in order to make use of the G4Cons solid type. The following
table parameters are specific to the cone solid:

* | LENGTH
  | Variable type: DOUBLE(10,3)
  | Required parameter
  | This value will be used to specify the z-extent of the cone’s dimensions.
* | RINNERSTART
  | Variable type: DOUBLE(10,3)
  | Optional parameter
  | If set, this value will be used to specify the inner radius of the start of the cone. The default value is zero.
* | RINNEREND
  | Variable type: DOUBLE(10,3)
  | Optional parameter
  | If set, this value will be used to specify the inner radius of the end of the cone. The default value is zero.
* | ROUTERSTART
  | Variable type: DOUBLE(10,3)
  | Required parameter
  | This value will be used to specify the outer radius of the start of the cone.
* | ROUTEREND
  | Variable type: DOUBLE(10,3)
  | Required parameter
  | This value will be used to specify the outer radius of the end of the cone.
* | STARTPHI
  | Variable type: DOUBLE(10,3)
  | Optional parameter
  | If set, this value will be used to specify the starting angle of the cone. The default value is zero.
* | DELTAPHI
  | Variable type: DOUBLE(10,3)
  | Optional parameter
  | If set, this value will be used to specify the delta angle of the cone. The default value is 2*pi.

’Torus’ Solid Types
___________________

Append _TORUS to the table name in order to make use of the G4Torus solid type. The following table
parameters are specific to the torus solid:

* | RINNER
  | Variable type: DOUBLE(10,3)
  | Optional parameter
  | If set, this value will be used to specify the inner radius of the torus tube. The default value is zero.
* | ROUTER
  | Variable type: DOUBLE(10,3)
  | Required parameter
  | This value will be used to specify the outer radius of the torus tube.
* | RSWEPT
  | Variable type: DOUBLE(10,3)
  | Required parameter
  | This value will be used to specify the swept radius of the torus. It is defined as being the distance from the centre of the torus ring to the centre of the torus tube. For this reason, this value should not be set to less than ROUTER.
* | STARTPHI
  | Variable type: DOUBLE(10,3)
  | Optional parameter
  | If set, this value will be used to specify the starting angle of the torus. The default value is zero.
* | DELTAPHI
  | Variable type: DOUBLE(10,3)
  | Optional parameter
  | If set, this value will be used to specify the delta swept angle of the torus. The default value is 2*pi.

’Polycone’ Solid Types
______________________

Append _POLYCONE to the table name in order to make use of the G4Polycone solid type. The following table
parameters are specific to the polycone solid:

* | NZPLANES
  | Variable type: INTEGER(11)
  | Required parameter
  | This value will be used to specify the number of z-planes to be used in the polycone. This value must be set to greater than 1.
* | PLANEPOS1, PLANEPOS2, ..., PLANEPOSN
  | Variable type: DOUBLE(10,3)
  | Required parameters
  | These values will be used to specify the z-position of the corresponding z-plane of the polycone. There should be as many PLANEPOS parameters set as the number of z-planes. For example, three z-planes will require that PLANEPOS1, PLANEPOS2, and PLANEPOS3 are all set up.
* | RINNER1, RINNER2, ..., RINNERN
  | Variable type: DOUBLE(10,3)
  | Required parameters
  | These values will be used to specify the inner radius of the corresponding z-plane of the polycone. There should be as many RINNER parameters set as the number of z-planes. For example, three z-planes will require that RINNER1, RINNER2, and RINNER3 are all set up.
* | ROUTER1, ROUTER2, ..., ROUTERN
  | Variable type: DOUBLE(10,3)
  | Required parameters
  | These values will be used to specify the outer radius of the corresponding z-plane of the polycone. There should be as many ROUTER parameters set as the number of z-planes. For example, 3 z-planes will require that ROUTER1, ROUTER2, and ROUTER3 are all set up.
* | STARTPHI
  | Variable type: DOUBLE(10,3)
  | Optional parameter
  | If set, this value will be used to specify the starting angle of the polycone. The default value is zero.
* | DELTAPHI
  | Variable type: DOUBLE(10,3)
  | Optional parameter
  | If set, this value will be used to specify the delta angle of the polycone. The default value is 2*pi.

’Elliptical Cone’ Solid Types
_____________________________

Append _ELLIPTICALCONE to the table name in order to make use of the G4Ellipticalcone solid type. The
following table parameters are specific to the elliptical cone solid:

* | XSEMIAXIS
  | Variable type: DOUBLE(10,3)
  | Required parameter
  | This value will be used to specify the Semiaxis in X.
* | YSEMIAXIS
  | Variable type: DOUBLE(10,3)
  | Required parameter
  | This value will be used to specify the Semiaxis in Y.
* | LENGTHZ
  | Variable type: DOUBLE(10,3)
  | Required parameter
  | This value will be used to specify the height of the elliptical cone.
* | ZCUT
  | Variable type: DOUBLE(10,3)
  | Required parameter
  | This value will be used to specify the upper cut plane level.

Note that the above parameters are used to define an elliptical cone with the following parametric
equations (in the usual Geant4 way)::

  X = XSEMIAXIS * (LENGTHZ - u) / u * Cos(v)
  Y = YSEMIAXIS * (LENGTHZ - u) / u * Sin(v)
  Z = u

.. include:: <isogrk1.txt>

where v is between 0 and 2 :math:`\pi` and u between 0 and h, respectively.

.. _mokka-list:

Creating a geometry list
^^^^^^^^^^^^^^^^^^^^^^^^

A geometry list is a simple file consisting of a list of file names that contain geometry
descriptions. This is the file that should be passed to the GMAD file when defining the
Mokka element. An example of a geometry list containing ’boxes.sql’ and ’cones.sql’ would be::

  # ’#’ symbols can be used for commenting out an entire line
  /directory/boxes.sql
  /directory/cones.sql

.. _mokka-element:
  
Defining a Mokka element
^^^^^^^^^^^^^^^^^^^^^^^^

The Mokka element can be defined by the following command::

  collimator : element, geometry=mokka:coll_geomlist.sql
