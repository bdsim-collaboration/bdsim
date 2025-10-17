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
#ifndef BDSLINKDETECTORCONSTRUCTION_H
#define BDSLINKDETECTORCONSTRUCTION_H
#include "BDSBeamline.hh"
#include "BDSCollimatorJaw.hh"
#include "BDSCollimatorTipJaw.hh"
#include "BDSExtent.hh"

#include "G4ThreeVector.hh"
#include "G4Types.hh"
#include "G4Version.hh"
#include "G4VUserDetectorConstruction.hh"

#include <string>

class BDSBeamline;
class BDSBeamlineElement;
class BDSBeamlineIntegral;
class BDSLinkPrimaryGeneratorAction;
class BDSLinkRegistry;
class BDSParticleDefinition;
class G4Box;
class G4ChannelingOptrMultiParticleChangeCrossSection;
class G4VPhysicalVolume;

#include "parser/element.h"
#include "parser/elementtype.h"

/**
 * @brief Construction of the geometry in the case of a link model.
 *
 * @author Laurie Nevay
 */

class BDSLinkDetectorConstruction: public G4VUserDetectorConstruction
{
public:
  /// Default constructor
  BDSLinkDetectorConstruction();

  virtual ~BDSLinkDetectorConstruction();
  virtual G4VPhysicalVolume* Construct();

  /// Interface to append a collimator of jaw style to the linking.
  G4int AddLinkCollimatorJaw(const std::string& collimatorName,
                             const std::string& materialName,
                             G4double length,
                             G4double halfApertureLeft,
                             G4double halfApertureRight,
                             G4double rotation,
                             G4double xOffset,
                             G4double yOffset,
                             G4double jawTiltLeft = 0.0,
                             G4double jawTiltRight = 0.0,
                             G4bool   buildLeftJaw  = true,
                             G4bool   buildRightJaw = true,
                             G4bool   isACrystal    = false,
                             G4double crystalAngle  = 0,
                             G4bool   sampleIn      = false);

  /// Interface to append a tip collimator jaw to the linking.
  G4int AddLinkCollimatorTipJaw(const std::string& collimatorName,
                                const std::string& materialName,
                                const std::string& tipMaterialName,
                                G4double tipThickness,
                                G4double length,
                                G4double halfApertureLeft,
                                G4double halfApertureRight,
                                G4double rotation,
                                G4double xOffset,
                                G4double yOffset,
                                G4double jawTiltLeft = 0.0,
                                G4double jawTiltRight = 0.0,
                                G4bool   buildLeftJaw  = true,
                                G4bool   buildRightJaw = true);
  /// Interface to append an element
  G4int AddLinkElement(GMAD::Element el);

  /// Set the design particle definition.
  inline void SetDesignParticle(const BDSParticleDefinition* defIn) {designParticle = defIn;}
  inline void SetPrimaryGeneratorAction(BDSLinkPrimaryGeneratorAction* pgIn) {primaryGeneratorAction = pgIn;}


  /// @{ Accessor.
  inline BDSExtent WorldExtent() const {return worldExtent;}
  inline BDSLinkRegistry* LinkRegistry() const {return linkRegistry;}
  /// @}
  
  void BuildPhysicsBias();

  inline const std::map<std::string, G4int>& NameToElementIndex() const {return nameToElementIndex;}
  inline const std::map<int, int>& LinkIDToBeamlineIndex() const {return linkIDToBeamlineIndex;}
  inline G4int NumberOfElements() const {return linkBeamline ? (G4int)linkBeamline->size() : 0;}
  inline void SetSamplerWorldID(G4int samplerWorldIDIn) {samplerWorldID = samplerWorldIDIn;}
  inline const BDSBeamline* LinkBeamline() const {return linkBeamline;}

 private:
  /// Create the worldSolid if it doesn't exist and if not expand it to the extent of the
  /// linkBeamline member. Update worldExtent member also.
  void UpdateWorldSolid();

  /// Place a beam line element in the world.
  G4int PlaceOneComponent(const BDSBeamlineElement* element, const G4String& originalName);

  G4Box* worldSolid;
  G4VPhysicalVolume* worldPV;
  BDSExtent worldExtent;
  BDSBeamline* linkBeamline;
  BDSLinkRegistry* linkRegistry;
  BDSLinkPrimaryGeneratorAction* primaryGeneratorAction;

  /// Particle definition all components are built w.r.t. Includes rigidity etc.
  const BDSParticleDefinition* designParticle;

#if G4VERSION_NUMBER > 1039
  G4ChannelingOptrMultiParticleChangeCrossSection* crystalBiasing;
#endif

  /// Cache of the index to which parallel world the sampler one is.
  G4int samplerWorldID;
  
  BDSBeamlineIntegral* integral;

  std::map<std::string, G4int> nameToElementIndex; ///< Build up a copy here too.
  std::map<G4int, G4int> linkIDToBeamlineIndex;    ///< Special linkID to linkBeamline index
};

#endif
