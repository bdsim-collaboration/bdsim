/*
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway,
University of London 2001 - 2019.

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

#include "BDSLaser.hh"
#include "globals.hh" // geant4 types / globals
#include <math.h>
#include <vector>
#include <cmath>
#include "BDSAcceleratorComponent.hh"
BDSLaser::BDSLaser(G4double laserWavelengthIn,
                   G4double laserM2In,
                   G4double laserPulseDurationIn,
                   G4double laserPulseEnergyIn,
                   G4double laserWaistIn):
        laserWavelength(laserWavelengthIn),
        laserM2(laserM2In),
        laserPulseDuration(laserPulseDurationIn),
        laserPulseEnergy(laserPulseEnergyIn),
        laserWaist(laserWaistIn)

{
// add in all variables

    laserPeakPower = laserPulseEnergy/laserPulseDuration;

}

BDSLaser::~BDSLaser()
{;}

BDSLaser::BDSLaser(const BDSLaser &laser)
{
    laserWaist = laser.Wavelength();
    laserM2 = laser.M2();
    laserPulseDuration = laser.PulseDuration();
    laserPulseEnergy = laser.PulseEnergy();
    laserWaist = laser.Waist();
}

G4double BDSLaser::LaserRayleigh()
{
    return (M_PI*laserWaist*laserWaist)/(laserWavelength*laserM2);
}

G4double BDSLaser::LaserWidth(G4double particlePosition)
{
    return laserWaist*sqrt(1.0-std::pow(particlePosition/this->LaserRayleigh(),2.0));
}

// this will not work because of the position needed
// need to call the particle position in coordinates relative to the laser vector to establish laser width and intensity

G4double BDSLaser::LaserIntensity(G4double radius, G4double distanceFromFocus)
{
 return (2.0*laserPeakPower)/(CLHEP::pi*this->LaserWidth(distanceFromFocus)*this->LaserWidth(distanceFromFocus))*exp((-2.0*radius*radius)/(this->LaserWidth(distanceFromFocus)*this->LaserWidth(distanceFromFocus)));
}

G4double BDSLaser::GetRadius()
{
return std::sqrt((this->laserWaist*log(1.0/(CLHEP::e_squared)))/-2.0);
}
