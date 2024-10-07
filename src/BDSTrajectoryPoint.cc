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
#include "BDSAuxiliaryNavigator.hh"
#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh"
#include "BDSPhysicalVolumeInfoRegistry.hh"
#include "BDSPhysicalVolumeInfo.hh"
#include "BDSStep.hh"
#include "BDSTrajectoryPoint.hh"
#include "BDSTrajectoryPointIon.hh"
#include "BDSTrajectoryPointLocal.hh"
#include "BDSTrajectoryPointLink.hh"
#include "BDSPhysicalConstants.hh"
#include "BDSPhysicsUtilities.hh"
#include "BDSUtilities.hh"
#ifdef BDSDEBUG_H
#include "BDSProcessMap.hh"
#endif

#include "globals.hh"
#include "G4Allocator.hh"
#include "G4ProcessType.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4Track.hh"
#include "G4TransportationProcessType.hh"
#include "G4VProcess.hh"

#include <ostream>

class G4Material;

G4Allocator<BDSTrajectoryPoint> bdsTrajectoryPointAllocator;

G4double BDSTrajectoryPoint::dEThresholdForScattering = 1e-8;

// Don't use transform caching in the aux navigator as it's used for all over the geometry here.
BDSAuxiliaryNavigator* BDSTrajectoryPoint::auxNavigator = new BDSAuxiliaryNavigator();

BDSTrajectoryPoint::BDSTrajectoryPoint():
  G4TrajectoryPoint(G4ThreeVector())
{
  InitialiseVariables();
}

BDSTrajectoryPoint::BDSTrajectoryPoint(const G4Track* track,
                                       G4bool storeExtrasLocal,
                                       G4bool storeExtrasLink,
                                       G4bool storeExtrasIon):
  G4TrajectoryPoint(track->GetPosition())
{
  InitialiseVariables();

  // Need to store the creator process
  const G4VProcess* creatorProcess = track->GetCreatorProcess();
  if (creatorProcess)
    {
      preProcessType     = track->GetCreatorProcess()->GetProcessType();
      preProcessSubType  = track->GetCreatorProcess()->GetProcessSubType();
      postProcessType    = preProcessType;
      postProcessSubType = preProcessSubType;
    }

  preWeight      = track->GetWeight();
  postWeight     = preWeight;
  energyDeposit  = 0.0; // does not lose any energy
  preEnergy      = track->GetKineticEnergy();
  postEnergy     = preEnergy;
  preMomentum    = track->GetMomentum();
  postMomentum   = preMomentum;
  preGlobalTime  = track->GetGlobalTime();
  postGlobalTime = preGlobalTime;
  // when we construct a trajectory from a track it hasn't taken a step yet,
  // so we don't know the material
  material       = nullptr;

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "Process (main|sub) (" << BDSProcessMap::Instance()->GetProcessName(postProcessType, postProcessSubType) << ")" << G4endl;
#endif
  
  // s position for pre and post step point
  // with a track, we're at the start and have no step - use 1nm for step to aid geometrical lookup
  BDSStep localPosMom = auxNavigator->ConvertToLocal(track->GetPosition(),
                                                     track->GetMomentum(),
                                                     1*CLHEP::nm,
                                                     true);
  prePosLocal = localPosMom.PreStepPoint();
  postPosLocal = prePosLocal;
  BDSPhysicalVolumeInfo* info = BDSPhysicalVolumeInfoRegistry::Instance()->GetInfo(localPosMom.VolumeForTransform());
  if (info)
    {
      G4double sCentre = info->GetSPos();
      preS             = sCentre + localPosMom.PreStepPoint().z();
      postS            = preS; // no step length yet
      beamlineIndex    = info->GetBeamlineMassWorldIndex();
      beamline         = info->GetBeamlineMassWorld();
    }

  if (storeExtrasLocal)
    {extraLocal = new BDSTrajectoryPointLocal(prePosLocal, localPosMom.PostStepPoint());} // "post step point" is momentum
  
  if (storeExtrasLink)
    {StoreExtrasLink(track);}

  if (storeExtrasIon)
    {StoreExtrasIon(track);}
}

BDSTrajectoryPoint::BDSTrajectoryPoint(const G4Step* step,
                                       G4bool storeExtrasLocal,
                                       G4bool storeExtrasLink,
                                       G4bool storeExtrasIon):
  G4TrajectoryPoint(step->GetPostStepPoint()->GetPosition())
{
  InitialiseVariables();
  
  const G4StepPoint* prePoint    = step->GetPreStepPoint();
  const G4StepPoint* postPoint   = step->GetPostStepPoint();
  const G4VProcess*  preProcess  = prePoint->GetProcessDefinedStep();
  const G4VProcess*  postProcess = postPoint->GetProcessDefinedStep();

  if (preProcess)
    {
      preProcessType    = preProcess->GetProcessType();
      preProcessSubType = preProcess->GetProcessSubType();
    }  
  if (postProcess)
    {
      postProcessType    = postProcess->GetProcessType();
      postProcessSubType = postProcess->GetProcessSubType();
    }

  preWeight      = prePoint->GetWeight();
  postWeight     = postPoint->GetWeight();
  energyDeposit  = step->GetTotalEnergyDeposit();
  preEnergy      = prePoint->GetKineticEnergy();
  postEnergy     = postPoint->GetKineticEnergy();
  preMomentum    = prePoint->GetMomentum();
  postMomentum   = postPoint->GetMomentum();
  preGlobalTime  = prePoint->GetGlobalTime();
  postGlobalTime = postPoint->GetGlobalTime();
  material       = prePoint->GetMaterial();

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << BDSProcessMap::Instance()->GetProcessName(postProcessType, postProcessSubType) << G4endl;
#endif
  
  // get local coordinates and volume for transform
  BDSStep localPosition = auxNavigator->ConvertToLocal(step);
  prePosLocal = localPosition.PreStepPoint();
  postPosLocal = localPosition.PostStepPoint();
  BDSPhysicalVolumeInfo* info = BDSPhysicalVolumeInfoRegistry::Instance()->GetInfo(localPosition.VolumeForTransform());
  if (info)
    {
      G4double sCentre = info->GetSPos();
      preS             = sCentre + localPosition.PreStepPoint().z();
      postS            = sCentre + localPosition.PostStepPoint().z();
      beamlineIndex    = info->GetBeamlineMassWorldIndex();
      beamline         = info->GetBeamlineMassWorld();
    }

  if (storeExtrasLocal)
    {
      G4ThreeVector postLocalMom = auxNavigator->ConvertAxisToLocal(postMomentum); // uses cached transform
      extraLocal = new BDSTrajectoryPointLocal(prePosLocal, postLocalMom);
    }

  G4Track* track = step->GetTrack();
  if (storeExtrasLink)
    {StoreExtrasLink(track);}

  if (storeExtrasIon)
    {StoreExtrasIon(track);}
}

BDSTrajectoryPoint::BDSTrajectoryPoint(const BDSTrajectoryPoint& other):
  G4TrajectoryPoint(static_cast<const G4TrajectoryPoint&>(other))
{
  extraLocal = other.extraLocal ? new BDSTrajectoryPointLocal(*other.extraLocal) : nullptr;
  extraLink  = other.extraLink  ? new BDSTrajectoryPointLink(*other.extraLink)   : nullptr;
  extraIon   = other.extraIon   ? new BDSTrajectoryPointIon(*other.extraIon)     : nullptr;
  preProcessType     = other.preProcessType;
  preProcessSubType  = other.preProcessSubType;
  postProcessType    = other.postProcessType;
  postProcessSubType = other.postProcessSubType;
  
  preWeight          = other.preWeight;
  postWeight         = other.postWeight;
  preEnergy          = other.preEnergy;
  postEnergy         = other.postEnergy;
  preMomentum        = other.preMomentum;
  postMomentum       = other.postMomentum;
  energyDeposit      = other.energyDeposit;
  preS               = other.preS;
  postS              = other.postS;
  preGlobalTime      = other.preGlobalTime;
  postGlobalTime     = other.postGlobalTime;
  beamlineIndex      = other.beamlineIndex;
  beamline           = other.beamline;
  prePosLocal        = other.prePosLocal;
  postPosLocal       = other.postPosLocal;
  material           = other.material;
}

BDSTrajectoryPoint::~BDSTrajectoryPoint()
{
  delete extraLocal;
  delete extraLink;
  delete extraIon;
}

void BDSTrajectoryPoint::InitialiseVariables()
{
  preProcessType     = -1;
  preProcessSubType  = -1;
  postProcessType    = -1;
  postProcessSubType = -1;
  preWeight          = -1.;
  postWeight         = -1.;
  preEnergy          = -1.;
  postEnergy         = -1.;
  preMomentum        = G4ThreeVector();
  postMomentum       = G4ThreeVector();
  energyDeposit      = 0.0;
  preS               = -1000;
  postS              = -1000;
  preGlobalTime      = 0;
  postGlobalTime     = 0;
  beamlineIndex      = -1;
  beamline           = nullptr;
  prePosLocal        = G4ThreeVector();
  postPosLocal       = G4ThreeVector();
  material           = nullptr;
  extraLocal         = nullptr;
  extraLink          = nullptr;
  extraIon           = nullptr;
}

void BDSTrajectoryPoint::StoreExtrasLink(const G4Track* track)
{
  const G4DynamicParticle* dynamicParticleDef = track->GetDynamicParticle();
  G4double charge   = dynamicParticleDef->GetCharge();
  G4double rigidity = 0;
  if (BDS::IsFinite(charge))
    {rigidity = BDS::Rigidity(track->GetMomentum().mag(), charge);}
  extraLink = new BDSTrajectoryPointLink((G4int)charge,
					 BDSGlobalConstants::Instance()->TurnsTaken(),
					 dynamicParticleDef->GetMass(),
					 rigidity);
}

void BDSTrajectoryPoint::StoreExtrasIon(const G4Track* track)
{
  const G4ParticleDefinition* particleDef        = track->GetParticleDefinition();
  const G4DynamicParticle*    dynamicParticleDef = track->GetDynamicParticle();
  G4bool isIon = BDS::IsIon(dynamicParticleDef);
  G4int nElectrons = dynamicParticleDef->GetTotalOccupancy();
  extraIon = new BDSTrajectoryPointIon(isIon,
				       particleDef->GetAtomicMass(),
				       particleDef->GetAtomicNumber(),
				       nElectrons);
}


G4bool BDSTrajectoryPoint::IsScatteringPoint() const
{
  // use general static function
  G4bool isScatteringPoint = BDSTrajectoryPoint::IsScatteringPoint(postProcessType,
								   postProcessSubType,
								   energyDeposit);

#ifdef BDSDEBUG
  if (isScatteringPoint)
    {
      G4cout << "Interaction point found at " << GetPreS()/CLHEP::m << " m - "
	     << BDSProcessMap::Instance()->GetProcessName(GetPostProcessType(), GetPostProcessSubType()) << G4endl;
    }
#endif
  return isScatteringPoint;
}

G4bool BDSTrajectoryPoint::NotTransportationLimitedStep() const
{
  // if prestep process doesn't exist it won't be set and will
  // default to value in InitialiseVariables which is -1
  G4bool preStep = (preProcessType  != G4ProcessType::fTransportation && preProcessType  != G4ProcessType::fParallel);
  G4bool posStep = (postProcessType != G4ProcessType::fTransportation && postProcessType != G4ProcessType::fParallel);
  return preStep || posStep;
}

std::ostream& operator<< (std::ostream& out, BDSTrajectoryPoint const &p)
{
  out << p.GetPosition();
  return out;
}

G4double BDSTrajectoryPoint::PrePosR() const
{
  return std::hypot(prePosLocal.x(), prePosLocal.y());
}

G4double BDSTrajectoryPoint::PostPosR() const
{
  return std::hypot(postPosLocal.x(), postPosLocal.y());
}

G4bool BDSTrajectoryPoint::IsScatteringPoint(const G4Step* step)
{
  const G4StepPoint* postPoint   = step->GetPostStepPoint();
  const G4VProcess*  postProcess = postPoint->GetProcessDefinedStep();
  
  G4double totalEnergyDeposit = step->GetTotalEnergyDeposit();
  
  G4Track* t = step->GetTrack();
  if (t->GetCurrentStepNumber() == 1 && t->GetStepLength() < 1e-5 && totalEnergyDeposit < 1e-5)
    {return false;} // ignore the first really small step

  G4int postProcessType    = -1;
  G4int postProcessSubType = -1;
  if (postProcess)
    {
      postProcessType    = postProcess->GetProcessType();
      postProcessSubType = postProcess->GetProcessSubType();
    }
  
  return BDSTrajectoryPoint::IsScatteringPoint(postProcessType, postProcessSubType, totalEnergyDeposit);
}
  
G4bool BDSTrajectoryPoint::IsScatteringPoint(G4int postProcessType,
					     G4int postProcessSubType,
					     G4double totalEnergyDeposit)
{
  // test against things we want to exclude like tracking - these are not
  // points of scattering
  G4bool initialised       = postProcessType != -1;
  G4bool notTransportation = postProcessType != G4ProcessType::fTransportation;
  G4bool notGeneral        = (postProcessType != G4ProcessType::fGeneral) && (postProcessSubType != STEP_LIMITER);
  G4bool notParallel       = postProcessType != G4ProcessType::fParallel;
  G4bool notUndefined      = postProcessType != G4ProcessType::fNotDefined; // for crystal channelling

  // energy can change in transportation step (EM)
  if (totalEnergyDeposit > dEThresholdForScattering)
    {return true;}

  G4bool result = initialised && notTransportation && notGeneral && notParallel && notUndefined;
  return result;
}
