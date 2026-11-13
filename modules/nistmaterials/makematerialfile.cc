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
/**
 * @file makematerialfile.cc
 */
#include "G4Element.hh"
#include "G4ElementVector.hh"
#include "G4IsotopeVector.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4String.hh"
#include "G4Types.hh"
#include "G4Version.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include <fstream>
#include <iomanip>
#include <ios>
#include <iostream>
#include <map>
#include <set>
#include <vector>

int main(int, char**)
{
  std::ofstream efile;
  efile.open("nist_elements.txt");
  
  G4double gcm3 = CLHEP::g / CLHEP::cm3;
  
  std::map<G4State, G4String> stateMap = {{G4State::kStateUndefined, "unknown"},
                                          {G4State::kStateGas,       "gas"},
                                          {G4State::kStateLiquid,    "liquid"},
                                          {G4State::kStateSolid,     "solid"}
  };
  
  // calling this forces construction of all elements and NIST materials
  G4NistManager* nm = G4NistManager::Instance();
  
  efile << "# V1.1\n";
  efile << "# NIST Elements from BDSIM for pyg4ometry\n";
  efile << "# Geant4 version: " << G4Version << "\n";
  efile << "# element\tZ\tName\tdensity(g/cm^3)\tI(eV)\tnIsotopes\tstate\ttemperature[K]\tpressure[Pa]\n";
  efile << "# for each isotope\n";
  efile << "# \tA\tfractional abundance\tmolar mass (g)\n";

  //int nElements = (int)nm->GetNistElementNames().size(); // nope
  int nElements = 99;
  efile << "# number of elements (unique Z): " << nElements << "\n";
  // the hard-coded number in G4NistMaterialBuilder (needed for density)
  // this number is not accessible in the code, so hard coded here
  for (G4int Z = 1; Z < nElements; Z++)
    {
      G4Element* el = nm->FindOrBuildElement(Z);
      G4String name = "G4_" + el->GetName();
      G4Material* mat = nm->FindOrBuildMaterial(name);
      G4double meanIonisationEnergy = nm->GetMeanIonisationEnergy(Z);
      G4int nIsotopes = (G4int)el->GetNumberOfIsotopes();
      efile << "element"
            << "\t" << Z << "\t" << name
            << "\t" << std::setw(12) << std::scientific << mat->GetDensity() / gcm3
            << "\t" << meanIonisationEnergy / CLHEP::eV;
      // we have to do this instead of << std::defaultfloat because GCC4.9 doesn't include this
      // despite being in the standard for C++11
      efile.unsetf(std::ios_base::floatfield);
      efile << "\t" << nIsotopes << "\t" << stateMap[mat->GetState()]
            << "\t" << mat->GetTemperature() / CLHEP::kelvin
            << "\t" << mat->GetPressure() / CLHEP::hep_pascal << "\n";
      
      G4double* abundances = el->GetRelativeAbundanceVector();
      G4IsotopeVector* isotopes = el->GetIsotopeVector();
      if (isotopes)
        {
          int i = 0;
          for (const auto& iso : *isotopes)
            {
        efile.unsetf(std::ios_base::floatfield);
              efile << "\t" << iso->GetN() // number of nucleons
                    << "\t" << std::setw(12) << std::scientific << abundances[i]
                    << "\t" << std::setw(12) << std::scientific << iso->GetA() / ( CLHEP::g/CLHEP::mole)
                    << "\n";
              i++;
            }
        }
      
    }
  efile.close();
  
  std::ofstream mfile;
  mfile.open("nist_materials.txt");
  
  mfile << "# V1.0\n";
  mfile << "# NIST materials from BDSIM for pyg4ometry\n";
  mfile << "# Geant4 version: " << G4Version << "\n";
  mfile << "# NIST materials by # of atoms of each element - each element includes isotopes\n";
  mfile << "# of atoms per unit is only approximate from Geant4 so best to go by fraction of mass\n";
  mfile << "# material   # of elements\t         name\t                density(g/cm^3)\tI(eV)\t\tstate\ttemperature[K]\tpressure[Pa]\n";
  mfile << "# for each element\n";
  mfile << "# \t         name\tZ\t# atoms    fraction of mass\n";
  
  std::vector<G4String> materialNames = nm->GetNistMaterialNames();
  
  // build a map of names to indices as much quicker and easier to search / use
  // pretty rubbish design, but G4NistManager *only* allows access to the mean ionisation
  // energy and the nominal density via an index of the material in its internal vector
  // which it gives no access to...  So we build a map of indices from the names and assume
  // the materials don't change with what we do here. Rubbish.
  std::map<G4String, G4int> materialNameToIndex;
  for (int i = 0; i < (int)materialNames.size(); i++)
    {materialNameToIndex[materialNames[i]] = i;}
  
  // filter out elements from the names as no way to access them separately
  std::vector<G4String> elementNames = nm->GetNistElementNames();
  std::set<G4String> g4ElementNames;
  for (const auto& name : elementNames)
    {
      if (!name.empty()) // apparently some are just empty...
        {g4ElementNames.insert("G4_"+name);}
    }
  std::set<G4String> materialSet = {materialNames.begin(), materialNames.end()};
  for (const auto& name : g4ElementNames)
    {materialSet.erase(name);}
  
  // write the file
  for (const auto& materialName : materialSet)
    {
      G4Material* mat = nm->FindOrBuildMaterial(materialName);
      // "Z" as the argument to this function is actually just the index in the material vector
      G4double meanIonisationEnergy = nm->GetMeanIonisationEnergy(materialNameToIndex[mat->GetName()]);
      mfile << "material " << std::setw(4);
      mfile.unsetf(std::ios_base::floatfield);
      mfile << mat->GetNumberOfElements()
            << "\t" << std::setw(40) << mat->GetName()
            << "\t" << std::setw(12) << std::scientific << mat->GetDensity() / gcm3
            << "\t" << meanIonisationEnergy / CLHEP::eV
            << "\t" << stateMap[mat->GetState()]
            << "\t" << mat->GetTemperature() / CLHEP::kelvin
            << "\t" << mat->GetPressure() / CLHEP::hep_pascal
            << "\n";
  
      const G4ElementVector* elementArray = mat->GetElementVector();
      const G4double* fractionArray = mat->GetFractionVector();
      const G4int* atomsVector      = mat->GetAtomsVector();
      
      for (int i = 0; i < (int)elementArray->size(); i++)
        {
          const auto element = (*elementArray)[i];
          mfile.unsetf(std::ios_base::floatfield);
          mfile << "\t" << std::setw(12) << element->GetName() << "\t";
          mfile.unsetf(std::ios_base::floatfield);
          mfile << element->GetZ()
                << "\t" << (*atomsVector + i)
                << "\t" << std::setw(12) << std::scientific << fractionArray[i]
                << "\n";
        }
    }
  mfile.close();
  return 0;
}
