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
#include "BDSSamplerBase.hh"

#include "BDSGlobalConstants.hh" 
#include "BDSDebug.hh"
#include "BDSExecOptions.hh"
#include "BDSSamplerSD.hh"
#include "G4LogicalVolume.hh"
#include "G4UserLimits.hh"

std::vector <G4String> BDSSamplerBase::outputNames;

int BDSSamplerBase::nSamplers = 0;

int BDSSamplerBase::GetNSamplers() { return nSamplers; }

void BDSSamplerBase::AddExternalSampler(G4String name) { nSamplers++; outputNames.push_back(name); }

BDSSamplerBase::BDSSamplerBase(G4String name,
			       G4double length,
			       G4String type):
  BDSAcceleratorComponent(name, length, 0, type)
{
  nThisSampler= nSamplers + 1; // start count at 1 (important for BDSOutputROOT)
  nSamplers++;
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " Nsamplers " << nSamplers << G4endl;
#endif
  outputNames.push_back(GetName() + "_" + std::to_string(nThisSampler));
}

void BDSSamplerBase::BuildContainerLogicalVolume()
{
#ifndef NOUSERLIMITS
  SetUserLimits();
#endif
  SetVisAttributes();
}

void BDSSamplerBase::SetUserLimits()
{
  G4UserLimits* outerUserLimits = new G4UserLimits();
  outerUserLimits->SetMaxAllowedStep(BDSGlobalConstants::Instance()->SamplerDiameter() * 0.5);
  containerLogicalVolume->SetUserLimits(outerUserLimits);
  RegisterUserLimits(outerUserLimits);
}

void BDSSamplerBase::SetVisAttributes()
{
  if (BDSExecOptions::Instance()->GetVisDebug())
    {containerLogicalVolume->SetVisAttributes(BDSGlobalConstants::Instance()->GetVisibleDebugVisAttr());}
  else
    {containerLogicalVolume->SetVisAttributes(BDSGlobalConstants::Instance()->GetInvisibleVisAttr());}
  containerLogicalVolume->SetSensitiveDetector(GetSensitiveDetector());
}

BDSSamplerBase::~BDSSamplerBase()
{
  --nSamplers;
  if(nSamplers<0) G4cerr << __METHOD_NAME__ << "WARNING: more samplers deleted than created!" << G4endl;
}
