/* 
Beam Delivery Simulation (BDSIM) Copyright (C) BDSIM Collaboration, 2001 - 2026.

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
#include <cstdlib>
#include <iostream>
#include "getEnv.h"

std::string getEnv(const char* env)
{
  //Check if it is only spaces
  const char* tmp = std::getenv(env);
  bool spacesOnly=true;
  if(tmp){
    std::string test = tmp;
    spacesOnly =  test.find_first_not_of (' ') == test.npos;
#ifdef BDSDEBUG
    std::cout << __FUNCTION__ << "> getEnv - test = " << test << std::endl;
    std::cout << __FUNCTION__ << "> getEnv - spacesOnly =" << spacesOnly << std::endl;
#endif
  }
  if((tmp==nullptr) || (spacesOnly)){
#ifdef BDSDEBUG
    std::cout << __FUNCTION__ << "> getEnv - no environment variable \"" << env << "\" is set" << std::endl;
#endif
    return (const char*)"";
  } else {
    std::string stmp = (std::string)tmp+ "/";
#ifdef BDSDEBUG
    std::cout << __FUNCTION__ << "> getEnv - environment variable \"" << stmp << "\" is set" << std::endl;
#endif
    return stmp;
  }
}
