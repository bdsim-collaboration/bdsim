#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
namespace py = pybind11;

#include "elementtype.h"

PYBIND11_MODULE(elementtype, m) {
  py::enum_<GMAD::ElementType>(m,"ElementType")
    .value("NONE", GMAD::ElementType::_NONE)
    .value("MARKER", GMAD::ElementType::_MARKER)
    .value("DRIFT", GMAD::ElementType::_DRIFT)
    .value("RF", GMAD::ElementType::_RF)
    .value("SBEND", GMAD::ElementType::_SBEND)
    .value("QUAD", GMAD::ElementType::_QUAD)
    .value("SEXTUPOLE", GMAD::ElementType::_SEXTUPOLE)
    .value("OCTUPOLE", GMAD::ElementType::_OCTUPOLE)
    .value("DECAPOLE", GMAD::ElementType::_DECAPOLE)
    .value("MULT", GMAD::ElementType::_MULT)
    .value("SOLENOID", GMAD::ElementType::_SOLENOID)
    .value("LINE", GMAD::ElementType::_LINE)
    .value("REV_LINE", GMAD::ElementType::_REV_LINE)
    .value("ECOL", GMAD::ElementType::_ECOL)
    .value("RCOL", GMAD::ElementType::_RCOL)
    .value("LASER", GMAD::ElementType::_LASER)
    .value("MATERIAL", GMAD::ElementType::_MATERIAL)
    .value("RBEND", GMAD::ElementType::_RBEND)
    .value("ELEMENT", GMAD::ElementType::_ELEMENT)
    .value("SCREEN", GMAD::ElementType::_SCREEN)
    .value("AWAKESCREEN", GMAD::ElementType::_AWAKESCREEN)
    .value("AWAKESPECTROMETER", GMAD::ElementType::_AWAKESPECTROMETER)
    .value("SHIELD", GMAD::ElementType::_SHIELD)
    .value("VKICKER", GMAD::ElementType::_VKICKER)
    .value("HKICKER", GMAD::ElementType::_HKICKER)
    .value("KICKER", GMAD::ElementType::_KICKER)
    .value("TKICKER", GMAD::ElementType::_TKICKER)
    .value("TRANSFORM3D", GMAD::ElementType::_TRANSFORM3D)
    .value("MUONSPOILER", GMAD::ElementType::_MUONSPOILER)
    .value("DEGRADER", GMAD::ElementType::_DEGRADER)
    .value("THINMULT", GMAD::ElementType::_THINMULT)
    .value("GAP", GMAD::ElementType::_GAP)
    .value("THINRMATRIX", GMAD::ElementType::_THINRMATRIX)
    .value("PARALLELTRANSPORTER", GMAD::ElementType::_PARALLELTRANSPORTER)
    .value("RMATRIX", GMAD::ElementType::_RMATRIX)
    .value("CRYSTALCOL", GMAD::ElementType::_CRYSTALCOL)
    .value("UNDULATOR", GMAD::ElementType::_UNDULATOR)
    .value("JCOL", GMAD::ElementType::_JCOL)
    .value("WIRESCANNER", GMAD::ElementType::_WIRESCANNER)
    .value("USERCOMPONENT", GMAD::ElementType::_USERCOMPONENT)
    .value("DUMP", GMAD::ElementType::_DUMP)
    .value("CT", GMAD::ElementType::_CT)
    .value("TARGET", GMAD::ElementType::_TARGET)
    .value("RFX", GMAD::ElementType::_RFX)
    .value("RFY", GMAD::ElementType::_RFY);
}



