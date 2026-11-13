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
#include "BDSOutputROOTEventCoords.hh"

#ifndef __ROOTBUILD__
#include "BDSParticleCoords.hh"
#include "BDSPrimaryVertexInformationV.hh"
#endif

ClassImp(BDSOutputROOTEventCoords)

BDSOutputROOTEventCoords::BDSOutputROOTEventCoords():
  n(0)
{
  FlushLocal();
}

BDSOutputROOTEventCoords::~BDSOutputROOTEventCoords()
{;}

void BDSOutputROOTEventCoords::Flush()
{
  FlushLocal();
}

void BDSOutputROOTEventCoords::FlushLocal()
{
  n = 0;
  X.clear();
  Y.clear();
  Z.clear();
  Xp.clear();
  Yp.clear();
  Zp.clear();
  T.clear();
}

#ifndef __ROOTBUILD__
void BDSOutputROOTEventCoords::Fill(const BDSParticleCoords& coords)
{
  n++;
  X.push_back(coords.x  / CLHEP::m);
  Y.push_back(coords.y  / CLHEP::m);
  Z.push_back(coords.z  / CLHEP::m);
  Xp.push_back(coords.xp / CLHEP::rad);
  Yp.push_back(coords.yp / CLHEP::rad);
  Zp.push_back(coords.zp / CLHEP::rad);
  T.push_back(coords.T  / CLHEP::ns); // to match sampler data
}

void BDSOutputROOTEventCoords::Fill(const BDSPrimaryVertexInformationV* vertexInfos)
{
  for (const auto& vertexInfo : vertexInfos->vertices)
    {Fill(vertexInfo.primaryVertex.global);}
}

#endif

void BDSOutputROOTEventCoords::Fill(const BDSOutputROOTEventCoords* other)
{
  if (!other)
    {return;}

  n  = other->n;
  X  = other->X;
  Y  = other->Y;
  Z  = other->Z;
  Xp = other->Xp;
  Yp = other->Yp;
  Zp = other->Zp;
  T  = other->T;
}
