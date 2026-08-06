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
    .def("NameExists", &GMAD::Element::NameExists)
    .def("AllNames", &GMAD::Element::AllNames);

  py::class_<GMAD::Element, GMAD::Published<GMAD::Element>>(m,"Element")
    .def(py::init<>())
    .def_readwrite("type", &GMAD::Element::type) // TODO inconsistent interface
    .def_readwrite("name", &GMAD::Element::name) // TODO inconsistent interface
    .def_readonly("userTypeName", &GMAD::Element::userTypeName)
    .def_readonly("userParameters", &GMAD::Element::userParameters)
    .def_readonly("l", &GMAD::Element::l)
    .def_readonly("scaling", &GMAD::Element::scaling)
    .def_readonly("scalingFieldOuter", &GMAD::Element::scalingFieldOuter)
    .def_readonly("ks", &GMAD::Element::ks)
    .def_readonly("k1", &GMAD::Element::k1)
    .def_readonly("k2", &GMAD::Element::k2)
    .def_readonly("k3", &GMAD::Element::k3)
    .def_readonly("k4", &GMAD::Element::k4)
    .def_readonly("angle", &GMAD::Element::angle)
    .def_readonly("B", &GMAD::Element::B)
    .def_readonly("e1", &GMAD::Element::e1)
    .def_readonly("e2", &GMAD::Element::e2)
    .def_readonly("fint", &GMAD::Element::fint)
    .def_readonly("fintx", &GMAD::Element::fintx)
    .def_readonly("fintK2", &GMAD::Element::fintK2)
    .def_readonly("hgap", &GMAD::Element::hgap)
    .def_readonly("h1", &GMAD::Element::h1)
    .def_readonly("h2", &GMAD::Element::h2)
    .def_readonly("kick", &GMAD::Element::kick)
    .def_readonly("hkick", &GMAD::Element::hkick)
    .def_readonly("vkick", &GMAD::Element::vkick)
    .def_readonly("knl", &GMAD::Element::knl)
    .def_readonly("ksl", &GMAD::Element::ksl)
    .def_readonly("gradient", &GMAD::Element::gradient)
    .def_readonly("E", &GMAD::Element::E)
    .def_readonly("frequency", &GMAD::Element::frequency)
    .def_readonly("phase", &GMAD::Element::phase)
    .def_readonly("tOffset", &GMAD::Element::tOffset)
    .def_readonly("kick1", &GMAD::Element::kick1)
    .def_readonly("kick2", &GMAD::Element::kick2)
    .def_readonly("kick3", &GMAD::Element::kick3)
    .def_readonly("kick4", &GMAD::Element::kick4)
    .def_readonly("rmat11", &GMAD::Element::rmat11)
    .def_readonly("rmat12", &GMAD::Element::rmat12)
    .def_readonly("rmat13", &GMAD::Element::rmat13)
    .def_readonly("rmat14", &GMAD::Element::rmat14)
    .def_readonly("rmat21", &GMAD::Element::rmat21)
    .def_readonly("rmat22", &GMAD::Element::rmat22)
    .def_readonly("rmat23", &GMAD::Element::rmat23)
    .def_readonly("rmat24", &GMAD::Element::rmat24)
    .def_readonly("rmat31", &GMAD::Element::rmat31)
    .def_readonly("rmat32", &GMAD::Element::rmat32)
    .def_readonly("rmat33", &GMAD::Element::rmat33)
    .def_readonly("rmat34", &GMAD::Element::rmat34)
    .def_readonly("rmat41", &GMAD::Element::rmat41)
    .def_readonly("rmat42", &GMAD::Element::rmat42)
    .def_readonly("rmat43", &GMAD::Element::rmat43)
    .def_readonly("rmat44", &GMAD::Element::rmat44)

    .def_readonly("beampipeThickness", &GMAD::Element::beampipeThickness)
    .def_readonly("aper1", &GMAD::Element::aper1)
    .def_readonly("aper2", &GMAD::Element::aper2)
    .def_readonly("aper3", &GMAD::Element::aper3)
    .def_readonly("aper4", &GMAD::Element::aper4)
    .def_readonly("apertureType", &GMAD::Element::apertureType)
    .def_readonly("beampipeMaterial", &GMAD::Element::beampipeMaterial)
    .def_readonly("vacuumMaterial", &GMAD::Element::vacuumMaterial)

    .def_readonly("magnetGeometryType", &GMAD::Element::magnetGeometryType)
    .def_readonly("horizontalWidth", &GMAD::Element::horizontalWidth)
    .def_readonly("yokeOnInside", &GMAD::Element::yokeOnInside)
    .def_readonly("hStyle", &GMAD::Element::hStyle)
    .def_readonly("vhRatio", &GMAD::Element::vhRatio)
    .def_readonly("coilWidthFraction", &GMAD::Element::coilWidthFraction)
    .def_readonly("coilHeightFraction", &GMAD::Element::coilHeightFraction)
    .def_readonly("tilt", &GMAD::Element::tilt)
    .def_readonly("xsize", &GMAD::Element::xsize)
    .def_readonly("ysize", &GMAD::Element::ysize)
    .def_readonly("xsizeOut", &GMAD::Element::xsizeOut)
    .def_readonly("ysizeOut", &GMAD::Element::ysizeOut)
    .def_readonly("xsizeLeft", &GMAD::Element::xsizeLeft)
    .def_readonly("xsizeRight", &GMAD::Element::xsizeRight)
    .def_readonly("jawTiltLeft", &GMAD::Element::jawTiltLeft)
    .def_readonly("jawTiltRight", &GMAD::Element::jawTiltRight)
    .def_readonly("offsetX", &GMAD::Element::offsetX)
    .def_readonly("offsetY", &GMAD::Element::offsetY)

    .def_readonly("xsize2", &GMAD::Element::xsize2)
    .def_readonly("ysize2", &GMAD::Element::ysize2)
    .def_readonly("offsetX2", &GMAD::Element::offsetX2)
    .def_readonly("offsetY2", &GMAD::Element::offsetY2)
    .def_readonly("tilt2", &GMAD::Element::tilt2)
    .def_readonly("outerShape", &GMAD::Element::outerShape)

    .def_readonly("tscint", &GMAD::Element::tscint)
    .def_readonly("twindow", &GMAD::Element::twindow)
    .def_readonly("tmount", &GMAD::Element::tmount)
    .def_readonly("windowScreenGap", &GMAD::Element::windowScreenGap)
    .def_readonly("screenXSize", &GMAD::Element::screenXSize)
    .def_readonly("screenYSize", &GMAD::Element::screenYSize)
    .def_readonly("layerThicknesses", &GMAD::Element::layerThicknesses)
    .def_readonly("layerMaterials", &GMAD::Element::layerMaterials)
    .def_readonly("layerIsSampler", &GMAD::Element::layerIsSampler)

    .def_readonly("screenPSize", &GMAD::Element::screenPSize)
    .def_readonly("screenEndZ", &GMAD::Element::screenEndZ)
    .def_readonly("poleStartZ", &GMAD::Element::poleStartZ)
    .def_readonly("screenWidth", &GMAD::Element::screenWidth)
    .def_readonly("awakeMagnetOffsetX", &GMAD::Element::awakeMagnetOffsetX)
    .def_readonly("windowmaterial", &GMAD::Element::windowmaterial)
    .def_readonly("scintmaterial", &GMAD::Element::scintmaterial)
    .def_readonly("mountmaterial", &GMAD::Element::mountmaterial)

    .def_readonly("xdir", &GMAD::Element::xdir)
    .def_readonly("ydir", &GMAD::Element::ydir)
    .def_readonly("zdir", &GMAD::Element::zdir)

    .def_readonly("wavelength", &GMAD::Element::wavelength)
    .def_readonly("phi", &GMAD::Element::phi)
    .def_readonly("theta", &GMAD::Element::theta)
    .def_readonly("psi", &GMAD::Element::psi)
    .def_readonly("axisX", &GMAD::Element::axisX)
    .def_readonly("axisY", &GMAD::Element::axisY)
    .def_readonly("axisZ", &GMAD::Element::axisZ)
    .def_readonly("axisAngle", &GMAD::Element::axisAngle)

    .def_readonly("numberWedges", &GMAD::Element::numberWedges)
    .def_readonly("wedgeLength", &GMAD::Element::wedgeLength)
    .def_readonly("degraderHeight", &GMAD::Element::degraderHeight)
    .def_readonly("materialThickness", &GMAD::Element::materialThickness)
    .def_readonly("degraderOffset", &GMAD::Element::degraderOffset)

    .def_readonly("wireDiameter", &GMAD::Element::wireDiameter)
    .def_readonly("wireLength", &GMAD::Element::wireLength)
    .def_readonly("wireOffsetX", &GMAD::Element::wireOffsetX)
    .def_readonly("wireOffsetY", &GMAD::Element::wireOffsetY)
    .def_readonly("wireOffsetZ", &GMAD::Element::wireOffsetZ)
    .def_readonly("wireAngle", &GMAD::Element::wireAngle)

    .def_readonly("undulatorPeriod", &GMAD::Element::undulatorPeriod)
    .def_readonly("undulatorGap", &GMAD::Element::undulatorGap)
    .def_readonly("undulatorMagnetHeight", &GMAD::Element::undulatorMagnetHeight)

    .def_readonly("anodeLength", &GMAD::Element::anodeLength)
    .def_readonly("anodeRadius", &GMAD::Element::anodeRadius)
    .def_readonly("anodeThickness", &GMAD::Element::anodeThickness)
    .def_readonly("electrodeLength", &GMAD::Element::electrodeLength)
    .def_readonly("electrodeRadius", &GMAD::Element::electrodeRadius)
    .def_readonly("electrodeThickness", &GMAD::Element::electrodeThickness)

    .def_readonly("tipThickness", &GMAD::Element::tipThickness)
    .def_readonly("tipMaterial", &GMAD::Element::tipMaterial)

    .def_readonly("bias", &GMAD::Element::bias)
    .def_readonly("biasMaterial", &GMAD::Element::biasMaterial)
    .def_readonly("biasVacuum", &GMAD::Element::biasVacuum)
    .def_readonly("biasMaterialList", &GMAD::Element::biasMaterialList)
    .def_readonly("biasVacuumList", &GMAD::Element::biasVacuumList)

    .def_readonly("minimumKineticEnergy", &GMAD::Element::minimumKineticEnergy)

    .def_readonly("samplerName", &GMAD::Element::samplerName)
    .def_readonly("samplerType", &GMAD::Element::samplerType)
    .def_readonly("samplerRadius", &GMAD::Element::samplerRadius)

    .def_readonly("samplerParticleSetID", &GMAD::Element::samplerParticleSetID)

    .def_readonly("region", &GMAD::Element::region)
    .def_readonly("fieldOuter", &GMAD::Element::fieldOuter)
    .def_readonly("fieldVacuum", &GMAD::Element::fieldVacuum)
    .def_readonly("fieldAll", &GMAD::Element::fieldAll)

    .def_readonly("geometryFile", &GMAD::Element::geometryFile)
    .def_readonly("stripOuterVolume", &GMAD::Element::stripOuterVolume)
    .def_readonly("autoColour", &GMAD::Element::autoColour)

    .def_readonly("elementLengthIsArcLength", &GMAD::Element::elementLengthIsArcLength)

    .def_readonly("material", &GMAD::Element::material)
    .def_readonly("namedVacuumVolumes", &GMAD::Element::namedVacuumVolumes)
    .def_readonly("markAsCollimator", &GMAD::Element::markAsCollimator)
    .def_readonly("spec", &GMAD::Element::spec)
    .def_readonly("cavityModel", &GMAD::Element::cavityModel)
    .def_readonly("cavityFieldType", &GMAD::Element::cavityFieldType)

    .def_readonly("dicomDataPath", &GMAD::Element::dicomDataPath)
    .def_readonly("dicomDataFile", &GMAD::Element::dicomDataFile)

    .def_readonly("colour", &GMAD::Element::colour)

    .def_readonly("crystalLeft", &GMAD::Element::crystalLeft)
    .def_readonly("crystalRight", &GMAD::Element::crystalRight)
    .def_readonly("crystalBoth", &GMAD::Element::crystalBoth)
    .def_readonly("crystalAngleYAxisLeft", &GMAD::Element::crystalAngleYAxisLeft)
    .def_readonly("crystalAngleYAxisRight", &GMAD::Element::crystalAngleYAxisRight)

    .def_readonly("angleSet", &GMAD::Element::angleSet)

    .def_readonly("scalingFieldOuterSet", &GMAD::Element::scalingFieldOuterSet)

    .def_readonly("lst", &GMAD::Element::lst)

    .def("print", &GMAD::Element::print)
    .def("flush", &GMAD::Element::flush)
    .def("isSpecial", &GMAD::Element::isSpecial)
    .def("property_lookup", &GMAD::Element::property_lookup)
    .def("setSamplerInfo", &GMAD::Element::setSamplerInfo)
    .def("set",[](GMAD::Element& element, const GMAD::Parameters& params) {element.set(params);})
    .def("set",[](GMAD::Element& element, const GMAD::Parameters& params, std::string nameIn, GMAD::ElementType typeIn) {element.set(params, nameIn, typeIn);})

    .def("set_value",[](GMAD::Element& self, std::string name, bool value) {self.set_value<bool>(name, value, true);})
    .def("set_value",[](GMAD::Element& self, std::string name, int value) {self.set_value<int>(name, value, true);})
    .def("set_value",[](GMAD::Element& self, std::string name, long int value) {self.set_value<long int>(name, value, true);})
    .def("set_value",[](GMAD::Element& self, std::string name, double value) {self.set_value<double>(name, value, true);})
    .def("set_value",[](GMAD::Element& self, std::string name, std::string value) {self.set_value<std::string>(name, value, true);})
    .def("set_value",[](GMAD::Element& self, std::string name, GMAD::Array *value) {self.set_value_array(name,value,true);})
    .def("get_value_array",[](GMAD::Element &self, std::string name) {return self.get_value_array(name);})
    .def("get_value",[](GMAD::Element &self,std::string name) {
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

      try {
        auto arrval = self.get_value_array(name);
        py::list result;
        for (const auto& value : arrval) {
          result.append(value);
        }
        retval = result;
        return retval;
      }
      catch (const std::runtime_error&) {}

      throw std::runtime_error("name not found : "+name);
    })

    .def("keys", [](GMAD::Element &self) {return self.AllNames();})
    .def("__len__", [](GMAD::Element &self) {return self.AllNames().size();})
    .def("__setitem__", [](GMAD::Element &self, const std::string& key, bool value) {self.set_value(key,value, false);})
    .def("__setitem__", [](GMAD::Element &self, const std::string& key, int value) {self.set_value(key,value, false);})
    .def("__setitem__", [](GMAD::Element &self, const std::string& key, double value) {self.set_value(key,value, false);})
    .def("__setitem__", [](GMAD::Element &self, const std::string& key, const std::string& value) {self.set_value(key, value, false);})
    .def("__setitem__", [](GMAD::Element &self, const std::string& key, GMAD::Array *value) {self.set_value(key, value, false);})
    .def("__setitem__", [](GMAD::Element &self, const std::string& key, py::list &value) {
      py::module_ mbdsim = py::module_::import("bdsim");
      py::object cls = mbdsim.attr("Array");  // get the class
      py::object obj = cls(value);       // call constructor
      auto array = obj.cast<GMAD::Array*>();
      self.set_value_array(key, array, false);
    })
    .def("_ipython_key_completions_", [](GMAD::Element &self) {return self.AllNames();});
}
