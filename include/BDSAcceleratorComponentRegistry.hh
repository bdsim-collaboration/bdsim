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
#ifndef BDSACCELERATORCOMPONENTREGISTRY_H
#define BDSACCELERATORCOMPONENTREGISTRY_H

#include "globals.hh" // geant4 globals / types

#include <functional>  // for all the std::hash business
#include <iterator>
#include <map>
#include <set>
#include <string>
#include <unordered_map>

class BDSAcceleratorComponent;

/**
 * @brief custom key class for pair of <name,rigidity>.
 * 
 * We want to store a map of (name,rigidity) but compare rigidity
 * with a numerical tolerance as it's a double. std::map requires
 * the less than operator so we'll use unordered_map that uses ==.
 * 
 * This class is a pair but with == specialised. Also, we provide
 * a hash function (specialisation for the template).
 * 
 * @author Laurie Nevay
 */
class ACRegistryKey
{
public:
  ACRegistryKey();
  ACRegistryKey(const std::string& componentNameIn, G4double rigidityIn);
  ~ACRegistryKey() = default;
  ACRegistryKey(const ACRegistryKey& other) = default;
  bool operator==(const ACRegistryKey& other) const;
  friend std::ostream& operator<<(std::ostream &out, ACRegistryKey const &k);
  
  std::string componentName; ///< Has to be std::string for hashing as G4String doesn't provide one.
  G4double rigidity;
  static G4double tolerance;
};

/// Specialise the hash function for our key to use it in std::unordered_map
namespace std
{
  template <>
  struct hash<ACRegistryKey>
  {
    std::size_t operator()(const ACRegistryKey& k) const
    {
      std::size_t h1 = std::hash<std::string>()(k.componentName);
      std::size_t h2 = std::hash<double>()(k.rigidity);
      std::size_t ht = h1 ^ (h2 + 0x9e3779b9 + (h1<<6) + (h1>>2)); ///< Boost's algorithm.
      return ht;
    }
  };
}

/**
 * @brief A registry of constructed BDSAcceleratorComponent instances that
 * can be searched. 
 * 
 * Uses an std::map rather than unordered map as although 
 * slower to access (less often and only at construction), a map is faster 
 * for iterating than an unordered map, which will be required to apply 
 * wrapper physics processes - relatively common.
 *
 * NOTE, the iterations of the registry applies only to components that can
 * be reused. The AllAllocatedComponents function should be used to access
 * all components held in memory in the registry.
 * 
 * @author Laurie Nevay
 */

class BDSAcceleratorComponentRegistry
{
private:
  // Typedefs up first so we can declare public iterators.
  /// Use a typedef for this specific map implementation so we can easily
  /// define iterators and internal member variables without risking getting
  /// the exact map declaration wrong. 
  typedef std::unordered_map<ACRegistryKey, BDSAcceleratorComponent*> RegistryMap;

  /// Registry is a map - note 'register' is a protected keyword.
  RegistryMap registry;

  /// A map for absolutely everything including components that are unique.
  RegistryMap registryForAllocated;
  
public:
  /// Singleton accessor
  static BDSAcceleratorComponentRegistry* Instance();

  /// Destructor resets instance to 0 so that the class can be reinstantiated
  /// if necessary.
  ~BDSAcceleratorComponentRegistry();

  /// Register a BDSAcceleratorComponent. Once in the registry, can be reused
  /// through GetComponent function. Optional flag isModified indicates that the
  /// component was modified beyond its original parser definition (ie likely to
  /// match the face of another component) and should not be reused even if it
  /// appears later in the lattice. In this case, it is stored here, purely for
  /// memory management. Note, the registry of allocated components relies on
  /// unique naming for unique components.  ie sb1_mod_0 and sb1_mod_1.
  void RegisterComponent(BDSAcceleratorComponent* component,
                         G4double                 rigidtyAtConstructionTime,
                         bool                     isModified = false);

  /// Check whether an accelerator component is already registered.
  G4bool IsRegistered(BDSAcceleratorComponent* component, G4double rigidtyAtConstructionTime);

  /// Check whether an accelerator component is already registered by name.
  G4bool IsRegistered(const G4String& componentName, G4double rigidtyAtConstructionTime);

  /// Check if a unique component is registered in the allocatedComponents.
  G4bool IsRegisteredAllocated(const BDSAcceleratorComponent* componentName) const;

  /// Access an already constructed component - will return null if no such component found.
  /// This is safe as this registry is primarily used by BDSComponentFactory which can return
  /// nullptr to BDSDetectorConstruction safely if an invalid component is requested.
  BDSAcceleratorComponent* GetComponent(const G4String& name, G4double rigidtyAtConstructionTime);

  /// Register a curvilinear component - purely to keep track of and delete at the
  /// end of the program.
  void RegisterCurvilinearComponent(BDSAcceleratorComponent* component);

  /// Register a tunnel component - purely to keep track of and delete at the end
  /// of the program.
  void RegisterTunnelComponent(BDSAcceleratorComponent* component);

  /// Access a map of all accelerator components by name, including ones that are
  /// uniquely built and stored only for memory management.
  std::unordered_map<ACRegistryKey, BDSAcceleratorComponent*> AllComponentsIncludingUnique() const;
  
  G4int AlreadyRegisteredNameCount(const G4String& name) const;

  /// NOTE the iterator works only over components that can be reused and not allocated
  /// components.
  /// @{ Iterator mechanics
  typedef RegistryMap::iterator       iterator;
  typedef RegistryMap::const_iterator const_iterator;
  iterator       begin()       {return registry.begin();}
  iterator       end()         {return registry.end();}
  const_iterator begin() const {return registry.begin();}
  const_iterator end()   const {return registry.end();}
  G4bool         empty() const {return registry.empty();}
  /// @}

  /// Size of registry
  size_t size() const {return registry.size();}
  
  /// Output stream
  friend std::ostream& operator<<(std::ostream &out, BDSAcceleratorComponentRegistry const &r);

  /// Print out the number of each type of component registered.
  void PrintNumberOfEachType() const;

private:
  /// Default constructor is private as singleton
  BDSAcceleratorComponentRegistry();
  
  /// The singleton instance
  static BDSAcceleratorComponentRegistry* instance;

  /// assignment and copy constructor not implemented nor used
  BDSAcceleratorComponentRegistry& operator=(const BDSAcceleratorComponentRegistry&);
  BDSAcceleratorComponentRegistry(BDSAcceleratorComponentRegistry&);

  /// Set of created components not in registry, for memory management
  std::set<BDSAcceleratorComponent*> allocatedComponents;
  
  /// Set of curvilinear components - purely for memory management.
  std::set<BDSAcceleratorComponent*> curvilinearComponents;

  std::set<BDSAcceleratorComponent*> tunnelComponents;

  /// Map to count the number of each type registered. We have to use std::string
  /// as G4String doesn't provide a hash for the unordered map. We use unordered map
  /// because it's faster for insertions and retrievals but slow for range iteration
  /// which we only do for debug print out or once.
  std::unordered_map<std::string, int> typeCounter;
  
  /// Counter for each component name. Used in case a component is registered but with
  /// a different rigidity. We'll have to name it differently with a numbered suffix.
  std::unordered_map<std::string, int> nameCounter;
};

#endif
