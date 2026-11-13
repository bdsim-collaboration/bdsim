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
#include "BDSBunchEventGenerator.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSParticleCoordsFull.hh"
#include "BDSPhysicsUtilities.hh"
#include "BDSUtilities.hh"
#include "BDSWarning.hh"

#include "parser/beam.h"

#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string> // for stoi

BDSBunchEventGenerator::BDSBunchEventGenerator():
  BDSBunchFileBased("eventgenerator"),
  eventGeneratorNEventsSkip(0),
  eventGeneratorMinX(0),
  eventGeneratorMaxX(0),
  eventGeneratorMinY(0),
  eventGeneratorMaxY(0),
  eventGeneratorMinZ(0),
  eventGeneratorMaxZ(0),
  eventGeneratorMinXp(0),
  eventGeneratorMaxXp(0),
  eventGeneratorMinYp(0),
  eventGeneratorMaxYp(0),
  eventGeneratorMinZp(0),
  eventGeneratorMaxZp(0),
  eventGeneratorMinRp(0),
  eventGeneratorMaxRp(0),
  eventGeneratorMinT(0),
  eventGeneratorMaxT(0),
  eventGeneratorMinEK(0),
  eventGeneratorMaxEK(0),
  Rp0(0),
  firstTime(true),
  testOnParticleType(true),
  acceptedParticlesString("")
{;}

BDSBunchEventGenerator::~BDSBunchEventGenerator() 
{;}

void BDSBunchEventGenerator::SetOptions(const BDSParticleDefinition* beamParticle,
                                        const GMAD::Beam&            beam,
                                        const BDSBunchType&          distrType,
                                        G4Transform3D                beamlineTransformIn,
                                        const G4double               beamlineSIn)
{
  BDSBunchFileBased::SetOptions(beamParticle, beam, distrType, beamlineTransformIn, beamlineSIn);
  
  eventGeneratorNEventsSkip = beam.eventGeneratorNEventsSkip;
  eventGeneratorMinX  = beam.eventGeneratorMinX * CLHEP::m;
  eventGeneratorMaxX  = beam.eventGeneratorMaxX * CLHEP::m;
  eventGeneratorMinY  = beam.eventGeneratorMinY * CLHEP::m;
  eventGeneratorMaxY  = beam.eventGeneratorMaxY * CLHEP::m;
  eventGeneratorMinZ  = beam.eventGeneratorMinZ * CLHEP::m;
  eventGeneratorMaxZ  = beam.eventGeneratorMaxZ * CLHEP::m;
  eventGeneratorMinXp = beam.eventGeneratorMinXp;
  eventGeneratorMaxXp = beam.eventGeneratorMaxXp;
  eventGeneratorMinYp = beam.eventGeneratorMinYp;
  eventGeneratorMaxYp = beam.eventGeneratorMaxYp;
  eventGeneratorMinZp = beam.eventGeneratorMinZp;
  eventGeneratorMaxZp = beam.eventGeneratorMaxZp;
  eventGeneratorMinRp = beam.eventGeneratorMinRp;
  eventGeneratorMaxRp = beam.eventGeneratorMaxRp;
  eventGeneratorMinT  = beam.eventGeneratorMinT * CLHEP::s;
  eventGeneratorMaxT  = beam.eventGeneratorMaxT * CLHEP::s;
  eventGeneratorMinEK = beam.eventGeneratorMinEK * CLHEP::GeV;
  eventGeneratorMaxEK = beam.eventGeneratorMaxEK * CLHEP::GeV;
  acceptedParticlesString = beam.eventGeneratorParticles;
  Rp0 = std::hypot(Xp0,Yp0);
}

void BDSBunchEventGenerator::CheckParameters()
{
  BDSBunch::CheckParameters();
  if (eventGeneratorNEventsSkip < 0)
    {throw BDSException(__METHOD_NAME__, "eventGeneratorNEventsSkip < 0");}
  if (eventGeneratorMinX >= eventGeneratorMaxX)
    {throw BDSException(__METHOD_NAME__, "eventGeneratorMinX >= eventGeneratorMaxX");}
  if (eventGeneratorMinY >= eventGeneratorMaxY)
    {throw BDSException(__METHOD_NAME__, "eventGeneratorMinY >= eventGeneratorMaxY");}
  if (eventGeneratorMinZ >= eventGeneratorMaxZ)
    {throw BDSException(__METHOD_NAME__, "eventGeneratorMinZ >= eventGeneratorMaxZ");}
  if (eventGeneratorMinXp >= eventGeneratorMaxXp)
    {throw BDSException(__METHOD_NAME__, "eventGeneratorMinXp >= eventGeneratorMaxXp");}
  if (eventGeneratorMinYp >= eventGeneratorMaxYp)
    {throw BDSException(__METHOD_NAME__, "eventGeneratorMinYp >= eventGeneratorMaxYp");}
  if (eventGeneratorMinZp >= eventGeneratorMaxZp)
    {throw BDSException(__METHOD_NAME__, "eventGeneratorMinZp >= eventGeneratorMaxZp");}
  if (eventGeneratorMinT >= eventGeneratorMaxT)
    {throw BDSException(__METHOD_NAME__, "eventGeneratorMinT >= eventGeneratorMaxT");}
  if (eventGeneratorMinEK >= eventGeneratorMaxEK)
    {throw BDSException(__METHOD_NAME__, "eventGeneratorMinEK >= eventGeneratorMaxEK");}
}

void BDSBunchEventGenerator::ParseAcceptedParticleIDs()
{
  if (!acceptedParticlesString.empty())
    {
      G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
      std::string particleIDStr;
      std::stringstream ss(acceptedParticlesString);
      while (ss >> particleIDStr)
        {
          try
            {// try and see if it's an integer and therefore PDG ID, if not search by string
              // we try this because std::stoi can throw a std::invalid_argument or
              // std::out_of_range exception, both of which inherit std::logic_error
              int particleID = std::stoi(particleIDStr);
              // we don't use the G4ParticleTable->FindParticle(int) because it unnecessarily
              // checks for physics readiness and throws an exception. here we just inspect
              // the encoding dictionary ourselves. it's all typedeffed but it's
              // std::map<G4int, G4ParticleDefinition*>
              G4ParticleTable::G4PTblEncodingDictionary* encoding = G4ParticleTable::fEncodingDictionary;
              auto search = encoding->find(particleID);
              if (search != encoding->end())
                {acceptedParticles.insert(particleID);}
              else
                {throw BDSException(__METHOD_NAME__,"PDG ID \"" + particleIDStr + "\" not found in particle table");}
            }
          catch (const std::logic_error&) // else, usual way by string search
            {
              G4ParticleDefinition* particleDef = particleTable->FindParticle(particleIDStr);
              if (!particleDef)
                {
                  BDS::PrintDefinedParticles();
                  throw BDSException(__METHOD_NAME__, "Particle \"" + particleIDStr + "\" not found.");      
                }
              else
                {acceptedParticles.insert(particleDef->GetPDGEncoding());}
            }   
        }
    }
  else
    {testOnParticleType = false;}
}

G4bool BDSBunchEventGenerator::AcceptParticle(const BDSParticleCoordsFull& coords,
                                              G4double rpOriginal,
                                              G4double kineticEnergy,
                                              G4int    pdgID)
{
  if (firstTime)
    {ParseAcceptedParticleIDs(); firstTime = false;}

  G4bool x  = coords.x  >= eventGeneratorMinX+X0   && coords.x  <= eventGeneratorMaxX+X0;
  G4bool y  = coords.y  >= eventGeneratorMinY+Y0   && coords.y  <= eventGeneratorMaxY+Y0;
  G4bool z  = coords.z  >= eventGeneratorMinZ      && coords.z  <= eventGeneratorMaxZ;
  G4bool xp = coords.xp >= eventGeneratorMinXp+Xp0 && coords.xp <= eventGeneratorMaxXp+Xp0;
  G4bool yp = coords.yp >= eventGeneratorMinYp+Yp0 && coords.yp <= eventGeneratorMaxYp+Yp0;
  G4bool zp = coords.zp >= eventGeneratorMinZp     && coords.zp <= eventGeneratorMaxZp;
  G4bool t  = coords.T  >= eventGeneratorMinT      && coords.T+T0  <= eventGeneratorMaxT+T0;
  G4bool ek = kineticEnergy >= eventGeneratorMinEK && kineticEnergy <= eventGeneratorMaxEK;
  G4bool rp = rpOriginal >= eventGeneratorMinRp && rpOriginal < eventGeneratorMaxRp;
  
  G4bool allowedParticle = true;
  if (testOnParticleType)
    {allowedParticle = acceptedParticles.count(pdgID) == 1;}
  
  return x && y && z && xp && yp && zp && rp && t && ek && allowedParticle;
}

G4RotationMatrix BDSBunchEventGenerator::ReferenceBeamMomentumOffset() const
{
  if (!BDS::IsFinite(Xp0) && !BDS::IsFinite(Yp0))
    {return G4RotationMatrix();}
  else
    {
      G4RotationMatrix result;
      result.rotateX(std::asin(-Yp0));
      result.rotateY(std::asin(Xp0));
      return result;
    }
}
