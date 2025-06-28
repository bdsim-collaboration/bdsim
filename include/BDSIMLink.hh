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
#ifndef BDSIMLINK_H
#define BDSIMLINK_H
#include "BDSHitSamplerLink.hh"
#include "BDSLinkRunAction.hh"

#include <map>
#include <string>
#include <vector>

class BDSBunch;
class BDSComponentConstructor;
class BDSComponentFactoryUser;
class BDSLinkComponent;
class BDSLinkDetectorConstruction;
class BDSOutput;
class BDSParser;
class BDSParticleCoordsFull;
class BDSParticleDefinition;
class BDSParticleExternal;
class G4RunManager;
class G4VModularPhysicsList;

#include "parser/element.h"
#include "parser/elementtype.h"

/** 
 * @brief Interface class to use BDSIM with trackers.
 *
 * First way to use:
 * bds = new BDSIMLink(argc, argv);
 * bds->TrackThin(...)
 * 
 * Second way to use (delayed initialisation):
 * bds = new BDSIMLink();
 * // modify bds in some way
 * bds->Initialise(argc, argv);
 * bds->TrackThin(...)
 *
 * @author Laurie Nevay
 */

class BDSIMLink
{
public:
  /// Construct an instance but don't initialise. Requires initialisation with
  /// arguments argc and arv
  explicit BDSIMLink(BDSBunch* bunchIn = nullptr);

  /// Initialise everything given these arguments. The minimumKinetic energy should be in GeV.
  int Initialise(int argc,
                 char** argv,
                 bool   usualPrintOut        = true,
                 double minimumKineticEnergy = 0,
                 bool   protonsAndIonsOnly   = false);

  /// Construct and initialise BDSIM.
  BDSIMLink(int argc, char** argv, bool usualPrintOut=true);

  /// The destructor opens the geometry in Geant4 and deletes everything.
  ~BDSIMLink();

  /// Accessor as to whether BDSIM kernel is initialised - ie all geometry and physics
  /// constructed.
  inline bool Initialised() const {return initialised;};

  inline int InitialisationResult() const {return initialisationResult;}

  /// Generate nGenerate events. If the default argument -1 is used, the number is taken
  /// from the standard input e.g. the executable option ngenerate and then the one specified
  /// in the input gmad files as an option.
  void BeamOn(int nGenerate=-1);
  
  void SelectLinkElement(const std::string& elementName, bool debug = false);
  void SelectLinkElement(int index, bool debug = false);

  /// Use standard C++ types as expected to be used externally.
  int AddLinkCollimatorJaw(const std::string& collimatorName,
                           const std::string& materialName,
                           double length,
                           double halfApertureLeft,
                           double halfApertureRight,
                           double rotation,
                           double xOffset,
                           double yOffset,
                           double jawTiltLeft = 0.0,
                           double jawTiltRight = 0.0,
                           bool   buildLeftJaw  = true,
                           bool   buildRightJaw = true,
                           bool   isACrystal    = false,
                           double crystalAngle  = 0,
                           bool   sampleIn      = false);

  int AddLinkCollimatorTipJaw(const std::string& collimatorName,
                              const std::string& materialName,
                              const std::string& tipMaterialName,
                              double tipThickness,
                              double length,
                              double halfApertureLeft,
                              double halfApertureRight,
                              double rotation,
                              double xOffset,
                              double yOffset,
                              double jawTiltLeft = 0.0,
                              double jawTiltRight = 0.0,
                              bool   buildLeftJaw  = true,
                              bool   buildRightJaw = true);

  int AddLinkElement(GMAD::Element &el);

  BDSHitsCollectionSamplerLink* SamplerHits() const;
  void ClearSamplerHits() {runAction->ClearSamplerHits();}
  
  int GetCurrentMaximumSixTrackParticleID() const;
  void SetCurrentMaximumExternalParticleID(int currentMaximumExternalParticleID);

  inline G4int NSecondariesToReturn() const {return runAction ? runAction->NSecondariesToReturn() : 0;}
  inline G4int NPrimariesToReturn()   const {return runAction ? runAction->NPrimariesToReturn() : 0;}
  
  /// Get the internal index of a component by name.
  int GetLinkIndex(const std::string& elementName) const;
  
  const BDSLinkComponent* GetLinkComponent(int linkID) const;
  /// @{ Access the length of a component. If bad name or ID given, -1 will be returned.
  double GetChordLengthOfLinkElement(int beamlineIndex) const;
  double GetChordLengthOfLinkElement(const std::string& elementName);
  double GetArcLengthOfLinkElement(int beamlineIndex) const;
  double GetArcLengthOfLinkElement(const std::string& elementName);
  /// @}

  BDSBunch* GetBunch() const;

  /// Provide a physics list that will be used inplace of the BDSIM generate one.
  void RegisterUserPhysicsList(G4VModularPhysicsList* userPhysicsListIn) {userPhysicsList = userPhysicsListIn;}
  G4VModularPhysicsList* UserPhysicsList() const {return userPhysicsList;} ///< Access user physics list.
  
private:
  /// The main function where everything is constructed.
  int Initialise(double minimumKineticEnergy = 0,
                 bool   protonsAndIonsOnly   = false);
  
  bool   ignoreSIGINT;         ///< For cmake testing.
  bool   usualPrintOut;        ///< Whether to allow the usual cout output.
  bool   initialised;          ///< Whether initialisation was completed safely
  int    initialisationResult; ///< Possible to not finish initialisation but have completed ok - flag for this.
  int    argcCache;            ///< Cache of argc.
  char** argvCache;            ///< Cache of argv.

  /// @{ Cache of main object in BDSIM.
  BDSParser*    parser;
  BDSOutput*    bdsOutput;
  BDSBunch*     bdsBunch;
  bool          internalBdsBunch;
  G4RunManager* runManager;
  BDSLinkDetectorConstruction* construction;
  BDSLinkRunAction*  runAction;
  /// @}
  
  std::vector<BDSParticleExternal*> externalParticles;
  std::map<std::string, int>        nameToElementIndex;
  std::map<int, int>                linkIDToBeamlineIndex;
  int                               currentElementIndex; ///< Element to track in.
  G4VModularPhysicsList*            userPhysicsList;     ///< Optional user registered physics list.
};

#endif
