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
#include "BDSException.hh"
#include "BDSOutputROOTEventModel.hh"
#include "DataLoader.hh"
#include "Model.hh"

#include "TChain.h"
#include "TROOT.h"
#include "TTree.h"
#include "TSystem.h"

#include <iostream>
#include <stdexcept>

void FindNearestTest(Model* model, const TVector3& testPoint);

int main(int argc, char** argv)
{
  if (argc < 2 || argc > 2)
    {std::cout << "Incorrect agruments\nusage: BDSModelTreeTester <datafile>" << std::endl; return 1;}

  std::string dataFile = std::string(argv[1]);
  
  try
    {
      DataLoader* dl = new DataLoader(dataFile);
      TTree* modelTree = dl->GetModelTree();
      Model* model     = dl->GetModel();
      
      modelTree->GetEntry(0);

      // genuine nearest
      TVector3 testPoint1(1.0, 0.7, 2.3); // in m
      FindNearestTest(model, testPoint1);

      // should be last
      TVector3 testPoint2(1.0, 0.7, 400.5); // in m
      FindNearestTest(model, testPoint2);

      // should be first
      TVector3 testPoint3(1.0, 0.7, -504.5); // in m
      FindNearestTest(model, testPoint3);
    }
  catch (const BDSException& e)
    {std::cout << e.what() << std::endl;}
  catch (const std::exception& e)
    {std::cout << e.what() << std::endl;}
  std::cout << "End of test" << std::endl;
  std::_Exit(0);
}

// define the ostream operator because ROOT doesn't :(
std::ostream& operator<<(std::ostream& stream, const TVector3& v)
{
  stream << "(" << v.x() << ", " << v.y() << ", " << v.z() << ")";
  return stream;
}

void FindNearestTest(Model* model, const TVector3& testPoint)
{
  BDSOutputROOTEventModel* mod = model->model;
  int indexToNearest = mod->findNearestElement(testPoint);
  std::cout << "Index to nearest: " << indexToNearest << " which is \"" << mod->componentName[indexToNearest] << "\"" << std::endl;
  std::cout << "Target point: " << testPoint << std::endl;
  if (indexToNearest > 0)
    {std::cout << "Element before mid point " << mod->midRefPos[indexToNearest-1] << std::endl;}
  else
    {std::cout << "First component found - no previous point" << std::endl;}
  std::cout    << "Found element mid point  " << mod->midRefPos[indexToNearest] << std::endl;
  if (indexToNearest < (int)(mod->midRefPos.size()-2))
    {std::cout << "Element after mid point  " << mod->midRefPos[indexToNearest+1] << std::endl;}
  else
    {std::cout << "Last component found - no after point" << std::endl;}
}
