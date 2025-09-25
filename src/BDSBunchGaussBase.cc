/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2024.

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
#include "BDSBunchGaussBase.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSGlobalConstants.hh"

#include "parser/beam.h"

#include "Randomize.hh"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/RandomObjects/RandMultiGauss.h"
#include "CLHEP/Units/PhysicalConstants.h"

#include <cmath>
#include <vector>

/// helper method
namespace {
  /// check if matrix is positive definite
  bool isPositiveDefinite(CLHEP::HepSymMatrix& S)
  {
    CLHEP::HepSymMatrix temp (S); // Since diagonalize does not take a const s
                                  // we have to copy S.
    CLHEP::HepMatrix U = diagonalize ( &temp ); // S = U Sdiag U.T()
    CLHEP::HepSymMatrix D = S.similarityT(U);   // D = U.T() S U = Sdiag
    for (G4int i = 1; i <= S.num_row(); i++)
      {
        G4double s2 = D(i,i);
        if ( s2 <= 0 )
          {return false;}
      }
    return true;
  }
}

BDSBunchGaussBase::BDSBunchGaussBase(const G4String& nameIn):
  BDSBunch(nameIn),
  meansGM(CLHEP::HepVector(6)),
  sigmaGM(CLHEP::HepSymMatrix(6)),
  gaussMultiGen(nullptr),
  offsetSampleMean(false),
  iPartIteration(0)
{
  coordinates = {&x0_v, &xp_v, &y0_v, &yp_v, &z0_v, &zp_v, &E_v, &t_v, &weight_v};
}

BDSBunchGaussBase::~BDSBunchGaussBase()
{
  delete gaussMultiGen;
}

void BDSBunchGaussBase::SetOptions(const BDSParticleDefinition* beamParticle,
                                  const GMAD::Beam& beam,
                                  const BDSBunchType& distrType,
                                  G4Transform3D beamlineTransformIn,
                                  const G4double beamlineSIn)
{
  BDSBunch::SetOptions(beamParticle, beam, distrType, beamlineTransformIn, beamlineSIn);

  // require sigmaE to be finite for sigma matrix to be positive definite
  // set to very small value if 0
  if (!finiteSigmaE) // set in BDSBunch::SetOptions
    {sigmaE = 1e-50;}
  if (!finiteSigmaT)
    {sigmaT = 1e-50;}

  offsetSampleMean  = beam.offsetSampleMean;
  iPartIteration    = 0;

  // undo units and redo after the multivariate Gaussian
  // easier for the Gauss classes where we have sigma^2 in places
  meansGM[0] = X0  / CLHEP::m;
  meansGM[1] = Xp0;
  meansGM[2] = Y0  / CLHEP::m;
  meansGM[3] = Yp0;
  meansGM[4] = T0  / CLHEP::s;
  meansGM[5] = 1;
}

void BDSBunchGaussBase::BeginOfRunAction(G4int numberOfEvents,
                                        G4bool /*batchMode*/)
{
  if (!offsetSampleMean)
    {return;}
  /// clear previous means
  for (auto& vec : coordinates)
    {vec->clear();}
  iPartIteration = 0;
  
  PreGenerateEvents(numberOfEvents);
}

CLHEP::RandMultiGauss* BDSBunchGaussBase::CreateMultiGauss(CLHEP::HepRandomEngine& anEngine,
                                                          const CLHEP::HepVector& mu,
                                                          CLHEP::HepSymMatrix& sigma)
{
  /// check if sigma matrix is positive definite
  /// if not add small offset and cout warning
  G4cout << __METHOD_NAME__ << "checking 6x6 sigma matrix is positive definite" << G4endl;
  if (!isPositiveDefinite(sigma))
    {
      G4cout << __METHOD_NAME__ << "WARNING bunch generator sigma matrix is not positive definite" << G4endl;
      G4cout << sigma << G4endl;
      G4cout << __METHOD_NAME__ << "adding a small error to zero diagonal elements" << G4endl;
      
      // very small number especially for time, since the sigma goes with the square
      G4double small_error = 1e-50;
      
      for (G4int i=0; i<6; i++)
        {
          if (sigma[i][i]==0)
            {sigma[i][i] += small_error;}
        }
      
      if (!isPositiveDefinite(sigma))
        {
          G4cout << __METHOD_NAME__ << "WARNING bunch generator sigma matrix is still not positive definite" << G4endl;
          G4cout << sigma << G4endl;
          G4cout << __METHOD_NAME__ << "adding a small error to all elements" << G4endl;
          for (G4int i=0; i<6; i++)
            {
              for (G4int j=0; j<6; j++)
                {
                  if (sigma[i][j]==0)
                    {sigma[i][j] += small_error;}
                }
            }
          if (!isPositiveDefinite(sigma))
            {
              G4cout << sigma << G4endl;
              throw BDSException(__METHOD_NAME__, "bunch generator sigma matrix is still not positive definite, giving up");
            }
        }
    }

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "mean "  << G4endl
         << mu    << G4endl
         << __METHOD_NAME__ << "sigma " << G4endl
         << sigma << G4endl;
#endif
  G4cout << __METHOD_NAME__ << "confirmed: positive definite matrix" << G4endl;
  return new CLHEP::RandMultiGauss(anEngine,mu,sigma); 
}

void BDSBunchGaussBase::PreGenerateEvents(G4int nGenerate)
{
  G4cout << __METHOD_NAME__ << "Pregenerating " << nGenerate << " events." << G4endl;
  // generate all required primaries first
  G4double x_a = 0.0, xp_a = 0.0, y_a = 0.0, yp_a = 0.0;
  G4double z_a = 0.0, zp_a = 0.0, E_a  = 0.0, t_a  = 0.0;

  for (G4int iParticle = 0; iParticle < nGenerate; ++iParticle)
    {
      BDSParticleCoordsFull fire = GetNextParticleLocalCoords();

      G4double nT = (G4double)iParticle + 1;
      G4double d = 0;
      d    = fire.x - x_a;
      x_a  = x_a + (d/nT);
      d    = fire.xp - xp_a;
      xp_a = xp_a + (d/nT);
      d    = fire.y - y_a;
      y_a  = y_a + (d/nT);
      d    = fire.yp - yp_a;
      yp_a = yp_a + (d/nT);
      d    = fire.z - z_a;
      z_a  = z_a + (d/nT);
      d    = fire.zp - zp_a;
      zp_a = zp_a + (d/nT);
      d    = fire.totalEnergy - E_a;
      E_a  = E_a + (d/nT);
      d    = fire.T - t_a;
      t_a  = t_a + (d/nT);
      
      x0_v.push_back(fire.x);
      xp_v.push_back(fire.xp);
      y0_v.push_back(fire.y);
      yp_v.push_back(fire.yp);
      z0_v.push_back(fire.z);
      zp_v.push_back(fire.zp);
      E_v.push_back(fire.totalEnergy);
      t_v.push_back(fire.T);
      weight_v.push_back(fire.weight);
    }

  // Compute difference between sample mean and specified means
  x_a  = x_a  - X0;
  xp_a = xp_a - Xp0;
  y_a  = y_a  - Y0;
  yp_a = yp_a - Yp0;
  z_a  = z_a  - Z0;
  zp_a = zp_a - Zp0;
  E_a  = E_a  - E0;
  t_a  = t_a  - T0;

  // Offset with different w.r.t. central value
  for (G4int iParticle = 0; iParticle < nGenerate; ++iParticle)
    {
      x0_v[iParticle] -= x_a;
      xp_v[iParticle] -= xp_a;
      y0_v[iParticle] -= y_a;
      yp_v[iParticle] -= yp_a;
      z0_v[iParticle] -= z_a;
      zp_v[iParticle] -= zp_a;
      E_v[iParticle]  -= E_a;
      t_v[iParticle]  -= t_a;
    }
}
  
BDSParticleCoordsFull BDSBunchGaussBase::GetNextParticleLocal()
{
  if (offsetSampleMean)
    {
      // iPartIteration should never exceed the size of each vector.
      // the units are already correct in the vector of stored coordinates
      G4double x      = x0_v[iPartIteration];
      G4double xp     = xp_v[iPartIteration];
      G4double y      = y0_v[iPartIteration];
      G4double yp     = yp_v[iPartIteration];
      G4double z      = z0_v[iPartIteration];
      G4double zp     = zp_v[iPartIteration];
      G4double t      = t_v[iPartIteration];
      G4double E      = E_v[iPartIteration];
      G4double weight = weight_v[iPartIteration];
      
      iPartIteration++;
      return BDSParticleCoordsFull(x,y,z,xp,yp,zp,t,S0,E,weight);
    }
  else
    {return GetNextParticleLocalCoords();}
}


BDSParticleCoordsFull BDSBunchGaussBase::GetNextParticleLocalCoords()
{
  CLHEP::HepVector v = gaussMultiGen->fire();
  // unlike other bunch distributions reintroduce units (taken out in set options)
  G4double x  = v[0] * CLHEP::m;
  G4double xp = v[1];
  G4double y  = v[2] * CLHEP::m;
  G4double yp = v[3];
  G4double t  = T0;
  if (finiteSigmaT)
    {t = v[4];}
  t *= CLHEP::s;
  G4double zp = CalculateZp(xp,yp,Zp0);
  G4double z  = Z0;
  G4double dz = 0;

  G4double E  = E0; // exceptionally left in G4 units
  if (finiteSigmaE)
    {E *= v[5];} // only if there's a finite energy spread
  // cov-matrix is E_bar(1), with sigma fractional - so no units here
  
  return BDSParticleCoordsFull(x,y,z,xp,yp,zp,t,S0+dz,E,/*weight=*/1.0);
}
