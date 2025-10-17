#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
namespace py = pybind11;

#include "fastlist.h"

#include "atom.h"
#include "aperture.h"
#include "blmplacement.h"
#include "cavitymodel.h"
#include "crystal.h"
#include "element.h"
#include "field.h"
#include "material.h"
#include "modulator.h"
#include "newcolour.h"
#include "physicsbiasing.h"
#include "placement.h"
#include "query.h"
#include "region.h"
#include "samplerplacement.h"
#include "scorer.h"
#include "scorermesh.h"
#include "tunnel.h"

PYBIND11_MAKE_OPAQUE(GMAD::FastList<GMAD::Atom>);

PYBIND11_MODULE(fastlist, m) {

  py::class_<GMAD::FastList<GMAD::Aperture>>(m,"FastList_Aperture")
    .def(py::init<>())
    .def("__iter__", [](const GMAD::FastList<GMAD::Aperture> &s)
    { return py::make_iterator(s.begin(), s.end()); },
    py::keep_alive<0,1>());


  py::class_<GMAD::FastList<GMAD::Atom>>(m,"FastList_Atom")
    .def(py::init<>())
    .def("__iter__", [](const GMAD::FastList<GMAD::Atom> &s)
                        { return py::make_iterator(s.begin(), s.end()); },
                        py::keep_alive<0,1>());

  py::class_<GMAD::FastList<GMAD::BLMPlacement>>(m,"FastList_BLMPlacement")
    .def(py::init<>())
    .def("__iter__", [](const GMAD::FastList<GMAD::BLMPlacement> &s)
    { return py::make_iterator(s.begin(), s.end()); },
    py::keep_alive<0,1>());

  py::class_<GMAD::FastList<GMAD::CavityModel>>(m,"FastList_CavityModel")
    .def(py::init<>())
    .def("__iter__", [](const GMAD::FastList<GMAD::CavityModel> &s)
    { return py::make_iterator(s.begin(), s.end()); },
    py::keep_alive<0,1>());

  py::class_<GMAD::FastList<GMAD::Crystal>>(m,"FastList_Crystal")
    .def(py::init<>())
    .def("__iter__", [](const GMAD::FastList<GMAD::Crystal> &s)
    { return py::make_iterator(s.begin(), s.end()); },
    py::keep_alive<0,1>());

  py::class_<GMAD::FastList<GMAD::Element>>(m,"FastList_Element")
    .def(py::init<>())
    .def("insert",[](GMAD::FastList<GMAD::Element> &fastlist,
                     GMAD::FastList<GMAD::Element>::FastListIterator position,
                     const GMAD::Element val)
                     {fastlist.insert(position, val);})
    .def("insert",[](GMAD::FastList<GMAD::Element> &fastlist,
                     GMAD::FastList<GMAD::Element>::FastListConstIterator position,
                     const GMAD::Element val)
                     {fastlist.insert(position, val);})
    .def("insert",[](GMAD::FastList<GMAD::Element> &fastlist,
                     GMAD::FastList<GMAD::Element>::FastListIterator position,
                     GMAD::FastList<GMAD::Element>::FastListIterator first,
                     GMAD::FastList<GMAD::Element>::FastListIterator last)
                     {fastlist.insert(position, first, last);})
    .def("insert",[](GMAD::FastList<GMAD::Element> &fastlist,
                     GMAD::FastList<GMAD::Element>::FastListConstIterator position,
                     GMAD::FastList<GMAD::Element>::FastListConstIterator first,
                     GMAD::FastList<GMAD::Element>::FastListConstIterator last)
                     {fastlist.insert(position, first, last);})
    .def("insert_before",[](GMAD::FastList<GMAD::Element> &fastlist,
                            std::string& name,
                            GMAD::Element &el)
                            {fastlist.insert_before(name,el);})
    .def("push_back",[](GMAD::FastList<GMAD::Element> &fastlist,
                        GMAD::Element el,
                        bool unique=false,
                        const std::string &objectName="element")
                        { fastlist.push_back(el,unique,objectName);})
    .def("empty",&GMAD::FastList<GMAD::Element>::empty)
    .def("size",&GMAD::FastList<GMAD::Element>::size)
    .def("clear",&GMAD::FastList<GMAD::Element>::clear)
    .def("erase",[](GMAD::FastList<GMAD::Element> &fastlist){fastlist.erase();})
    // erase
    // erase
    .def("begin",[](GMAD::FastList<GMAD::Element> &fastlist) {return fastlist.begin();})
    .def("end",[](GMAD::FastList<GMAD::Element> &fastlist) {return fastlist.end();})
    .def("getVector",&GMAD::FastList<GMAD::Element>::getVector)
    .def("__iter__", [](GMAD::FastList<GMAD::Element> &fastlist) { return py::make_iterator(fastlist.begin(), fastlist.end());});

  py::class_<GMAD::FastList<GMAD::Element>::FastListIterator>(m,"FastList_FastListIterator")
    .def("__eq__",[](GMAD::FastList<GMAD::Element>::FastListIterator &i1,
                     GMAD::FastList<GMAD::Element>::FastListIterator &i2)
                     {return i1 == i2;})
    .def("__ne__",[](GMAD::FastList<GMAD::Element>::FastListIterator &i1,
                     GMAD::FastList<GMAD::Element>::FastListIterator &i2)
                     {return i1 != i2;})
    .def("object", [](GMAD::FastList<GMAD::Element>::FastListIterator &i1) {return *i1;})
    .def("next", [](GMAD::FastList<GMAD::Element>::FastListIterator &i1) {i1++;})
    .def("prev", [](GMAD::FastList<GMAD::Element>::FastListIterator &i1) {i1--;});

  py::class_<GMAD::FastList<GMAD::Element>::FastListConstIterator>(m,"FastList_Element_FastListConstIterator");
  py::class_<GMAD::FastList<GMAD::Element>::FastMapIterator>(m,"FastList_Element_FastMapIterator");
  py::class_<GMAD::FastList<GMAD::Element>::FastMapConstIterator>(m,"FastList_Element_FastMapConstIterator");
  py::class_<GMAD::FastList<GMAD::Element>::FastMapIteratorPair>(m,"FastList_Element_FastMapIteratorPair");
  py::class_<GMAD::FastList<GMAD::Element>::FastMapConstIteratorPair>(m,"FastList_Element_FastMapConstIteratorPair");

  py::class_<GMAD::FastList<GMAD::Field>>(m,"FastList_Field")
    .def(py::init<>())
    .def("__iter__", [](const GMAD::FastList<GMAD::Field> &s)
    { return py::make_iterator(s.begin(), s.end()); },
    py::keep_alive<0,1>());

  py::class_<GMAD::FastList<GMAD::Material>>(m,"FastList_Material")
    .def(py::init<>())
    .def("__iter__", [](const GMAD::FastList<GMAD::Material> &s)
    { return py::make_iterator(s.begin(), s.end()); },
    py::keep_alive<0,1>());

  py::class_<GMAD::FastList<GMAD::Modulator>>(m,"FastList_Modulator")
    .def(py::init<>())
    .def("__iter__", [](const GMAD::FastList<GMAD::Modulator> &s)
    { return py::make_iterator(s.begin(), s.end()); },
    py::keep_alive<0,1>());

  py::class_<GMAD::FastList<GMAD::NewColour>>(m,"FastList_NewColour")
    .def(py::init<>())
    .def("__iter__", [](const GMAD::FastList<GMAD::NewColour> &s)
    { return py::make_iterator(s.begin(), s.end()); },
    py::keep_alive<0,1>());

  py::class_<GMAD::FastList<GMAD::PhysicsBiasing>>(m,"FastList_PhysicsBiasing")
    .def(py::init<>())
    .def("__iter__", [](const GMAD::FastList<GMAD::PhysicsBiasing> &s)
    { return py::make_iterator(s.begin(), s.end()); },
    py::keep_alive<0,1>());

  py::class_<GMAD::FastList<GMAD::Placement>>(m,"FastList_Placement")
    .def(py::init<>())
    .def("__iter__", [](const GMAD::FastList<GMAD::Placement> &s)
    { return py::make_iterator(s.begin(), s.end()); },
    py::keep_alive<0,1>());

  py::class_<GMAD::FastList<GMAD::Query>>(m,"FastList_Query")
    .def(py::init<>())
    .def("__iter__", [](const GMAD::FastList<GMAD::Query> &s)
    { return py::make_iterator(s.begin(), s.end()); },
    py::keep_alive<0,1>());

  py::class_<GMAD::FastList<GMAD::Region>>(m,"FastList_Region")
   .def(py::init<>())
   .def("__iter__", [](const GMAD::FastList<GMAD::Region> &s)
   { return py::make_iterator(s.begin(), s.end()); },
   py::keep_alive<0,1>());

  py::class_<GMAD::FastList<GMAD::SamplerPlacement>>(m,"FastList_SamplerPlacement")
    .def(py::init<>())
    .def("__iter__", [](const GMAD::FastList<GMAD::SamplerPlacement> &s)
    { return py::make_iterator(s.begin(), s.end()); },
    py::keep_alive<0,1>());

  py::class_<GMAD::FastList<GMAD::Scorer>>(m,"FastList_Scorer")
    .def(py::init<>())
    .def("__iter__", [](const GMAD::FastList<GMAD::Scorer> &s)
    { return py::make_iterator(s.begin(), s.end()); },
    py::keep_alive<0,1>());

  py::class_<GMAD::FastList<GMAD::ScorerMesh>>(m,"FastList_ScorerMesh")
    .def(py::init<>())
    .def("__iter__", [](const GMAD::FastList<GMAD::ScorerMesh> &s)
    { return py::make_iterator(s.begin(), s.end()); },
    py::keep_alive<0,1>());

  py::class_<GMAD::FastList<GMAD::Tunnel>>(m,"FastList_Tunnel")
    .def(py::init<>())
    .def("__iter__", [](const GMAD::FastList<GMAD::Tunnel> &s)
    { return py::make_iterator(s.begin(), s.end()); },
    py::keep_alive<0,1>());

}