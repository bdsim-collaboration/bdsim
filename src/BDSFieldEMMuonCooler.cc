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
#include "BDSFieldMagDipoleEnge.hh"
#include "BDSFieldMagDipoleHardEdgeMuonCooler.hh"
#include "BDSFieldEMRFCavity.hh"
#include "BDSFieldType.hh"

#include "G4ThreeVector.hh"
#include "G4Types.hh"

#include <algorithm>
#include <cmath>
#include <limits>
#include <utility>
#include <vector>

BDSFieldEMMuonCooler::BDSFieldEMMuonCooler(const BDSFieldInfoExtraMuonCooler* info,
                                           G4double /*brho*/)
{
  BuildMagnets(info);
  BuildDipoles(info);
  BuildRF(info);
  BuildZBins();
}

BDSFieldEMMuonCooler::~BDSFieldEMMuonCooler()
{
  for (auto& e : entries)
    {
      delete e.mag;
      delete e.em;
    }
}

void BDSFieldEMMuonCooler::BuildZBins()
{
  G4int n = (G4int)entries.size();
  const G4double inf = std::numeric_limits<G4double>::max();

  G4double minExtent  =  inf;
  G4double globalZMin =  inf;
  G4double globalZMax = -inf;
  for (G4int i = 0; i < n; i++)
    {
      G4double ze = entries[i].zHalfExtent;
      if (ze >= inf / 2.0)
        {
          if (entries[i].type == FieldEntry::Type::Mag)
            {alwaysOn.push_back(i);}
          else
            {throw BDSException(__METHOD_NAME__, "EM field entry with infinite z extent is not supported in muon cooler");}
          continue;
        }
      G4double oz = entries[i].offset.z();
      minExtent  = std::min(minExtent, ze);
      globalZMin = std::min(globalZMin, oz - ze);
      globalZMax = std::max(globalZMax, oz + ze);
    }

  if (minExtent >= inf / 2.0)
    {return;} // all fields are always-on; nothing to bin

  binWidth = minExtent / 2.0;
  const G4int maxBins = 100000;
  nBins = std::min((G4int)std::ceil((globalZMax - globalZMin) / binWidth) + 2, maxBins);
  if (nBins == maxBins)
    {binWidth = (globalZMax - globalZMin) / (maxBins - 1);}

  zBinMin = globalZMin;
  zbins.resize(nBins);

  for (G4int i = 0; i < n; i++)
    {
      G4double ze = entries[i].zHalfExtent;
      if (ze >= inf / 2.0)
        {continue;}
      G4double oz  = entries[i].offset.z();
      G4int binLo  = std::max(0,       (G4int)((oz - ze - zBinMin) / binWidth) - 1);
      G4int binHi  = std::min(nBins-1, (G4int)((oz + ze - zBinMin) / binWidth) + 1);
      for (G4int b = binLo; b <= binHi; b++)
        zbins[b].push_back(i);
    }
}

void BDSFieldEMMuonCooler::BuildMagnets(const BDSFieldInfoExtraMuonCooler* info)
{

  switch (info->magneticFieldType.underlying())
    {
    case BDSFieldType::solenoidblock:
      {
        for (const auto& ci : info->coilInfos)
          {
            G4int gridPts = ci.useGrid ? ci.gridPointsPerMm : 0;
            auto* f = new BDSFieldMagSolenoidBlock(ci.current,
                                                   true,
                                                   ci.innerRadius,
                                                   ci.radialThickness,
                                                   ci.fullLengthZ,
                                                   ci.tiltX,
                                                   ci.tiltY,
                                                   ci.tiltZ,
                                                   ci.onAxisTolerance,
                                                   ci.nSheets,
                                                   gridPts,
                                                   ci.interpolator);
            FieldEntry e;
            e.type        = FieldEntry::Type::Mag;
            e.mag         = f;
            e.offset      = G4ThreeVector(ci.offsetX, ci.offsetY, ci.offsetZ);
            e.zHalfExtent = f->GetZHalfExtent();
            entries.push_back(std::move(e));
          }
        break;
      }
    case BDSFieldType::solenoidsheet:
      {
        for (const auto& ci : info->coilInfos)
          {
            G4int gridPts = ci.useGrid ? ci.gridPointsPerMm : 0;
            auto* f = new BDSFieldMagSolenoidSheet(ci.current,
                                                   true,
                                                   ci.innerRadius + 0.5*ci.radialThickness,
                                                   ci.fullLengthZ,
                                                   ci.tiltX,
                                                   ci.tiltY,
                                                   ci.tiltZ,
                                                   ci.onAxisTolerance,
                                                   gridPts,
                                                   ci.interpolator);
            FieldEntry e;
            e.type        = FieldEntry::Type::Mag;
            e.mag         = f;
            e.offset      = G4ThreeVector(ci.offsetX, ci.offsetY, ci.offsetZ);
            e.zHalfExtent = f->GetZHalfExtent();
            entries.push_back(std::move(e));
          }
        break;
      }
    case BDSFieldType::solenoidloop:
      {
        for (const auto& ci : info->coilInfos)
          {
            const G4double inf = std::numeric_limits<G4double>::max();

            FieldEntry e;
            e.type        = FieldEntry::Type::Mag;
            e.mag         = new BDSFieldMagSolenoidLoop(ci.current,
                                                        true,
                                                        ci.innerRadius + 0.5*ci.radialThickness);
            e.offset      = G4ThreeVector(0, 0, ci.offsetZ);
            e.zHalfExtent = inf; // no tolerance set; always evaluate
            entries.push_back(std::move(e));
          }
        break;
      }
    default:
      {
        G4String msg = "\"" + info->magneticFieldType.ToString();
        msg += "\" is not a valid field model for a muon cooler B field";
        throw BDSException(__METHOD_NAME__, msg);
      }
    }
}

void BDSFieldEMMuonCooler::BuildDipoles(const BDSFieldInfoExtraMuonCooler* info)
{

  switch (info->dipoleFieldType.underlying())
    {
    case BDSFieldType::dipole:
      {
        for (const auto& di : info->dipoleInfos)
          {
            FieldEntry e;
            e.type        = FieldEntry::Type::Mag;
            e.mag         = new BDSFieldMagDipoleHardEdgeMuonCooler(di.fieldStrength,
                                                                     di.apertureRadius,
                                                                     di.fullLengthZ);
            e.offset      = G4ThreeVector(0, 0, di.offsetZ);
            e.zHalfExtent = di.fullLengthZ / 2.0;
            entries.push_back(std::move(e));
          }
        break;
      }
    case BDSFieldType::dipoleenge:
      {
        for (const auto& di : info->dipoleInfos)
          {
            FieldEntry e;
            e.type   = FieldEntry::Type::Mag;
            auto* f  = new BDSFieldMagDipoleEnge(di.fieldStrength,
                                                  di.apertureRadius,
                                                  di.fullLengthZ,
                                                  di.engeCoefficient,
                                                  di.useGrid,
                                                  di.gridPointsPerMm,
                                                  di.interpolator);
            e.mag         = f;
            e.offset      = G4ThreeVector(0, 0, di.offsetZ);
            e.zHalfExtent = f->GetZHalfExtent();
            entries.push_back(std::move(e));
          }
        break;
      }
    default:
      {
        G4String msg = "\"" + info->dipoleFieldType.ToString();
        msg += "\" is not a valid dipole field model for a muon cooler B field";
        throw BDSException(__METHOD_NAME__, msg);
      }
    }
}

void BDSFieldEMMuonCooler::BuildRF(const BDSFieldInfoExtraMuonCooler* info)
{
  for (const auto& ci : info->cavityInfos)
    {
      FieldEntry e;
      e.type        = FieldEntry::Type::EM;
      e.em          = new BDSFieldEMRFCavity(ci.peakEField,
                                             ci.frequency,
                                             ci.phaseOffset,
                                             ci.cavityRadius,
                                             0.0); // tOffset provided globally
      e.offset      = G4ThreeVector(0.0, 0.0, ci.offsetZ);
      e.timeOffset  = ci.globalTimeOffset;
      e.zHalfExtent = ci.lengthZ / 2.0;
      entries.push_back(std::move(e));
    }
}

std::pair<G4ThreeVector, G4ThreeVector> BDSFieldEMMuonCooler::GetField(const G4ThreeVector& position,
                                                                        const G4double       t) const
{
  std::pair<G4ThreeVector, G4ThreeVector> result;

  for (G4int i : alwaysOn)
    {
      const FieldEntry& e = entries[i];
      G4ThreeVector dr = position - e.offset;
      result.first += e.mag->GetField(dr, t);
    }

  if (nBins > 0)
    {
      G4int bin = (G4int)((position.z() - zBinMin) / binWidth);
      if (bin >= 0 && bin < nBins)
        {
          for (G4int i : zbins[bin])
            {
              const FieldEntry& e = entries[i];
              G4ThreeVector dr = position - e.offset;
              if (e.type == FieldEntry::Type::Mag)
                {result.first += e.mag->GetField(dr, t);}
              else
                {
                  if (std::fabs(dr.z()) > e.zHalfExtent)
                    {continue;}
                  auto fe = e.em->GetField(dr, t - e.timeOffset);
                  result.first  += fe.first;
                  result.second += fe.second;
                }
            }
        }
    }

  return result;
}
