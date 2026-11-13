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
#ifndef BDSOUTPUTROOTPARTICLEDATA_H
#define BDSOUTPUTROOTPARTICLEDATA_H

#include "Rtypes.h"
#include "TObject.h"

#ifndef __ROOTBUILD__
#include "globals.hh"
#endif

#include <cstdlib>  // for std::abs(int)
#include <cmath>
#include <map>
#include <limits>
#include <string>

/**
 * @brief Geant4 particle data for particles used in simulation
 * 
 * @author Laurie Nevay
 */
class BDSOutputROOTParticleData: public TObject
{
public:
  // structs up front
  /// Simple particle information to be stored per particle.
  struct ParticleInfo
  {
    std::string name;   ///< Name of particle.
    int         charge; ///< Charge in units of e.
    double      mass;   ///< Mass in GeV.

    double rigidity(const double totalEnergy) const
    {
      if (!(std::abs(charge) > std::numeric_limits<double>::epsilon()))
	{return 0;} // not finite charge, so rigidity 0
      if (totalEnergy <= mass)
	{return 0;} // invalid - just return 0
      // calculation reproduced from BDSParticleDefinition but with hard coded constants
      // for units here and to ensure no Geant4 linkage for output structures
      // p = sqrt(E^2 - m^2) in GeV
      double momentum = std::sqrt(std::pow(totalEnergy,2) - std::pow(mass,2));
      double brho = momentum / 0.29979245799999998 / charge;
      return brho;
    }
    double rigidity(const double totalEnergy, const double chargeIn) const
    {
      if (!(std::abs(chargeIn) > std::numeric_limits<double>::epsilon()))
	{return 0;} // not finite charge, so rigidity 0
      if (totalEnergy <= mass)
	{return 0;} // invalid - just return 0
      // calculation reproduced from BDSParticleDefinition but with hard coded constants
      // for units here and to ensure no Geant4 linkage for output structures
      // p = sqrt(E^2 - m^2) in GeV
      double momentum = std::sqrt(std::pow(totalEnergy,2) - std::pow(mass,2));
      double brho = momentum / 0.29979245799999998 / chargeIn;
      return brho;
    }
  };

  /// Simple particle information to be stored per ion specifically.
  struct IonInfo
  {
    std::string name;   ///< Name of particle.
    int         charge; ///< Charge in units of e.
    double      mass;   ///< Mass in GeV.
    int a; ///< Mass number - number of nucleons in nucleus.
    int z; ///< Atomic number - number of protons in nucleus.
    
    double rigidity(const double totalEnergy) const
    {
      if (!(std::abs(charge) > std::numeric_limits<double>::epsilon()))
	{return 0;} // not finite charge, so rigidity 0
      if (totalEnergy <= mass)
	{return 0;} // invalid - just return 0
      double momentum = std::sqrt(std::pow(totalEnergy,2) - std::pow(mass,2));
      double brho = momentum / 0.29979245799999998 / charge;
      return brho;
    }
    double rigidity(const double totalEnergy, const double chargeIn) const
    {
      if (!(std::abs(chargeIn) > std::numeric_limits<double>::epsilon()))
	{return 0;} // not finite charge, so rigidity 0
      if (totalEnergy <= mass)
	{return 0;} // invalid - just return 0
      double momentum = std::sqrt(std::pow(totalEnergy,2) - std::pow(mass,2));
      double brho = momentum / 0.29979245799999998 / chargeIn;
      return brho;
    }  
  };
  
  BDSOutputROOTParticleData();
  virtual ~BDSOutputROOTParticleData(){;}

  /// Clear maps.
  virtual void Flush();

  const ParticleInfo GetParticleInfo(const int& pdgID) const;
  const IonInfo      GetIonInfo(const int& pdgID) const;
  int         Charge(const int& pdgID) const;
  double      Mass(const int& pdgID) const;

  /// Calculate rigidity of particle given PDG ID and total energy in GeV.
  double      Rigidity(const int&    pdgID,
		       const double& totalEnergy) const;

  /// Calculate kinetic energy of particle given PDG ID and total energy in GeV.
  double      KineticEnergy(const int&    pdgID,
               const double& totalEnergy) const;

  std::string Name(const int& pdgID) const;
  int         IonA(const int& pdgID) const;
  int         IonZ(const int& pdgID) const;

  /// This doesn't count a proton (even with electrons) as an ion.
  inline bool IsIon(const int& pdgID) const {return pdgID > 1000000000;}
  
#ifndef __ROOTBUILD__
  /// Fill maps of particle information from Geant4.
  void Fill(G4bool fillIons);
#endif

  std::map<int, ParticleInfo> particles;
  std::map<int, IonInfo>      ions;

  ClassDef(BDSOutputROOTParticleData, 1);
};

#endif
