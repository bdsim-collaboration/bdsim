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
#ifndef SAMPLERANALYSIS_H
#define SAMPLERANALYSIS_H

#include "BDSOutputROOTEventSampler.hh"

/**
 * @brief Analysis routines for an individual sampler.
 *
 * @author Stewart Boogert, Andrey Abramov
 */

class SamplerAnalysis
{
public:
  SamplerAnalysis();

  // Note compile time float / double templating.
#ifndef __ROOTDOUBLE__
  SamplerAnalysis(BDSOutputROOTEventSampler<float>* samplerIn,
		  bool debugIn = false);
#else 
  SamplerAnalysis(BDSOutputROOTEventSampler<double>* samplerIn,
		  bool debugIn = false);
#endif
  /// Initialisation of arrays for optical function calculations
  void CommonCtor();
  virtual ~SamplerAnalysis();

  /// Initialise variables.
  void Initialise();

  /// Loop over all entries in the sampler and accumulate power sums over variuos moments.
  void Process(bool firstTime = false);

  /// Calculate optical functions based on combinations of moments already accumulated.
  std::vector<double>  Terminate(std::vector<double> emittance,
				 bool useEmittanceFromFirstSampler = true);

  /// Accessor for optical functions
  std::vector<std::vector<double> > GetOpticalFunctions() {return optical;}

  /// Set primary particle mass for optical functions from sampler data
  static void UpdateMass(SamplerAnalysis* s);

  /// Set primary particle mass for optical functions from particle name
  static void UpdateMass(const std::string& particleName);

#ifndef __ROOTDOUBLE__
  BDSOutputROOTEventSampler<float> *s;
#else 
  BDSOutputROOTEventSampler<double> *s;
  BDSOutputROOTEventSampler<double> *p;
#endif
protected:

  static double particleMass;
  
  // sums - initialised to zero as that's what they start at
  long long int npart;
  double S;

  //6d phase space coordinates for each event
  std::vector<double> coordinates;

  // initial values to use for mean subtraction.
  std::vector<double> offsets;

  typedef std::vector<std::vector<double>>                           twoDArray;
  typedef std::vector<std::vector<std::vector<double>>>              threeDArray; 
  typedef std::vector<std::vector<std::vector<std::vector<double>>>> fourDArray;

  fourDArray    powSums;
  fourDArray    cenMoms;

  fourDArray    powSumsFirst;
  fourDArray    cenMomsFirst;
  
  threeDArray   covMats;
  threeDArray   derivMats;
  twoDArray     optical;     ///< emt, alf, bta, gma, eta, etapr, mean, sigma
  twoDArray     varOptical;  ///< variances of optical functions



  /// Returns a central moment calculated from the corresponding coordinate power sums.
  /// Arguments:
  ///    powSums: array contatining the coordinate power sums
  ///    a, b:  integer identifier for the coordinate (0->x, 1->xp, 2->y, 3->yp, 4->E, 5->t)
  ///    m, n:  order of the moment wrt to the coordinate
  ///    note:  total order of the mixed moment is given by k = m + n
  double powSumToCentralMoment(fourDArray& powSum,
			       long long int npartIn,
			       int i,
			       int j,
			       int m,
			       int n);

  /// Returns a matrix element of the parameter covariance matrix which is a 3x3 symmetric
  /// matrix in each plane (coupling is ignored). 
  /// Arguments:
  ///     int k:   plane specifier (k=0: horizontal, k=1: vertical, k=2: longitudinal)
  ///     int i,j: indices of matrix elements (i,j=0: <uu>, i,j=1: <u'u'>, i,j=2: <uu'>)
  ///     e.g. covMat[0][1][2] = cov[<x'x'>,<xx'>], covMat[1][0][0] = cov[<yy>,<yy>]
  double centMomToCovariance(fourDArray&   centMoms,
			     long long int npartIn,
			     int k,
			     int i,
			     int j);

  /// Returns the derivative of an optical function w.r.t. central moments. 
  /// Arguments:
  ///     int t: function specifier, corresponds to index of the function in the
  ///            optical function vector. 
  ///     int k: plane specifier (k=0: horizontal, k=1: vertical, k=2: longitudinal)
  ///     int i: central moment to diffrentiate w.r.t, i=0: <uu>, i=1: <u'u'>, i=2: <uu'>
  ///     e.g. derivMat[t=2][k=0][i=0]: d(beta)/d<xx> , derivMat[t=0][k=1][i=1]: d(emittance)/d<yy'>
  double centMomToDerivative(fourDArray &centMoms, int k, int t, int i);


  //Prints out the beam correlation matrix at that sampler to the console
  //The beam correlation matrix gives good idication of the presence and magnitude of all couplings.
  void printBeamCorrelationMatrix(fourDArray&   centMoms);

private:
  bool debug;
};

#endif
