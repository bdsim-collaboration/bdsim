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
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSGeometryWriter.hh"
#include "BDSUtilities.hh"

#include "globals.hh" // geant4 types / globals

#ifdef USE_GDML
#include "G4Colour.hh"
#include "G4GDMLAuxStructType.hh"
#include "G4GDMLParser.hh"
#include "G4VisAttributes.hh"
#endif

#include "G4FileUtilities.hh"
#include "G4String.hh"
#include "G4TransportationManager.hh"

#include <array>
#include <map>
#include <sstream>

class G4VPhysicalVolume;

/// "bds_vrgba" - short to minimise output file size: "bdsim visibility rgba"
const G4String BDSGeometryWriter::auxType = "bds_vrgba";

BDSGeometryWriter::~BDSGeometryWriter()
{;}

BDSGeometryWriter::BDSGeometryWriter()
{;}

void BDSGeometryWriter::ExportGeometry(const G4String& geometryType,
                                       const G4String& geometryFileName)
{
  if (geometryType == "gdml")
    {
#ifdef USE_GDML
      WriteToGDML(geometryFileName);
#else
      throw BDSException(__METHOD_NAME__, "Unable to write out " + geometryFileName + ", as compiled without GDML support");
#endif
    }
  else
    {throw BDSException(__METHOD_NAME__, "unknown geometry export type \"" + geometryType + "\".");}
}

#ifdef USE_GDML
void BDSGeometryWriter::WriteToGDML(const G4String&    outputFileName,
                                    G4VPhysicalVolume* volume)
{
  if (!volume)
    {volume = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking()->GetWorldVolume();}
  // gdml parser throws exception if file exists. Delete file in that case
  if (BDS::FileExists(outputFileName))
    {
      G4FileUtilities fileUtilities;
      G4cout << "Removing existing file: \"" << outputFileName << "\"" << G4endl;
      fileUtilities.DeleteFile(outputFileName, "");
    }

  // if the filename is in part a path to a directory, check that directory exists
  // otherwise the GDML writer will give an abort trap bad access
  if (BDS::StrContains(outputFileName, "/"))
    {
      G4String dirName = BDS::GetFullPath(outputFileName, true);
      if (!BDS::DirectoryExists(dirName))
        {throw BDSException(__METHOD_NAME__, "directory \"" + dirName + "\" does not exist - please create it first.");}
    }

  auto volToAuxMap = PrepareColourInformation(volume);

  G4GDMLParser parser;
  RegisterVolumeAuxiliaryInformation(parser, volToAuxMap);
  parser.Write(outputFileName, volume, true);
}

G4String BDSGeometryWriter::ColourToVRGBAString(G4bool visible,
                                                const G4Colour& colour)
{
  std::array<G4double, 4> numbers = {colour.GetRed(),
                                     colour.GetGreen(),
                                     colour.GetBlue(),
                                     colour.GetAlpha()};
  std::stringstream ss;
  ss << visible << " ";
  for (auto cv : numbers)
    {ss << std::setprecision(4) << cv << " ";} // colours are already normalised 0 to 1.
  G4String result = ss.str();
  result.pop_back(); // remove last space
  return result;
}

std::map<G4LogicalVolume*, G4GDMLAuxStructType> BDSGeometryWriter::PrepareColourInformation(G4VPhysicalVolume* volume)
{
  std::map<G4LogicalVolume*, G4GDMLAuxStructType> result;
  auto lv = volume->GetLogicalVolume();
  AddLVColour(result, lv);
  return result;
}

void BDSGeometryWriter::RegisterVolumeAuxiliaryInformation(G4GDMLParser& parser,
                                                           const std::map<G4LogicalVolume*, G4GDMLAuxStructType>& volToAuxMap)
{
  for (const auto& lvAux : volToAuxMap)
    {parser.AddVolumeAuxiliary(lvAux.second, lvAux.first);}
}

void BDSGeometryWriter::AddLVColour(std::map<G4LogicalVolume*, G4GDMLAuxStructType>& map, G4LogicalVolume* lv)
{
  const G4VisAttributes* visAttr = lv->GetVisAttributes();
  if (visAttr)
    {
      G4GDMLAuxStructType c;
      c.type = auxType;
      c.value = ColourToVRGBAString(visAttr->IsVisible(), visAttr->GetColour());
      c.auxList = nullptr;
      map[lv] = c;
    }
  for (std::size_t i = 0; i < lv->GetNoDaughters(); i++)
    {
      auto dlv = lv->GetDaughter(i)->GetLogicalVolume();
      AddLVColour(map, dlv);
    }
}

#endif
