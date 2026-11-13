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
#ifndef PTCTFSFILE_H
#define PTCTFSFILE_H

#include <map>
#include <string>
#include <vector>


namespace PTC
{
  struct observation
  {
    int    index;
    double x;
    double px;
    double y;
    double py;
    double T;
    double pt;
    double s;
    double energy;
  };
    
  struct segment
  {
    std::string name;
    std::vector<PTC::observation> observations;

    inline size_t size() const {return observations.size();}
    inline int nObservations() const {return (int)observations.size();}
  };
  
class TfsFile
{
public:
  TfsFile(const std::string& fileNameIn);
  ~TfsFile(){;}

  void Load();

  std::string HeaderValue(const std::string& key) const;

  PTC::segment Segment(const int& index) const;

  ///@{ Iterator mechanics
  typedef std::vector<PTC::segment>::iterator iterator;
  typedef std::vector<PTC::segment>::const_iterator const_iterator;
  iterator       begin()       {return segments.begin();}
  iterator       end()         {return segments.end();}
  const_iterator begin() const {return segments.begin();}
  const_iterator end()   const {return segments.end();}
  bool           empty() const {return segments.empty();}
  ///@}

  inline size_t size() const {return segments.size();}

  const std::string                  fileName;
  std::map<std::string, std::string> header;
  std::vector<std::string>           columnNames;
  std::vector<PTC::segment>          segments;

private:

  void ParseHeaderLine(const std::string& line);
  void ParseColumns(const std::string& line);
  void ParseSegment(const std::string& line);
  void ParseData(const std::string& line);

  std::vector<std::string> BreakOnWhiteSpace(const std::string& line) const;

  int currentSegmentNumber;
  int observationCounter;
  int nObservationsThisSegment;
};
}

#endif
