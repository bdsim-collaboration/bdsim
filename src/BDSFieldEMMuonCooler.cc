/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2022.

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
#include "BDSFieldEMMuonCooler.hh"
#include "BDSFieldInfoExtra.hh"
#include "BDSFieldMagSolenoidBlock.hh"
#include "BDSFieldMagSolenoidSheet.hh"
#include "BDSFieldMagSolenoidLoop.hh"
#include "BDSFieldMagDipole.hh"
#include "BDSFieldMagDipoleEnge.hh"
#include "BDSFieldMagDipoleHardEdgeMuonCooler.hh"
#include "BDSFieldMagVectorSum.hh"
#include "BDSFieldMag.hh"
#include "BDSFieldEMVectorSum.hh"
#include "BDSFieldEMRFCavity.hh"
#include "BDSFieldType.hh"
#include "BDSMuonCoolerStructs.hh"
#include "BDSUtilities.hh"

#include "CLHEP/Units/PhysicalConstants.h"
#include "G4ThreeVector.hh"
#include "G4Types.hh"

#include <utility>

BDSFieldEMMuonCooler::BDSFieldEMMuonCooler(const BDSFieldInfoExtraMuonCooler* info,
                                           G4double /*brho*/):
  coilField(nullptr),
  dipoleField(nullptr),
  rfField(nullptr)
{
  BuildMagnets(info);
  BuildDipoles(info);
  BuildRF(info);
}

void BDSFieldEMMuonCooler::BuildMagnets(const BDSFieldInfoExtraMuonCooler* info)
{
  switch (info->magneticFieldType.underlying())
    {
    case BDSFieldType::solenoidblock:
      {
        const auto& coilInfos = info->coilInfos;
        std::vector<BDSFieldMag*> fields;
        std::vector<G4ThreeVector> fieldOffsets;
        for (const auto& ci : coilInfos)
          {
            fields.push_back(new BDSFieldMagSolenoidBlock(ci.current,
                                                          true,
                                                          ci.innerRadius,
                                                          ci.radialThickness,
                                                          ci.fullLengthZ,
                                                          ci.onAxisTolerance,
                                                          ci.nSheets));
            fieldOffsets.emplace_back(0,0,ci.offsetZ);
          }
        coilField = new BDSFieldMagVectorSum(fields, fieldOffsets);
        break;
      }
    case BDSFieldType::solenoidsheet:
      {
        const auto& coilInfos = info->coilInfos;
        std::vector<BDSFieldMag*> fields;
        std::vector<G4ThreeVector> fieldOffsets;
        for (const auto& ci : coilInfos)
          {
            fields.push_back(new BDSFieldMagSolenoidSheet(ci.current,
                                                          true,
                                                          ci.innerRadius + 0.5*ci.radialThickness,
                                                          ci.fullLengthZ,
                                                          ci.onAxisTolerance));
            fieldOffsets.emplace_back(0,0,ci.offsetZ);
          }
        coilField = new BDSFieldMagVectorSum(fields, fieldOffsets);
        break;
      }
    case BDSFieldType::solenoidloop:
      {
        const auto& coilInfos = info->coilInfos;
        std::vector<BDSFieldMag*> fields;
        std::vector<G4ThreeVector> fieldOffsets;
        for (const auto& ci : coilInfos)
          {
            fields.push_back(new BDSFieldMagSolenoidLoop(ci.current,
                                                         true,
                                                         ci.innerRadius + 0.5*ci.radialThickness));
            fieldOffsets.emplace_back(0,0,ci.offsetZ);
          }
        coilField = new BDSFieldMagVectorSum(fields, fieldOffsets);
        break;
      }
    default:
      {
        G4String msg = "\"" + info->magneticFieldType.ToString();
        msg += "\" is not a valid field model for a muon cooler B field";
        throw BDSException(__METHOD_NAME__, msg);
        break;
      }
    }
}

void BDSFieldEMMuonCooler::BuildDipoles(const BDSFieldInfoExtraMuonCooler* info)
{
  switch (info->dipoleFieldType.underlying())
    {
    case BDSFieldType::dipole:
      {
        const auto& dipoleInfos = info->dipoleInfos;
        std::vector<BDSFieldMag*> fields;
        std::vector<G4ThreeVector> fieldOffsets;
        for (const auto& di : dipoleInfos)
          {
            fields.push_back(new BDSFieldMagDipoleHardEdgeMuonCooler(di.fieldStrength,
                                                           di.apertureRadius,
                                                           di.fullLengthZ));
            fieldOffsets.emplace_back(0,0,di.offsetZ);
          }
        dipoleField = new BDSFieldMagVectorSum(fields, fieldOffsets);
        break;
      }
    case BDSFieldType::dipoleenge:
      {
        const auto& dipoleInfos = info->dipoleInfos;
        std::vector<BDSFieldMag*> fields;
        std::vector<G4ThreeVector> fieldOffsets;
        for (const auto& di : dipoleInfos)
          {
            fields.push_back(new BDSFieldMagDipoleEnge(di.fieldStrength,
                                                       di.apertureRadius,
                                                       di.fullLengthZ,
                                                       di.engeCoefficient,
                                                       di.onAxisTolerance));
            fieldOffsets.emplace_back(0,0,di.offsetZ);
          }
        dipoleField = new BDSFieldMagVectorSum(fields, fieldOffsets);
        break;
      }  
    default:
      {
        G4String msg = "\"" + info->dipoleFieldType.ToString();
        msg += "\" is not a valid dipole field model for a muon cooler B field";
        throw BDSException(__METHOD_NAME__, msg);
        break;
      }
    }
}

void BDSFieldEMMuonCooler::BuildRF(const BDSFieldInfoExtraMuonCooler* info)
{
  const auto& cavityInfos = info->cavityInfos;
  std::vector<G4ThreeVector> fieldOffsets;
  BDSFieldEMVectorSum* emSum = new BDSFieldEMVectorSum();
  for (const auto& ci : cavityInfos)
    {
      BDSFieldEMRFCavity* rfCav = new BDSFieldEMRFCavity(
           ci.peakEField,
           ci.frequency,
           ci.phaseOffset,
           ci.cavityRadius,
           0.0 // We provide a global tOffset instead
           );
      double lengthZ = ci.lengthZ;
      G4ThreeVector posOffset(0.0, 0.0, ci.offsetZ);
      double tOffset = ci.globalTimeOffset;
      emSum->PushBackField(posOffset, tOffset, lengthZ, rfCav);
    }
  rfField = emSum;
}

BDSFieldEMMuonCooler::~BDSFieldEMMuonCooler()
{
  delete coilField;
  delete dipoleField;
  delete rfField;
}

std::pair<G4ThreeVector, G4ThreeVector> BDSFieldEMMuonCooler::GetField(const G4ThreeVector& position,
                                                                       const G4double       t) const
{
  auto result = rfField->GetField(position, t); // result is a pair like <Bfield, Efield>
  G4ThreeVector solfieldOut = coilField->GetField(position, t);
  G4ThreeVector dipolefieldOut = dipoleField->GetField(position, t);
  result.first += solfieldOut;
  result.first += dipolefieldOut;
  return result;
}
