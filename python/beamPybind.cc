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
//
// Created by Stewart Boogert on 12/03/2023.
//
#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
namespace py = pybind11;

#include "beam.h"
#include "beamBase.h"

PYBIND11_MODULE(beam, m) {

py::class_<GMAD::BeamBase>(m,"BeamBase")
  .def(py::init<>())
  .def_readwrite("particle", &GMAD::BeamBase::particle)
  .def_readwrite("beamParticleName", &GMAD::BeamBase::beamParticleName)
  .def_readwrite("beamEnergy", &GMAD::BeamBase::beamEnergy)
  .def_readwrite("beamKineticEnergy", &GMAD::BeamBase::beamKineticEnergy)
  .def_readwrite("beamMomentum", &GMAD::BeamBase::beamMomentum)
  .def_readwrite("distrType", &GMAD::BeamBase::distrType)
  .def_readwrite("xDistrType", &GMAD::BeamBase::xDistrType)
  .def_readwrite("yDistrType", &GMAD::BeamBase::yDistrType)
  .def_readwrite("zDistrType", &GMAD::BeamBase::zDistrType)
  .def_readwrite("spaceDistrType", &GMAD::BeamBase::spaceDistrType)
  .def_readwrite("directionDistrType", &GMAD::BeamBase::directionDistrType)
  .def_readwrite("energyDistrType", &GMAD::BeamBase::energyDistrType)
  .def_readwrite("distrFile", &GMAD::BeamBase::distrFile)
  .def_readwrite("distrFileFomat", &GMAD::BeamBase::distrFileFormat)
  .def_readwrite("distrFileFromExecOptions", &GMAD::BeamBase::distrFileFromExecOptions)
  .def_readwrite("distrFileMatchLength", &GMAD::BeamBase::distrFileMatchLength)
  .def_readwrite("distrFileLoop", &GMAD::BeamBase::distrFileLoop)
  .def_readwrite("removeUnstableWithoutDecay", &GMAD::BeamBase::removeUnstableWithoutDecay)
  .def_readwrite("nlinesIgnore", &GMAD::BeamBase::nlinesIgnore)
  .def_readwrite("nlinesSkip", &GMAD::BeamBase::nlinesSkip)

  .def_readwrite("bunchFrequency", &GMAD::BeamBase::bunchFrequency)
  .def_readwrite("bunchPeriod", &GMAD::BeamBase::bunchPeriod)
  .def_readwrite("eventsPerBunch", &GMAD::BeamBase::eventsPerBunch)

  .def_readwrite("X0", &GMAD::BeamBase::X0)
  .def_readwrite("Y0", &GMAD::BeamBase::Y0)
  .def_readwrite("Z0", &GMAD::BeamBase::Z0)
  .def_readwrite("S0", &GMAD::BeamBase::S0)
  .def_readwrite("T0", &GMAD::BeamBase::T0)
  .def_readwrite("E0", &GMAD::BeamBase::E0)
  .def_readwrite("Ek0", &GMAD::BeamBase::Ek0)
  .def_readwrite("P0", &GMAD::BeamBase::P0)

  .def_readwrite("tilt", &GMAD::BeamBase::tilt)

  .def_readwrite("sigmaT", &GMAD::BeamBase::sigmaT)

  .def_readwrite("sigmaE", &GMAD::BeamBase::sigmaE)
  .def_readwrite("sigmaEk", &GMAD::BeamBase::sigmaEk)
  .def_readwrite("sigmaP", &GMAD::BeamBase::sigmaP)

  .def_readwrite("betx", &GMAD::BeamBase::betx)
  .def_readwrite("bety", &GMAD::BeamBase::bety)
  .def_readwrite("alfx", &GMAD::BeamBase::alfx)
  .def_readwrite("alfy", &GMAD::BeamBase::alfy)
  .def_readwrite("emitx", &GMAD::BeamBase::emitx)
  .def_readwrite("emity", &GMAD::BeamBase::emity)
  .def_readwrite("dispx", &GMAD::BeamBase::dispx)
  .def_readwrite("dispy", &GMAD::BeamBase::dispy)
  .def_readwrite("dispxp", &GMAD::BeamBase::dispxp)
  .def_readwrite("dispyp", &GMAD::BeamBase::dispyp)
  .def_readwrite("emitNX", &GMAD::BeamBase::emitNX)
  .def_readwrite("emitNY", &GMAD::BeamBase::emitNY)

  .def_readwrite("sigmaX", &GMAD::BeamBase::sigmaX)
  .def_readwrite("sigmaXp", &GMAD::BeamBase::sigmaXp)
  .def_readwrite("sigmaY", &GMAD::BeamBase::sigmaY)
  .def_readwrite("sigmaYp", &GMAD::BeamBase::sigmaYp)

  .def_readwrite("envelopeX", &GMAD::BeamBase::envelopeX)
  .def_readwrite("envelopeXp", &GMAD::BeamBase::envelopeXp)
  .def_readwrite("envelopeY", &GMAD::BeamBase::envelopeY)
  .def_readwrite("envelopeYp", &GMAD::BeamBase::envelopeYp)
  .def_readwrite("envelopeZ", &GMAD::BeamBase::envelopeZ)
  .def_readwrite("envelopeZp", &GMAD::BeamBase::envelopeZp)
  .def_readwrite("envelopeT", &GMAD::BeamBase::envelopeT)
  .def_readwrite("envelopeE", &GMAD::BeamBase::envelopeE)
  .def_readwrite("envelopeR", &GMAD::BeamBase::envelopeRp)

  .def_readwrite("sigma11", &GMAD::BeamBase::sigma11)
  .def_readwrite("sigma12", &GMAD::BeamBase::sigma12)
  .def_readwrite("sigma13", &GMAD::BeamBase::sigma13)
  .def_readwrite("sigma14", &GMAD::BeamBase::sigma14)
  .def_readwrite("sigma15", &GMAD::BeamBase::sigma15)
  .def_readwrite("sigma16", &GMAD::BeamBase::sigma16)
  .def_readwrite("sigma22", &GMAD::BeamBase::sigma22)
  .def_readwrite("sigma23", &GMAD::BeamBase::sigma23)
  .def_readwrite("sigma24", &GMAD::BeamBase::sigma24)
  .def_readwrite("sigma25", &GMAD::BeamBase::sigma25)
  .def_readwrite("sigma26", &GMAD::BeamBase::sigma26)
  .def_readwrite("sigma33", &GMAD::BeamBase::sigma33)
  .def_readwrite("sigma34", &GMAD::BeamBase::sigma34)
  .def_readwrite("sigma35", &GMAD::BeamBase::sigma35)
  .def_readwrite("sigma36", &GMAD::BeamBase::sigma36)
  .def_readwrite("sigma44", &GMAD::BeamBase::sigma44)
  .def_readwrite("sigma45", &GMAD::BeamBase::sigma45)
  .def_readwrite("sigma46", &GMAD::BeamBase::sigma46)
  .def_readwrite("sigma55", &GMAD::BeamBase::sigma55)
  .def_readwrite("sigma56", &GMAD::BeamBase::sigma56)
  .def_readwrite("sigma66", &GMAD::BeamBase::sigma66)

  .def_readwrite("shellX", &GMAD::BeamBase::shellX)
  .def_readwrite("shellXp", &GMAD::BeamBase::shellXp)
  .def_readwrite("shellY", &GMAD::BeamBase::shellY)
  .def_readwrite("shellYp", &GMAD::BeamBase::shellYp)
  .def_readwrite("shellXWidth", &GMAD::BeamBase::shellXWidth)
  .def_readwrite("shellXpWidth", &GMAD::BeamBase::shellXpWidth)
  .def_readwrite("shellYWidth", &GMAD::BeamBase::shellYWidth)
  .def_readwrite("shellYpWidth", &GMAD::BeamBase::shellYpWidth)

  .def_readwrite("Rmin", &GMAD::BeamBase::Rmin)
  .def_readwrite("Rmax", &GMAD::BeamBase::Rmax)

  .def_readwrite("haloNSigmaXInner",&GMAD::BeamBase::haloNSigmaXInner)
  .def_readwrite("haloNSigmaXOuter",&GMAD::BeamBase::haloNSigmaXOuter)
  .def_readwrite("haloNSigmaYInner",&GMAD::BeamBase::haloNSigmaYInner)
  .def_readwrite("haloNSigmaYOuter",&GMAD::BeamBase::haloNSigmaYOuter)
  .def_readwrite("haloXCutInner",&GMAD::BeamBase::haloXCutInner)
  .def_readwrite("haloYCutInner",&GMAD::BeamBase::haloYCutInner)
  .def_readwrite("haloXCutOuter",&GMAD::BeamBase::haloXCutOuter)
  .def_readwrite("haloYCutOuter",&GMAD::BeamBase::haloYCutOuter)
  .def_readwrite("haloXpCutInner",&GMAD::BeamBase::haloXpCutInner)
  .def_readwrite("haloYpCutInner",&GMAD::BeamBase::haloYpCutInner)
  .def_readwrite("haloXpCutOuter",&GMAD::BeamBase::haloXpCutOuter)
  .def_readwrite("haloYpCutOuter",&GMAD::BeamBase::haloYpCutOuter)
  .def_readwrite("haloPSWeightParameter",&GMAD::BeamBase::haloPSWeightParameter)
  .def_readwrite("haloPSWeightFunction",&GMAD::BeamBase::haloPSWeightFunction)

  .def_readwrite("offsetSampleMean",&GMAD::BeamBase::offsetSampleMean)

  .def_readwrite("eventGeneratorNEventsSkip",&GMAD::BeamBase::eventGeneratorNEventsSkip)
  .def_readwrite("eventGeneratorMinX",&GMAD::BeamBase::eventGeneratorMinX)
  .def_readwrite("eventGeneratorMaxX",&GMAD::BeamBase::eventGeneratorMaxX)
  .def_readwrite("eventGeneratorMinY",&GMAD::BeamBase::eventGeneratorMinY)
  .def_readwrite("eventGeneratorMaxY",&GMAD::BeamBase::eventGeneratorMaxY)
  .def_readwrite("eventGeneratorMinZ",&GMAD::BeamBase::eventGeneratorMinZ)
  .def_readwrite("eventGeneratorMaxZ",&GMAD::BeamBase::eventGeneratorMaxZ)
  .def_readwrite("eventGeneratorMinXp",&GMAD::BeamBase::eventGeneratorMinXp)
  .def_readwrite("eventGeneratorMaxXp",&GMAD::BeamBase::eventGeneratorMaxXp)
  .def_readwrite("eventGeneratorMinYp",&GMAD::BeamBase::eventGeneratorMinYp)
  .def_readwrite("eventGeneratorMaxYp",&GMAD::BeamBase::eventGeneratorMaxYp)
  .def_readwrite("eventGeneratorMinZp",&GMAD::BeamBase::eventGeneratorMinZp)
  .def_readwrite("eventGeneratorMaxZp",&GMAD::BeamBase::eventGeneratorMaxZp)
  .def_readwrite("eventGeneratorMinRp",&GMAD::BeamBase::eventGeneratorMinRp)
  .def_readwrite("eventGeneratorMaxRp",&GMAD::BeamBase::eventGeneratorMaxRp)
  .def_readwrite("eventGeneratorMinT",&GMAD::BeamBase::eventGeneratorMinT)
  .def_readwrite("eventGeneratorMaxT",&GMAD::BeamBase::eventGeneratorMaxT)
  .def_readwrite("eventGeneratorMinEK",&GMAD::BeamBase::eventGeneratorMinEK)
  .def_readwrite("eventGeneratorMaxEK",&GMAD::BeamBase::eventGeneratorMaxEK)
  .def_readwrite("eventGeneratorParticles",&GMAD::BeamBase::eventGeneratorParticles)
  .def_readwrite("eventGeneratorWarnSkippedParticles",&GMAD::BeamBase::eventGeneratorWarnSkippedParticles)

  .def_readwrite("dTStart",&GMAD::BeamBase::dTStart)
  .def_readwrite("dTStop",&GMAD::BeamBase::dTStop)
  .def_readwrite("dPStart",&GMAD::BeamBase::dPStart)
  .def_readwrite("dPStop",&GMAD::BeamBase::dPStop)

  .def_readwrite("setKeys",&GMAD::BeamBase::setKeys)

  .def_readwrite("polarization1",&GMAD::BeamBase::polarization1)
  .def_readwrite("polarization2",&GMAD::BeamBase::polarization2)
  .def_readwrite("polarization3",&GMAD::BeamBase::polarization3);

py::class_<GMAD::Published<GMAD::BeamBase>>(m,"PublishedBeamBase")
  .def("NameExists",&GMAD::Beam::NameExists)
  .def("AllNames", &GMAD::Beam::AllNames);

py::class_<GMAD::Beam, GMAD::Published<GMAD::BeamBase>, GMAD::BeamBase>(m,"Beam")
  .def(py::init<>())
  .def("print",&GMAD::Beam::print)
  .def("Amalgamate", &GMAD::Beam::Amalgamate)
  .def("HasBeenSet", &GMAD::Beam::HasBeenSet)
  .def("KeysOfSetValues", &GMAD::Beam::KeysOfSetValues)

  .def("set_value",[](GMAD::Beam &beam,std::string name,std::string value) {beam.set_value<std::string>(name,value);})
  .def("set_value",[](GMAD::Beam &beam,std::string name,int value) {beam.set_value<int>(name, value);})
  .def("set_value",[](GMAD::Beam &beam,std::string name,bool value) {beam.set_value<bool>(name,value);})
  .def("set_value",[](GMAD::Beam &beam,std::string name,long int value) {beam.set_value<long int>(name,value);})
  .def("set_value",[](GMAD::Beam &beam,std::string name,double value) {beam.set_value<double>(name,value);})
  .def("get_value", &GMAD::Beam::get_value)

  .def("keys", [](GMAD::Beam &self) {return self.AllNames();})
  .def("__len__", [](GMAD::Beam &self) {return self.AllNames().size();})
  .def("__setitem__", [](GMAD::Beam &self, const std::string& key, int value) {self.set_value(key, value, false);})
  .def("__setitem__", [](GMAD::Beam &self, const std::string& key, double value) {self.set_value(key, value, false);})
  .def("__setitem__", [](GMAD::Beam &self, const std::string& key, const std::string& value) {self.set_value(key, value, false);})
  .def("_ipython_key_completions_", [](GMAD::Beam &self) {return self.AllNames();})

  .def("copy_from",[](GMAD::Beam &self, GMAD::Beam &other) {
    self.particle = other.particle;
    self.beamParticleName = other.beamParticleName;
    self.beamEnergy = other.beamEnergy;
    self.beamKineticEnergy = other.beamKineticEnergy;
    self.beamMomentum = other.beamMomentum;
    self.distrType = other.distrType;
    self.xDistrType = other.xDistrType;
    self.zDistrType = other.zDistrType;
    self.spaceDistrType = other.spaceDistrType;
    self.directionDistrType = other.directionDistrType;
    self.distrFile = other.distrFile;
    self.distrFileFormat = other.distrFileFormat;
    self.distrFileMatchLength = other.distrFileMatchLength;
    self.distrFileLoop = other.distrFileLoop;
    self.removeUnstableWithoutDecay = other.removeUnstableWithoutDecay;
    self.nlinesIgnore = other.nlinesIgnore;
    self.nlinesSkip = other.nlinesSkip;

    self.bunchFrequency = other.bunchFrequency;
    self.bunchPeriod = other.bunchPeriod;
    self.eventsPerBunch = other.eventsPerBunch;

    self.X0 = other.X0;
    self.Y0 = other.Y0;
    self.Z0 = other.Z0;
    self.T0 = other.T0;
    self.E0 = other.E0;
    self.Ek0 = other.Ek0;
    self.P0 = other.P0;

    self.tilt = other.tilt;

    self.sigmaE = other.sigmaE;
    self.sigmaEk = other.sigmaEk;
    self.sigmaP = other.sigmaP;

    self.betx = other.betx;
    self.bety = other.bety;
    self.alfx = other.alfx;
    self.alfy = other.alfy;
    self.emitx = other.emitx;
    self.emity = other.emity;
    self.dispx = other.dispx;
    self.dispy = other.dispy;
    self.dispxp = other.dispxp;
    self.dispyp = other.dispyp;
    self.emitNX = other.emitNX;
    self.emitNY = other.emitNY;

    self.sigmaX = other.sigmaX;
    self.sigmaXp = other.sigmaXp;
    self.sigmaY = other.sigmaY;
    self.sigmaYp = other.sigmaYp;

    self.envelopeX = other.envelopeX;
    self.envelopeXp = other.envelopeXp;
    self.envelopeY = other.envelopeY;
    self.envelopeYp = other.envelopeYp;
    self.envelopeZ = other.envelopeZ;
    self.envelopeZp = other.envelopeZp;
    self.envelopeT = other.envelopeT;
    self.envelopeRp = other.envelopeRp;
    self.zFromT = other.zFromT;

    self.sigma11 = other.sigma11;
    self.sigma12 = other.sigma12;
    self.sigma13 = other.sigma13;
    self.sigma14 = other.sigma14;
    self.sigma15 = other.sigma15;
    self.sigma16 = other.sigma16;
    self.sigma22 = other.sigma22;
    self.sigma23 = other.sigma23;
    self.sigma24 = other.sigma24;
    self.sigma25 = other.sigma25;
    self.sigma26 = other.sigma26;
    self.sigma33 = other.sigma33;
    self.sigma34 = other.sigma34;
    self.sigma35 = other.sigma35;
    self.sigma36 = other.sigma36;
    self.sigma44 = other.sigma44;
    self.sigma45 = other.sigma45;
    self.sigma46 = other.sigma46;
    self.sigma55 = other.sigma55;
    self.sigma56 = other.sigma56;
    self.sigma66 = other.sigma66;

    self.shellX = other.shellX;
    self.shellXp = other.shellXp;
    self.shellY = other.shellY;
    self.shellYp = other.shellYp;
    self.shellXWidth = other.shellXWidth;
    self.shellXpWidth = other.shellXpWidth;
    self.shellYWidth = other.shellYpWidth;

    self.Rmin = other.Rmin;
    self.Rmax = other.Rmax;

    self.haloNSigmaXInner = other.haloNSigmaXInner;
    self.haloNSigmaXOuter = other.haloNSigmaXOuter;
    self.haloNSigmaYInner = other.haloNSigmaYInner;
    self.haloNSigmaYOuter = other.haloNSigmaYOuter;
    self.haloXCutInner = other.haloXCutInner;
    self.haloYCutInner = other.haloYCutInner;
    self.haloXCutOuter = other.haloXCutOuter;
    self.haloYCutOuter = other.haloYCutOuter;
    self.haloXpCutInner = other.haloXpCutInner;
    self.haloYpCutInner = other.haloYpCutInner;
    self.haloXpCutOuter = other.haloXpCutOuter;
    self.haloYpCutOuter = other.haloYpCutOuter;
    self.haloPSWeightParameter = other.haloPSWeightParameter;
    self.haloPSWeightFunction = other.haloPSWeightFunction;

    self.offsetSampleMean = other.offsetSampleMean;

    self.eventGeneratorNEventsSkip = other.eventGeneratorNEventsSkip;
    self.eventGeneratorMinX = other.eventGeneratorMinX;
    self.eventGeneratorMaxX = other.eventGeneratorMaxX;
    self.eventGeneratorMinY = other.eventGeneratorMinY;
    self.eventGeneratorMaxY = other.eventGeneratorMaxY;
    self.eventGeneratorMinZ = other.eventGeneratorMinZ;
    self.eventGeneratorMaxZ = other.eventGeneratorMaxZ;
    self.eventGeneratorMinXp = other.eventGeneratorMinXp;
    self.eventGeneratorMaxXp = other.eventGeneratorMaxXp;
    self.eventGeneratorMinYp = other.eventGeneratorMinYp;
    self.eventGeneratorMaxYp = other.eventGeneratorMaxYp;
    self.eventGeneratorMinZp = other.eventGeneratorMinZp;
    self.eventGeneratorMaxZp = other.eventGeneratorMaxZp;
    self.eventGeneratorMinRp = other.eventGeneratorMinRp;
    self.eventGeneratorMaxRp = other.eventGeneratorMaxRp;
    self.eventGeneratorMinT = other.eventGeneratorMinT;
    self.eventGeneratorMaxT = other.eventGeneratorMaxT;
    self.eventGeneratorMinEK = other.eventGeneratorMinEK;
    self.eventGeneratorMaxEK = other.eventGeneratorMaxEK;
    self.eventGeneratorParticles = other.eventGeneratorParticles;
    self.eventGeneratorWarnSkippedParticles = other.eventGeneratorWarnSkippedParticles;

    self.dTStart = other.dTStart;
    self.dTStop = other.dTStop;
    self.dPStart = other.dPStart;
    self.dPStop = other.dPStop;

    self.polarization1 = other.polarization1;
    self.polarization2 = other.polarization2;
    self.polarization3 = other.polarization3;
  });
}