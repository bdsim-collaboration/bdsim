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
#pragma link C++ class BDSOutputROOTParticleData+;
#pragma link C++ class BDSOutputROOTParticleData::ParticleInfo+;
#pragma link C++ class BDSOutputROOTParticleData::IonInfo+;
#pragma link C++ class std::map<int, BDSOutputROOTParticleData::ParticleInfo>+;
#pragma link C++ class std::map<int, BDSOutputROOTParticleData::IonInfo>+;
#pragma link C++ class std::pair<int, BDSOutputROOTParticleData::ParticleInfo>+;
#pragma link C++ class std::pair<int, BDSOutputROOTParticleData::IonInfo>+;

#pragma read sourceClass="BDSOutputROOTGeant4Data" \
  version="[-2]" \
  targetClass="BDSOutputROOTParticleData"

#pragma read sourceClass="BDSOutputROOTGeant4Data::ParticleInfo" \
  version="[-2]" \
  targetClass="BDSOutputROOTParticleData::ParticleInfo"

#pragma read sourceClass="BDSOutputROOTGeant4Data::IonInfo" \
  version="[-2]" \
  targetClass="BDSOutputROOTParticleData::IonInfo"
