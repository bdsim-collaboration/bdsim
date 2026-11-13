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
#ifndef BDSSAMPLERREGISTRY_H
#define BDSSAMPLERREGISTRY_H
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSSamplerPlacementRecord.hh"
#include "BDSSamplerType.hh"

#include "globals.hh" // geant4 types / globals
#include "G4Transform3D.hh"

#include <map>
#include <set>
#include <utility>
#include <vector>

class BDSBeamlineElement;
class BDSSampler;

/**
 * @brief Associated information for the placement of a sampler.
 *
 * This singleton registry stores the transform and S position for a
 * given sampler. This may simply be a placement of the same sampler object
 * again. It does not own the samplers. Registration of a sampler returns
 * an unique integer that should be used as the copy number for that placement
 * so that BDSSDSampler will find the correct information for the sampler registered
 * here.  Also, this integer is used for output purposes as a unique output identifier.
 * Note, if a sampler is placed in the real mass world inside a component it may not be 
 * known at construction time where this will lie in the world, therefore a default
 * identity transform and s position can be used signalling BDSSDSampler to look up
 * the transform dynamically. The BDSSampler* registered in each case isn't currently
 * used, so a nullptr can also be registered safely - in case BDSSDSampler is attached
 * to an arbitrary logical volume not part of a BDSSampler instance.
 *
 * This owns all BDSSampler instances.
 * 
 * @author Laurie Nevay
 */

class BDSSamplerRegistry
{
private:
  /// Typedefs up first so we can declare public iterators.
  typedef std::vector<BDSSamplerPlacementRecord> InfoVector;

  /// Storage of registered information.
  InfoVector infos;
  
public:
  /// Accessor for registry.
  static BDSSamplerRegistry* Instance();

  ~BDSSamplerRegistry();

  /// Register a sampler. This register an instance of a sampler
  /// (note could be same sampler object again and again) with a
  /// specific placement transform. The registration returns an
  /// integer that the sampler transform, name etc can be looked
  /// up in this registry by - ie by the output. The name does not
  /// need to be unique. This registry ensures the returned integer
  /// will be unique however. Default transform is unity - ie up
  /// to developer (outside registry) to test on this and determine
  /// transform externally as can't be known at construction time.
  /// The transform is the transform the sampler is placed with in
  /// the world volume, so the inverse is required to get global to
  /// local transformation. S is global s position with unphysical
  /// default of -1m.
  G4int RegisterSampler(const G4String&      name,
                        BDSSampler*          sampler,
                        const G4Transform3D& transform   = G4Transform3D(),
                        G4double             S            = -1000,
                        const BDSBeamlineElement* element = nullptr,
                        BDSSamplerType            type    = BDSSamplerType::plane,
                        G4double                  radius  = 0);

  G4int RegisterSampler(BDSSamplerPlacementRecord& info);

  ///@{ Iterator mechanics
  typedef InfoVector::iterator       iterator;
  typedef InfoVector::const_iterator const_iterator;
  iterator       begin()       {return infos.begin();}
  iterator       end()         {return infos.end();}
  const_iterator begin() const {return infos.begin();}
  const_iterator end()   const {return infos.end();}
  G4bool         empty() const {return infos.empty();}
  ///@}

  /// @{ Accessor
  inline G4String       GetName(G4int index) const;
  inline G4String       GetNameUnique(G4int index) const;
  inline BDSSampler*    GetSampler(G4int index) const;
  inline G4Transform3D  GetTransform(G4int index) const;
  inline G4Transform3D  GetTransformInverse(G4int index) const;
  inline G4double       GetSPosition(G4int index) const;
  inline const BDSSamplerPlacementRecord& GetInfo(G4int index) const {return infos.at(index);}
  inline G4int          GetBeamlineIndex(G4int index) const;
  /// @}

  /// Access all names at once
  std::vector<G4String> GetNames() const;

  /// Access all the unique names at once
  std::vector<G4String> GetUniqueNames() const;
  std::vector<G4String> GetUniqueNamesPlane() const;
  std::vector<G4String> GetUniqueNamesCylinder() const;
  std::vector<G4String> GetUniqueNamesSphere() const;
  
  /// @{ For output in standard C++ types. Also in m for units.
  std::map<std::string, double> GetUniqueNameToRadiusCylinder() const;
  std::map<std::string, double> GetUniqueNameToRadiusSphere() const;
  /// @}
  
  inline std::vector<G4int> GetSamplerIDsPlane() const    {return samplerIDsPerType.at(BDSSamplerType::plane);}
  inline std::vector<G4int> GetSamplerIDsCylinder() const {return samplerIDsPerType.at(BDSSamplerType::cylinder);}
  inline std::vector<G4int> GetSamplerIDsSphere() const   {return samplerIDsPerType.at(BDSSamplerType::sphere);}
  
  /// Access all the unique names and their corresponding s position at once.
  std::vector<std::pair<G4String, G4double> > GetUniquePlaneNamesAndSPosition() const;

  /// Get number of registered samplers
  inline G4int NumberOfExistingSamplers() const;
  inline size_t size() const;

  inline G4bool SafeIndex(G4int index) const {return index < (G4int)infos.size();}
  
private:
  /// Private constructor to enforce singleton pattern
  BDSSamplerRegistry();

  /// Singleton instance
  static BDSSamplerRegistry* instance;

  /// Counter for easy checking of out of bounds and incrementing.
  /// Also the index in the member vectors, so zero counting.
  G4int numberOfEntries;

  /// Store  the already added names to ensure that a sampler can
  /// also have a unique name for output purposes. Also store the number
  /// of times that name has been used.
  std::map<G4String, G4int> existingNames;

  /// Cache of unique sampler objects for memory management.
  std::set<BDSSampler*> samplerObjects;
  
  /// Map to reduce 'forward' sampler types to simple sampler types for keeping a record of IDs.
  std::map<BDSSamplerType, BDSSamplerType> samplerTypeToCategory;
  std::map<BDSSamplerType, std::vector<G4int>> samplerIDsPerType;
};

inline G4String BDSSamplerRegistry::GetName(G4int index) const
{return SafeIndex(index) ? infos.at(index).Name() : throw BDSException(__METHOD_NAME__, "invalid index");}

inline G4String BDSSamplerRegistry::GetNameUnique(G4int index) const
{return SafeIndex(index) ? infos.at(index).UniqueName() : throw BDSException(__METHOD_NAME__, "invalid index");}

inline BDSSampler* BDSSamplerRegistry::GetSampler(G4int index) const
{return SafeIndex(index) ? infos.at(index).Sampler() : throw BDSException(__METHOD_NAME__, "invalid index");}

inline G4Transform3D BDSSamplerRegistry::GetTransform(G4int index) const
{return SafeIndex(index) ? infos.at(index).Transform() : throw BDSException(__METHOD_NAME__, "invalid index");}

inline G4Transform3D BDSSamplerRegistry::GetTransformInverse(G4int index) const
{return SafeIndex(index) ? infos.at(index).TransformInverse() : throw BDSException(__METHOD_NAME__, "invalid index");}

inline G4double BDSSamplerRegistry::GetSPosition(G4int index) const
{return SafeIndex(index) ? infos.at(index).SPosition() : throw BDSException(__METHOD_NAME__, "invalid index");}

inline G4int BDSSamplerRegistry::NumberOfExistingSamplers() const
{return numberOfEntries;}

inline size_t BDSSamplerRegistry::size() const
{return infos.size();}

inline G4int BDSSamplerRegistry::GetBeamlineIndex(G4int index) const
{return SafeIndex(index) ? infos.at(index).BeamlineIndex() : throw BDSException(__METHOD_NAME__, "invalid index");}

#endif
