/* 
Beam Delivery Simulation (BDSIM) Copyright (C) BDSIM Collaboration, 2001 - 2026.

This file is part of BDSIM.

BDSIM is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published 
by the Free Software Foundation version 3 of the License.

BDSIM is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BDSIM.  If not, see <http://www.gnu.org/licenses/>.
*/
// gab (after NJW)
#ifndef myMultipoleField_h
#define myMultipoleField_h 1


//#include "merlin_config.h"

#include <vector>
#include <utility>
#include <iostream>
//#include "NumericalUtils/Complex.h"
// gab: 
#include<complex>

// Space3D
//#include "EuclideanGeometry/Space3D.h"
#include"G4ThreeVector.hh"
#include "globals.hh"

// EMFieldRegion
//#include "AcceleratorModel/EMFieldRegion.h"

//	A representation of a multipole expansion of a magnetic
//	field having only a non-zero component of the magnetic
//	vector potential in the z-direction. The general
//	expression for the series is give as
//
//	                                                         
//	                                        n
//	     By+i Bx =  Sum  B0 (bn-i an) (z/r0)
//	                n=0
//
//	where z=(x+iy). B0 is the overall scale (in Tesla), and
//	r0 is an aribitrary defining radius. For Multipole
//	expansions representing ideal single multipoles (e.g.
//	dipole, quadrupole etc), then B0 is set to the field
//	strength at r0, and consequently |bn+ i an|=1.

      typedef complex<G4double> Complex;
      typedef G4ThreeVector Vector3D;
      typedef G4ThreeVector Point3D;

class myMultipoleField 
{
  public:

      typedef std::vector<Complex > TermExpansion;


   typedef TermExpansion::iterator iterator;

   typedef TermExpansion::const_iterator const_iterator;

  public:
      myMultipoleField (G4double scale, size_t nt = 0);
      myMultipoleField (int np, G4double bn, G4double an, G4double r0);

      myMultipoleField (int np, G4double bn, G4double r0, bool skew = false);
      myMultipoleField (int np, G4double bn, bool skew = false);



      //	Returns the current normalising scale for the field (in
      //	Tesla)
      G4double GetFieldScale () const;

      //	Set the (normalising) scale of the field (units are in
      //	Tesla).
      void SetFieldScale (G4double scale);

      //	Returns true if this field is a null field.
      bool IsNullField () const;

      //	Returns the normalised field component Kn for the
      //	specified magnetic rigidity brho. Kn is defined as
      //	Dn[By+iBx]/brho, where Dn is the n-th complex derivative
      //	wrt x+i y, evaluated at (x+iy)=0. If brho is in
      //	Tesla.meter, Kn has the units 1/meter^(n+1).
      Complex GetKn (int np, G4double rigidity) const;

      //	Returns the 2-D magnetic field in Tesla as the complex
      //	number By+i Bx at the point (x,y) (in meters). The
      //	optional parameter exclude can be used to exclude terms
      //	lower than or equal to exclude Thus a value of exclude=1
      //	gives only that field due to the non-linear terms
      //	(sextupole and above). A Value of -1 (default) includes
      //	all multipole terms.
      Complex GetField2D (G4double x, G4double y, int exclude = -1) const;

      //	Returns the magnetic field at the point x. The time
      //	variable t is ignored.
      G4double* GetBFieldAt (const G4double* x, 
                                           G4double t = 0) const;

      //	Returns a null vector (pure magnetric field).
      Vector3D GetEFieldAt (const Point3D& x, G4double t = 0) const;

      //	Return the force due to the magnetic field on a particle
      //	of charge q, with position x and velocity v. The time
      //	variable t is ignored (static magnetic field).

        virtual Vector3D GetForceAt (const Point3D& x, 
                    const Vector3D& v, G4double q, G4double t = 0) const;

  //      	Rotates this field 180 degrees about the local vertical
  //    	axis.
        void RotateY180 ();

      //	Prints an ascii representation (table) of the field to
      //	os.
      void PrintField (std::ostream& os) const;

      //	Add the specified component to the field. The field
      //	components bn and an are in Tesla , and are normalised
      //	to the radius r (default = 1 meter).
      void SetComponent (int np, G4double bn, G4double an = 0, G4double r0 = 1);

      //	Returns the field (in Tesla) at the radius r0 due to the
      //	np-th multipole component
      Complex GetComponent (int np, G4double r0 = 1.0) const;

      //	Returns the unitless complex coefficient for the np-th
      //	term (bn+i*an).The coefficient is relative to the
      //	specified pole radius r0 (default = 1meter).
      Complex GetCoefficient (int np, G4double r0 = 1.0) const;

      //	Sets the unitless complex coefficient for the np-th term
      //	(bn+i*an).The coefficient is relative to the specified
      //	pole radius r0 (default = 1meter).
      void SetCoefficient (int np, const Complex& b, G4double r0 = 1.0);

      int HighestMultipole () const;

  protected:
  private:
    // Data Members for Class Attributes

      //	The scale of the field in Tesla. The coefficients of the
      //	expansion terms are normalised so that they have no
      //	field unit.
      G4double B0;
      TermExpansion expansion;

  private:  
};

// Class myMultipoleField 

inline myMultipoleField::myMultipoleField (G4double scale, size_t nt)
  : B0(scale),expansion(nt,Complex(0,0)){}

inline bool myMultipoleField::IsNullField () const
{
	return B0==0;
}

inline int myMultipoleField::HighestMultipole () const
{
	return expansion.size()-1;
}


#endif

