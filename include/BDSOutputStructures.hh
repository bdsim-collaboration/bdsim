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
#ifndef BDSOUTPUTSTRUCTURES_H
#define BDSOUTPUTSTRUCTURES_H

#include "globals.hh"

#include <map>
#include <string>
#include <utility>
#include <vector>

// forward declarations
class BDSGlobalConstants;
class BDSOutputROOTEventAperture;
class BDSOutputROOTEventBeam;
class BDSOutputROOTEventCavityInfo;
class BDSOutputROOTEventCollimator;
class BDSOutputROOTEventCollimatorInfo;
class BDSOutputROOTEventCoords;
class BDSOutputROOTEventHeader;
class BDSOutputROOTEventHistograms;
class BDSOutputROOTEventInfo;
class BDSOutputROOTEventLoss;
class BDSOutputROOTEventLossWorld;
class BDSOutputROOTEventModel;
class BDSOutputROOTEventOptions;
class BDSOutputROOTEventRunInfo;
template<class T> class BDSOutputROOTEventSampler;
class BDSOutputROOTEventSamplerC;
class BDSOutputROOTEventSamplerS;
class BDSOutputROOTEventTrajectory;
class BDSOutputROOTParticleData;
class G4Material;

/**
 * @brief Holder for output information.
 * 
 * This holds instances of the output classes that can be filled
 * and directly copied to the output file.  It also holds and fills
 * the histograms.
 *
 * @author S. Boogert, L. Nevay
 */

class BDSOutputStructures
{
protected:
  explicit BDSOutputStructures(const BDSGlobalConstants* globals);
  virtual ~BDSOutputStructures();

  /// Construct samplers.
  void InitialiseSamplers();
  
  /// Construct a map of material pointer to integer ID and name.
  void InitialiseMaterialMap();
  
  /// Interface to allow setting up samplers later for dynamic geometry construction a la SixTrack. Not for regular use.
  G4int UpdateSamplerStructures();
  
  /// Extract number of collimators and their names from beam line. Two stage
  /// initialisation for collimators so histograms can be made dynamically if
  /// required or not based on number of collimators.
  void PrepareCollimatorInformation();

  /// Extract number of collimators and their names from beam line.
  void PrepareCavityInformation();

  /// Construct collimators.
  void InitialiseCollimators();

  /// Clear the local particle data structure.
  void ClearStructuresParticleData();

  /// Clear the local header structure.
  void ClearStructuresHeader();

  /// Clear the local model structure.
  void ClearStructuresModel();

  /// Clear the local beam structure.
  void ClearStructuresBeam();

  /// Clear the local options structure.
  void ClearStructuresOptions();
  
  /// Clear the local structures in this class in preparation for a new event.
  void ClearStructuresEventLevel();

  /// Clear the local structures in this class in preparation for a new run.
  void ClearStructuresRunLevel();
  
  ///@{ Create histograms in evtHistos and create a per-run accumulator. Return index from evtHistos.
  G4int Create1DHistogram(const G4String& name,
                          const G4String& title,
                          G4int nbins,
                          G4double xmin,
                          G4double xmax);
  G4int Create1DHistogram(const G4String& name,
                          const G4String& title,
                          const std::vector<double>& edges);
  G4int Create3DHistogram(const G4String& name,
                          const G4String& title,
                          G4int nBinsX, G4double xMin, G4double xMax,
                          G4int nBinsY, G4double yMin, G4double yMax,
                          G4int nBinsZ, G4double zMin, G4double zMax);
  G4int Create4DHistogram(const G4String& name,
                          const G4String& title,
                          const G4String& eScale,
                          const std::vector<double>& eBinsEdges,
                          G4int nBinsX, G4double xMin, G4double xMax,
                          G4int nBinsY, G4double yMin, G4double yMax,
                          G4int nBinsZ, G4double zMin, G4double zMax,
                          G4int nBinsE, G4double eMin, G4double eMax);
  ///@}

  BDSOutputROOTParticleData* particleDataOutput; ///< Geant4 information / particle tables.
  BDSOutputROOTEventHeader*  headerOutput;     ///< Information about the file.
  BDSOutputROOTEventBeam*    beamOutput;       ///< Beam output.
  BDSOutputROOTEventOptions* optionsOutput;    ///< Options output.
  BDSOutputROOTEventModel*   modelOutput;      ///< Model output.

  /// Primary sampler structure.
#ifdef __ROOTDOUBLE__
  BDSOutputROOTEventSampler<double>* primary;
#else 
  BDSOutputROOTEventSampler<float>* primary;
#endif
  BDSOutputROOTEventCoords* primaryGlobal; /// Global coordinates of primary particle.
  /// Sampler structures.
#ifdef __ROOTDOUBLE__
  std::vector<BDSOutputROOTEventSampler<double>*> samplerTrees;
#else
  std::vector<BDSOutputROOTEventSampler<float>*> samplerTrees;
#endif
  std::vector<std::string> samplerNames; ///< Sampler names to use.
  std::vector<BDSOutputROOTEventSamplerC*> samplerCTrees;
  std::vector<BDSOutputROOTEventSamplerS*> samplerSTrees;
  std::vector<std::string> samplerCNames;
  std::vector<std::string> samplerSNames;
  std::map<G4int, G4int> samplerIDToIndexPlane;
  std::map<G4int, G4int> samplerIDToIndexCylinder;
  std::map<G4int, G4int> samplerIDToIndexSphere;
  
  BDSOutputROOTEventRunInfo*    runInfo;            ///< Run information.
  BDSOutputROOTEventHistograms* runHistos;          ///< Run level histograms
  BDSOutputROOTEventLoss*       eLoss;              ///< General energy deposition.
  BDSOutputROOTEventLoss*       pFirstHit;          ///< Primary hit point.
  BDSOutputROOTEventLoss*       pLastHit;           ///< Primary loss point.
  BDSOutputROOTEventLoss*       eLossVacuum;        ///< General energy deposition.
  BDSOutputROOTEventLoss*       eLossTunnel;        ///< Tunnel energy deposition.
  BDSOutputROOTEventLossWorld*  eLossWorld;         ///< World energy deposition.
  BDSOutputROOTEventLossWorld*  eLossWorldExit;     ///< World exit hits.
  BDSOutputROOTEventLossWorld*  eLossWorldContents; ///< Externally supplied world contents hits.
  BDSOutputROOTEventAperture*   apertureImpacts;    ///< Impacts on the aperture.
  BDSOutputROOTEventTrajectory* traj;               ///< Trajectories.
  BDSOutputROOTEventHistograms* evtHistos;          ///< Event level histograms.
  BDSOutputROOTEventInfo*       evtInfo;            ///< Event information.

  // collimator specific output
  std::vector<BDSOutputROOTEventCollimator*> collimators; ///< Collimator output structures.
  std::vector<G4String>     collimatorNames;         ///< Names of collimators in output structures.
  G4int                     nCollimators;            ///< Number of collimators in beam line.
  std::vector<G4int>        collimatorIndices;       ///< Indices in beam line that are collimators.
  std::map<G4String, G4int> collimatorIndicesByName; ///< Indices mapped to their name.
  std::vector<BDSOutputROOTEventCollimatorInfo> collimatorInfo; ///< Collimator parameters.
  /// Cache of aperture differences for each collimator info to avoid repeated calculation and
  /// to avoid storing unnecessary output in the collimator info.
  std::vector<std::pair<G4double, G4double> >   collimatorDifferences;

  // cavity specific output
  std::vector<G4String>     cavityNames;         ///< Names of cavities in output structures.
  G4int                     nCavities;           ///< Number of cavities in beam line.
  std::vector<G4int>        cavityIndices;       ///< Indices in beam line that are cavities.
  std::map<G4String, G4int> cavityIndicesByName; ///< Indices mapped to their name.
  std::vector<BDSOutputROOTEventCavityInfo> cavityInfo; ///< Cavity parameters.

  std::map<G4Material*, short int> materialToID;
  std::map<short int, G4String>    materialIDToNameUnique;
  
private:
  /// Whether we've set up the member vector of samplers. Can only be done once the geometry
  /// is constructed so it's done in initialisation. This happens with each new file, but we
  /// should only prepare the local samplers once, hence this cache variable.
  G4bool localSamplersInitialised;

  /// Whether we've setup the member vector of collimators. Similarly to localSamplersInitialised.
  G4bool localCollimatorsInitialised;
  
  ///@{ Unused default constructors
  BDSOutputStructures() = delete;
  BDSOutputStructures(const BDSOutputStructures&) = delete;
  BDSOutputStructures& operator=(const BDSOutputStructures&) = delete;
  ///@}
};

#endif
