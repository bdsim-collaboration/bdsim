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
#ifndef BDSDETECTORCONSTRUCTION_H
#define BDSDETECTORCONSTRUCTION_H 

#include "BDSExtent.hh"
#include "BDSExtentGlobal.hh"

#include "globals.hh" // geant4 types / globals
#include "G4Transform3D.hh"
#include "G4Version.hh"
#include "G4VUserDetectorConstruction.hh"

#include <list>
#include <map>
#include <set>
#include <string>
#include <vector>

class G4LogicalVolume;
class G4Region;
class G4VPhysicalVolume;

namespace GMAD {
  class BLMPlacement;
  struct Element;
  template<typename T> class FastList;
  class Placement;
  class Query;
  class SamplerPlacement;
  class ScorerMesh;
}

class BDSAcceleratorModel;
class BDSBeamline;
class BDSBeamlineIntegral;
class BDSBeamlineSet;
class BDSComponentFactoryUser;
class BDSFieldObjects;
class BDSFieldQueryInfo;
class BDSParticleDefinition;
class BDSSamplerInfo;

#if G4VERSION_NUMBER > 1009
class BDSBOptrMultiParticleChangeCrossSection;
#endif

/**
 * @brief Class that constructs a Geant4 model of an accelerator.
 *
 * Mandatory class that must be supplied for a valid Geant4 simulation.
 * As construction is based on user input (via the parser) for a potentially
 * large number of items, the construction process is factorised into key steps
 * with member functions and makes use of factories for components and tunnel segments.
 * 
 * Maintained by Laurie Nevay & Jochem Snuverink
 */

class BDSDetectorConstruction: public G4VUserDetectorConstruction
{
public:
  explicit BDSDetectorConstruction(BDSComponentFactoryUser* userComponentFactoryIn = nullptr);
  virtual ~BDSDetectorConstruction();

  /// Loop over beam line and work out maximum tolerable sampler radius. Count all samplers
  /// and set member nSamplers and the same time.
  void UpdateSamplerDiameterAndCountSamplers();

  /// Overridden Geant4 method that must be implemented. Constructs the Geant4 geometry
  /// and returns the finished world physical volume.
  virtual G4VPhysicalVolume* Construct();

  /// Construct sensitive detectors and fields.
  virtual void ConstructSDandField();

  /// Create biasing operations. This is done as a separate step as it has to be controlled
  /// externally and run only after RunManager->Initialise(). This means the bias can't be
  /// constructed as we go along in the component factory.
  void BuildPhysicsBias();

  /// Set the (initial) design particle definition.
  inline void SetDesignParticle(const BDSParticleDefinition* defIn) {designParticle = defIn;}

  /// Public access to the world extent.
  BDSExtent WorldExtent() const {return worldExtent;}
  
  /// Access vector of query objects.
  const std::vector<BDSFieldQueryInfo*>& FieldQueries() const {return fieldQueries;}

  /// Loop over a beam line and place elements in a container (world). If a sensitive
  /// detector is specified in each component, this is applied to each volume. If regions
  /// are desired,
  /// the element is looked up in the region definitions and that is set up. If
  /// registerInfo, physical volume info is created and placed in a pv info registry.
  /// Public and static so it can be used by parallel world constructors. Last argument
  /// is whether to use the placement transform for curvilinear coordinate geometry that's
  /// different in the case of tilted dipoles.
  static void PlaceBeamlineInWorld(BDSBeamline*          beamline,
				   G4VPhysicalVolume*    containerPV,
				   G4bool                checkOverlaps     = false,
				   G4bool                setRegions        = false,
				   G4bool                registerInfo      = false,
				   G4bool                useCLPlacementTransform = false,
				   G4bool                useIncrementalCopyNumbers = false,
				   G4bool                registerPlacementNamesForOutput = false);

  /// Create a transform based on the information in the placement. If S is supplied, it's
  /// updated with the final S coordinate calculated. If an extent is given - only in the
  /// case of a placement transform w.r.t. the curvilinear system (ie not global) - the side
  /// parameter from the placement is used to include an offset to the edge of the element.
  static G4Transform3D CreatePlacementTransform(const GMAD::Placement& placement,
						const BDSBeamline*     beamLine,
						G4double*              S               = nullptr,
						BDSExtent*             placementExtent = nullptr,
						const G4String&        objectTypeForErrorMsg = "placement");

  // Create a scorermesh placement transform. Turns the scorermesh into a
  /// placement and uses the above function.
  static G4Transform3D CreatePlacementTransform(const GMAD::ScorerMesh& scorerMesh,
                                                const BDSBeamline*      beamLine,
						G4double*               S = nullptr);
  
  /// Create a sampler placement transform. Turns the sampler placement into a
  /// placement and uses the above function.
  static G4Transform3D CreatePlacementTransform(const GMAD::SamplerPlacement& samplerPlacement,
						const BDSBeamline*            beamLine,
						G4double*                     S = nullptr);

  /// Create a blm placement from a blm placement.
  static G4Transform3D CreatePlacementTransform(const GMAD::BLMPlacement& blmPlacement,
						const BDSBeamline*        beamLine,
						G4double*                 S         = nullptr,
						BDSExtent*                blmExtent = nullptr);
  
  /// Create a query placement from a query placement.
  static G4Transform3D CreatePlacementTransform(const GMAD::Query& queryPlacement,
                                                const BDSBeamline* beamLine,
                                                G4double* S = nullptr);

  ///  Attach component with extent2 to component with extent1 with placement.
  static G4ThreeVector SideToLocalOffset(const GMAD::Placement& placement,
					 const BDSBeamline*     beamLine,
					 const BDSExtent&       placementExtent);
  
  /// Construct sampler specifications from a GMAD element. May return nullptr if
  /// none type is specified.
  static BDSSamplerInfo* BuildSamplerInfo(const GMAD::Element* element);

  /// Whether to build a sampler world or not. If we've counted more than one sampler we
  /// should build the world in the end.
  G4bool BuildSamplerWorld() const {return nSamplers > 0;}
  
  G4bool BuildPlacementFieldsWorld() const {return buildPlacementFieldsWorld;}

  /// Prepare field queries from parser information.
  static std::vector<BDSFieldQueryInfo*> PrepareFieldQueries(const BDSBeamline* mainBeamline);
  
private:
  /// assignment and copy constructor not implemented nor used
  BDSDetectorConstruction& operator=(const BDSDetectorConstruction&) = delete;
  BDSDetectorConstruction(BDSDetectorConstruction&) = delete;
  
  /// Prepare extra sampler sensitive detector classes required if we have any particle
  /// filters created for samplers.
  void PrepareExtraSamplerSDs();
  
  /// Count number of fields required for placements.
  void CountPlacementFields();
  
  /// Create and set parameters for various G4Regions
  void InitialiseRegions();

  /// Create all aperture definitions from parser and store in BDSAcceleratorModel.
  void InitialiseApertures();
  
  /// Build the main beam line and then any other required beam lines.
  void BuildBeamlines();

  /// Convert a parser beamline_list to BDSAcceleratorComponents with help of
  /// BDSComponentFactory and put in a BDSBeamline container that calculates coordinates
  /// and extents of the beamline.
  BDSBeamlineSet BuildBeamline(const GMAD::FastList<GMAD::Element>& beamLine,
                               const G4String&            name,
                               const BDSBeamlineIntegral& startingIntegral,
                               BDSBeamlineIntegral*&      integral,
                               const G4Transform3D&       initialTransform    = G4Transform3D(),
                               G4bool                     beamlineIsCircular  = false,
                               G4bool                     isPlacementBeamline = false);

  /// Build the tunnel around the already constructed flat beam line.
  void BuildTunnel();
  
  /// Build the world volume using the extent of the BDSBeamline instance created
  /// in BuildBeamline()
  G4VPhysicalVolume* BuildWorld();
  
  /// Place beam line, tunnel beam line, end pieces and placements in world.
  void ComponentPlacement(G4VPhysicalVolume* worldPV);

  /// Detect whether the first element has an angled face such that it might overlap
  /// with a previous element.  Only used in case of a circular machine.
  G4bool UnsuitableFirstElement(std::list<GMAD::Element>::const_iterator element);

  /// Calculate local extent of custom user sampler.
  BDSExtent CalculateExtentOfSamplerPlacement(const GMAD::SamplerPlacement& sp) const;

  /// Calculate the maximum global extent of all sampler placements from the parser. Beam line
  /// supplied to calculate placements in some cases.
  BDSExtentGlobal CalculateExtentOfSamplerPlacements(const BDSBeamline* beamLine) const;

  /// Calculate local extent of scorer mesh in 3D.
  BDSExtent CalculateExtentOfScorerMesh(const GMAD::ScorerMesh& sm) const;

  /// Calculate the maximum global extent of all scoerer meshes from the parser.  Beam line
  /// supplied to calculate placements in some cases.
  BDSExtentGlobal CalculateExtentOfScorerMeshes(const BDSBeamline* bl) const;

#if G4VERSION_NUMBER > 1009
  /// Function that creates physics biasing cross section
  BDSBOptrMultiParticleChangeCrossSection* BuildCrossSectionBias(const std::list<std::string>& biasList,
                                                                 const std::list<std::string>& defaultBias,
                                                                 const G4String& elementName);

  /// Construct scoring meshes.
  void ConstructScoringMeshes();

  /// Print out the sensitivity of every single volume so far constructed in the world.
  void VerboseSensitivity() const;
  /// Recursive function to print out each sensitive detector name.
  void PrintSensitiveDetectorsOfLV(const G4LogicalVolume* lv, G4int currentDepth) const;

  /// List of bias objects - for memory management
  std::vector<BDSBOptrMultiParticleChangeCrossSection*> biasObjects;
  std::map<G4String, BDSBOptrMultiParticleChangeCrossSection*> biasSetObjects;
#endif

#ifdef BDSDEBUG
  bool debug = true;
#else
  bool debug = false;
#endif

  ///@{ Variable copied from global constants
  G4bool verbose;
  G4bool checkOverlaps;
  ///@}

  /// Accelerator model pointer
  BDSAcceleratorModel* acceleratorModel;

  /// All fields
  std::vector<BDSFieldObjects*> fields;

  G4bool       circular;    ///< Whether or not we're building a circular machine.
  BDSExtent    worldExtent; ///< Record of the world extent.
  BDSBeamline* placementBL; ///< Placement beam line.
  /// Particle definition all components are built w.r.t. Includes rigidity etc.
  const BDSParticleDefinition* designParticle;
  G4bool canSampleAngledFaces; ///< Whether the integrator set permits sampling elements with angled faces.

  BDSComponentFactoryUser* userComponentFactory;

  G4int nSamplers; ///< Count of number of samplers to be built.
  G4bool buildPlacementFieldsWorld;
  
  /// Cache of possibly loaded logical volumes from a world geometry file - used for biasing.
  std::set<G4LogicalVolume*> worldContentsLogicalVolumes;
  std::set<G4LogicalVolume*> worldVacuumLogicalVolumes;
  G4LogicalVolume* worldLogicalVolume;
  
  std::vector<BDSFieldQueryInfo*> fieldQueries;
	G4int subElementIDCounter;

  // for developer checks only
#ifdef BDSCHECKUSERLIMITS
  void PrintUserLimitsSummary(const G4VPhysicalVolume* world) const;
  void PrintUserLimitsPV(const G4VPhysicalVolume* aPV, G4double globalMinEK) const;
#endif
};

#endif

