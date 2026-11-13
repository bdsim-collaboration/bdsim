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
#ifndef BDSGEOMETRYWRITER_H
#define BDSGEOMETRYWRITER_H

#include "globals.hh" // geant4 types / globals
#include "G4VisAttributes.hh"
#include "G4String.hh"

#include <map>

class G4Colour;
class G4GDMLParser;
class G4LogicalVolume;
class G4VPhysicalVolume;
struct G4GDMLAuxStructType;

/**
 * @brief A class for writing fully constructed geometry from BDSIM
 * out to other formats.
 * 
 * @author Laurie Nevay
 */

class BDSGeometryWriter
{
public:
  BDSGeometryWriter();
  ~BDSGeometryWriter();

  const static G4String auxType;

  /// Dispatch function that matches the geometry type and uses the appropriate
  /// member function to write out the geometry
  void ExportGeometry(const G4String& geometryType,
                      const G4String& geometryFileName);

  /// Return a string with the format "int float float float float" for
  /// (bool) visible, (double) rgb, and (double) alpha values separated by a space.
  /// e.g. "1 0.2 0.4 0.6 1.0"
  static G4String ColourToVRGBAString(G4bool visible,
                                      const G4Colour& colour);

private:
#ifdef USE_GDML
  /// Write the geometry to specified output filename. The filename should
  /// be the filename ending in .gdml. If the supplied volume is nullptr (default),
  /// the world volume will be written out.
  void WriteToGDML(const G4String& outputFileName, G4VPhysicalVolume* volume = nullptr);

  /// Generate a map of logical volume pointers to output auxiliary structure
  /// information for writing out to GDML.
  static std::map<G4LogicalVolume*, G4GDMLAuxStructType> PrepareColourInformation(G4VPhysicalVolume* volume);

  /// Register the auxiliary information with the GDML parser (writer).
  static void RegisterVolumeAuxiliaryInformation(G4GDMLParser& parser,
                                                 const std::map<G4LogicalVolume*, G4GDMLAuxStructType>& volToAuxMap);

  /// Recursively add information to the map by reference for each logical
  /// volume vis attributes if they exist.
  static void AddLVColour(std::map<G4LogicalVolume*, G4GDMLAuxStructType>&, G4LogicalVolume* lv);
#endif
};

#endif
