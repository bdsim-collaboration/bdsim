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
#ifndef CRYSTAL_H
#define CRYSTAL_H

#include <iomanip>
#include <iostream>
#include <list>
#include <string>

#include "published.h"

namespace GMAD
{
  /**
   * @brief Crystal class for parser
   * 
   * @author Laurie Nevay
   */
  
  class Crystal: public Published<Crystal>
  {
  public:
    std::string name;                 ///< Name of object.
    std::string model;                ///< "legacy" or "fastsim".
    std::string material;             ///< Material of crystal.
    std::string data;                 ///< Which channelling data to use.
    std::string lattice;              ///< FastSim lattice, e.g. "(111)" or "<111>".
    std::string fastSimDataPath;       ///< Optional custom FastSim data directory.
    std::string shape;                ///< Which geometry to create.
    double      lengthX;              ///< X size.
    double      lengthY;              ///< Y size.
    double      lengthZ;              ///< Z size.
    double      sizeA;
    double      sizeB;
    double      sizeC;
    double      alpha;
    double      beta;
    double      gamma;
    int         spaceGroup;
    double      bendingAngleYAxis;    ///< Bending angle about Y axis.
    double      bendingAngleZAxis;    ///< Bending angle about Z axis.
    double      miscutAngleY;

    /// FastSim internal crystalline-undulator geometry.
    double      fastSimCUAmplitude;
    double      fastSimCUPeriod;
    double      fastSimCUPhase;
    std::string fastSimCUGeometryFile;

    /// FastSim model applicability controls.
    std::list<std::string> fastSimParticles;
    double                 fastSimDefaultLowKineticEnergy;
    double                 fastSimDefaultLindhardAngleHighLimit;
    double                 fastSimDefaultHighAngleLimit;
    std::list<std::string> fastSimLowEnergyParticles;
    std::list<double>      fastSimLowEnergyLimits;
    std::list<std::string> fastSimLindhardAngleParticles;
    std::list<double>      fastSimLindhardAngleLimits;
    std::list<std::string> fastSimHighAngleParticles;
    std::list<double>      fastSimHighAngleLimits;
    int                    fastSimMaxPhotonsPerStep;

    /// Baier-Katkov radiation controls.
    bool                   radiation;
    double                 radiationSinglePhotonProbabilityLimit;
    int                    radiationSmallTrajectorySteps;
    int                    radiationSamplingPhotons;
    double                 radiationAngleFactor;
    double                 radiationMinPhotonEnergy;
    double                 radiationMaxPhotonEnergy;
    int                    radiationSpectrumBins;
    std::list<double>      radiationStatisticsMinEnergy;
    std::list<double>      radiationStatisticsMaxEnergy;
    std::list<double>      radiationStatisticsMultiple;
    std::string            radiationVirtualCollimator;
    double                 radiationCollimatorHalfWidthX;
    double                 radiationCollimatorHalfWidthY;
    double                 radiationCollimatorCentreX;
    double                 radiationCollimatorCentreY;

    /// Coherent pair-production controls.
    bool   coherentPairProduction;
    bool   coherentPairProductionIncoherent;
    double coherentPairProductionLowEnergyLimit;
    double coherentPairProductionHighAngleLimit;
    double coherentPairProductionKineticEnergyCut;
    int    coherentPairProductionSamplingPairs;
    double coherentPairProductionAngleFactor;
    int    coherentPairProductionTrajectorySteps;
    
    /// Constructor
    Crystal();
    /// Reset
    void clear();
    /// Print some properties
    void print()const;
    /// Set methods by property name and value
    template <typename T>
    void set_value(std::string property, T value, bool bExit = true);

  private:
    /// publish members
    void PublishMembers();
  };
  
  template <typename T>
  void Crystal::set_value(std::string property, T value, bool bExit)
    {
#ifdef BDSDEBUG
      std::cout << "crystal> Setting value " << std::setw(25) << std::left << property << value << std::endl;
#endif
      // member method can throw runtime_error, catch and exit gracefully
      try
        {set(this,property,value);}
      catch (const std::runtime_error&)
        {
          std::cerr << "Error: crystal> unknown option \"" << property << "\" with value \"" << value << "\"" << std::endl;
	        if(bExit)
            exit(1);
          else
            std::rethrow_exception(std::current_exception());
        }
    }
}

#endif
