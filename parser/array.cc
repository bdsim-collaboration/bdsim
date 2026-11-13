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
#include "array.h"

#include "sym_table.h"

#include <iostream>

using namespace GMAD;

Array::Array()
{
}

Array::Array(Symtab* array)
{
  for(double it : array->array)
    {
      data.push_back(it);
    }
}

Array::Array(unsigned int n)
{
  data.resize(n);
}

Array* Array::Add(Array* a1, Array* a2)
{
  unsigned int size = std::min(a1->data.size(),a2->data.size());
  Array* a = new Array(size);
  for(unsigned int i=0;i<size;i++)
    {
      a->data[i] = a1->data[i] + a2->data[i];
    }
  return a;
}

Array* Array::Subtract(Array* a1, Array* a2)
{
  unsigned int size = std::min(a1->data.size(),a2->data.size());
  Array* a = new Array(size);
  for(unsigned int i=0;i<size;i++)
    {
      a->data[i] = a1->data[i] - a2->data[i];
    }
  return a;
}

Array* Array::Add(Array* a1, double d2)
{
  unsigned int size = a1->data.size();
  Array* a = new Array(size);
  for(unsigned int i=0;i<size;i++)
    {
      a->data[i] = a1->data[i] + d2;
    }
  return a;
}

Array* Array::Subtract(Array* a1, double d2)
{
  unsigned int size = a1->data.size();
  Array* a = new Array(size);
  for(unsigned int i=0;i<size;i++)
    {
      a->data[i] = a1->data[i] - d2;
    }
  return a;
}

Array* Array::Multiply(Array* a1, double d2)
{
  unsigned int size = a1->data.size();
  Array* a = new Array(size);
  for(unsigned int i=0;i<size;i++)
    {
      a->data[i] = a1->data[i] * d2;
    }
  return a;
}

Array* Array::Divide(Array* a1, double d2)
{
  unsigned int size = a1->data.size();
  Array* a = new Array(size);
  for(unsigned int i=0;i<size;i++)
    {
      a->data[i] = a1->data[i] / d2;
    }
  return a;
}

double Array::Product(Array* a)
{
  if(data.size() != a->data.size())
    {
      std::cerr << "ERROR: vector dimensions do not match" << std::endl;
      exit(1);
    }

  double dummy = 0;
  for(unsigned int i=0;i<data.size();i++)
    dummy += data[i] * a->data[i];
  return dummy;
}

void Array::Clear()
{
  data.clear();
  symbols.clear();
}

void Array::Print()
{
  for(double d : data)
    {
      std::cout << d << " ";
    }
  std::cout << std::endl;
  for(std::string s : symbols)
    {
      std::cout << s << " ";
    }
  std::cout << std::endl;
}

std::list<std::string> Array::GetSymbolsList() const
{
  std::list<std::string> r;
  for (const std::string& s : symbols)
    {r.push_back(s);}
  return r;
}

std::list<double> Array::GetDataList() const
{
  std::list<double> r;
  for (double d : data)
    {r.push_back(d);}
  return r;
}
