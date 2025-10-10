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

// Subclass of G4ParticleDefinition for millicharged particles

#include "BDSParticleMilli.hh"
#include "BDSGlobalConstants.hh"

#include "G4SystemOfUnits.hh"
#include "G4ParticleTable.hh"

ParticleMilli* ParticleMilli::theInstance = 0;

ParticleMilli* ParticleMilli::Definition()
{
    if (theInstance !=0) return theInstance;

    G4String name = BDSGlobalConstants::Instance()->millichargeName();
    // search in particle table
    G4ParticleTable* pTable = G4ParticleTable::GetParticleTable();
    G4ParticleDefinition* anInstance = pTable->FindParticle(name);

    if (anInstance ==0)
    {
        G4double mass = BDSGlobalConstants::Instance()->millichargeMass();
        G4double charge = BDSGlobalConstants::Instance()->millichargeCharge();
        G4int pdgID = BDSGlobalConstants::Instance()->millichargeID();

        // create particle
        //
        //    Arguments for constructor are as follows
        //               name             mass          width         charge
        //             2*spin           parity  C-conjugation
        //          2*Isospin       2*Isospin3       G-parity
        //               type    lepton number  baryon number   PDG encoding
        //             stable         lifetime    decay table
        //             shortlived      subType    anti_encoding

        anInstance = new G4ParticleDefinition(
                name,       mass*MeV,     0.0*MeV,    charge*eplus,
                1,            0,           0,
                0,          0,         0,
                "fermion",    0,           0,         pdgID,
                true,        -1.0,       NULL,
                false,    "none"
        );

        // mag_moment = 0.5 * g * q * h_bar * spin / m_q
        //G4double muB =  0.5 * 2.0023 * charge * eplus * CLHEP::hbar_Planck * CLHEP::c_squared/mass*MeV;
        //anInstance->SetPDGMagneticMoment(muB);
    }
    theInstance = reinterpret_cast<ParticleMilli*>(anInstance);
    return theInstance;
}

ParticleMilli*  ParticleMilli::MillichargeDefinition()
{
    return Definition();
}

ParticleMilli*  ParticleMilli::Millicharge()
{
    return Definition();
}