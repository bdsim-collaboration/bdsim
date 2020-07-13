//
// Created by Eliott Ramoisiaux on 05/06/2020.
//

#ifndef BDSBH4D_HH
#define BDSBH4D_HH

#include <boost/histogram.hpp>

#include "Rtypes.h"
#include "TObject.h"

typedef boost::histogram::histogram<std::__1::tuple<boost::histogram::axis::regular<double, boost::use_default, boost::use_default, boost::use_default>, boost::histogram::axis::regular<double, boost::use_default, boost::use_default, boost::use_default>, boost::histogram::axis::regular<double, boost::use_default, boost::use_default, boost::use_default>, boost::histogram::axis::regular<double, boost::histogram::axis::transform::log, boost::use_default, boost::use_default> >, boost::histogram::storage_adaptor<std::__1::vector<double, std::__1::allocator<double> > > > boost_histogram;

class BDSBH4D : public TObject {

public:
    BDSBH4D();
    BDSBH4D(unsigned int nxbins, double xmin, double xmax,
            unsigned int nybins, double ymin, double ymax,
            unsigned int nzbins, double zmin, double zmax,
            unsigned int nebins, double emin, double emax);
    virtual ~BDSBH4D() {;}

    boost_histogram h;
    unsigned int h_nxbins;
    unsigned int h_nybins;
    unsigned int h_nzbins;
    unsigned int h_nebins;
    double h_xmin;
    double h_xmax;
    double h_ymin;
    double h_ymax;
    double h_zmin;
    double h_zmax;
    double h_emin;
    double h_emax;

ClassDef(BDSBH4D,1);

};


#endif //BDSBH4D_HH
