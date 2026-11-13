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
#include "BDSProcessMap.hh"
#include <unordered_map>

#include "globals.hh" // geant4 types / globals

BDSProcessMap* BDSProcessMap::instance = nullptr;

BDSProcessMap* BDSProcessMap::Instance()
{
  if (!instance)
    {instance = new BDSProcessMap();}
  return instance;
}

BDSProcessMap::~BDSProcessMap()
{
  delete instance;
  instance = nullptr;
}

G4String BDSProcessMap::operator()(const G4int& type, const G4int& subType) const
{
  G4String output  = "";
  auto result = processes.find(type);
  if (result == processes.end())
    {
      output += "Unknown";
      G4cout << "Unknown process ID \"" << type << "\"" << G4endl;
    }
  else
    {output += result->second;}

  // sub type is optional as some don't have it
  // need to have valid process type to look up subprocess map
  if (subType > 0 && result != processes.end())
    {
      output += " : ";
      // note have to use 'at' for const-ness
      const auto& subMap = subProcesses.at(result->first);
      auto result2 = subMap.find(subType);
      if (result2 == subMap.end())
	{
	  output += "Unknown";
	  G4cout << "Unknown subprocess ID \"" << subType << "\"" << G4endl;
	}
      else
	{output += result2->second;}
    }
  return output;
}

BDSProcessMap::BDSProcessMap()
{
  /// note 'processMap' is typedef in header
  processes[0]  = "fNotDefined";
  processes[1]  = "fTransportation";
  subProcesses[1] = processMap
    {
      {91, "TRANSPORTATION"},
      {92, "COUPLED_TRANSPORTATION"}
    };
  processes[2]  = "fElectromagnetic";
  subProcesses[2] = processMap
    {
      {1,  "fCoulombScattering"},
      {2,  "fIonisation"},
      {3,  "fBremsstrahlung"},
      {4,  "fPairProdByCharged"},
      {5,  "fAnnihilation"},
      {6,  "fAnnihilationToMuMu"},
      {7,  "fAnnihilationToHadrons"},
      {8,  "fNuclearStopping"},
      {10, "fMultipleScattering"},   
      {11, "fRayleigh"},
      {12, "fPhotoElectricEffect"},
      {13, "fComptonScattering"},
      {14, "fGammaConversion"},
      {15, "fGammaConversionToMuMu"},
      {21, "fCerenkov"},
      {22, "fScintillation"},
      {23, "fSynchrotronRadiation"},
      {24, "fTransitionRadiation"}
    };
  processes[3]  = "fOptical";
  subProcesses[3] = processMap
    {
      {0, "kCerenkov"},
      {1, "kScintillation"},
      {2, "kAbsorption"},
      {3, "kRayleigh"},
      {4, "kMieHG"},
      {5, "kBoundary"},
      {6, "kWLS"},
      {7, "kNoProcess"}
    };
  processes[4]  = "fHadronic";
  subProcesses[4] = processMap
    {
      {111, "fHadronElastic"},
      {121, "fHadronInelastic"},
      {131, "fCapture"},
      {141, "fFission"},
      {151, "fHadronAtRest"},
      {152, "fLeptonAtRest"},
      {161, "fChargeExchange"},
      {210, "fRadioactiveDecay"}
    };
  processes[5]  = "fPhotolepton_hadron";
  processes[6]  = "fDecay";
  subProcesses[6] = processMap
    {
      {201, "DECAY"},
      {202, "DECAY_WithSpin"},
      {203, "DECAY_PionMakeSpin"},
      {210, "DECAY_Radioactive"},
      {211, "DECAY_Unknown"},
      {231, "DECAY_External"}
    };
  processes[7]  = "fGeneral";
  subProcesses[7] = processMap
    {
      {401, "STEP_LIMITER"},
      {402, "USER_SPECIAL_CUTS"},
      {403, "NEUTRON_KILLER"}
    };
  processes[8]  = "fParameterisation";
  processes[9]  = "fUserDefined";
  processes[10] = "fParallel";
  subProcesses[10] = processMap
    {
      {491, "Parallel"} //SBLN unknown - but something to do with parallel world transportation.
    };
  processes[11] = "fPhonon";
  processes[12] = "fUCN";
}
