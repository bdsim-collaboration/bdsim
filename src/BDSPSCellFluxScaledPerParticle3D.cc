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
#include "BDSHistBinMapper.hh"
#include "BDSScorerConversionLoader.hh"
#include "BDSPSCellFluxScaledPerParticle3D.hh"
#include "BDSUtilities.hh"

#include "globals.hh"
#include "G4PhysicsVector.hh"
#include "G4SystemOfUnits.hh"
#include "G4VSolid.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VPVParameterisation.hh"

#include <fstream>
#include <map>
#include <string>

#ifdef USE_GZSTREAM
#include "src-external/gzstream/gzstream.h"
#endif

BDSPSCellFluxScaledPerParticle3D::BDSPSCellFluxScaledPerParticle3D(const G4String&         scorerName,
                                                                   const BDSHistBinMapper* mapperIn,
                                                                   const G4String&         pathname,
                                                                   const G4String&         unitIn,
                                                                   G4int ni,
                                                                   G4int nj,
                                                                   G4int nk,
                                                                   G4int depi,
                                                                   G4int depj,
                                                                   G4int depk):
  BDSPSCellFluxScaled3D(scorerName, mapperIn, unitIn, ni, nj, nk, depi, depj, depk)
{
  G4String filePath = BDS::GetFullPath(pathname);
  if (filePath.back() != '/')
    {filePath += '/';}

  const std::map<std::string, G4int> files = {{"protons.dat",   2212},
					      {"neutrons.dat",  2112},
					      {"photons.dat",   22},
					      {"electrons.dat", 11},
					      {"positrons.dat", -11}};

  G4cout << "Scorer \"" << GetName() << "\" - adding conversionFiles:" << G4endl;
  BDSScorerConversionLoader<std::ifstream> loader;
  for (const auto& filePDG : files)
    {
      G4String uncompressedFile = filePath + filePDG.first;
      G4String compressedFile   = filePath + filePDG.first+".gz";
      if (BDS::FileExists(uncompressedFile))
	{
	  conversionFactors[filePDG.second] = loader.Load(uncompressedFile);
	  G4cout << "Adding: " << uncompressedFile << G4endl;
	}
      else if (BDS::FileExists(compressedFile))
	{
#ifdef USE_GZSTREAM
	  BDSScorerConversionLoader<igzstream> loaderC;
	  conversionFactors[filePDG.second] = loaderC.Load(compressedFile);
	  G4cout << "Adding: " << compressedFile << G4endl;
#else
	  throw BDSException(__METHOD_NAME__, "Compressed file loading - but BDSIM not compiled with ZLIB.");
#endif
	}
    }
  if (conversionFactors.empty())
    {
      G4String message = "no conversion files for scorer \"" + scorerName + "\" found. Please specify one of:\n";
      for (const auto &kv : files)
        {message += kv.first + "\n";}
      message += "In the specified directory \"" + pathname + "\"\n";
      message += "Would result in a factor of 0 for all particles and all energies.";
      throw BDSException(__METHOD_NAME__, message);
    }
}

BDSPSCellFluxScaledPerParticle3D::~BDSPSCellFluxScaledPerParticle3D()
{
  for (auto cf : conversionFactors)
    {delete cf.second;}
}

G4double BDSPSCellFluxScaledPerParticle3D::GetConversionFactor(G4int particleID, G4double kineticEnergy) const
{
  auto search = conversionFactors.find(particleID);
  if (search != conversionFactors.end())
    {return search->second->Value(kineticEnergy);}
  else
    {return 0;}
}
