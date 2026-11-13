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
#ifndef BDSPARTICLEDEFINITION_H
#define BDSPARTICLEDEFINITION_H
#include "BDSIonDefinition.hh"

#include "CLHEP/Units/PhysicalConstants.h"

#include "globals.hh"

#include <ostream>

class G4ParticleDefinition;

/**
 * @brief Wrapper for particle definition.
 *
 * Used to calculate momentum and kinetic energy and associate any other information
 * required along with a G4ParticleDefinition instance.
 *
 * Bad construction can throw a BDSException.
 *
 * @author Laurie Nevay
 */

class BDSParticleDefinition
{
public:
  /// Constructor requires G4 particle definition as well as one of total energy,
  /// kineticEnergy or momentum. Only 1 of these can be non-zero (will throw
  /// exception otherwise). The others are calculated using the mass. The optional
  /// ion definition may be supplied to override the charge of the ion. The PDGID
  /// should also be supplied in this case too. ffact is typically 1 or -1 to flip
  /// the rigidity for convention.
  BDSParticleDefinition(G4ParticleDefinition* particleIn,
			G4double              totalEnergyIn,
			G4double              kineticEnergyIn,
			G4double              momentumIn,
			G4double              ffact,
			BDSIonDefinition*     ionDefinitionIn = nullptr,
			G4int                 ionPDGID        = 0);
  
  /// Alternative constructor for when we don't have access to the particle table
  /// information. G4ParticleDefinition can be updated later. Developer
  /// responsibility to ensure this matches the contents of this class.
  /// ffact is typically 1 or -1 to flip the rigidity for convention. As before
  /// only one of totalEnergy, kineticEnergy or momentum should be specified.
  BDSParticleDefinition(const G4String&    nameIn,
			G4double          massIn,
			G4double          chargeIn,
			G4double          totalEnergyIn,
			G4double          kineticEnergyIn,
			G4double          momentumIn,
			G4double          ffact,
			BDSIonDefinition* ionDefinitionIn = nullptr,
			G4int             ionPDGID        = 0);

  /// @{ Copy, move and assignment constructors specified as we have to copy the ionDefinition if it exists.
  BDSParticleDefinition(const BDSParticleDefinition& other);
  BDSParticleDefinition(BDSParticleDefinition&& other) noexcept;
  BDSParticleDefinition& operator=(BDSParticleDefinition&& other) noexcept;
  /// @}
  
  ~BDSParticleDefinition();

  /// Check in order whether totalEnergy, kineticEnergy or momentum are specified and
  /// copy that parameter to the member variable. Then calculate the other two and set
  /// them to the member variables.
  void SetEnergies(G4double totalEnergyIn,
		   G4double kineticEnergyIn,
		   G4double momentumIn);

  /// Update the G4 particle definition member. Developer responsibility to ensure
  /// this matches the contents of the class.
  void UpdateG4ParticleDefinition(G4ParticleDefinition* particleIn) {particle = particleIn;}


  /// Utility function to update quantities by adding on dEK (can be negative).
  void ApplyChangeInKineticEnergy(G4double dEk);

  /// @{ Accessor.
  inline G4ParticleDefinition* ParticleDefinition() const {return particle;}
  inline BDSIonDefinition*     IonDefinition()      const {return ionDefinition;}
  inline G4String Name()          const {return name;}
  inline G4double Mass()          const {return mass;}
  inline G4double Charge()        const {return charge;}
  inline G4double TotalEnergy()   const {return totalEnergy;}
  inline G4double KineticEnergy() const {return kineticEnergy;}
  inline G4double Momentum()      const {return momentum;}
  inline G4double Gamma()         const {return gamma;}
  inline G4double Beta()          const {return beta;}
  inline G4double BRho()          const {return brho;}
  inline G4double FFact()         const {return ffact;}
  inline G4bool   IsAnIon()       const {return ionDefinition != nullptr;}
  inline G4bool   NElectrons()    const {return ionDefinition != nullptr ? ionDefinition->NElectrons() : 0;}
  inline G4double Velocity()      const {return Beta() * CLHEP::c_light;}
  inline G4bool   Forwards()      const {return forwards;}
  /// @}

  /// Safely access the PDG ID of the particle. If the physics table isn't ready then we might
  /// not have the G4ParticleDefinition object in the case of an ion. In this case if there's an
  /// ion definition return the cache ion PDG ID, else return 0 as default.
  G4int PDGID() const;

  /// Output stream operator implementation.
  friend std::ostream& operator<< (std::ostream& out, BDSParticleDefinition const& def);
  
private:
  /// No default constructor.
  BDSParticleDefinition() = delete;

  /// Calculate and set momentum based on totalEnergy and mass.
  void CalculateMomentum();

  /// Calculate and set rigidity based on charge and momentum.
  void CalculateRigidity(const G4double& ffactIn);

  /// Calculate and set lorentz factors.
  void CalculateLorentzFactors();
  
  G4ParticleDefinition* particle;      ///< Does not own.
  BDSIonDefinition*     ionDefinition; ///< Optional ion definition. Does own.
  G4int                 ionPDGID;      ///< Cache this for ions only.

  G4String name;           ///< Particle name.
  G4double mass;           ///< Particle mass.
  G4double charge;         ///< Particle charge.
  G4double totalEnergy;    ///< Particle total energy.
  G4double kineticEnergy;  ///< Particle kinetic energy.
  G4double momentum;       ///< Particle momentum.
  G4double gamma;          ///< Relativistic gamma.
  G4double beta;           ///< Relativistic beta.
  G4double brho;           ///< Particle rigidity.
  G4double ffact;          ///< Flip factor.
  G4bool   forwards;       ///< In case of change of direction.
};

#endif
