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
#include "physicsbiasing.h"

#include <iomanip>
#include <iostream>
#include <sstream>

#include "array.h"

using namespace GMAD;

PhysicsBiasing::PhysicsBiasing(){
  PublishMembers();
}

void PhysicsBiasing::PublishMembers()
{
  publish("name",  &PhysicsBiasing::name);
  publish("particle", &PhysicsBiasing::particle);
  publish("process", &PhysicsBiasing::process);
  publish("processList", &PhysicsBiasing::processList);
  publish("factor", &PhysicsBiasing::factor);
  publish("flag", &PhysicsBiasing::flag);
}

void PhysicsBiasing::clear()
{
  name = "";
  particle = "";
  process = "";
  processList.clear();
  factor.clear();
  flag.clear();
}

void PhysicsBiasing::print() const
{
  std::cout << "physicsbiasing: " << std::endl
	    << "name "     << name     << " " << std::endl
	    << "particle " << particle << " " << std::endl
	    << "process "  << process  << " " << std::endl
	    << "factor ";  
  for (const auto& i : factor) std::cout << i << " ";
  std::cout << std::endl << "flag ";
  for (const auto& i : flag) std::cout << static_cast<int>(i) << " ";
  std::cout << std::endl;
}

void PhysicsBiasing::set_value(const std::string& property, double value, bool bExit)
{
#ifdef BDSDEBUG
  std::cout << "parser> Setting value " << std::setw(25) << std::left << property << value << std::endl;
#endif

  if (property=="flag") {flag.push_back(static_cast<PhysicsBiasingType>((int)value)); return;}
  if (property=="xsecfact") {factor.push_back(value); return;}
  
  std::cerr << "Error: parser> unknown physicsbiasing option \"" << property << "\" with value " << value << std::endl; 
  if(bExit)
    exit(1);
  else
    std::rethrow_exception(std::current_exception());
}

void PhysicsBiasing::set_value(const std::string& property, Array* value, bool bExit)
{
  if (property=="flag")
    {
      for (const auto& i : value->GetData())
	{flag.push_back(static_cast<PhysicsBiasingType>((int)i));}
    }
  else if (property=="xsecfact")
    {value->set_vector(factor);}
  else if (property=="factor")
    {value->set_vector(factor);}
  else if (property=="processList")
    {value->set_vector(processList);}
  else
    {
      std::cerr << "Error: parser> unknown physicsbiasing option \"" << property << "\" with value ";
      for (const auto& i : value->GetData())
        {std::cout << i << " ";}

      std::cout << std::endl;
      if(bExit)
        exit(1);
      else
        std::rethrow_exception(std::current_exception());
    }
}

void PhysicsBiasing::set_value(const std::string& property, std::string value, bool bExit)
{
#ifdef BDSDEBUG
  std::cout << "parser> Setting value " << std::setw(25) << std::left << property << value << std::endl;
#endif

  if (property=="name")
    {name     = value;}
  else if (property=="particle")
    {particle = value;}
  else if ((property=="proc") || (property=="process"))
    {
      process = value; 
      std::stringstream ss(process);
      std::string tok;
      while(ss >> tok)
        {processList.push_back(tok);}
      return;
    }
  else
    {
      std::cerr << "Error: parser> unknown physicsbiasing option \"" << property
                << "\" with value " << value << std::endl;
      if(bExit)
        exit(1);
      else
        std::rethrow_exception(std::current_exception());
    }
}
