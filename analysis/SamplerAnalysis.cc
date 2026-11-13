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
#include "SamplerAnalysis.hh"
#include "rebdsim.hh"

#include <cmath>
#include <vector>

double SamplerAnalysis::particleMass = 0;

SamplerAnalysis::SamplerAnalysis():
  s(nullptr),
  npart(0),
  S(0),
  debug(false)
{
  CommonCtor();
}

#ifndef __ROOTDOUBLE__
SamplerAnalysis::SamplerAnalysis(BDSOutputROOTEventSampler<float> *samplerIn,
				 bool debugIn):
#else 
SamplerAnalysis::SamplerAnalysis(BDSOutputROOTEventSampler<double> *samplerIn,
                                 bool debugIn):
#endif
  s(samplerIn),
  npart(0),
  S(0),
  debug(debugIn)
{
  CommonCtor();
}

void SamplerAnalysis::UpdateMass(SamplerAnalysis* s)
{
  int id = s->s->partID[0];
  switch (std::abs(id))
    {
    case 11:
    case -11:
      {
	std::cout << "Primary particle: e-" << std::endl;
	particleMass = 0.000510999; break;}
    case 2212:
      {
	std::cout << "Primary particle: proton" << std::endl;
	particleMass = 0.938272; break;}
    default:
      {
	std::cout << "Primary particle: unknown -> m=0" << std::endl;
	particleMass = 0; break;}
    }
}

void SamplerAnalysis::UpdateMass(const std::string& particleName)
{
  if ((particleName == "e-") || (particleName == "e+"))
    {
      std::cout << "Primary particle: e-" << std::endl;
      particleMass = 0.000510999;;
    }
  else if (particleName == "proton")
    {
      std::cout << "Primary particle: proton" << std::endl;
      particleMass = 0.938272;
    }
  else
    {
      std::cout << "Primary particle: unknown -> m=0" << std::endl;
      particleMass = 0;
    }
}

void SamplerAnalysis::CommonCtor()
{
  npart = 0;

  // initialise a vector to store the coordinates of every event in a sampler
  coordinates.resize(6, 0);

  // initialise a vector to store the first values in a sampler for assumed mean subtraction
  offsets.resize(6, 0);
  
  optical.resize(3); // resize to 3 entries initialised to 0
  varOptical.resize(3);
  for(int i=0;i<3;++i)
    {
      optical[i].resize(25, 0);   // 12 for central values and 12 for errors and 1 for xy correlation
      varOptical[i].resize(12, 0);
    }

  covMats.resize(3);
  for(int i=0;i<3;++i)
  {
    covMats[i].resize(3);
    for(int j=0;j<3;++j)
      {covMats[i][j].resize(3, 0);}
  }

  derivMats.resize(3);
  for(int i=0;i<3;++i)
  {
    derivMats[i].resize(12);
    for(int j=0;j<12;++j)
      {derivMats[i][j].resize(3, 0);}
  }
  
  powSums.resize(6);
  cenMoms.resize(6);

  powSumsFirst.resize(6); // first sampler
  cenMomsFirst.resize(6);

  // (x,xp,y,yp,E,t) (x,xp,y,yp,E,t) v1pow, v2pow
  for(int i=0;i<6;++i)
  {
    powSums[i].resize(6);
    cenMoms[i].resize(6);

    powSumsFirst[i].resize(6);
    cenMomsFirst[i].resize(6);

    for(int j=0;j<6;++j)
    {
      powSums[i][j].resize(5);
      cenMoms[i][j].resize(5);

      powSumsFirst[i][j].resize(5);
      cenMomsFirst[i][j].resize(5);
      
      for(int k=0;k<=4;++k)
      {
        powSums[i][j][k].resize(5, 0);
        cenMoms[i][j][k].resize(5, 0);

	powSumsFirst[i][j][k].resize(5, 0);
        cenMomsFirst[i][j][k].resize(5, 0);
      }
    }
  }
}

SamplerAnalysis::~SamplerAnalysis()
{;}

void SamplerAnalysis::Initialise()
{
  npart = 0;
}

void SamplerAnalysis::Process(bool firstTime)
{
  if(debug)
    {std::cout << __METHOD_NAME__ << "\"" << s->samplerName << "\" with " << s->n << " entries" << std::endl;}

  double m2 = std::pow(particleMass,2);
  
  // loop over all entries
  for(int i=0;i<s->n;++i)
  {
    if (i == 0)
      {S = s->S;} // update sampler on first round - do here so not to load default data
    if (s->parentID[i] != 0)
      {continue;} // select only primary particles
    if (s->turnNumber[i] > 1)
      {continue;} // only use first turn particles
    if (s->zp[i] <= 0)
      {continue;} // only forward going particles - sampler can intercept backwards particles

    coordinates[0] = s->x[i];
    coordinates[1] = s->xp[i];
    coordinates[2] = s->y[i];
    coordinates[3] = s->yp[i];
    coordinates[4] = std::sqrt(std::pow(s->energy[i],2) - m2); // p = sqrt(E^2 - M^2)
    coordinates[5] = s->T[i];

    if (firstTime)
      {offsets = coordinates;}

    // power sums
    for(int a=0;a<6;++a)
      {
	for(int b=0;b<6;++b)
	  {
	    for (int j = 0; j <= 4; ++j)
	      {
		for (int k = 0; k <= 4; ++k)
		  {
		    powSums[a][b][j][k] += std::pow(coordinates[a]-offsets[a],j)*std::pow(coordinates[b]-offsets[b],k);
		  }
	      }
	  }
      }
  npart++;  
  }
}

std::vector<double> SamplerAnalysis::Terminate(std::vector<double> emittance,
					       bool useEmittanceFromFirstSampler)
{
  if(debug)
    {std::cout << " " << __METHOD_NAME__ << s->modelID << " " << npart << std::flush;}

  // determine whether the input emittance is non-zero
  bool nonZeroEmittanceIn = !std::all_of(emittance.begin(), emittance.end(), [](double l) { return l==0; });

  // central moments
  for(int a=0;a<6;++a)
  {
    for(int b=0;b<6;++b)
      {
	for (int j = 0; j <= 4; ++j)
	  {
	    for (int k = 0; k <= 4; ++k)
	      {
		cenMoms[a][b][j][k] = powSumToCentralMoment(powSums, npart, a, b, j, k);
	      }
	  }
      }
  }

  if (debug)
    {printBeamCorrelationMatrix(cenMoms);}

  // optical function calculation
  for(int i=0;i<3;++i)
  {
    int j = 0;
    if(i == 1) j = 2;
    if(i == 2) j = 4;

    // note: optical functions vector not populated in sequential order in order
    // to apply dispersion correction to lattice funcs.

    optical[i][6]  = cenMoms[j][j+1][1][0]; // mean spatial (transv.)/ mean P (longit.)
    optical[i][7]  = cenMoms[j][j+1][0][1]; // mean divergence (transv.)/ mean t (longit.)
    optical[i][8]  = sqrt(cenMoms[j][j+1][2][0]); // sigma spatial   (transv.)/ sigma P (longit.)
    optical[i][9]  = sqrt(cenMoms[j][j+1][0][2]); // sigma divergence (transv.)/ sigma t (longit.)

    // transverse optical function calculation skipped for longitudinal plane,
    // only mean and sigma of longit. coordinates recorded
    if (i==2)
      {continue;}

    optical[i][4]  = (cenMoms[4][4][1][0]*cenMoms[j][4][1][1])/cenMoms[4][4][2][0];    // eta
    optical[i][5]  = (cenMoms[4][4][1][0]*cenMoms[j+1][4][1][1])/cenMoms[4][4][2][0];  // eta prime

    // check for nans (expected if dP=0) and set disp=0 if found
    if (!std::isfinite(optical[i][4]))
      {optical[i][4] = 0;}
    if (!std::isfinite(optical[i][5]))
      {optical[i][5] = 0;}

    // temp variables to store dispersion corrected moments, aka moments without dispersion contribution
    double corrCentMom_2_0 = 0.0, corrCentMom_1_1 = 0.0;
    double corrCentMom_0_2 = 0.0;

    //Correction example: var[x]_corrected = var[x] - eta_x^2*sigma[dP/P]^2
    corrCentMom_2_0 =
      cenMoms[j][j+1][2][0] - std::pow(optical[i][4],2)*cenMoms[4][4][2][0]/std::pow(cenMoms[4][4][1][0],2);

    corrCentMom_0_2 =
      cenMoms[j][j+1][0][2] - std::pow(optical[i][5],2)*cenMoms[4][4][2][0]/std::pow(cenMoms[4][4][1][0],2);

    corrCentMom_1_1 =
      cenMoms[j][j+1][1][1] - (optical[i][4]*optical[i][5])*cenMoms[4][4][2][0]/std::pow(cenMoms[4][4][1][0],2);

    if(useEmittanceFromFirstSampler && nonZeroEmittanceIn)
      {optical[i][0]  = emittance[i];}
    else
      {optical[i][0] = sqrt(corrCentMom_2_0 * corrCentMom_0_2 - std::pow(corrCentMom_1_1, 2));}  //emittance

    optical[i][1]  = -corrCentMom_1_1/optical[i][0];                                             // alpha
    optical[i][2]  = corrCentMom_2_0/optical[i][0];                                              // beta
    optical[i][3]  = (1+std::pow(optical[i][1],2))/optical[i][2];                                // gamma

    optical[i][10] = S;
    optical[i][11] = (double)npart;
  }

  // statistical error calculation
  
  // covariance matrix of central moments for optical functions.
  for(int i=0;i<3;++i)
    {
      for(int j=0;j<3;++j)
	{
	  for(int k=0;k<3;++k)
	    {
	      covMats[i][j][k]=centMomToCovariance(cenMoms, npart, i, j, k);
	    }
	}
    }
  
  // derivative matrix of parameters for optical functions. Each entry is
  // a first order derivative w.r.t central moments.  
  for(int i=0;i<3;++i)
    {
      for(int j=0;j<12;++j) //loop over optical functions.
	{
	  for(int k=0;k<3;++k) //loop over derivative indices
	    {
	      derivMats[i][j][k]=centMomToDerivative(cenMoms, i, j, k);
	    }
	}
    }
  
  // compute variances of optical functions
  for(int i=0;i<3;++i)      
    {
      for(int j=0;j<12;++j)
	{
	  for(int k=0;k<3;++k)
	    {
	      for(int l=0;l<3;++l)
		{
		  varOptical[i][j] += derivMats[i][j][k]*derivMats[i][j][l]*covMats[i][k][l];
		}
	    }
	}
    }

  // compute the sigmas of optical functions
  for(int i=0;i<3;++i)      
    {
      for(int j=0;j<12;++j)
	{
	  if(j==6 || j==7)
	    {optical[i][j+12]=optical[i][j+2]/sqrt(npart);} //errors of the means 
	  else if(j == 10 || j == 11)
	    {optical[i][j+12]= 0.0;}                        //no errors on S and Npart
          else if (j==0)
            {
                if(useEmittanceFromFirstSampler && nonZeroEmittanceIn)
                {
                    optical[i][j+12]=emittance[j+2];
                }
                else
                {
                    optical[i][j+12]=sqrt(varOptical[i][j]);
                }
            }
	  else
	    {optical[i][j+12]=sqrt(varOptical[i][j]);}
	}
    }

  // convert meanP back to meanE
  double m2 = std::pow(particleMass,2);
  double meanP = optical[2][6];
  optical[2][6] = std::sqrt(std::pow(meanP,2) + m2);

  //convert sigmaP back to SigmaE
  double gamma = optical[2][6] / particleMass;
  double beta = std::sqrt(1.0 - 1.0/std::pow(gamma,2));
  double sigmaP = optical[2][8];
  optical[2][8] = (sigmaP/meanP)*std::pow(beta,2);

  // write out the correlation x-y coefficient to the output as a matrix of horizontal-vertical coupling
  // writen only to the x vector, but 0 is added to y and z vectors to keep all vector sizes the same
  optical[0][24]=cenMoms[0][2][1][1]/std::sqrt(cenMoms[0][2][2][0]*cenMoms[0][2][0][2]);

  // emitt_x, emitt_y, err_emitt_x, err_emitt_y
  std::vector<double> emittanceOut = {optical[0][0],
				      optical[1][0],
				      optical[0][12],
				      optical[1][12]};

  return emittanceOut;
}

double SamplerAnalysis::powSumToCentralMoment(fourDArray&   powSumsIn,
					      long long int npartIn,
					      int a,
					      int b,
					      int m,
					      int n)
{
  double moment = 0.0;

  // Explicitly multiply out the number of particles to achieve the appropriate power.          
  // Typecast to a double is done to avoid integer overflow for large number of particles.
  double npartPow1 = (double)npartIn;

  // Done because of potential problems with the pow() funtion in C++ where some
  // arguments cause NaNs
  double npartPow2 = npartPow1 * npartPow1;
  double npartPow3 = npartPow2 * npartPow1;   
  double npartPow4 = npartPow3 * npartPow1;

  if((m == 1 && n == 0) || (m == 0 && n == 1))
    {
      double s_1_0 = powSumsIn[a][b][m][n];
      int k = m > n ? a : b;

      moment = s_1_0/(double)npartIn+offsets[k];
    }

  else if((n == 2 && m == 0) || (n == 0 && m == 2))
    {
      double s_1_0 = 0.0, s_2_0 = 0.0;
      if(m == 2)
	{
	  s_1_0 = powSumsIn[a][b][m-1][n];
	  s_2_0 = powSumsIn[a][b][m][n];
	}
      else if(n == 2)
	{
	  s_1_0 = powSumsIn[a][b][m][n-1];
	  s_2_0 = powSumsIn[a][b][m][n];
	}
      
      moment =  (npartPow1*s_2_0 - std::pow(std::abs(s_1_0),2))/(npartPow1*(npartPow1-1));
    }
  
  else if(n == 1 && m == 1)
    {
      double s_1_0 = 0.0, s_0_1 = 0.0, s_1_1 = 0.0;
      
      s_1_0 = powSumsIn[a][b][m][n-1];
      s_0_1 = powSumsIn[a][b][m-1][n];
      s_1_1 = powSumsIn[a][b][m][n];

      moment =  (npartPow1*s_1_1 - s_0_1*s_1_0)/(npartPow1*(npartPow1-1));
    }
  
  else if((n == 4 && m == 0) || (n == 0 && m == 4))
    {
      double s_1_0 = 0.0, s_2_0 = 0.0, s_3_0 = 0.0, s_4_0 = 0.0;
      if(m == 4)
	{
	  s_1_0 = powSumsIn[a][b][m-3][n];
	  s_2_0 = powSumsIn[a][b][m-2][n];
	  s_3_0 = powSumsIn[a][b][m-1][n];
	  s_4_0 = powSumsIn[a][b][m][n];
	}
      else if( n == 4)
	{
	  s_1_0 = powSumsIn[a][b][m][n-3];
	  s_2_0 = powSumsIn[a][b][m][n-2];
	  s_3_0 = powSumsIn[a][b][m][n-1];
	  s_4_0 = powSumsIn[a][b][m][n];
	}
      
      moment = - (3*std::pow(s_1_0,4))/npartPow4 + (6*std::pow(s_1_0,2)*s_2_0)/npartPow3
	- (4*s_1_0*s_3_0)/npartPow2 + s_4_0/npartPow1;
    }
  
  else if((m == 3 && n == 1) || (m == 1 && n ==3))
    {
      double s_1_0 = 0.0, s_0_1 = 0.0, s_1_1 = 0.0, s_2_0 = 0.0, s_2_1 = 0.0, s_3_0 = 0.0, s_3_1 = 0.0;
      
      if(m == 3)
	{
	  s_1_0 = powSumsIn[a][b][m-2][n-1];
	  s_0_1 = powSumsIn[a][b][m-3][n];
	  s_1_1 = powSumsIn[a][b][m-2][n];
	  s_2_0 = powSumsIn[a][b][m-1][n-1];
	  s_2_1 = powSumsIn[a][b][m-1][n];
	  s_3_0 = powSumsIn[a][b][m][n-1];
	  s_3_1 = powSumsIn[a][b][m][n];
	}
      else if(n == 3)
	{
	  s_1_0 = powSumsIn[a][b][m-1][n-2];
	  s_0_1 = powSumsIn[a][b][m][n-3];
	  s_1_1 = powSumsIn[a][b][m][n-2];
	  s_2_0 = powSumsIn[a][b][m-1][n-1];
	  s_2_1 = powSumsIn[a][b][m][n-1];
	  s_3_0 = powSumsIn[a][b][m-1][n];
	  s_3_1 = powSumsIn[a][b][m][n];
	}
      
      moment = - (3*s_0_1*std::pow(s_1_0,3))/npartPow4 + (3*s_1_0*s_1_0*s_1_1)/npartPow3
	+ (3*s_0_1*s_1_0*s_2_0)/npartPow3 - (3*s_1_0*s_2_1)/npartPow2
	- (s_0_1*s_3_0)/npartPow2 + s_3_1/npartPow1;
    }
  
  else if(m == 2 && n == 2)
    {
      double s_1_0 = 0.0, s_0_1 = 0.0, s_1_1 = 0.0, s_2_0 = 0.0, s_0_2 = 0.0, s_1_2 = 0.0, s_2_1 = 0.0, s_2_2 = 0.0;
      
      s_1_0 = powSumsIn[a][b][m-1][n-2];
      s_0_1 = powSumsIn[a][b][m-2][n-1];
      s_1_1 = powSumsIn[a][b][m-1][n-1];
      s_2_0 = powSumsIn[a][b][m][n-2];
      s_0_2 = powSumsIn[a][b][m-2][n];
      s_1_2 = powSumsIn[a][b][m-1][n];
      s_2_1 = powSumsIn[a][b][m][n-1];
      s_2_2 = powSumsIn[a][b][m][n];

      moment = - (3*std::pow(s_0_1,2)*std::pow(s_1_0,2))/npartPow4 + (s_0_2*std::pow(s_1_0,2))/npartPow3
	       + (4*s_0_1*s_1_0*s_1_1)/npartPow3 - (2*s_1_0*s_1_2)/npartPow2
	       + (std::pow(s_0_1,2)*s_2_0)/npartPow3 - (2*s_0_1*s_2_1)/npartPow2 + s_2_2/npartPow1;
    }

    return moment;
}

double SamplerAnalysis::centMomToCovariance(fourDArray&   centMoms,
					    long long int npartIn,
					    int k,
					    int i,
					    int j)
{
  double npartInD = (double)npartIn;
  double cov = 0.0;

  int a = 0;
  if(k == 1) {a=2;}
  if(k == 2) {a=4;}

  if((i == 0 && j == 0) || (i == 1 && j == 1))
    {
      double m_4_0 = 0.0, m_2_0 = 0.0;
      
      if(i == 0)
	{	
	  m_4_0 = centMoms[a][a+1][4][0];
	  m_2_0 = centMoms[a][a+1][2][0];
	}
      
      else if(i == 1)
	{
	  m_4_0 = centMoms[a][a+1][0][4];
	      m_2_0 = centMoms[a][a+1][0][2];
	}

      cov = -((npartInD-3)*std::pow(m_2_0,2))/(npartInD*(npartInD-1)) + m_4_0/npartInD;
    }
  
  else if(i == 2 && j == 2)
    {
      double m_1_1 = 0.0, m_2_0 = 0.0, m_0_2 = 0.0, m_2_2 = 0.0;

      m_1_1 = centMoms[a][a+1][1][1];
      m_2_0 = centMoms[a][a+1][2][0];
      m_0_2 = centMoms[a][a+1][0][2];
      m_2_2 = centMoms[a][a+1][2][2];

      cov = -((npartInD-2)*std::pow(m_1_1,2))/(npartInD*(npartInD-1)) + (m_0_2*m_2_0)/(npartInD*(npartInD-1)) + m_2_2/npartInD;
    }

  else if((i == 0 && j == 2) || (i == 1 && j == 2) || (i == 2 && j == 0) || (i == 2 && j == 1))
  {
    double m_1_1 = 0.0, m_2_0 = 0.0, m_3_1 = 0.0;

    if((i == 0 && j == 2) || (i == 2 && j == 0 ))
    {
      m_1_1 = centMoms[a][a+1][1][1];
      m_2_0 = centMoms[a][a+1][2][0];
      m_3_1 = centMoms[a][a+1][3][1];
    }
    else if((i == 1 && j == 2) || (i == 2 && j == 1))
    {
      m_1_1 = centMoms[a][a+1][1][1];
      m_2_0 = centMoms[a][a+1][0][2];
      m_3_1 = centMoms[a][a+1][1][3];
    }

    cov = -((npartInD-3)*m_1_1*m_2_0)/(npartInD*(npartInD-1)) + m_3_1/npartInD;
  }
  else if((i == 0 && j == 1) || (i == 1 && j == 0) )
  {
    double m_1_1 = 0.0, m_2_0 = 0.0, m_0_2 = 0.0,  m_2_2 = 0.0;
      
    m_1_1 = centMoms[a][a+1][1][1];
    m_2_0 = centMoms[a][a+1][0][2];
    m_0_2 = centMoms[a][a+1][2][0];
    m_2_2 = centMoms[a][a+1][2][2];

    cov = 2*std::pow(m_1_1,2)/(npartInD*(npartInD-1)) - m_2_0*m_0_2/npartInD + m_2_2/npartInD;
  }

  return cov;
}

double SamplerAnalysis::centMomToDerivative(fourDArray& centMoms,
					    int k,
					    int t,
					    int i)
 
{
  double deriv = 0.0;

  int a = 0;
  if(k == 1) {a=2;}
  if(k == 2) {a=4;}
  
  switch(t)   
    {
    case 0:
      //emittance
      if(i == 0 && k < 2)  // k<2 check selects transverse planes, longitudinal parameters are not calculated
      {
	deriv = centMoms[a][a+1][0][2]/(2*sqrt(centMoms[a][a+1][2][0]*centMoms[a][a+1][0][2]-std::pow(centMoms[a][a+1][1][1],2)));
      }
      else if(i == 1 && k < 2)
      {
	deriv = centMoms[a][a+1][2][0]/(2*sqrt(centMoms[a][a+1][2][0]*centMoms[a][a+1][0][2]-std::pow(centMoms[a][a+1][1][1],2)));
      }
      else if(i == 2 && k < 2)
      {
	deriv = -centMoms[a][a+1][1][1]/(sqrt(centMoms[a][a+1][2][0]*centMoms[a][a+1][0][2]-std::pow(centMoms[a][a+1][1][1],2)));
      }
      else {deriv=0;}
      
      return deriv;
      break;
      
    case 1:
      //alpha
      if(i == 0 && k < 2) 
      {
	deriv = centMoms[a][a+1][0][2]*centMoms[a][a+1][1][1]/(2*std::pow(centMoms[a][a+1][2][0]*centMoms[a][a+1][0][2]-std::pow(centMoms[a][a+1][1][1],2),3./2.));
      }
      else if(i == 1 && k < 2)
      {
	deriv = centMoms[a][a+1][2][0]*centMoms[a][a+1][1][1]/(2*std::pow(centMoms[a][a+1][2][0]*centMoms[a][a+1][0][2]-std::pow(centMoms[a][a+1][1][1],2),3./2.)); 
      }
      else if(i == 2 && k < 2)
      {
	deriv = - centMoms[a][a+1][2][0]*centMoms[a][a+1][0][2]/std::pow(centMoms[a][a+1][2][0]*centMoms[a][a+1][0][2]-std::pow(centMoms[a][a+1][1][1],2),3./2.);
      }
      else {deriv=0;}
      
      return deriv;
      break;

    case 2:
      //beta
      if(i == 0 && k < 2) 
      {
	deriv = (centMoms[a][a+1][2][0]*centMoms[a][a+1][0][2]-2*std::pow(centMoms[a][a+1][1][1],2))/(2*std::pow(centMoms[a][a+1][2][0]*centMoms[a][a+1][0][2]-std::pow(centMoms[a][a+1][1][1],2),3./2.)); 
      }
      else if(i == 1 && k < 2)
      {
	deriv = - std::pow(centMoms[a][a+1][2][0],2)/(2*std::pow(centMoms[a][a+1][2][0]*centMoms[a][a+1][0][2]-std::pow(centMoms[a][a+1][1][1],2),3./2.));
      }
      else if(i == 2 && k < 2)
      {
	deriv = centMoms[a][a+1][2][0]*centMoms[a][a+1][1][1]/std::pow(centMoms[a][a+1][2][0]*centMoms[a][a+1][0][2]-std::pow(centMoms[a][a+1][1][1],2),3./2.);
      }
      else {deriv=0;}
      
      return deriv;
      break;


    case 3:
      //gamma
      return 0; // TODO
      break;

    case 4:
      //eta
      if(i == 0 && k < 2)
      {
	deriv = 0;
      }
      else if(i == 1 && k < 2)
      {
	deriv = -centMoms[a][4][1][1]/std::pow(centMoms[4][4][2][0],2);
      }
      else if(i == 2 && k < 2)
      {
	deriv = 1/centMoms[4][4][2][0];
      }
      else {deriv=0;}
      
      return deriv;
      break;

    case 5:
      //eta prime
      if(i == 0 && k < 2)
      {
	deriv = 0;
      }
      else if(i == 1 && k < 2)
      {
	deriv = -centMoms[a+1][4][1][1]/std::pow(centMoms[4][4][2][0],2);
      }
      else if(i == 2 && k < 2)
      {
	deriv = 1/centMoms[4][4][2][0];
      }
      else {deriv=0;}
      
      return deriv;
      break;

    case 6:                // mean derivatives are all 0 as they don't depend on second order moments
      //mean spatial    
      return 0;              
      break;

    case 7:
      //mean divergence
      return 0;
      break;

    case 8:
      //sigma spatial
      if(i == 0)
      {
	deriv = 1/(2*sqrt(centMoms[a][a+1][2][0]));
      }
      
      else {deriv=0;}
      
      return deriv;
      break;

    case 9:
      //sigma divergence
      if(i == 1)
      {
	deriv = 1/(2*sqrt(centMoms[a][a+1][0][2]));
      }
      
      else {deriv=0;}
      
      return deriv;
      break;
      
    default:
      return 0;
      break;
    }
}


void SamplerAnalysis::printBeamCorrelationMatrix(fourDArray&   centMoms)
{
  std::cout<<"\nCorrelation matrix for sampler: "<<s->samplerName<<std::endl;
  double corr = 0.0;
  for (int i=0; i<6; i++)
    {
      for(int j=0; j<6; j++)
        {
          corr = centMoms[i][j][1][1]/std::sqrt(centMoms[i][j][2][0]*centMoms[i][j][0][2]);
          std::printf("%- *.6e ", 12, corr);
        }
      std::printf("\n");
    }

}
