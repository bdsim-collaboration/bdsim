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
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSInterpolatorRoutines.hh"
#include "BDSPolygon.hh"
#include "BDSTube.hh"

#include "G4TessellatedSolid.hh"
#include "G4ThreeVector.hh"
#include "G4TriangularFacet.hh"
#include "G4TwoVector.hh"
#include "G4Types.hh"
#include "G4VFacet.hh" // defines G4FacetVertexType

#include <string>
#include <vector>

BDSTube::BDSTube(const G4String&   nameIn,
                 G4double          fullLengthIn,
                 const BDSPolygon& startingShape):
  BDSTube(nameIn, fullLengthIn, startingShape.Points(), startingShape.Points())
{;}

BDSTube::BDSTube(const G4String&   nameIn,
                 G4double          fullLengthIn,
                 const BDSPolygon& startingShape,
                 const BDSPolygon& finishingShape,
                 unsigned int      numberOfSuggestedZSections):
  BDSTube(nameIn, fullLengthIn, startingShape.Points(), finishingShape.Points(), numberOfSuggestedZSections)
{;}

BDSTube::BDSTube(const G4String& nameIn,
                 G4double        fullLengthIn,
                 const std::vector<G4TwoVector>& startingPoints):
  BDSTube(nameIn, fullLengthIn, startingPoints, startingPoints)
{;}

BDSTube::BDSTube(const G4String& nameIn,
                 G4double        fullLengthIn,
                 const std::vector<G4TwoVector>& startingPoints,
                 const std::vector<G4TwoVector>& finishingPoints,
                 unsigned int    numberOfSuggestedZSections):
  BDSTube(nameIn, -0.5*fullLengthIn, 0.5*fullLengthIn, startingPoints, finishingPoints, numberOfSuggestedZSections)
{;}

BDSTube::BDSTube(const G4String& nameIn,
                 G4double        dZNegative,
                 G4double        dZPositive,
                 const std::vector<G4TwoVector>& startingPoints,
                 const std::vector<G4TwoVector>& finishingPoints,
                 unsigned int    numberOfSuggestedZSections):
  G4TessellatedSolid(nameIn)
{
#ifdef BDSDEBUG
  G4cout << "BDSTube " << nameIn << " dZneg: " << dZNegative << " dZpos: " << dZPositive << G4endl;
  G4cout << "Incoming polygon: ";
  for (auto p : startingPoints)
    {G4cout << "(" << p.x() << "," << p.y() << ")";}
  G4cout << G4endl;
  for (auto p : finishingPoints)
    {G4cout << "(" << p.x() << "," << p.y() << ")";}
  G4cout << G4endl;
#endif

  // checks
  // full length must be > 0
  if (dZPositive <= dZNegative)
    {throw BDSException(__METHOD_NAME__, "length <= 0");}
  if (startingPoints.size() < 3)
    {throw BDSException(__METHOD_NAME__, "too few points (" + std::to_string(startingPoints.size()) + ")");}
  if (finishingPoints.size() < 3)
    {throw BDSException(__METHOD_NAME__, "too few points (" + std::to_string(finishingPoints.size()) + ")");}

  unsigned int spSize = (unsigned int)startingPoints.size();
  unsigned int fpSize = (unsigned int)finishingPoints.size();
  if (spSize != fpSize)
    {
      if (spSize % fpSize == 0 || fpSize % spSize == 0)
        {
          if (numberOfSuggestedZSections > 2)
            {throw BDSException(__METHOD_NAME__, "cannot use multiple z-sections with non 1:1 points.");}
          SubMultipleConstruction(dZNegative, dZPositive, startingPoints, finishingPoints);}
      else
        {throw BDSException(__METHOD_NAME__, "mismatched sizes of points.");}
    }
  else
    {
      // if rotation between points > 90deg, add 1 z plane per 90deg - TODO
      std::vector<G4double> z;
      G4double dzNorm = 1.0 / ((G4double)numberOfSuggestedZSections - 1.0);
      for (unsigned int i = 0; i < numberOfSuggestedZSections; i++)
        {
          G4double t = (G4double)i * dzNorm;
          G4double points[2] = {dZNegative,dZPositive}; // TBC
          z.push_back(BDS::Linear1D(points, t));
        }
      RegularConstruction(z, startingPoints, finishingPoints);   
    }
}

void BDSTube::RegularConstruction(const std::vector<G4double>&    z,
                                  const std::vector<G4TwoVector>& startingPoints,
                                  const std::vector<G4TwoVector>& finishingPoints)
{
  if (z.size() < 2)
    {throw BDSException(__METHOD_NAME__, "must be at least 2 z positions.");}
  
  // shortcuts / aliases
  const std::vector<G4TwoVector>& sp = startingPoints;
  const std::vector<G4TwoVector>& fp = finishingPoints;
  typedef G4ThreeVector G4TV;
  
  G4double dzNorm = 1.0 / ((G4double)z.size() - 1.0); // here we assume even spacing
  
  for (G4int zi = 0; zi < (G4int)z.size() - 1; zi++)
    {
      G4double za = z[zi];
      G4double zb = z[zi+1];
      
      G4int limit = (G4int)sp.size();
      for (G4int i = 0; i < limit; i++)
        {
          G4int j = i + 1;
          if (j == limit)
            {j = 0;}

          G4TwoVector ii[2] = {sp[i],fp[i]};
          G4TwoVector jj[2] = {sp[j],fp[j]};
          G4TwoVector si = BDS::Linear1D(ii, (G4double)zi   * dzNorm);
          G4TwoVector sj = BDS::Linear1D(jj, (G4double)zi   * dzNorm);
          G4TwoVector fi = BDS::Linear1D(ii, ((G4double)zi+1) * dzNorm);
          G4TwoVector fj = BDS::Linear1D(jj, ((G4double)zi+1) * dzNorm);
          
          G4TriangularFacet* facet1 = new G4TriangularFacet(G4TV(si.x(), si.y(), za),
                                                            G4TV(sj.x(), sj.y(), za),
                                                            G4TV(fi.x(), fi.y(), zb),
                                                            G4FacetVertexType::ABSOLUTE);
          G4TriangularFacet* facet2 = new G4TriangularFacet(G4TV(sj.x(), sj.y(), za),
                                                            G4TV(fj.x(), fj.y(), zb),
                                                            G4TV(fi.x(), fi.y(), zb),
                                                            G4FacetVertexType::ABSOLUTE);
          G4TriangularFacet* facetF = new G4TriangularFacet(G4TV(0,      0,      za),
                                                            G4TV(sj.x(), sj.y(), za),
                                                            G4TV(si.x(), si.y(), za),
                                                            G4FacetVertexType::ABSOLUTE);
          G4TriangularFacet* facetB = new G4TriangularFacet(G4TV(0,      0,      zb),
                                                            G4TV(fi.x(), fi.y(), zb),
                                                            G4TV(fj.x(), fj.y(), zb),
                                                            G4FacetVertexType::ABSOLUTE);
          AddFacet(facet1);
          AddFacet(facet2);
          AddFacet(facetF);
          AddFacet(facetB);
        }
    }
  SetSolidClosed(true);
}

void BDSTube::SubMultipleConstruction(G4double dZNegative,
                                      G4double dZPositive,
                                      const std::vector<G4TwoVector>& startingPoints,
                                      const std::vector<G4TwoVector>& finishingPoints)
{
  // shortcuts / aliases
  const std::vector<G4TwoVector>& sp = startingPoints;
  const std::vector<G4TwoVector>& fp = finishingPoints;
  typedef G4ThreeVector G4TV;
  G4double za = dZNegative;
  G4double zb = dZPositive;
  
  unsigned int sn = (unsigned int)sp.size();
  unsigned int fn = (unsigned int)fp.size();
  G4bool endHasMore = sn > fn;
  if (endHasMore)
    {
      unsigned int factor = fn / sn;
      for (unsigned int si = 0; si < sn; si++)
        {// for each starting point (fewer)
          for (unsigned int fi = 0; fi < factor; fi++)
            {// one triangle from starting point to each end point multiple
              // The tessellated solid owns the facets.
              G4TriangularFacet* f = new G4TriangularFacet(G4TV(sp[si].x(),      sp[si].y(),      za),
                                                           G4TV(fp[si+fi].x(),   fp[si+fi].y(),   zb),
                                                           G4TV(fp[si+fi+1].x(), fp[si+fi+1].y(), zb),
                                                           G4FacetVertexType::ABSOLUTE);
              AddFacet(f);
            }
        }
    }
  else
    {// start has more points
      unsigned int factor = sn / fn;
      for (unsigned int fi = 0; fi < fn; fi++)
        {// for each finishing point (fewer)
          for (unsigned int si = 0; si < factor; si++)
            {// one triangle from finishing point to each start point multiple
              G4TriangularFacet* f = new G4TriangularFacet(G4TV(fp[fi].x(),      fp[fi].y(),      zb),
                                                           G4TV(sp[fi+si+1].x(), sp[fi+si+1].y(), za),
                                                           G4TV(sp[fi+si].x(),   sp[fi+si].y(),   za),
                                                           G4FacetVertexType::ABSOLUTE);
              AddFacet(f);
            }
        }
    }

  // start face polygons
  for (unsigned int i = 0; i < sn; i++)
    {
      unsigned int j = i + 1;
      if (j == sn)
        {j = 0;}
      G4TriangularFacet* f = new G4TriangularFacet(G4TV(0,         0,         za),
                                                   G4TV(sp[j].x(), sp[j].y(), za),
                                                   G4TV(sp[i].x(), sp[i].y(), za),
                                                   G4FacetVertexType::ABSOLUTE);
      AddFacet(f);
    }

  // ending face polygons
  for (unsigned int i = 0; i < fn; i++)
    {
      unsigned int j = i + 1;
      if (j == fn)
        {j = 0;}
      G4TriangularFacet* f = new G4TriangularFacet(G4TV(0,         0,         zb),
                                                   G4TV(fp[i].x(), fp[i].y(), zb),
                                                   G4TV(fp[j].x(), fp[j].y(), zb),
                                                   G4FacetVertexType::ABSOLUTE);
      AddFacet(f);
    }
  
  SetSolidClosed(true);
}
