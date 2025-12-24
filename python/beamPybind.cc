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
  .def_readonly("particle", &GMAD::BeamBase::particle)
  .def_readonly("beamParticleName", &GMAD::BeamBase::beamParticleName)
  .def_readonly("beamEnergy", &GMAD::BeamBase::beamEnergy)
  .def_readonly("beamKineticEnergy", &GMAD::BeamBase::beamKineticEnergy)
  .def_readonly("beamMomentum", &GMAD::BeamBase::beamMomentum)
  .def_readonly("distrType", &GMAD::BeamBase::distrType)
  .def_readonly("xDistrType", &GMAD::BeamBase::xDistrType)
  .def_readonly("yDistrType", &GMAD::BeamBase::yDistrType)
  .def_readonly("zDistrType", &GMAD::BeamBase::zDistrType)
  .def_readonly("spaceDistrType", &GMAD::BeamBase::spaceDistrType)
  .def_readonly("directionDistrType", &GMAD::BeamBase::directionDistrType)
  .def_readonly("energyDistrType", &GMAD::BeamBase::energyDistrType)
  .def_readonly("distrFile", &GMAD::BeamBase::distrFile)
  .def_readonly("distrFileFomat", &GMAD::BeamBase::distrFileFormat)
  .def_readonly("distrFileFromExecOptions", &GMAD::BeamBase::distrFileFromExecOptions)
  .def_readonly("distrFileMatchLength", &GMAD::BeamBase::distrFileMatchLength)
  .def_readonly("distrFileLoop", &GMAD::BeamBase::distrFileLoop)
  .def_readonly("removeUnstableWithoutDecay", &GMAD::BeamBase::removeUnstableWithoutDecay)
  .def_readonly("nlinesIgnore", &GMAD::BeamBase::nlinesIgnore)
  .def_readonly("nlinesSkip", &GMAD::BeamBase::nlinesSkip)

  .def_readonly("bunchFrequency", &GMAD::BeamBase::bunchFrequency)
  .def_readonly("bunchPeriod", &GMAD::BeamBase::bunchPeriod)
  .def_readonly("eventsPerBunch", &GMAD::BeamBase::eventsPerBunch)

  .def_readonly("X0", &GMAD::BeamBase::X0)
  .def_readonly("Y0", &GMAD::BeamBase::Y0)
  .def_readonly("Z0", &GMAD::BeamBase::Z0)
  .def_readonly("S0", &GMAD::BeamBase::S0)
  .def_readonly("T0", &GMAD::BeamBase::T0)
  .def_readonly("E0", &GMAD::BeamBase::E0)
  .def_readonly("Ek0", &GMAD::BeamBase::Ek0)
  .def_readonly("P0", &GMAD::BeamBase::P0)

  .def_readonly("tilt", &GMAD::BeamBase::tilt)

  .def_readonly("sigmaT", &GMAD::BeamBase::sigmaT)

  .def_readonly("sigmaE", &GMAD::BeamBase::sigmaE)
  .def_readonly("sigmaEk", &GMAD::BeamBase::sigmaEk)
  .def_readonly("sigmaP", &GMAD::BeamBase::sigmaP)

  .def_readonly("betx", &GMAD::BeamBase::betx)
  .def_readonly("bety", &GMAD::BeamBase::bety)
  .def_readonly("alfx", &GMAD::BeamBase::alfx)
  .def_readonly("alfy", &GMAD::BeamBase::alfy)
  .def_readonly("emitx", &GMAD::BeamBase::emitx)
  .def_readonly("emity", &GMAD::BeamBase::emity)
  .def_readonly("dispx", &GMAD::BeamBase::dispx)
  .def_readonly("dispy", &GMAD::BeamBase::dispy)
  .def_readonly("dispxp", &GMAD::BeamBase::dispxp)
  .def_readonly("dispyp", &GMAD::BeamBase::dispyp)
  .def_readonly("emitNX", &GMAD::BeamBase::emitNX)
  .def_readonly("emitNY", &GMAD::BeamBase::emitNY)

  .def_readonly("sigmaX", &GMAD::BeamBase::sigmaX)
  .def_readonly("sigmaXp", &GMAD::BeamBase::sigmaXp)
  .def_readonly("sigmaY", &GMAD::BeamBase::sigmaY)
  .def_readonly("sigmaYp", &GMAD::BeamBase::sigmaYp)

  .def_readonly("envelopeX", &GMAD::BeamBase::envelopeX)
  .def_readonly("envelopeXp", &GMAD::BeamBase::envelopeXp)
  .def_readonly("envelopeY", &GMAD::BeamBase::envelopeY)
  .def_readonly("envelopeYp", &GMAD::BeamBase::envelopeYp)
  .def_readonly("envelopeZ", &GMAD::BeamBase::envelopeZ)
  .def_readonly("envelopeZp", &GMAD::BeamBase::envelopeZp)
  .def_readonly("envelopeT", &GMAD::BeamBase::envelopeT)
  .def_readonly("envelopeE", &GMAD::BeamBase::envelopeE)
  .def_readonly("envelopeR", &GMAD::BeamBase::envelopeRp)

  .def_readonly("sigma11", &GMAD::BeamBase::sigma11)
  .def_readonly("sigma12", &GMAD::BeamBase::sigma12)
  .def_readonly("sigma13", &GMAD::BeamBase::sigma13)
  .def_readonly("sigma14", &GMAD::BeamBase::sigma14)
  .def_readonly("sigma15", &GMAD::BeamBase::sigma15)
  .def_readonly("sigma16", &GMAD::BeamBase::sigma16)
  .def_readonly("sigma22", &GMAD::BeamBase::sigma22)
  .def_readonly("sigma23", &GMAD::BeamBase::sigma23)
  .def_readonly("sigma24", &GMAD::BeamBase::sigma24)
  .def_readonly("sigma25", &GMAD::BeamBase::sigma25)
  .def_readonly("sigma26", &GMAD::BeamBase::sigma26)
  .def_readonly("sigma33", &GMAD::BeamBase::sigma33)
  .def_readonly("sigma34", &GMAD::BeamBase::sigma34)
  .def_readonly("sigma35", &GMAD::BeamBase::sigma35)
  .def_readonly("sigma36", &GMAD::BeamBase::sigma36)
  .def_readonly("sigma44", &GMAD::BeamBase::sigma44)
  .def_readonly("sigma45", &GMAD::BeamBase::sigma45)
  .def_readonly("sigma46", &GMAD::BeamBase::sigma46)
  .def_readonly("sigma55", &GMAD::BeamBase::sigma55)
  .def_readonly("sigma56", &GMAD::BeamBase::sigma56)
  .def_readonly("sigma66", &GMAD::BeamBase::sigma66)

  .def_readonly("shellX", &GMAD::BeamBase::shellX)
  .def_readonly("shellXp", &GMAD::BeamBase::shellXp)
  .def_readonly("shellY", &GMAD::BeamBase::shellY)
  .def_readonly("shellYp", &GMAD::BeamBase::shellYp)
  .def_readonly("shellXWidth", &GMAD::BeamBase::shellXWidth)
  .def_readonly("shellXpWidth", &GMAD::BeamBase::shellXpWidth)
  .def_readonly("shellYWidth", &GMAD::BeamBase::shellYWidth)
  .def_readonly("shellYpWidth", &GMAD::BeamBase::shellYpWidth)

  .def_readonly("Rmin", &GMAD::BeamBase::Rmin)
  .def_readonly("Rmax", &GMAD::BeamBase::Rmax)

  .def_readonly("haloNSigmaXInner",&GMAD::BeamBase::haloNSigmaXInner)
  .def_readonly("haloNSigmaXOuter",&GMAD::BeamBase::haloNSigmaXOuter)
  .def_readonly("haloNSigmaYInner",&GMAD::BeamBase::haloNSigmaYInner)
  .def_readonly("haloNSigmaYOuter",&GMAD::BeamBase::haloNSigmaYOuter)
  .def_readonly("haloXCutInner",&GMAD::BeamBase::haloXCutInner)
  .def_readonly("haloYCutInner",&GMAD::BeamBase::haloYCutInner)
  .def_readonly("haloXCutOuter",&GMAD::BeamBase::haloXCutOuter)
  .def_readonly("haloYCutOuter",&GMAD::BeamBase::haloYCutOuter)
  .def_readonly("haloXpCutInner",&GMAD::BeamBase::haloXpCutInner)
  .def_readonly("haloYpCutInner",&GMAD::BeamBase::haloYpCutInner)
  .def_readonly("haloXpCutOuter",&GMAD::BeamBase::haloXpCutOuter)
  .def_readonly("haloYpCutOuter",&GMAD::BeamBase::haloYpCutOuter)
  .def_readonly("haloPSWeightParameter",&GMAD::BeamBase::haloPSWeightParameter)
  .def_readonly("haloPSWeightFunction",&GMAD::BeamBase::haloPSWeightFunction)

  .def_readonly("offsetSampleMean",&GMAD::BeamBase::offsetSampleMean)

  .def_readonly("eventGeneratorNEventsSkip",&GMAD::BeamBase::eventGeneratorNEventsSkip)
  .def_readonly("eventGeneratorMinX",&GMAD::BeamBase::eventGeneratorMinX)
  .def_readonly("eventGeneratorMaxX",&GMAD::BeamBase::eventGeneratorMaxX)
  .def_readonly("eventGeneratorMinY",&GMAD::BeamBase::eventGeneratorMinY)
  .def_readonly("eventGeneratorMaxY",&GMAD::BeamBase::eventGeneratorMaxY)
  .def_readonly("eventGeneratorMinZ",&GMAD::BeamBase::eventGeneratorMinZ)
  .def_readonly("eventGeneratorMaxZ",&GMAD::BeamBase::eventGeneratorMaxZ)
  .def_readonly("eventGeneratorMinXp",&GMAD::BeamBase::eventGeneratorMinXp)
  .def_readonly("eventGeneratorMaxXp",&GMAD::BeamBase::eventGeneratorMaxXp)
  .def_readonly("eventGeneratorMinYp",&GMAD::BeamBase::eventGeneratorMinYp)
  .def_readonly("eventGeneratorMaxYp",&GMAD::BeamBase::eventGeneratorMaxYp)
  .def_readonly("eventGeneratorMinZp",&GMAD::BeamBase::eventGeneratorMinZp)
  .def_readonly("eventGeneratorMaxZp",&GMAD::BeamBase::eventGeneratorMaxZp)
  .def_readonly("eventGeneratorMinRp",&GMAD::BeamBase::eventGeneratorMinRp)
  .def_readonly("eventGeneratorMaxRp",&GMAD::BeamBase::eventGeneratorMaxRp)
  .def_readonly("eventGeneratorMinT",&GMAD::BeamBase::eventGeneratorMinT)
  .def_readonly("eventGeneratorMaxT",&GMAD::BeamBase::eventGeneratorMaxT)
  .def_readonly("eventGeneratorMinEK",&GMAD::BeamBase::eventGeneratorMinEK)
  .def_readonly("eventGeneratorMaxEK",&GMAD::BeamBase::eventGeneratorMaxEK)
  .def_readonly("eventGeneratorParticles",&GMAD::BeamBase::eventGeneratorParticles)
  .def_readonly("eventGeneratorWarnSkippedParticles",&GMAD::BeamBase::eventGeneratorWarnSkippedParticles)

  .def_readonly("dTStart",&GMAD::BeamBase::dTStart)
  .def_readonly("dTStop",&GMAD::BeamBase::dTStop)
  .def_readonly("dPStart",&GMAD::BeamBase::dPStart)
  .def_readonly("dPStop",&GMAD::BeamBase::dPStop)

  .def_readonly("setKeys",&GMAD::BeamBase::setKeys)

  .def_readonly("polarization1",&GMAD::BeamBase::polarization1)
  .def_readonly("polarization2",&GMAD::BeamBase::polarization2)
  .def_readonly("polarization3",&GMAD::BeamBase::polarization3);

py::class_<GMAD::Published<GMAD::BeamBase>>(m,"PublishedBeamBase")
  .def("NameExists",&GMAD::Beam::NameExists)
  .def("AllNames", &GMAD::Beam::AllNames);

py::class_<GMAD::Beam, GMAD::Published<GMAD::BeamBase>, GMAD::BeamBase>(m,"Beam")
  .def(py::init<>())
  .def("print",&GMAD::Beam::print)
  .def("Amalgamate", &GMAD::Beam::Amalgamate)
  .def("HasBeenSet", &GMAD::Beam::HasBeenSet)
  .def("KeysOfSetValues", &GMAD::Beam::KeysOfSetValues)

  .def("set_value",[](GMAD::Beam &self,std::string name,bool value) {self.set_value<bool>(name,value);})
  .def("set_value",[](GMAD::Beam &self,std::string name,int value) {self.set_value<int>(name, value);})
  .def("set_value",[](GMAD::Beam &self,std::string name,long int value) {self.set_value<long int>(name,value);})
  .def("set_value",[](GMAD::Beam &self,std::string name,double value) {self.set_value<double>(name,value);})
  .def("set_value",[](GMAD::Beam &self,std::string name,std::string value) {self.set_value<std::string>(name,value);})
  .def("get_value",[](GMAD::Beam &self,std::string name) {
    std::variant<bool, int, double, std::string, py::list> retval;

    try {
      retval = self.get<bool>(&self,name);
      return retval;
    }
    catch (const std::runtime_error&) {}

    try {
      retval = self.get<int>(&self,name);
      return retval;
    }
    catch (const std::runtime_error&) {}

    try {
      retval = self.get<double>(&self,name);
      return retval;
    }
    catch (const std::runtime_error&) {}

    try {
      retval = self.get<std::string>(&self,name);
      return retval;
    }
    catch (const std::runtime_error&) {}

    throw std::runtime_error("name not found : "+name);
})


  .def("keys", [](GMAD::Beam &self) {return self.AllNames();})
  .def("__len__", [](GMAD::Beam &self) {return self.AllNames().size();})
  .def("__setitem__", [](GMAD::Beam &self, const std::string& key, int value) {self.set_value(key, value, false);})
  .def("__setitem__", [](GMAD::Beam &self, const std::string& key, double value) {self.set_value(key, value, false);})
  .def("__setitem__", [](GMAD::Beam &self, const std::string& key, const std::string& value) {self.set_value(key, value, false);})
  .def("_ipython_key_completions_", [](GMAD::Beam &self) {return self.AllNames();});
}