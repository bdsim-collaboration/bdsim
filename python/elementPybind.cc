/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2023.

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
#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
namespace py = pybind11;

#include "element.h"
#include "elementtype.h"
#include "parameters.h"

PYBIND11_MODULE(element, m)
{
  py::class_<GMAD::Published<GMAD::Element>>(m,"PublishedElement")
    .def("NameExists", &GMAD::Element::NameExists);

  py::class_<GMAD::Element, GMAD::Published<GMAD::Element>>(m,"Element")
    .def(py::init<>())
    .def_readwrite("type", &GMAD::Element::type)
    .def_readwrite("name", &GMAD::Element::name)
    .def_readwrite("userTypeName", &GMAD::Element::userTypeName)
    .def_readwrite("userParameters", &GMAD::Element::userParameters)
    .def_readwrite("l", &GMAD::Element::l)
    .def_readwrite("scaling", &GMAD::Element::scaling)
    .def_readwrite("scalingFieldOuter", &GMAD::Element::scalingFieldOuter)
    .def_readwrite("ks", &GMAD::Element::ks)
    .def_readwrite("k1", &GMAD::Element::k1)
    .def_readwrite("k2", &GMAD::Element::k2)
    .def_readwrite("k3", &GMAD::Element::k3)
    .def_readwrite("k4", &GMAD::Element::k4)
    .def_readwrite("angle", &GMAD::Element::angle)
    .def_readwrite("B", &GMAD::Element::B)
    .def_readwrite("e1", &GMAD::Element::e1)
    .def_readwrite("e2", &GMAD::Element::e2)
    .def_readwrite("fint", &GMAD::Element::fint)
    .def_readwrite("fintx", &GMAD::Element::fintx)
    .def_readwrite("fintK2", &GMAD::Element::fintK2)
    .def_readwrite("hgap", &GMAD::Element::hgap)
    .def_readwrite("h1", &GMAD::Element::h1)
    .def_readwrite("h2", &GMAD::Element::h2)
    .def_readwrite("kick", &GMAD::Element::kick)
    .def_readwrite("hkick", &GMAD::Element::hkick)
    .def_readwrite("vkick", &GMAD::Element::vkick)
    .def_readwrite("knl", &GMAD::Element::knl)
    .def_readwrite("ksl", &GMAD::Element::ksl)
    .def_readwrite("gradient", &GMAD::Element::gradient)
    .def_readwrite("E", &GMAD::Element::E)
    .def_readwrite("frequency", &GMAD::Element::frequency)
    .def_readwrite("phase", &GMAD::Element::phase)
    .def_readwrite("tOffset", &GMAD::Element::tOffset)
    .def_readwrite("kick1", &GMAD::Element::kick1)
    .def_readwrite("kick2", &GMAD::Element::kick2)
    .def_readwrite("kick3", &GMAD::Element::kick3)
    .def_readwrite("kick4", &GMAD::Element::kick4)
    .def_readwrite("rmat11", &GMAD::Element::rmat11)
    .def_readwrite("rmat12", &GMAD::Element::rmat12)
    .def_readwrite("rmat13", &GMAD::Element::rmat13)
    .def_readwrite("rmat14", &GMAD::Element::rmat14)
    .def_readwrite("rmat21", &GMAD::Element::rmat21)
    .def_readwrite("rmat22", &GMAD::Element::rmat22)
    .def_readwrite("rmat23", &GMAD::Element::rmat23)
    .def_readwrite("rmat24", &GMAD::Element::rmat24)
    .def_readwrite("rmat31", &GMAD::Element::rmat31)
    .def_readwrite("rmat32", &GMAD::Element::rmat32)
    .def_readwrite("rmat33", &GMAD::Element::rmat33)
    .def_readwrite("rmat34", &GMAD::Element::rmat34)
    .def_readwrite("rmat41", &GMAD::Element::rmat41)
    .def_readwrite("rmat42", &GMAD::Element::rmat42)
    .def_readwrite("rmat43", &GMAD::Element::rmat43)
    .def_readwrite("rmat44", &GMAD::Element::rmat44)

    .def_readwrite("beampipeThickness", &GMAD::Element::beampipeThickness)
    .def_readwrite("aper1", &GMAD::Element::aper1)
    .def_readwrite("aper2", &GMAD::Element::aper2)
    .def_readwrite("aper3", &GMAD::Element::aper3)
    .def_readwrite("aper4", &GMAD::Element::aper4)
    .def_readwrite("apertureType", &GMAD::Element::apertureType)
    .def_readwrite("beampipeMaterial", &GMAD::Element::beampipeMaterial)
    .def_readwrite("vacuumMaterial", &GMAD::Element::vacuumMaterial)

    .def_readwrite("magnetGeometryType", &GMAD::Element::magnetGeometryType)
    .def_readwrite("horizontalWidth", &GMAD::Element::horizontalWidth)
    .def_readwrite("yokeOnInside", &GMAD::Element::yokeOnInside)
    .def_readwrite("hStyle", &GMAD::Element::hStyle)
    .def_readwrite("vhRatio", &GMAD::Element::vhRatio)
    .def_readwrite("coilWidthFraction", &GMAD::Element::coilWidthFraction)
    .def_readwrite("coilHeightFraction", &GMAD::Element::coilHeightFraction)
    .def_readwrite("tilt", &GMAD::Element::tilt)
    .def_readwrite("xsize", &GMAD::Element::xsize)
    .def_readwrite("ysize", &GMAD::Element::ysize)
    .def_readwrite("xsizeOut", &GMAD::Element::xsizeOut)
    .def_readwrite("ysizeOut", &GMAD::Element::ysizeOut)
    .def_readwrite("xsizeLeft", &GMAD::Element::xsizeLeft)
    .def_readwrite("xsizeRight", &GMAD::Element::xsizeRight)
    .def_readwrite("jawTiltLeft", &GMAD::Element::jawTiltLeft)
    .def_readwrite("jawTiltRight", &GMAD::Element::jawTiltRight)
    .def_readwrite("offsetX", &GMAD::Element::offsetX)
    .def_readwrite("offsetY", &GMAD::Element::offsetY)

    .def_readwrite("tscint", &GMAD::Element::tscint)
    .def_readwrite("twindow", &GMAD::Element::twindow)
    .def_readwrite("tmount", &GMAD::Element::tmount)
    .def_readwrite("windowScreenGap", &GMAD::Element::windowScreenGap)
    .def_readwrite("screenXSize", &GMAD::Element::screenXSize)
    .def_readwrite("screenYSize", &GMAD::Element::screenYSize)
    .def_readwrite("layerThicknesses", &GMAD::Element::layerThicknesses)
    .def_readwrite("layerMaterials", &GMAD::Element::layerMaterials)
    .def_readwrite("layerIsSampler", &GMAD::Element::layerIsSampler)

    .def_readwrite("screenPSize", &GMAD::Element::screenPSize)
    .def_readwrite("screenEndZ", &GMAD::Element::screenEndZ)
    .def_readwrite("poleStartZ", &GMAD::Element::poleStartZ)
    .def_readwrite("screenWidth", &GMAD::Element::screenWidth)
    .def_readwrite("awakeMagnetOffsetX", &GMAD::Element::awakeMagnetOffsetX)
    .def_readwrite("windowmaterial", &GMAD::Element::windowmaterial)
    .def_readwrite("scintmaterial", &GMAD::Element::scintmaterial)
    .def_readwrite("mountmaterial", &GMAD::Element::mountmaterial)

    .def_readwrite("xdir", &GMAD::Element::xdir)
    .def_readwrite("ydir", &GMAD::Element::ydir)
    .def_readwrite("zdir", &GMAD::Element::zdir)

    .def_readwrite("waveLength", &GMAD::Element::waveLength)
    .def_readwrite("phi", &GMAD::Element::phi)
    .def_readwrite("theta", &GMAD::Element::theta)
    .def_readwrite("psi", &GMAD::Element::psi)
    .def_readwrite("axisX", &GMAD::Element::axisX)
    .def_readwrite("axisY", &GMAD::Element::axisY)
    .def_readwrite("axisZ", &GMAD::Element::axisZ)
    .def_readwrite("axisAngle", &GMAD::Element::axisAngle)

    .def_readwrite("numberWedges", &GMAD::Element::numberWedges)
    .def_readwrite("wedgeLength", &GMAD::Element::wedgeLength)
    .def_readwrite("degraderHeight", &GMAD::Element::degraderHeight)
    .def_readwrite("materialThickness", &GMAD::Element::materialThickness)
    .def_readwrite("degraderOffset", &GMAD::Element::degraderOffset)

    .def_readwrite("wireDiameter", &GMAD::Element::wireDiameter)
    .def_readwrite("wireLength", &GMAD::Element::wireLength)
    .def_readwrite("wireOffsetX", &GMAD::Element::wireOffsetX)
    .def_readwrite("wireOffsetY", &GMAD::Element::wireOffsetY)
    .def_readwrite("wireOffsetZ", &GMAD::Element::wireOffsetZ)
    .def_readwrite("wireAngle", &GMAD::Element::wireAngle)

    .def_readwrite("undulatorPeriod", &GMAD::Element::undulatorPeriod)
    .def_readwrite("undulatorGap", &GMAD::Element::undulatorGap)
    .def_readwrite("undulatorMagnetHeight", &GMAD::Element::undulatorMagnetHeight)

    .def_readwrite("bias", &GMAD::Element::bias)
    .def_readwrite("biasMaterial", &GMAD::Element::biasMaterial)
    .def_readwrite("biasVacuum", &GMAD::Element::biasVacuum)
    .def_readwrite("biasMaterialList", &GMAD::Element::biasMaterialList)
    .def_readwrite("biasVacuumList", &GMAD::Element::biasVacuumList)

    .def_readwrite("minimumKineticEnergy", &GMAD::Element::minimumKineticEnergy)

    .def_readwrite("samplerName", &GMAD::Element::samplerName)
    .def_readwrite("samplerType", &GMAD::Element::samplerType)
    .def_readwrite("samplerRadius", &GMAD::Element::samplerRadius)

    .def_readwrite("samplerParticleSetID", &GMAD::Element::samplerParticleSetID)

    .def_readwrite("region", &GMAD::Element::region)
    .def_readwrite("fieldOuter", &GMAD::Element::fieldOuter)
    .def_readwrite("fieldVacuum", &GMAD::Element::fieldVacuum)
    .def_readwrite("fieldAll", &GMAD::Element::fieldAll)

    .def_readwrite("geometryFile", &GMAD::Element::geometryFile)
    .def_readwrite("stripOuterVolume", &GMAD::Element::stripOuterVolume)
    .def_readwrite("autoColour", &GMAD::Element::autoColour)

    .def_readwrite("elementLengthIsArcLength", &GMAD::Element::elementLengthIsArcLength)

    .def_readwrite("material", &GMAD::Element::material)
    .def_readwrite("namedVacuumVolumes", &GMAD::Element::namedVacuumVolumes)
    .def_readwrite("markAsCollimator", &GMAD::Element::markAsCollimator)
    .def_readwrite("spec", &GMAD::Element::spec)
    .def_readwrite("cavityModel", &GMAD::Element::cavityModel)
    .def_readwrite("cavityFieldType", &GMAD::Element::cavityFieldType)

    .def_readwrite("dicomDataPath", &GMAD::Element::dicomDataPath)
    .def_readwrite("dicomDataFile", &GMAD::Element::dicomDataFile)

    .def_readwrite("colour", &GMAD::Element::colour)

    .def_readwrite("crystalLeft", &GMAD::Element::crystalLeft)
    .def_readwrite("crystalRight", &GMAD::Element::crystalRight)
    .def_readwrite("crystalBoth", &GMAD::Element::crystalBoth)
    .def_readwrite("crystalAngleYAxisLeft", &GMAD::Element::crystalAngleYAxisLeft)
    .def_readwrite("crystalAngleYAxisRight", &GMAD::Element::crystalAngleYAxisRight)

    .def_readwrite("angleSet", &GMAD::Element::angleSet)

    .def_readwrite("scalingFieldOuterSet", &GMAD::Element::scalingFieldOuterSet)

    .def_readwrite("lst", &GMAD::Element::lst)

    .def("print", &GMAD::Element::print)
    .def("flush", &GMAD::Element::flush)
    .def("isSpecial", &GMAD::Element::isSpecial)
    .def("property_lookup", &GMAD::Element::property_lookup)
    .def("setSamplerInfo", &GMAD::Element::setSamplerInfo)
    .def("set",[](GMAD::Element& element, const GMAD::Parameters& params) {element.set(params);})
    .def("set",[](GMAD::Element& element, const GMAD::Parameters& params, std::string nameIn, GMAD::ElementType typeIn) {element.set(params, nameIn, typeIn);})

    .def("set_value",[](GMAD::Element& element, std::string name, std::string value) {element.set_value<std::string>(name, value, true);})
    .def("set_value",[](GMAD::Element& element, std::string name, int value) {element.set_value<int>(name, value, true);})
    .def("set_value",[](GMAD::Element& element, std::string name, bool value) {element.set_value<bool>(name, value, true);})
    .def("set_value",[](GMAD::Element& element, std::string name, long int value) {element.set_value<long int>(name, value, true);})
    .def("set_value",[](GMAD::Element& element, std::string name, double value) {element.set_value<double>(name, value, true);});
}
