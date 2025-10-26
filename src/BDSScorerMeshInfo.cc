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
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSExtent.hh"
#include "BDSScorerMeshInfo.hh"
#include "BDSUtilities.hh"
#ifdef USE_BOOST
#include "BDSBH4DTypeDefs.hh"
#endif

#include "G4Types.hh"

#include "parser/scorermesh.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include <algorithm>
#include <fstream>
#include <iterator>
#include <vector>

BDSScorerMeshInfo::BDSScorerMeshInfo(const GMAD::ScorerMesh& mesh)
{
  name = G4String(mesh.name);
  geometryType = BDS::LowerCase(G4String(mesh.geometryType));
  nBinsX = mesh.nx;
  nBinsY = mesh.ny;
  nBinsZ = mesh.nz;
  nBinsR = mesh.nr;
  nBinsPhi = mesh.nphi;
  nBinsE = mesh.ne;

  if (geometryType == "box")
    {
      if (!BDS::IsFinite(mesh.xsize))
        {throw BDSException(__METHOD_NAME__, "xsize must be > 0 and finite in mesh \"" + mesh.name + "\"");}
      if (!BDS::IsFinite(mesh.ysize))
        {throw BDSException(__METHOD_NAME__, "ysize must be > 0 and finite in mesh \"" + mesh.name + "\"");}
      if (!BDS::IsFinite(mesh.zsize))
        {throw BDSException(__METHOD_NAME__, "zsize must be > 0 and finite in mesh \"" + mesh.name + "\"");}
      if (!BDS::IsFinite(nBinsX))
        {throw BDSException(__METHOD_NAME__, "nx must be > 0 and finite in mesh \"" + mesh.name + "\"");}
      if (!BDS::IsFinite(nBinsY))
        {throw BDSException(__METHOD_NAME__, "ny must be > 0 and finite in mesh \"" + mesh.name + "\"");}
      if (!BDS::IsFinite(nBinsZ))
        {throw BDSException(__METHOD_NAME__, "nz must be > 0 and finite in mesh \"" + mesh.name + "\"");}
    }
  else if (geometryType == "cylindrical")
   {
     if (!BDS::IsFinite(mesh.zsize))
       {throw BDSException(__METHOD_NAME__, "zsize must be > 0 and finite in mesh \"" + mesh.name + "\"");}
     if (!BDS::IsFinite(mesh.rOuter))
       {throw BDSException(__METHOD_NAME__, "rHigh must be > 0 and finite in mesh \"" + mesh.name + "\"");}
     if (!BDS::IsFinite(nBinsZ))
       {throw BDSException(__METHOD_NAME__, "nz must be > 0 and finite in mesh \"" + mesh.name + "\"");}
     if (!BDS::IsFinite(nBinsPhi))
       {throw BDSException(__METHOD_NAME__, "nphi must be > 0 and finite in mesh \"" + mesh.name + "\"");}
     if (!BDS::IsFinite(nBinsR))
       {throw BDSException(__METHOD_NAME__, "nr must be > 0 and finite in mesh \"" + mesh.name + "\"");}
   }

  xLow  = -0.5*mesh.xsize * CLHEP::m;
  xHigh =  0.5*mesh.xsize * CLHEP::m;
  yLow  = -0.5*mesh.ysize * CLHEP::m;
  yHigh =  0.5*mesh.ysize * CLHEP::m;
  zLow  = -0.5*mesh.zsize * CLHEP::m;
  zHigh =  0.5*mesh.zsize * CLHEP::m;
  rInner = mesh.rInner * CLHEP::m;
  rOuter = mesh.rOuter * CLHEP::m;
  eLow   =  mesh.eLow * CLHEP::GeV;
  eHigh  =  mesh.eHigh * CLHEP::GeV;
  eScale = mesh.eScale;

  extent = BDSExtent(xLow, xHigh,
                     yLow, yHigh,
                     zLow, zHigh);

  if (eScale == "user")
    {// In future we can move RBDS::BinLoader to a separate library and use that both here and in rebdsim
      std::string const BinsEdgesFile(mesh.eBinsEdgesFilenamePath);
      std::ifstream file(BinsEdgesFile.c_str());
        
      if (file)
        {
          // Reading of the bins edges file.
          std::istream_iterator<double> it(file);
          std::istream_iterator<double> end;
          std::back_insert_iterator<std::vector<double>> it2(eBinsEdges);
          
          std::copy(it, end, it2);
        }
      else
        {throw BDSException(__METHOD_NAME__, "eBinsEdgesFilenamePath must be the path to a .txt file");}
      
      nBinsE = (G4int)eBinsEdges.size()-1;
      eLow   = eBinsEdges[0];
      eHigh  = eBinsEdges[nBinsE];
    }
  
  if (nBinsE > 1)
    {
#ifdef USE_BOOST
      if (eScale == "linear")
        {energyAxis = new boost_histogram_linear_axis(nBinsE, eLow, eHigh, "energy");}
      else if (eScale == "log")
        {energyAxis = new boost_histogram_log_axis(nBinsE, eLow, eHigh, "energy");}
      else if (eScale == "user")
        {
          std::vector<double> eBinsEdgesEnergyAxis = eBinsEdges;
          std::for_each(eBinsEdgesEnergyAxis.begin(), eBinsEdgesEnergyAxis.end(), [](double& el){el *= CLHEP::GeV;});
          energyAxis = new boost_histogram_variable_axis(eBinsEdgesEnergyAxis, "energy");
        }
      else
        {throw BDSException(__METHOD_NAME__, "eScale must be 'linear', 'log' or 'user' in mesh \"" + mesh.name + "\"");}
#endif
    }
}
