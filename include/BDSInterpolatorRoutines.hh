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
#ifndef BDSINTERPOLATORROUTINES_H
#define BDSINTERPOLATORROUTINES_H

#include "G4Types.hh"

#include "BDSFieldValue.hh"

#include <cmath>

namespace BDS
{
  // These routines are templated for any type. They can be used with a type
  // that provides both L and R operator+ operator- with their own type and
  // operator+ operator- operator* with a double.

  /// Define a magnitude function that will work for numeric types
  /// but also for a field value (3 vector).
  template<class T>
  T mag(const T& v) {return std::abs(v);}
  /// Specialisation for a 3-vector field value.
  template<template<class> class BDSFieldValue, class V>
  V mag(const BDSFieldValue<V>& v){return v.mag();}

  /// Linear interpolation in 1 dimension. The "p" is the real value to be
  /// interpolated. 'x' must be on the interval [0,1], ie normalised.
  template<class T>
  T Linear1D(const T p[2],
             G4double x)
  {return p[0]*(1.-x) + p[1]*x;}

  /// Linear interpolation in 2 dimensions.
  template<class T>
  T Linear2D(const T p[2][2],
             G4double x,
             G4double y)
  {
    T arr[2];
    arr[0] = BDS::Linear1D<T>(p[0], y);
    arr[1] = BDS::Linear1D<T>(p[1], y);
    return BDS::Linear1D<T>(arr, x);
  }

  /// Linear interpolation in 3 dimensions.
  template<class T>
  T Linear3D(const T p[2][2][2],
             G4double x,
             G4double y,
             G4double z)
  {
    T arr[2];
    arr[0] = BDS::Linear2D<T>(p[0], y, z);
    arr[1] = BDS::Linear2D<T>(p[1], y, z);
    return BDS::Linear1D<T>(arr, x);
  }

  /// Linear interpolation in 4 dimensions.
  template<class T>
  T Linear4D(const T p[2][2][2][2],
             G4double x,
             G4double y,
             G4double z,
             G4double t)
  {
    T arr[2];
    arr[0] = BDS::Linear3D<T>(p[0], y, z, t);
    arr[1] = BDS::Linear3D<T>(p[1], y, z, t);
    return BDS::Linear1D<T>(arr, x);
  }

  /// Cubic interpolation in 1 dimension.  The "p" is the real value to be
  /// interpolated. 'x' must be on the interval [0,1], ie normalised.
  template<class T>
  T Cubic1D(const T p[4],
            G4double x)
  {
    return p[1]+0.5 * x*(p[2]-p[0]+x*(2.*p[0]-5.*p[1]+4.*p[2]-p[3]+x*(3.*(p[1]-p[2])+p[3]-p[0])));
  }

  /// Cubic interpolation in 2 dimensions.
  template<class T>
  T Cubic2D(const T p[4][4],
            G4double x,
            G4double y)
  {
    T arr[4];
    arr[0] = BDS::Cubic1D<T>(p[0], y);
    arr[1] = BDS::Cubic1D<T>(p[1], y);
    arr[2] = BDS::Cubic1D<T>(p[2], y);
    arr[3] = BDS::Cubic1D<T>(p[3], y);
    return BDS::Cubic1D<T>(arr, x);
  }

  /// Cubic interpolation in 3 dimensions.
  template<class T>
  T Cubic3D(const T p[4][4][4],
            G4double x,
            G4double y,
            G4double z)
  {
    T arr[4];
    arr[0] = BDS::Cubic2D<T>(p[0], y, z);
    arr[1] = BDS::Cubic2D<T>(p[1], y, z);
    arr[2] = BDS::Cubic2D<T>(p[2], y, z);
    arr[3] = BDS::Cubic2D<T>(p[3], y, z);
    return BDS::Cubic1D<T>(arr, x);
  }

  /// Cubic interpolation in 4 dimensions.
  template<class T>
  T Cubic4D(const T p[4][4][4][4],
            G4double x,
            G4double y,
            G4double z,
            G4double t)
  {
    T arr[4];
    arr[0] = BDS::Cubic3D<T>(p[0], y, z, t);
    arr[1] = BDS::Cubic3D<T>(p[1], y, z, t);
    arr[2] = BDS::Cubic3D<T>(p[2], y, z, t);
    arr[3] = BDS::Cubic3D<T>(p[3], y, z, t);
    return BDS::Cubic1D<T>(arr, x);
  }

  /// Linear interpolation of the magnitude in 1 dimension
  template<class T>
  double Linear1DMagOnly(const T p[2],
                         G4double x)
  {
    return (double)(mag(p[0])*(1.-x) + mag(p[1])*x);
  }

  /// Linear interpolation of the magnitude in 2 dimensions.
  template<class T>
  double Linear2DMagOnly(const T p[2][2],
                         G4double x,
                         G4double y)
  {
    double arr[2];
    arr[0] = BDS::Linear1DMagOnly(p[0], y);
    arr[1] = BDS::Linear1DMagOnly(p[1], y);
    return BDS::Linear1DMagOnly(arr, x);
  }

  /// Linear interpolation of the magnitude in 3 dimensions.
  template<class T>
  double Linear3DMagOnly(const T p[2][2][2],
                         G4double x,
                         G4double y,
                         G4double z)
  {
    double arr[2];
    arr[0] = BDS::Linear2DMagOnly(p[0], y, z);
    arr[1] = BDS::Linear2DMagOnly(p[1], y, z);
    return BDS::Linear1DMagOnly(arr, x);
  }

  /// Linear interpolation of the magnitude in 4 dimensions.
  template<class T>
  double Linear4DMagOnly(const T p[2][2][2][2],
                         G4double x,
                         G4double y,
                         G4double z,
                         G4double t)
  {
    double arr[2];
    arr[0] = BDS::Linear3DMagOnly(p[0], y, z, t);
    arr[1] = BDS::Linear3DMagOnly(p[1], y, z, t);
    return BDS::Linear1DMagOnly(arr, x);
  }

  /// Linear interpolation in 1 dimension including magnitude interpolation.
  template<class T>
  T Linear1DMag(const T p[2],
                G4double x)
  {
    T v = BDS::Linear1D(p,x);
    double vMag = Linear1DMagOnly(p, x);
    double factor = vMag / mag(v);
    if (std::isnan(factor))
      {factor = 1.0;}
    v *= factor;
    return v;
  }

  /// Linear interpolation in 2 dimensions including magnitude interpolation.
  template<class T>
  T Linear2DMag(const T p[2][2],
                G4double x,
                G4double y)
  {
    T v = BDS::Linear2D(p, x, y);
    double vMag = BDS::Linear2DMagOnly(p, x, y);
    double factor = vMag / mag(v);
    if (std::isnan(factor))
      {factor = 1.0;}
    v *= factor;
    return v;
  }

   /// Linear interpolation in 3 dimensions including magnitude interpolation.
  template<class T>
  T Linear3DMag(const T p[2][2][2],
                G4double x,
                G4double y,
                G4double z)
  {
    T v = BDS::Linear3D(p, x, y, z);
    double vMag = BDS::Linear3DMagOnly(p, x, y, z);
    double factor = vMag / mag(v);
    if (std::isnan(factor))
      {factor = 1.0;}
    v *= factor;
    return v;
  }

  /// Linear interpolation in 4 dimensions including magnitude interpolation.
  template<class T>
  T Linear4DMag(const T p[2][2][2][2],
                G4double x,
                G4double y,
                G4double z,
                G4double t)
  {
    T v = BDS::Linear4D(p, x, y, z, t);
    double vMag = BDS::Linear4DMagOnly(p, x, y, z, t);
    double factor = vMag / mag(v);
    if (std::isnan(factor))
      {factor = 1.0;}
    v *= factor;
    return v;
  }
}

#endif
