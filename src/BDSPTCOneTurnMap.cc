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
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSGlobalConstants.hh"
#include "BDSParticleCoordsFullGlobal.hh"
#include "BDSParticleDefinition.hh"
#include "BDSPTCOneTurnMap.hh"
#include "BDSTrajectoryPrimary.hh"
#include "BDSUtilities.hh"

#include "globals.hh" // Geant4 typedefs

#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>
#include <fstream>
#include <set>
#include <sstream>
#include <string>

BDSPTCOneTurnMap::BDSPTCOneTurnMap(const G4String& maptableFile,
				   const BDSParticleDefinition* designParticle):
  initialPrimaryMomentum(0),
  beamOffsetS0(false),
  lastTurnNumber(0),
  xLastTurn(0),
  pxLastTurn(0),
  yLastTurn(0),
  pyLastTurn(0),
  deltaPLastTurn(0)
{
  referenceMomentum = designParticle->Momentum();
  mass = designParticle->Mass();

  G4String filePath = BDS::GetFullPath(maptableFile);
  G4cout << __METHOD_NAME__ << "Using map table " << filePath << G4endl;
  std::ifstream infile(filePath);
  if (!infile)
    {throw BDSException(__METHOD_NAME__, "Failed to read maptable: \"" + maptableFile + "\"");}

  // The columns of the maptable TFS (read into below with the stringstream).
  G4String name = "";
  G4double coefficient = 0;
  G4int nVector = 0;
  G4int dimensionality = 0;
  G4int totalOrder = 0;
  G4int nx = 0;
  G4int npx = 0;
  G4int ny = 0;
  G4int npy = 0;
  G4int ndeltaP = 0;
  G4int nt = 0;

  G4String line = "";
  while (std::getline(infile, line))
    {
      if (line.at(0) == '@' || line.at(0) == '*' || line.at(0) == '$')
	{continue;}
      std::istringstream stream(line);

      stream >> name >> coefficient >> nVector >> dimensionality >> totalOrder >>
        nx >> npx >> ny >> npy >> ndeltaP >> nt;

      PTCMapTerm term{coefficient, nx, npx, ny, npy, ndeltaP};

      switch (nVector)
	{
	case 1:
	  {xTerms.push_back(term); break;}
	case 2:
	  {pxTerms.push_back(term); break;}
	case 3:
	  {yTerms.push_back(term);  break;}
	case 4:
	  {pyTerms.push_back(term); break;}
	case 5:
	  {deltaPTerms.push_back(term); break;}
	default:
	  {
	    throw BDSException(__METHOD_NAME__, "Unrecognised PTC term index - maptable file is perhaps malformed.");
	    break;
	  }
	}
    }
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "> Loaded Map:" << maptableFile << G4endl;
#endif
}

void BDSPTCOneTurnMap::SetInitialPrimaryCoordinates(const BDSParticleCoordsFullGlobal& coords,
						    G4bool beamOffsetS0In)
{
  lastTurnNumber = BDSGlobalConstants::Instance()->TurnsTaken();
  initialPrimaryMomentum = std::sqrt(std::pow(coords.local.totalEnergy, 2) - std::pow(mass, 2));
  // Converting to PTC Coordinates:
  xLastTurn  = coords.local.x / CLHEP::m;
  pxLastTurn = coords.global.xp * initialPrimaryMomentum / referenceMomentum;
  yLastTurn  = coords.local.y / CLHEP::m;
  pyLastTurn = coords.global.yp * initialPrimaryMomentum / referenceMomentum;
  deltaPLastTurn = (initialPrimaryMomentum - referenceMomentum) / referenceMomentum;

  turnsScattered.clear();

  // If we're using curvilinear then S0 != 0 and we shouldn't apply
  // map on first turn.  record this setting here for the teleporter's
  // consideration.
  beamOffsetS0 = beamOffsetS0In;
}

void BDSPTCOneTurnMap::GetThisTurn(G4double& x,
				   G4double& px,
				   G4double& y,
                   G4double& py,
				   G4double& pz,
				   G4int turnsTaken)
{
  G4double xOut = 0.0;
  G4double yOut = 0.0;
  G4double pxOut = 0.0;
  G4double pyOut = 0.0;
  G4double pzOut = 0.0;
  G4double deltaPOut = 0.0;

  // In short: lastTurnNumber exists to prevent the map being
  // applied multiple times in one turn.
  // The terminator is placed before the teleporter, so on the "first
  // turn" in the teleporter (where this method is called),
  // TurnsTaken() will actually return 2.  So lastTurnNumber, will be
  // one less than returned by TurnsTaken(), until it is incremented
  // below.  If (lastTurnNumber ==
  // turnsTaken (turn number seen in the TeleporterIntegrator), then
  // the map has already been applied on this turn.  In which case,
  // return the cached values below.
  if (lastTurnNumber < turnsTaken)
    {
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "Applying Map: " << G4endl;
      G4cout << "Before map application: " << G4endl;
      G4cout << "xLastTurn = " << xLastTurn << G4endl;
      G4cout << "pxLastTurn = " << pxLastTurn << G4endl;
      G4cout << "yLastTurn = " << yLastTurn << G4endl;
      G4cout << "pyLastTurn = " << pyLastTurn << G4endl;
#endif

      lastTurnNumber = turnsTaken;
      xOut = Evaluate(xTerms,
		      xLastTurn, pxLastTurn,
		      yLastTurn, pyLastTurn,
		      deltaPLastTurn);
      pxOut = Evaluate(pxTerms,
		       xLastTurn, pxLastTurn,
		       yLastTurn, pyLastTurn,
		       deltaPLastTurn);
      yOut = Evaluate(yTerms,
		      xLastTurn, pxLastTurn,
		      yLastTurn, pyLastTurn,
		      deltaPLastTurn);
      pyOut = Evaluate(pyTerms,
		       xLastTurn, pxLastTurn,
		       yLastTurn, pyLastTurn,
		       deltaPLastTurn);
      deltaPOut = Evaluate(deltaPTerms,
			   xLastTurn, pxLastTurn,
			   yLastTurn, pyLastTurn,
			   deltaPLastTurn);
      // Cache results for next turn.  Do it here, before we convert to BDSIM coordinates.
      xLastTurn      = xOut;
      pxLastTurn     = pxOut;
      yLastTurn      = yOut;
      pyLastTurn     = pyOut;
      deltaPLastTurn = deltaPOut;
#ifdef BDSDEBUG
      G4cout << "After map application: " << G4endl;
      G4cout << "xOut = " << xOut << G4endl;
      G4cout << "pxOut = " << pxOut << G4endl;
      G4cout << "yOut = " << yOut << G4endl;
      G4cout << "pyOut = " << pyOut << G4endl;
#endif
    }
  else
    {
      xOut      = xLastTurn;
      pxOut     = pxLastTurn;
      yOut      = yLastTurn;
      pyOut     = pyLastTurn;
      deltaPOut = deltaPLastTurn;
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "Returning Cached Values:" << G4endl;
      G4cout << "xOut = " << xOut << G4endl;
      G4cout << "pxOut = " << pxOut << G4endl;
      G4cout << "yOut = " << yOut << G4endl;
      G4cout << "pyOut = " << pyOut << G4endl;
#endif
    }


  // Now convert to BDSIM units:
  // Convert local positions from metres to millimetres.
  xOut *= CLHEP::m;
  yOut *= CLHEP::m;

  // PTC momenta are scaled w.r.t the _REFERENCE_MOMENTUM_.
  pxOut *= referenceMomentum;
  pyOut *= referenceMomentum;
  // by defn the particle has the initial primary momentum, which we
  // used to calculate pz.
  pzOut = std::sqrt(std::pow(initialPrimaryMomentum, 2)
		    - std::pow(px, 2)
		    - std::pow(py, 2));

  // Now set output for arguments passed by reference.
  x  = xOut;
  px = pxOut;
  y  = yOut;
  py = pyOut;
  pz = pzOut;

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "Converting PTC to BDSIM " << G4endl;
  G4cout << "xOut = " << xOut << G4endl;
  G4cout << "pxOut = " << pxOut << G4endl;
  G4cout << "yOut = " << yOut << G4endl;
  G4cout << "pyOut = " << pyOut << G4endl;
  G4cout << "initialPrimaryMomentum = " << initialPrimaryMomentum << G4endl;
  G4cout << "referenceMomentum = " << referenceMomentum << G4endl;
  G4cout << "pzOut = " << pz << G4endl;
#endif
}

G4double BDSPTCOneTurnMap::Evaluate(std::vector<PTCMapTerm>& terms,
				    G4double x,
                                    G4double px,
				    G4double y,
				    G4double py,
                                    G4double deltaP) const
{
  G4double result = 0;
  for (const auto& term : terms)
    {
      result += (term.coefficient
		 * std::pow(x, term.nx)
		 * std::pow(px, term.npx)
		 * std::pow(y, term.ny)
		 * std::pow(py, term.npy)
		 * std::pow(deltaP, term.ndeltaP));
    }
  return result;
}

G4bool BDSPTCOneTurnMap::ShouldApplyToPrimary(G4double momentum,
                                              G4int turnsTaken)
{
  // We have to use the externally provided turnsTaken rather than
  // internal lastTurnNumber so that the OTM is definitely not applied
  // in this case (because lastTurnNumber member of this class will
  // not have the same value for multiple applications on the same
  // turn) 2 and not 1 because teleporter comes after
  // terminator, where the turn number is incremented.
  G4bool offsetBeamS0AndOnFirstTurn = beamOffsetS0 && turnsTaken == 2;

  // We reset the public static bool hasScatteredThisTurn at the end
  // of this method.  But what if the stepper is applied again on this
  // turn?  then we would be lead to believe it did not scatter when
  // it in fact did, so we cache the turns in which it scattered so
  // that this method returns the same result for calls on the same
  // turn for the same primary.  This is necessary because we can't
  // force the Teleporter stepper to be called just once.
  G4bool didScatterThisTurn = BDSTrajectoryPrimary::hasScatteredThisTurn ||
                            turnsScattered.count(turnsTaken);

  // We always insert it into the set as nothing happens if it already exists, so
  // we're safe inserting it every time for simplicity.
  if (didScatterThisTurn)
    {turnsScattered.insert(turnsTaken);}

  // Have some tolerance for dealing with primaries far off momentum.
  G4double ratioOffReference = std::abs((momentum - referenceMomentum) / referenceMomentum);
  G4double tolerance = 0.05; // arbitrarily chosen 5%
  G4bool tooFarOffMomentum = ratioOffReference > tolerance;

  G4bool should = !offsetBeamS0AndOnFirstTurn && !didScatterThisTurn && !tooFarOffMomentum;

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__
         << "scatteredThisTurn = " << BDS::BoolToString(didScatterThisTurn)
         << G4endl;
  G4cout << __METHOD_NAME__
         << "beamOffsetS0 = " << BDS::BoolToString(beamOffsetS0) << G4endl;
  G4cout << __METHOD_NAME__ << "turnsTaken = " << turnsTaken << G4endl;
  G4cout << __METHOD_NAME__ << "Is on first turn with S0 != 0?"
         << BDS::BoolToString(offsetBeamS0AndOnFirstTurn) << G4endl;
  G4cout << __METHOD_NAME__
         << "tooFarOffMomentum = " << BDS::BoolToString(tooFarOffMomentum)
         << G4endl;
  G4cout << __METHOD_NAME__ << "ShouldApply = " << BDS::BoolToString(should)
         << G4endl;
#endif
  // Reset the static public bool as we (maybe) start the next turn.
  BDSTrajectoryPrimary::hasScatteredThisTurn = false;
  return should;
}

void BDSPTCOneTurnMap::UpdateCoordinates(G4ThreeVector localPosition,
                                         G4ThreeVector localMomentum,
					 G4int turnsTaken)
{
  if (lastTurnNumber < turnsTaken)
    {
      // This method is called in the integrator if the OTM is active but
      // NOT applicable.  So given that the TeleporterIntegrator will be called
      // multiple times, whatever happens here, it should not suddenly
      // make the OTM applicable for subsequent calls to the Teleporter
      // stepper on the same turn.
      xLastTurn = localPosition.x() / CLHEP::m;
      yLastTurn = localPosition.y() / CLHEP::m;
      pxLastTurn = localMomentum.x() / referenceMomentum;
      pyLastTurn = localMomentum.y() / referenceMomentum;
      G4double totalMomentum = localMomentum.mag();
      deltaPLastTurn = (totalMomentum - referenceMomentum) / referenceMomentum;
      // deltaPLastTurn assumed to not change between turns for 5D map.
      lastTurnNumber = turnsTaken;
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__
	     << "Updating map coords without use of map:" << G4endl;
      G4cout << "xLastTurn = " << xLastTurn << G4endl;
      G4cout << "yLastTurn = " << yLastTurn << G4endl;
      G4cout << "pxLastTurn = " << pxLastTurn << G4endl;
      G4cout << "pyLastTurn = " << pyLastTurn << G4endl;
#endif
    }
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__
         << "Skipping further coordinate updates without use of map" << G4endl;
#endif
}
