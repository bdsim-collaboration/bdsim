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
 /*
 * GMAD interface
 *
 */
#include "parser.h"

#include <exception>
#include <cstdio>
#include <iostream>

using namespace GMAD;

int main(int argc, char *argv[])
{
  if(argc<2)
    {
      std::cout << "GMAD parser needs an input file" << std::endl;
      return 1;
    }
  if(argc>2)
    {
      std::cout << "GMAD parser needs only one input file" << std::endl;
      return 1;
    }
  try
    {Parser::Instance(std::string(argv[1]));}
  catch (std::exception& e)
    {std::cerr << e.what() << std::endl; return 1;}
  return 0;
}

