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
.def_readwrite("sigma66", &GMAD::BeamBase::sigma66);

py::class_<GMAD::Published<GMAD::BeamBase>>(m,"PublishedBeamBase")
.def("NameExists",&GMAD::Beam::NameExists);

py::class_<GMAD::Beam, GMAD::Published<GMAD::BeamBase>, GMAD::BeamBase>(m,"Beam")
.def(py::init<>())
.def("set_value",[](GMAD::Beam &beam,std::string name,std::string value) {beam.set_value<std::string>(name,value);})
.def("set_value",[](GMAD::Beam &beam,std::string name,int value) {beam.set_value<int>(name, value);})
.def("set_value",[](GMAD::Beam &beam,std::string name,bool value) {beam.set_value<bool>(name,value);})
.def("set_value",[](GMAD::Beam &beam,std::string name,long int value) {beam.set_value<long int>(name,value);})
.def("set_value",[](GMAD::Beam &beam,std::string name,double value) {beam.set_value<double>(name,value);});
}