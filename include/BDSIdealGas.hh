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
#ifndef BDSIDEALGAS_H
#define BDSIDEALGAS_H

#include "BDSMaterials.hh"
#include "BDSUtilities.hh"
#include "BDSWarning.hh"

#include "globals.hh" // geant4 globals / types

#include <iomanip>
#include <list>
#include <map>
#include <set>
#include <vector>

/**
 * @brief A class to perform ideal gas calculations.
 *
 * @author Marin Deniaud
 */

G4double R = CLHEP::Avogadro * CLHEP::k_Boltzmann / CLHEP::joule * 10;

class BDSIdealGas{
public:
    template <typename Type>
    static G4double CalculateDensityFromPressureTemperature(const std::list<G4String>& components,
                                                            const std::list<Type>& componentFractions,
                                                            G4double pressure,
                                                            G4double temperature) {

      G4double averageMolarMass = CalculateAverageMolarMass(components, componentFractions);
      G4double density = (pressure*averageMolarMass)/(R*temperature);

      return density;
    }

    template <typename Type>
    static G4double CalculateTemperatureFromPressureDensity(const std::list<G4String>& components,
                                                            const std::list<Type>& componentFractions,
                                                            G4double pressure,
                                                            G4double density) {

      G4double averageMolarMass = CalculateAverageMolarMass(components, componentFractions);
      G4double temperature = (pressure*averageMolarMass)/(R*density);

      return temperature;
    }

    template <typename Type>
    static G4double CalculatePressureFromTemperatureDensity(const std::list<G4String>& components,
                                                     const std::list<Type>& componentFractions,
                                                     G4double temperature,
                                                     G4double density) {

      G4double averageMolarMass = CalculateAverageMolarMass(components, componentFractions);
      G4double pressure = (density*R*temperature)/(averageMolarMass);

      return pressure;
    }

    template <typename Type>
    static G4double CalculateDensityFromNumberDensity(const std::list<G4String>& components,
                                               const std::list<Type>& componentFractions,
                                               G4double numberDensity) {

      G4double averageMolarMass = CalculateAverageMolarMass(components, componentFractions);
      G4double density = numberDensity*averageMolarMass/CLHEP::Avogadro;

      return density;
    }

    template <typename Type>
    static G4double CalculateDensityFromMolarDensity(const std::list<G4String>& components,
                                              const std::list<Type>& componentFractions,
                                              G4double molarDensity) {

      G4double averageMolarMass = CalculateAverageMolarMass(components, componentFractions);
      G4double density = molarDensity*averageMolarMass;

      return density;
    }

    template <typename Type>
    static G4double CalculateAverageMolarMass(const std::list<G4String>& components,
                                              const std::list<Type>& componentFractions){

      std::vector<G4String> componentsVector{ components.begin(), components.end() };
      std::vector<Type> componentFractionsVector{ componentFractions.begin(), componentFractions.end() };
      std::map<G4String, Type> componentsTable;

      G4double averageMolarMass = 0;
      G4double fracSum = 0;
      for (size_t i=0; i < componentsVector.size(); i++)
      {
        const G4String& componentName = componentsVector[i];
        auto component = BDSMaterials::Instance()->GetMaterial(componentName);

        size_t nbelement = component->GetNumberOfElements();
        if (nbelement == 1)
        {
          auto element = component->GetElement(0);
          auto molarMass = element->GetN();
          averageMolarMass = averageMolarMass + componentFractionsVector[i] * molarMass;
          fracSum = fracSum + componentFractionsVector[i];
        }
        else
        {
          auto elementVector = component->GetElementVector();
          std::list<G4String> elementNames;
          std::list<Type> elementFractions;
          for (const auto element: *elementVector)
          {
            elementNames.push_back(element->GetName());
          }
          for (size_t ii=0; ii < elementVector->size(); ii++)
          {
            elementFractions.push_back(component->GetFractionVector()[ii]);
          }
          averageMolarMass = averageMolarMass + componentFractionsVector[i] * CalculateAverageMolarMass(elementNames, elementFractions);
          fracSum = fracSum + componentFractionsVector[i];
        }
      }

      return averageMolarMass/fracSum;
    }


    template <typename Type>
    static void CheckGasLaw(G4String name,
                            G4double &temperature,
                            G4double &pressure,
                            G4double &density,
                            const std::list<G4String>& components,
                            const std::list<Type>& componentFractions) {

#ifdef BDSDEBUG
      G4cout << "BDSIdealGas::CheckGasLaw: " << G4endl;
#endif
      if (density != 0 and pressure == 0)
      {
        G4double calcPressure = CalculatePressureFromTemperatureDensity(components, componentFractions,
                                                                        temperature, density);
        pressure = calcPressure;
        G4String msg = "BDSIdealGas :: Computing pressure " + std::to_string(calcPressure);
        msg += " atmosphere for material " + name;
        BDS::Warning(msg);
      }

      else if (density !=0 and pressure !=0 and temperature == 300)
      {
        G4double calcTemp = CalculateTemperatureFromPressureDensity(components, componentFractions,
                                                                    pressure, density);
        temperature = calcTemp;
        G4String msg = "BDSIdealGas :: Computing temperature " + std::to_string(calcTemp);
        msg += " kelvin for material " + name;
        BDS::Warning(msg);
      }

      else if (density == 0 and pressure !=0)
      {
        G4double calcDens = CalculateDensityFromPressureTemperature(components, componentFractions,
                                                                    pressure, temperature);
        density = calcDens;
        G4String msg = "BDSIdealGas :: Computing density " + std::to_string(calcDens);
        msg += " g.cm-3 for material " + name;
        BDS::Warning(msg);
      }

      else if (density !=0 and pressure !=0 and temperature != 300)
      {
        G4double calcDensity = CalculateDensityFromPressureTemperature(components, componentFractions, pressure, temperature);
        if(density != calcDensity)
        {
          G4String msg = "Ideal gas density calculated from pressure and temperature doesn't match given density\n";
          msg += "Assuming temperature of 300K and computing correct pressure for this density";
          BDS::Warning(msg);
          temperature = 300;
          pressure = CalculatePressureFromTemperatureDensity(components, componentFractions, temperature, density);
        }
      }
    }
};

#endif
